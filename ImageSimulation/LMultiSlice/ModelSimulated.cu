#include "stdafx.h"
#include "ModelSimulated.h"
#include "kernel.cuh"

ModelSimulated::ModelSimulated(void) {
	
}

ModelSimulated::~ModelSimulated(void) {
	if(this->modelPotential != nullptr) this->modelPotential = nullptr;
}

ModelSimulated::ModelSimulated(ModelPotential* modelPotential, size_t nx, size_t ny, size_t nz, double dpa) {
	this->modelPotential = modelPotential;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	this->dpa = dpa;
}

int ModelSimulated::imageCalculation(Image *result, Microscope *microscope) {
	const double keV = microscope->getKeV();
	double *potential = modelPotential->potential;
	
	fftw_complex *pfftw_in  = nullptr; 
 	fftw_complex *pfftw_out = nullptr; 
 	cudaMallocManaged(&(pfftw_in),  nx * ny * sizeof(fftw_complex));
 	cudaMallocManaged(&(pfftw_out), nx * ny * sizeof(fftw_complex));

	for(size_t i = 0; i < nx * ny; i++) {
		pfftw_in[i][0] = 1.0;	
		pfftw_in[i][1] = 0.0;
	}
	
	for(size_t kz = 0; kz < nz; kz++) {	
		for(size_t iy = 0; iy < ny; iy++) {
			for(size_t jx = 0; jx < nx; jx++) {
				////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// T(x, y) = exp(sigma * p(x, y))
				////////////////////////////////////////////////////////////////////////////////////////////////////////
				double fi_re = cos(microscope->getSigma() * potential[ nx * ny * kz + nx * iy + jx ] / 1000.0); // k - eV
				double fi_im = sin(microscope->getSigma() * potential[ nx * ny * kz + nx * iy + jx ] / 1000.0);
				
				std::complex<double> fi(fi_re, fi_im);
				std::complex<double> fi2(pfftw_in[nx * iy + jx][0], pfftw_in[nx * iy + jx][1]);

				/////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// [ T(x, y) * phi(x, y) ]
				/////////////////////////////////////////////////////////////////////////////////////////////////////////
				pfftw_in[nx * iy + jx][0]	= (fi * fi2).real();
				pfftw_in[nx * iy + jx][1]	= (fi * fi2).imag();
			}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		///// PHI(k) = FT [ phi(x, y) ]
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		fftw_plan fftw_forward = fftw_plan_dft_2d( (int) nx, (int) ny, pfftw_in, pfftw_out, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_forward);
		fftw_destroy_plan(fftw_forward);

		for(size_t i = 0; i < nx * ny; i++) {
			pfftw_out[i][0] /= (double) nx;
			pfftw_out[i][1] /= (double) nx;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////
		/// Rearrangement 
		///	4 3  to 2 1 
		/// 1 2     3 4
		/////////////////////////////////////////////////////////////////////////////////////////////////
		const size_t nx2 = nx / 2;
		const size_t ny2 = ny / 2;
		for(size_t iy = 0; iy < ny2; iy++) {
			for(size_t jx = 0; jx < nx2; jx++) {
				// 4 - 2
				swap<double>(pfftw_out[iy * nx + jx][0], pfftw_out[(iy + ny2) * nx + nx2 + jx][0]);
				swap<double>(pfftw_out[iy * nx + jx][1], pfftw_out[(iy + ny2) * nx + nx2 + jx][1]);

				// 1 - 3
				swap<double>(pfftw_out[((ny2 - 1 - iy) + ny2) * nx + jx][0], pfftw_out[(ny2 - 1 - iy) * nx + nx2 + jx][0]);
				swap<double>(pfftw_out[((ny2 - 1 - iy) + ny2) * nx + jx][1], pfftw_out[(ny2 - 1 - iy) * nx + nx2 + jx][1]);
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// H(k) * PHI(k)
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		const double dImgSize = nx / dpa; // размер всего изображения в ангстремах
		//double Z = 0;
		
		for(size_t iy = 0; iy < ny; iy++) {
			for(size_t jx = 0; jx < nx; jx++) {
				///////////////////////////////////////////////////////////////////////////////////
				double u1 = fabs(ny / 2.0 - iy) / dImgSize;
				double u2 = fabs(nx / 2.0 - jx) / dImgSize;
				double k = u1 * u1 + u2 * u2;
				double alpha = microscope->alpha(k);
				double Es = microscope->Es(k);
				std::complex<double> w1(Es * cos(alpha), Es * sin(alpha));
				std::complex<double> w2(pfftw_out[iy * nx + jx][0], pfftw_out[iy * nx + jx][1]);
				pfftw_out[iy * nx + jx][0] = (w1 * w2).real();
				pfftw_out[iy * nx + jx][1] = (w1 * w2).imag();
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// phi(x, y) = FT^(-1) { PHI(k) }
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		fftw_plan fftw_backward = fftw_plan_dft_2d((int) nx, (int) ny, pfftw_out, pfftw_in, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_backward);
		fftw_destroy_plan(fftw_backward);
		
		for(size_t i = 0; i < nx * ny; i++) {
			pfftw_in[i][0] /= (double) nx;
			pfftw_in[i][1] /= (double) nx;
		}
		
		/// !!!!!!!!!!!!!!!!!!!!
		//Z = 10;
	}

	Image::copyFFTtoImage<double>(result, pfftw_in, 0);

	cudaFree(pfftw_in);
	cudaFree(pfftw_out);
	return 0;
}