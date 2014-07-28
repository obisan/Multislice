#include "stdafx.h"
#include "kernel.cuh"
#include "cusp.cuh"

#define ATOMS_PER_PIXEL_FOR_KERNEL 1024
#define APP ATOMS_PER_PIXEL_FOR_KERNEL

__global__ void calculateProjectedPotential(int1 *atomId, float3 *atomXYZ, unsigned int nAtoms, double a, double b, double c, double dx, double dy, double dz, double *image, unsigned int nx, unsigned int ny, unsigned int nz, double r, double dk) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int iz = blockDim.z * blockIdx.z + threadIdx.z;
	const int LINESIZE = (gridDim.x * blockDim.x);
	const int SLIDESIZE = (gridDim.x * blockDim.x) * (gridDim.y * blockDim.y);
	
	int1 atom[APP];
	float1 atomdist[APP];
	
	int k = 0;
	for(int l = 0; l < nAtoms && k < APP; l++) {
		double dX = fabs(atomXYZ[l].x * a - (ix * dx));
		double dY = fabs(atomXYZ[l].y * b - (iy * dy));
		
		dX = ( dX >= a / 2.0 ) ? dX - a : dX;
		dY = ( dY >= b / 2.0 ) ? dY - b : dY;

		double dR = sqrt(dX * dX + dY * dY);
		
		if(dR > r) continue;

		atom[k].x = atomId[l].x;
		atomdist[k].x = ( dR < 1.0e-10 ) ? 1.0e-10 : dR;
		
		k++;
	}

	__syncthreads();

	for(int l = 0; l < k; l++) 
		image[ SLIDESIZE * iz + LINESIZE * iy + ix ] += calculateProjectedPotential(atom[l].x, atomdist[l].x);

}

__global__ void phaseObject(double *potential, fftw_complex* pfftw, unsigned int nx, unsigned int ny, double sigma) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE = gridDim.x * blockDim.x;

	/// T(x, y) = exp(sigma * p(x, y))
	double fi_re = cos(sigma * potential[ LINESIZE * iy + ix ] / 1000.0); // k - eV
	double fi_im = sin(sigma * potential[ LINESIZE * iy + ix ] / 1000.0);

	cusp::complex<double> fi(fi_re, fi_im);
	cusp::complex<double> fi2(pfftw[LINESIZE * iy + ix][0], pfftw[LINESIZE * iy + ix][1]);

	/// [ T(x, y) * phi(x, y) ]
	pfftw[LINESIZE * iy + ix][0] = (fi * fi2).real();
	pfftw[LINESIZE * iy + ix][1] = (fi * fi2).imag();
}

__global__ void	normalize(fftw_complex *pfftw, unsigned int n) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;

	if(ix < n * n) {
		pfftw[ix][0] = pfftw[ix][0] / n;
		pfftw[ix][1] = pfftw[ix][1] / n;
	}
}

__global__ void	rearrangement(fftw_complex *pfftw) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE2 = gridDim.x * blockDim.x;
	const int LINESIZE = 2 * LINESIZE2;

	// 4 - 2
	swap(pfftw[iy * LINESIZE + ix][0], pfftw[(iy + LINESIZE2) * LINESIZE + LINESIZE2 + ix][0]);
	swap(pfftw[iy * LINESIZE + ix][1], pfftw[(iy + LINESIZE2) * LINESIZE + LINESIZE2 + ix][1]);

	// 1 - 3
	swap(pfftw[((LINESIZE2 - 1 - iy) + LINESIZE2) * LINESIZE + ix][0], pfftw[(LINESIZE2 - 1 - iy) * LINESIZE + LINESIZE2 + ix][0]);
	swap(pfftw[((LINESIZE2 - 1 - iy) + LINESIZE2) * LINESIZE + ix][1], pfftw[(LINESIZE2 - 1 - iy) * LINESIZE + LINESIZE2 + ix][1]);
}	

__global__ void objectLens(fftw_complex *pfftw, double lambda, double Cs, double aperture, double defocus, double imageSizeAngstrems) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE = gridDim.x * blockDim.x;

	double u1 = fabs(LINESIZE / 2.0 - iy) / imageSizeAngstrems;
	double u2 = fabs(LINESIZE / 2.0 - ix) / imageSizeAngstrems;
	double k = u1 * u1 + u2 * u2;
	double alpha = getAlpha(k, lambda, Cs, defocus);
	double Es =  getEs(k, lambda, Cs, aperture, defocus);
	cusp::complex<double> w1(Es * cos(alpha), Es * sin(alpha));
	cusp::complex<double> w2(pfftw[iy * LINESIZE + ix][0], pfftw[iy * LINESIZE + ix][1]);
	pfftw[iy * LINESIZE + ix][0] = (w1 * w2).real();
	pfftw[iy * LINESIZE + ix][1] = (w1 * w2).imag();

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

__device__ void		swap(double& a, double& b) {
		double buffer = a;
		a = b;
		b = buffer;
}

__device__ double	getAlpha(double k, double lambda, double Cs, double defocus) {
	return M_PI * lambda * k * k * (0.5 * Cs * lambda * lambda * k * k - defocus * 10);
}

__device__ double	getEs(double k, double lambda, double Cs, double aperture, double defocus) {
	return exp(- pow(M_PI * aperture / (lambda * 1000), 2) * pow(Cs * pow(lambda, 3) * k * k * k + defocus * lambda * k, 2));
}