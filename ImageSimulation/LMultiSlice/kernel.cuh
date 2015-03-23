#pragma once

__global__ void phaseObject(double *potential, cusp::complex<double> *wave, unsigned int nx, unsigned int ny, double sigma);
__global__ void nulling(cusp::complex<double> *pfftw);
__global__ void	normalize(cusp::complex<double> *wave, unsigned int n);
__global__ void rearrangement(cusp::complex<double> *wave);
__global__ void propagate(cusp::complex<double> *wave_0, cusp::complex<double> *wave_1, double lambda, double dZ, double imageSizeAngstrems);
__global__ void objectLens(cusp::complex<double> *pfftw, double lambda, double Cs, double aperture, double defocus, double imageSizeAngstrems);

__device__ double	calculateProjectedPotential(int numberAtom, double r); 
__device__ void		swap(double& a, double& b);
__device__ double	getAlpha(double k, double lambda, double Cs, double defocus);
__device__ double	getEs(double k, double lambda, double Cs, double aperture, double defocus);