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
	

	int *atomId;
	float (*xyz)[3];
	float *val;

	cudaMallocManaged(&atomId, nAtoms * sizeof(int));
	cudaMallocManaged(&xyz, nAtoms * 3 * sizeof(float));
	cudaMallocManaged(&val, sizeof(float));
	
	for(size_t kz = 0; kz < nz; kz++) {
		
		Slice *currentSlice = modelFragmented->getSliceByZ((kz + 1) * dz);
		for(size_t i = 0; i < currentSlice->size(); i++) {
			atomId[i] = modelFragmented->getModelSource()->getNumberByName((*currentSlice)[i].element.Atom);
			xyz[i][0] = (*currentSlice)[i].element.xsCoordinate.x;
			xyz[i][1] = (*currentSlice)[i].element.xsCoordinate.y;
			xyz[i][2] = (*currentSlice)[i].element.xsCoordinate.z;
		}

 		for(size_t iy = 0; iy < ny; iy++) {
  			double *pResult = result->getPointer<double>(0, iy);
  			for(size_t jx = 0; jx < nx; jx++) {
				for(size_t l = 0; l < nAtoms; l++) {
					double dX = fabs(xyz[l][0] * a - (jx * dx));
  					double dY = fabs(xyz[l][1] * b - (iy * dy));
  					double dZ = fabs(xyz[l][2] * c - (0 * dz));
  					
  					if(dZ >= dz) continue;
  
  					if( dX >= a / 2.0 ) dX = dX - a;
  					if( dY >= b / 2.0 ) dY = dY - b;
  					
  					double dR = sqrt(dX * dX + dY * dY) * dk;
  					int m = atomId[l] - 1;
  					double calculateProjectedAtomicPotential;
  
  					if( dR < 1.0e-10 ) dR = 1.0e-10;
  
  					//pResult[nChannels * jx + 0] += calculateProjectedPotential(m, dR);
					//calculateProjectedPotential(m, dR, val);
					calculateProjectedPotentialGPU<<<1, 1>>> (m, dR, val);
					cudaThreadSynchronize();
					
					float hval;
					memcpy(&hval, val, sizeof(float));

					pResult[nChannels * jx + 0] += hval;
  					pResult[nChannels * jx + 1] = 0;
				} // atom
  			} // x
  		} // y

	} // z

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
