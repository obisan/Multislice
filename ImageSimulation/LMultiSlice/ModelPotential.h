#pragma once

#include "stdafx.h"
#include "ModelFragmented.h"
#include "fparams.h"
#include "Microscope.h"

#define M_PI	3.14159265358979323846

//#define __CUDACC__

class ModelPotential {
public:
	ModelPotential(void);
	ModelPotential(ModelFragmented* modelFragmented, size_t nx, size_t ny, double dpa, double radius);
	~ModelPotential(void);

	int		calculatePotentialGrid(Image *result);
	int		savePotential(Image *image);
	
	size_t	getNx();
	size_t	getNy();
	size_t	getNz();
	
private:
	ModelFragmented		*modelFragmented;
	size_t				nx;
	size_t				ny;
	size_t				nz;
	
	double				radius;
	double				dpa;
};