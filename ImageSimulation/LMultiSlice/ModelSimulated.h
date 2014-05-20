#pragma once

#include "stdafx.h"
#include "Microscope.h"
#include "ModelPotential.h"

class ModelSimulated {
public:
	ModelSimulated(void);
	~ModelSimulated(void);
	ModelSimulated(ModelPotential* modelPotential, ModelFragmented *modelFragmented, size_t nx, size_t ny, double dpa);
	int imageCalculation(Image *result, Microscope *microscope);
private:
	ModelPotential		*modelPotential;
	ModelFragmented		*modelFragmented;

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