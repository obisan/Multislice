#include "stdafx.h"
#include "ModelFragmented.h"

ModelFragmented::ModelFragmented(void) {

}

ModelFragmented::ModelFragmented(AModel::Model* model, int numberSlices) {
	this->modelSource = model;
	this->numberSlices = numberSlices;

	cudaMallocManaged(&atomCount, model->getNumberAtoms() * sizeof(int));
	cudaMallocManaged(&atomId, model->getNumberAtoms() * sizeof(int));
	cudaMallocManaged(&xyz, model->getNumberAtoms() * 3 * sizeof(float));

	memset(atomCount, 0, model->getNumberAtoms() * sizeof(int));
	memset(atomId, 0, model->getNumberAtoms() * sizeof(int));
	memset(xyz, 0, model->getNumberAtoms() * 3 * sizeof(float));
}

ModelFragmented::~ModelFragmented(void) {
	if(this->modelSource != nullptr) this->modelSource = nullptr;
	if(this->atomCount != nullptr) {cudaFree(this->atomCount); this->atomCount = nullptr;}
	if(this->atomId != nullptr) {cudaFree(this->atomId); this->atomId = nullptr;}
	if(this->xyz != nullptr) {cudaFree(this->xyz); this->xyz = nullptr;}
}

int ModelFragmented::dividing() {
	float	dx = 1;
	float	dy = 1;
	float	dz = modelSource->getC() / numberSlices;
	float	a = modelSource->getA();
	float	b = modelSource->getB();
	float	c = modelSource->getC();
	size_t	nAtoms = modelSource->getNumberAtoms();
	AModel::Cortege* pAtoms = modelSource->getTableCell();
	AModel::Abc paramModel(a,b,c);

	if(numberSlices < 1) {
		std::cerr << "Bad number slices (... < 1)!" << std::endl;
		return -1;
	}
		
	for(size_t z = 0, l = 0; z * dz < c; z++) {
		for(size_t i = 0; i < nAtoms; i++) {
			AModel::XYZ ppAtom = pAtoms[i].element.xsCoordinate;
			if( (ppAtom.z * c > z * dz) && (ppAtom.z * c <= (z + 1) * dz) ) {
				atomCount[l] = z;
				atomId[l] = modelSource->getNumberByName(pAtoms[i].element.Atom);
				xyz[l][0] = ppAtom.x;
				xyz[l][1] = ppAtom.y;
				xyz[l][2] = ppAtom.z;
				l++;
			}
		}
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