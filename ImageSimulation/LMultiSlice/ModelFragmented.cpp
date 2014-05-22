#include "stdafx.h"
#include "ModelFragmented.h"

ModelFragmented::ModelFragmented(void) {

}

ModelFragmented::ModelFragmented(AModel::Model* model, int numberSlices) {
	this->modelSource = model;
	this->numberSlices = numberSlices;

	cudaMallocManaged(&atomId,	model->getNumberAtoms() * sizeof(int));
	cudaMallocManaged(&xyz,		model->getNumberAtoms() * 3 * sizeof(float));

	memset(atomId,	0,	model->getNumberAtoms() * sizeof(int));
	memset(xyz,		0,	model->getNumberAtoms() * 3 * sizeof(float));
}

ModelFragmented::~ModelFragmented(void) {
	if(this->modelSource != nullptr) this->modelSource = nullptr;
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
	
	if(numberSlices < 1) {
		std::cerr << "Bad number slices (... < 1)!" << std::endl;
		return -1;
	}
		
	AModel::Cortege* pAtoms = modelSource->getTableCell();

	std::sort(pAtoms, pAtoms + modelSource->getNumberAtoms());

	for(size_t i = 0; i < nAtoms; i++) {
		atomId[i] = modelSource->getNumberByName(pAtoms[i].element.Atom) - 1;
		xyz[i][0] = pAtoms[i].element.xsCoordinate.x;
		xyz[i][1] = pAtoms[i].element.xsCoordinate.y;
		xyz[i][2] = pAtoms[i].element.xsCoordinate.z;
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