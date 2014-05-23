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
}

ModelPotential::~ModelPotential(void) {
	if(this->model != nullptr) { model = nullptr; }
}

int ModelPotential::calculatePotentialGrid(Image *result) {
	const size_t nChannels = result->nChannels;
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

	int		*atomId;
	float	*atomR;
	int		*atomsinpixel;
	
	cudaMallocManaged(&atomId,			result->width * result->height * 50 * sizeof(int));
	cudaMallocManaged(&atomR,			result->width * result->height * 50 * sizeof(float));
	cudaMallocManaged(&atomsinpixel,	result->width * result->height * sizeof(int));

	CUERR

	memset(atomId,			1,	result->width * result->height * 50 * sizeof(int));
	memset(atomR,			0,	result->width * result->height * 50 * sizeof(float));
	memset(atomsinpixel,	0,	result->width * result->height * sizeof(int));

	const size_t MAX_THREADS = 16;
	dim3 threads(MAX_THREADS, MAX_THREADS, 1);														//размер квардатика
	dim3 grid(result->width / MAX_THREADS, result->height / MAX_THREADS, 1 );		//сколько квадратиков нужно чтобы покрыть все изображение

	CUERR

	cudaEvent_t start,stop;
	float time = 0.0f;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
	
	AModel::Cortege *pAtoms = model->getTableCell();
	for(size_t kz = 0; kz * dz < c; kz++) {

		for(size_t ky = 0; ky * dy < b; ky++) {
			for(size_t kx = 0; kx * dx < a; kx++) {

				size_t batomsinpixel = 0;
				for(size_t i = 0; i < nAtoms && batomsinpixel < 50; i++) {
					AModel::XYZ ppAtoms = pAtoms[i].element.xsCoordinate;
					if( (ppAtoms.z * c > kz * dz) && (ppAtoms.z * c <= (kz + 1) * dz) ) {
						double dX = fabs(ppAtoms.x * a - (kx * dx));
						double dY = fabs(ppAtoms.y * b - (ky * dy));
						 		
						dX = ( dX >= a / 2.0 ) ? dX - a : dX;
						dY = ( dY >= b / 2.0 ) ? dY - b : dY;
						 
						double dR = sqrt(dX * dX + dY * dY);
						 		
						if(dR > radius) continue;
						
						atomId	[50 * nx * ky + 50 * kx + batomsinpixel] = model->getNumberByName(pAtoms[i].element.Atom) - 1;
						atomR	[50 * nx * ky + 50 * kx + batomsinpixel] = ( dR < 1.0e-10 ) ? 1.0e-10 : dR;
						atomsinpixel[nx * ky + kx] = ++batomsinpixel;
					}
				}		
			}
		}

		
		calculateProjectedPotential<<<grid, threads>>>(atomsinpixel, atomId, atomR, a, b, c, dx, dy, dz, /*(double*) (result->imageData)*/ result->getPointer<double>(kz, 0), nChannels, nx, ny, nz, radius, dk);
		cudaThreadSynchronize();
	}
	
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);
	
	CUERR
	
	std::cout << std::endl << "Kernel time: " << time << "ms." << std::endl << std::endl;

	cudaFree(atomsinpixel);
	cudaFree(atomId);
	cudaFree(atomR);

	atomId = nullptr;
	atomR = nullptr;
	atomsinpixel = nullptr;

	return 0;
}

int ModelPotential::savePotential(Image *image) {

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
