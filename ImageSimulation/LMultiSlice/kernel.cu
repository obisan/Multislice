#include "stdafx.h"
#include "kernel.cuh"

__global__ void phaseObject(double *potential, cusp::complex<double>* wave, unsigned int nx, unsigned int ny, double sigma) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE = gridDim.x * blockDim.x;

	/// t(x, y) = exp(sigma * potential(x, y))		
	double fi_re = cos(sigma * potential[ LINESIZE * iy + ix ] / 1000.0); // k - eV
	double fi_im = sin(sigma * potential[ LINESIZE * iy + ix ] / 1000.0);

	////////////////////////////////////////////
	cusp::complex<double> fi(fi_re, fi_im);
	
	/// [ t(x, y) * phi(x, y) ]
	wave[LINESIZE * iy + ix] = fi * wave[LINESIZE * iy + ix];
}

__global__ void nulling(cusp::complex<double> *pfftw) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	pfftw[ix].x = pfftw[ix].y = 0.0;
}

__global__ void	normalize(cusp::complex<double> *pfftw, unsigned int n) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;

	if(ix < n * n) {
		pfftw[ix].x = pfftw[ix].x / n;
		pfftw[ix].y = pfftw[ix].y / n;
	}
}

__global__ void	rearrangement(cusp::complex<double> *pfftw) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE2 = gridDim.x * blockDim.x;
	const int LINESIZE = 2 * LINESIZE2;

	// 4 - 2
	swap(pfftw[iy * LINESIZE + ix].x, pfftw[(iy + LINESIZE2) * LINESIZE + LINESIZE2 + ix].x);
	swap(pfftw[iy * LINESIZE + ix].y, pfftw[(iy + LINESIZE2) * LINESIZE + LINESIZE2 + ix].y);

	// 1 - 3
	swap(pfftw[((LINESIZE2 - 1 - iy) + LINESIZE2) * LINESIZE + ix].x, pfftw[(LINESIZE2 - 1 - iy) * LINESIZE + LINESIZE2 + ix].x);
	swap(pfftw[((LINESIZE2 - 1 - iy) + LINESIZE2) * LINESIZE + ix].y, pfftw[(LINESIZE2 - 1 - iy) * LINESIZE + LINESIZE2 + ix].y);
}	

__global__ void objectLens(cusp::complex<double> *wave, double lambda, double Cs, double aperture, double defocus, double imageSizeAngstrems) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE = gridDim.x * blockDim.x;
	
	double u1 = fabs(LINESIZE / 2.0 - iy) / imageSizeAngstrems;
	double u2 = fabs(LINESIZE / 2.0 - ix) / imageSizeAngstrems;
	double k = u1 * u1 + u2 * u2;

	double alpha = getAlpha(k, lambda, Cs, defocus);
	double Es =  getEs(k, lambda, Cs, aperture, defocus);
	cusp::complex<double> w1(Es * cos(alpha), Es * sin(alpha));
	cusp::complex<double> w2(wave[iy * LINESIZE + ix].x, wave[iy * LINESIZE + ix].y);
	wave[iy * LINESIZE + ix].x = (w1 * w2).real();
	wave[iy * LINESIZE + ix].y = (w1 * w2).imag();
}

__global__ void propagate(cusp::complex<double> *wave_in, cusp::complex<double> *wave_out, double lambda, double dZ, double imageSizeAngstrems) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE = gridDim.x * blockDim.x;
	
	double u1 = fabs(LINESIZE / 2.0 - iy) / imageSizeAngstrems;
	double u2 = fabs(LINESIZE / 2.0 - ix) / imageSizeAngstrems;
	double k = u1 * u1 + u2 * u2;
	
	//cusp::complex<double> w1( 1.0 / (lambda * dZ) * sin(M_PI / (lambda * dZ) * k), - 1.0 / (lambda * dZ) * cos(M_PI / (lambda * dZ) * k)); // p-mini after sanya
	cusp::complex<double> w1( cos(M_PI * lambda * k * k * dZ), sin(M_PI * lambda * k * k * dZ)); // p-big
	wave_out[iy * LINESIZE + ix] = w1 * wave_in[iy * LINESIZE + ix];
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