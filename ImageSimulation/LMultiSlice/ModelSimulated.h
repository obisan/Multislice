#pragma once

#include "stdafx.h"
#include "Microscope.h"

class ModelSimulated {
public:
	ModelSimulated(void);
	~ModelSimulated(void);
	ModelSimulated(const char* PotentialDirectory, AModel::Model* model, int nx, int ny, int countSlices, double dpa);
	int imageCalculation(Image *result, Microscope *microscope);
private:
	PotentialBuilder::ModelPotential		*modelPotential;
	
	size_t				nx;
	size_t				ny;
	size_t				nz;
	
	double				dpa;

	template<class T> static void		swap(T& a, T& b) {
		T buffer = a;
		a = b;
		b = buffer;
	}

};