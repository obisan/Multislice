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

// 	inline	double		calculatePotential(int numberAtom, double r) {
// 		double sumf = 0, sums = 0;
// 		double dR1 = 6.2831853071796 * r; // 2 * PI * r
// 		for(int k = 0; k < 3; k++) {
// 			int Offs = (numberAtom) * 12 + k * 2;
// 			sumf += FParams[Offs + 0] / r * exp(- dR1 * sqrt(FParams[Offs + 1]));
// 		}				
// 		sumf *= 150.365396971475; // 4 * PI * PI *a0 * e
// 		
// 		for(int k = 0; k < 3; k++) {
// 			int Offs = (numberAtom) * 12 + k * 2;
// 			sums += FParams[Offs + 6] * pow(FParams[Offs + 7], -3.0 / 2.0) * exp(-(6.2831853071796 * r * r) / FParams[Offs + 7]);
// 		}
// 		sums *= 266.5157269050303; // 2 * PI * PI * a0 * e
// 		return (sumf + sums);
// 	}
};