#include "stdafx.h"
#include "ModelPotential.h"
#include "kernel.cuh"

ModelPotential::ModelPotential(void) {

}

ModelPotential::ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double dpa, double radius) {
	this->model = model;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	this->dpa = dpa;
	this->radius = radius;
	
	cudaMallocManaged(&(this->potential), nx * ny * nz * sizeof(double));

	cudaError_t err;
	if ((err = cudaGetLastError()) != cudaSuccess)
		printf("CUDA error: %s, file %s, line %d\n", cudaGetErrorString(err), __FILE__,  __LINE__);
	
	memset(this->potential, 0, nx * ny * nz * sizeof(double));
}

ModelPotential::~ModelPotential(void) {
	if(this->model != nullptr) { model = nullptr; }
}

int ModelPotential::calculatePotentialGrid() {
	const size_t nAtoms = model->getNumberAtoms();
	const double dk = 1.0 / dpa;
	const double a = model->getA();
	const double b = model->getB();
	const double c = model->getC();
	const double dx = a / this->nx;
	const double dy = b / this->ny;
	const double dz = c / this->nz;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	int deviceCount = 0;
	cudaGetDeviceCount(&deviceCount);
	printf("\nDetected %d CUDA accelerators:\n", deviceCount);
	int dev;
	for (dev=0; dev < deviceCount; dev++) {
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, dev);
		printf("  [%d]: '%s'  Clock: %.1f GHz  Mem: %dMB  Rev: %d.%d\n", 
			   dev, deviceProp.name, 
			   deviceProp.clockRate / 1000000.0f, deviceProp.totalGlobalMem / (1024*1024),
			   deviceProp.major, deviceProp.minor);
	  }

	int cudadev = 0;
	printf("  Single-threaded single-GPU test run.\n");
	printf("  Opening CUDA device %d...\n\n", cudadev);
	cudaSetDevice(cudadev);
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	cudaEvent_t start,stop;
	float time = 0.0f;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
	
	const size_t MAX_THREADS = 16;
	dim3 threads(MAX_THREADS, MAX_THREADS, 1);							// размер квардатика
	dim3 grid(this->nx / MAX_THREADS, this->ny / MAX_THREADS, 1 );		// сколько квадратиков нужно чтобы покрыть все изображение

	AModel::Cortege *pAtoms = model->getTableCell();
	std::sort(pAtoms, pAtoms + nAtoms);
	
	thrust::device_vector<int1>		atomIDDevice;
	thrust::device_vector<float3>	atomXYZDevice;

	int1 buffiD;
	float3 buffXYZ;
				
	for(size_t kz = 0; kz * dz < c; kz++) {
		for(size_t i = 0; i < nAtoms; i++) {
			if( kz * dz <= pAtoms[i].element.xsCoordinate.z * c && pAtoms[i].element.xsCoordinate.z * c < (kz + 1) * dz ) {
				buffiD.x = model->getNumberByName(pAtoms[i].element.Atom) - 1;
				buffXYZ.x = pAtoms[i].element.xsCoordinate.x;
				buffXYZ.y = pAtoms[i].element.xsCoordinate.y;
				buffXYZ.z = pAtoms[i].element.xsCoordinate.z;

				atomIDDevice.push_back(buffiD);
				atomXYZDevice.push_back(buffXYZ);
			}
		}
		int1	*patomID	= thrust::raw_pointer_cast(&atomIDDevice[0]);
		float3	*patomXYZ	= thrust::raw_pointer_cast(&atomXYZDevice[0]);

		calculateProjectedPotential<<<grid, threads>>>(atomIDDevice.size(), patomID, patomXYZ, a, b, c, dx, dy, dz, potential + nx * ny * kz, radius, dk);
		cudaThreadSynchronize();

		atomIDDevice.clear();
		atomXYZDevice.clear();
	}

	pAtoms = nullptr;
	
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);
	
	CUERR
	
	std::cout << std::endl << "Kernel time: " << time << "ms." << std::endl << std::endl;
	
	return 0;
}

int	ModelPotential::savePotential(const char* filename) {
	Image *image = new Image(nx, ny, nz, sizeof(double), 1);
	memcpy(image->imageData, this->potential, nx * ny * nz * sizeof(double));
	image->saveMRC(filename, model, nx, ny, nz, mrc_FLOAT);
	delete image;

	return 0;
}

AModel::Model* ModelPotential::getModel() {
	return model;
}

size_t ModelPotential::getNx() {
	return nx;
}

size_t ModelPotential::getNy() {
	return ny;
}

size_t ModelPotential::getNz() {
	return nz;
}
