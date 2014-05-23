#pragma once

#include "stdafx.h"
#include "fparams.h"
#include "Microscope.h"

#define M_PI	3.14159265358979323846

#define CUERR { cudaError_t err; \
	if ((err = cudaGetLastError()) != cudaSuccess) { \
	printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__); \
	system("pause"); \
	return -1; }}

class ModelPotential {
public:
	ModelPotential(void);
	ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double dpa, double radius);
	~ModelPotential(void);

	int		calculatePotentialGrid(Image *result);
	int		savePotential(Image *image);
	
	size_t	getNx();
	size_t	getNy();
	size_t	getNz();
	
private:
	AModel::Model		*model;
	size_t				nx;
	size_t				ny;
	size_t				nz;
	
	double				radius;
	double				dpa;
};