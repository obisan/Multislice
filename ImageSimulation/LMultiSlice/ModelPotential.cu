#include "stdafx.h"
#include "ModelPotential.h"
#include "kernel.cuh"

ModelPotential::ModelPotential(void) {

}

ModelPotential::ModelPotential(ModelFragmented* modelFragmented, size_t nx, size_t ny, double dpa, double radius) {
	this->modelFragmented = modelFragmented;
	this->nx = nx;
	this->ny = ny;
	this->nz = modelFragmented->getNumberSlices();
	this->dpa = dpa;
	this->radius = radius;
}

ModelPotential::~ModelPotential(void) {
	if(this->modelFragmented != nullptr) this->modelFragmented = nullptr;
}

int ModelPotential::calculatePotentialGrid(Image *result) {
	const size_t nChannels = result->nChannels;
	const size_t numberSlices = modelFragmented->getNumberSlices();
	const size_t nAtoms = modelFragmented->getModelSource()->getNumberAtoms();
	const double dk = 1.0 / dpa;
	const double a = modelFragmented->getModelSource()->getA();
	const double b = modelFragmented->getModelSource()->getB();
	const double c = modelFragmented->getModelSource()->getC();
	const double dx = a / this->nx;
	const double dy = b / this->ny;
	const double dz = c / numberSlices;
	
	int *atomCountInSlice = modelFragmented->atomCountInSlice;
	int *atomId = modelFragmented->atomId;
	float (*xyz)[3] = modelFragmented->xyz;
	
	const size_t MAX_THREADS = 32;
	dim3 threads(MAX_THREADS, MAX_THREADS, 1);														//������ ����������
	dim3 grid(result->width / MAX_THREADS, result->height / MAX_THREADS, result->thickness );		//������� ����������� ����� ����� ������� ��� �����������

	cudaError_t err = cudaSuccess;
	cudaEvent_t start,stop;
	float time = 0.0f;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
	calculateProjectedPotential<<<grid, threads>>>(atomCountInSlice, atomId, xyz, nAtoms, a, b, c, dx, dy, dz, (double*) (result->imageData), nChannels, nx, ny, numberSlices, dk);
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);
	
	err = cudaGetLastError();

    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch kernel (error code %s)!\n", cudaGetErrorString(err));
		system("pause");
        exit(EXIT_FAILURE);
    }
	
	std::cout << std::endl << "Kernel time: " << time << "ms." << std::endl << std::endl;

	atomCountInSlice = nullptr;
	atomId = nullptr;
	xyz = nullptr;

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
