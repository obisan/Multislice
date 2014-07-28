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
		printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__);
	
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

	int1	*atomId;
	float3	*atomXYZ;
	
	cudaMallocManaged(&atomId,	nAtoms * sizeof(int3));
	cudaMallocManaged(&atomXYZ,	nAtoms * sizeof(float3));
	
	CUERR

	AModel::Cortege *pAtoms = model->getTableCell();
	std::sort(pAtoms, pAtoms + nAtoms);
	for(size_t i = 0; i < nAtoms; i++) {
		atomId[i].x  = model->getNumberByName(pAtoms[i].element.Atom) - 1;
		atomXYZ[i].x = pAtoms[i].element.xsCoordinate.x;
		atomXYZ[i].y = pAtoms[i].element.xsCoordinate.y;
		atomXYZ[i].z = pAtoms[i].element.xsCoordinate.z;
	}
	pAtoms = nullptr;

	const size_t MAX_THREADS = 16;
	dim3 threads(MAX_THREADS, MAX_THREADS, 1);							// размер квардатика
	dim3 grid(this->nx / MAX_THREADS, this->ny / MAX_THREADS, 1 );		// сколько квадратиков нужно чтобы покрыть все изображение

	CUERR

	cudaEvent_t start,stop;
	float time = 0.0f;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
	
	for(size_t kz = 0; kz * dz < c; kz++) {
		calculateProjectedPotential<<<grid, threads>>>(atomId, atomXYZ, nAtoms, a, b, c, dx, dy, dz, potential + nx * ny * kz, nx, ny, nz, radius, dk);
		cudaThreadSynchronize();
	}
	
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);
	
	CUERR
	
	std::cout << std::endl << "Kernel time: " << time << "ms." << std::endl << std::endl;

	cudaFree(atomId);
	cudaFree(atomXYZ);

	atomId = nullptr;
	atomXYZ = nullptr;
	
	return 0;
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
