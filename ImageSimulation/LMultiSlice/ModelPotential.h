#pragma once

#include "stdafx.h"
#include "ModelFragmented.h"
#include "fparams.h"
#include "Microscope.h"

#define M_PI	3.14159265358979323846

class ModelPotential {
public:
	ModelPotential(void);
	ModelPotential(ModelFragmented* modelFragmented, size_t nx, size_t ny, double dpa, double radius);
	~ModelPotential(void);

	int		calculatePotentialGrid(Image *result, Image *v);
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

	inline	double		bessk0( double x ) {
	double ax, x2, sum;
	double k0a[] = { -0.57721566, 0.42278420, 0.23069756,
		0.03488590, 0.00262698, 0.00010750, 0.00000740};
	
	double k0b[] = { 1.25331414, -0.07832358, 0.02189568,
		-0.01062446, 0.00587872, -0.00251540, 0.00053208};
	
	ax = fabs( x );
	if( (ax > 0.0)  && ( ax <=  2.0 ) ) {
		x2 = ax / 2.0;
		x2 = x2 * x2;
		sum = k0a[6];
		for( int i = 5; i >= 0; i--) sum = sum * x2 + k0a[i];
		sum = -log(ax / 2.0) * bessi0( x ) + sum;
	} else if( ax > 2.0 ) {
		x2 = 2.0/ax;
		sum = k0b[6];
		for( int i=5; i>=0; i--) sum = sum*x2 + k0b[i];
		sum = exp( -ax ) * sum / sqrt( ax );
	} else sum = 1.0e20;
	return ( sum );
}
	inline	double		bessi0( double x ){
 	double ax, sum, t;
 	double i0a[] = { 1.0, 3.5156229, 3.0899424, 1.2067492,
		0.2659732, 0.0360768, 0.0045813 };
 	double i0b[] = { 0.39894228, 0.01328592, 0.00225319,
 		-0.00157565, 0.00916281, -0.02057706, 0.02635537,
 		-0.01647633, 0.00392377};

	ax = fabs( x );
	if( ax <= 3.75 ) {
		t = x / 3.75;
		t = t * t;
		sum = i0a[6];
		for( int  i = 5; i >= 0; i--) sum = sum * t + i0a[i]; 
	} else {
		t = 3.75 / ax;
		sum = i0b[8];
		for( int i = 7; i >= 0; i--) sum = sum * t + i0b[i];
		sum = exp( ax ) * sum / sqrt( ax );
	}
	return( sum );
}
	inline	double		calculateProjectedPotential(int numberAtom, double r) {
		double sumf = 0, sums = 0;
		double dR1 = 6.2831853071796 * r; // 2 * PI * r
		for(int k = 0; k < 3; k++) {
			int Offs = (numberAtom) * 12 + k * 2;
			sumf += FParams[Offs + 0] * bessk0(dR1 * sqrt(FParams[Offs + 1]));
		}
				
		sumf *= 300.73079394295; // 4 * PI * PI *a0 * e
		for(int k = 0; k < 3; k++) {
			int Offs = (numberAtom) * 12 + k * 2;
			sums += (FParams[Offs + 6] / FParams[Offs + 7]) * exp(-(6.2831853071796 * r * r) / FParams[Offs + 7]);
		}
		
		sums *= 150.36539697148; // 2 * PI * PI * a0 * e
		return (sumf + sums);
	}
	inline	double		calculatePotential(int numberAtom, double r) {
		double sumf = 0, sums = 0;
		double dR1 = 6.2831853071796 * r; // 2 * PI * r
		for(int k = 0; k < 3; k++) {
			int Offs = (numberAtom) * 12 + k * 2;
			sumf += FParams[Offs + 0] / r * exp(- dR1 * sqrt(FParams[Offs + 1]));
		}				
		sumf *= 150.365396971475; // 4 * PI * PI *a0 * e
		
		for(int k = 0; k < 3; k++) {
			int Offs = (numberAtom) * 12 + k * 2;
			sums += FParams[Offs + 6] * pow(FParams[Offs + 7], -3.0 / 2.0) * exp(-(6.2831853071796 * r * r) / FParams[Offs + 7]);
		}
		sums *= 266.5157269050303; // 2 * PI * PI * a0 * e
		return (sumf + sums);
	}
};