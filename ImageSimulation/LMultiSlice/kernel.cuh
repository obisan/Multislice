#pragma once

struct __align__(16) atom {
	int id;
	int num;
	float x;
	float y;
};

//__global__ void calculatePotentialGridGPU(int nAtoms, double a, double b, double c, double dx, double dy, double *potential, double r, double dk);
__global__ void calculatePotentialGridGPU(double *potential, int *bin_num, atom *bin_d, double a, double b, double c, double dx, double dy, double binx, double biny, double bindimx, double bindimy, double radius);
__global__ void phaseObject(double *potential, cusp::complex<double> *wave, unsigned int nx, unsigned int ny, double sigma);
__global__ void nulling(cusp::complex<double> *pfftw);
__global__ void	normalize(cusp::complex<double> *wave, unsigned int n);
__global__ void rearrangement(cusp::complex<double> *wave);
__global__ void propagate(cusp::complex<double> *wave_0, cusp::complex<double> *wave_1, double lambda, double dZ, double imageSizeAngstrems);
__global__ void objectLens(cusp::complex<double> *pfftw, double lambda, double Cs, double aperture, double defocus, double imageSizeAngstrems);

__device__ double	calculateProjectedPotential(int numberAtom, double r); 
__device__ double	bessk0( double x );
__device__ double	bessi0( double x );
__device__ void		swap(double& a, double& b);
__device__ double	getAlpha(double k, double lambda, double Cs, double defocus);
__device__ double	getEs(double k, double lambda, double Cs, double aperture, double defocus);