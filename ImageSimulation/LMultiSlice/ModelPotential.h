#pragma once

#include "stdafx.h"
#include "fparams.h"
#include "Microscope.h"

class ModelPotential {
public:
	ModelPotential(void);
	ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double dpa, double radius);
	~ModelPotential(void);

	int		calculatePotentialGrid();
	int		savePotential(const char* filename);
	
	AModel::Model*		getModel();
	size_t	getNx();
	size_t	getNy();
	size_t	getNz();
	
	double				*potential;

private:
	AModel::Model		*model;
	size_t				nx;
	size_t				ny;
	size_t				nz;
	
	double				radius;
	double				dpa;
};