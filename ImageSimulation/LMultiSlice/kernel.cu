#include "stdafx.h"
#include "kernel.cuh"

__global__ void calculateProjectedPotential(int *atomId, float (*xyz)[3], unsigned int nAtoms, double a, double b, double c, double dx, double dy, double dz, double *image, unsigned int nChannels, unsigned int nx, unsigned int ny, unsigned int nz, double r, double dk) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int iz = blockDim.z * blockIdx.z + threadIdx.z;
	const int lineSize = (gridDim.x * blockDim.x);
	const int slideSize = (gridDim.x * blockDim.x) * (gridDim.y * blockDim.y);
	
	int l;
	for(l = 0; l < nAtoms; l++) {
		double dX = fabs(xyz[l][0] * a - (ix * dx));
		double dY = fabs(xyz[l][1] * b - (iy * dy));
		double dZ = fabs(xyz[l][2] * b - (iz * dz));
		int m = atomId[l];

		dX = ( dX >= a / 2.0 ) ? dX - a : dX;
		dY = ( dY >= b / 2.0 ) ? dY - b : dY;
	
		double dR = sqrt(dX * dX + dY * dY);
		
		if(dZ > dz) continue;
		if(dR > r) continue;

		dR *= dk;
		dR = ( dR < 1.0e-10 ) ? 1.0e-10 : dR;
				
		image[ nChannels * (slideSize * iz + lineSize * iy + ix) + 0 ] += calculateProjectedPotential(m, dR);
		image[ nChannels * (slideSize * iz + lineSize * iy + ix) + 1 ] = 0;
	}
	
}


__device__ double calculateProjectedPotential(int numberAtom, double r) {
	double sumf = 0, sums = 0;
 	double dR1 = 6.2831853071796 * r; // 2 * PI * r
 	for(int k = 0; k < 3; k++) {
 		int Offs = (numberAtom) * 12 + k * 2;
 		sumf += FParamsDevice[Offs + 0] * bessk0(dR1 * sqrt(FParamsDevice[Offs + 1]));
 	}
 				
 	sumf *= 300.73079394295; // 4 * PI * PI *a0 * e
 	for(int k = 0; k < 3; k++) {
 		int Offs = (numberAtom) * 12 + k * 2;
 		sums += (FParamsDevice[Offs + 6] / FParamsDevice[Offs + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[Offs + 7]);
 	}
 		
 	sums *= 150.36539697148; // 2 * PI * PI * a0 * e
 	
	return (sumf + sums);
}

__device__ double	bessk0( double x ) {
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

__device__ double	bessi0( double x ) {
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