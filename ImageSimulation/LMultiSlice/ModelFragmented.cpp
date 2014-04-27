#include "stdafx.h"
#include "ModelFragmented.h"

ModelFragmented::ModelFragmented(void) {

}

ModelFragmented::ModelFragmented(AModel::Model* model, int numberSlices) {
	this->modelSource = model;
	this->numberSlices = numberSlices;
}

ModelFragmented::~ModelFragmented(void) {
	if(this->modelSource != nullptr) this->modelSource = nullptr;
}

int ModelFragmented::dividing() {
	float	dx = 1;
	float	dy = 1;
	float	dz = modelSource->getC() / numberSlices;
	float	a = modelSource->getA();
	float	b = modelSource->getB();
	float	c = modelSource->getC();

	if(numberSlices < 1) {
		std::cerr << "Bad number slices (... < 1)!" << std::endl;
		return -1;
	}

	size_t nAtoms = modelSource->getNumberAtoms();
	AModel::Cortege* pAtoms = modelSource->getTableCell();
	AModel::Abc paramModel(a,b,c);
	
	for(size_t k = 0; k * dz < c; k++) {
		Slice currentSlice(k, k * dz, (k + 1) * dz);
		for(size_t i = 0; i < nAtoms; i++) {
			AModel::XYZ pAtom = pAtoms[i].element.xsCoordinate * paramModel;
			if( (pAtom.z >= currentSlice.getLowerBound()) && (pAtom.z <= currentSlice.getUpperBound()))
				currentSlice.push_back(&pAtoms[i]);
		}
		slices.push_back(currentSlice);
	}

	pAtoms = nullptr;
	return 0;
}

AModel::Model* ModelFragmented::getModelSource() {
	return modelSource;
}

int ModelFragmented::getNumberSlices() {
	return numberSlices;
}

Slice* ModelFragmented::getSliceByZ(double Z) {
	for(auto p = slices.begin(); p != slices.end(); p++)
		if(Z >= p->getLowerBound() && Z <= p->getUpperBound()) 
			return &(*p);
	return nullptr;
}

std::vector<Slice>& ModelFragmented::getSlices() {
	return slices;
}