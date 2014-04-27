#include "stdafx.h"
#include "ModelPotential.h"

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

int ModelPotential::calculatePotentialGrid(Image *result, Image *v) {
	AModel::Cortege *tableAtoms = modelFragmented->getModelSource()->getTableCell();
	const size_t numberAtoms = modelFragmented->getModelSource()->getNumberAtoms();
	const int numberSlices = modelFragmented->getNumberSlices();
	const double dk = 1.0 / dpa;
	const double a = modelFragmented->getModelSource()->getA();
	const double b = modelFragmented->getModelSource()->getB();
	const double c = modelFragmented->getModelSource()->getC();
	const double dx = a / this->nx;
	const double dy = b / this->ny;
	const double dz = c / numberSlices;
	const size_t nChannels = result->nChannels;


	for(size_t kz = 0; kz < nz; kz++) {
		Slice *currentSlice = modelFragmented->getSliceByZ((kz + 1) * dz);	
		for(size_t iy = 0; iy < ny; iy++) {
			double *pResult = result->getPointer<double>(kz, iy);
			double *pV = v->getPointer<double>(kz, iy);
			for(size_t jx = 0; jx < nx; jx++) {
				for(auto p = currentSlice->begin(); p != currentSlice->end(); p++) {
					AModel::Cortege pCortege = (**p);

					double dX = fabs(pCortege.element.xsCoordinate.x * a - (jx * dx));
					double dY = fabs(pCortege.element.xsCoordinate.y * b - (iy * dy));
					double dZ = fabs(pCortege.element.xsCoordinate.z * c - (kz * dz));
					
					if(dZ >= dz) continue;

					if( dX >= a / 2.0 ) dX = dX - a;
					if( dY >= b / 2.0 ) dY = dY - b;
					
					double dR = sqrt(dX * dX + dY * dY) * dk;
					int m = modelFragmented->getModelSource()->getNumberByName(pCortege.element.Atom) - 1;
					double calculateProjectedAtomicPotential;

					if( dR < 1.0e-10 ) dR = 1.0e-10;

					calculateProjectedAtomicPotential = calculateProjectedPotential(m, dR);
					
					pResult[nChannels * jx + 0] += calculateProjectedAtomicPotential;
					pResult[nChannels * jx + 1] = 0;
					pV[nChannels * jx + 0] += calculateProjectedAtomicPotential;
					pV[nChannels * jx + 1] = 0;
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
