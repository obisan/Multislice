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
	fftw_complex *pfftw_in  = nullptr; 
 	fftw_complex *pfftw_out = nullptr; 
 	cudaMallocManaged(&(pfftw_in),  nx * ny * sizeof(fftw_complex));
 	cudaMallocManaged(&(pfftw_out), nx * ny * sizeof(fftw_complex));

	for(size_t i = 0; i < nx * ny; i++) {
		pfftw_in[i][0] = 1.0;	
		pfftw_in[i][1] = 0.0;
	}
	
	double *potential = modelPotential->potential;
	double dz = this->modelPotential->getModel()->getC() / nx;
	for(size_t kz = 0; kz < nz; kz++) {	
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// t(x, y) = exp(sigma * potential(x, y))
		/// [ t(x, y) * phi(x, y) ]
		////////////////////////////////////////////////////////////////////////////////////////////////////////		
		const unsigned int MAX_THREADS_PHASE_OBJECT = 16;
		dim3 threads_phase(MAX_THREADS_PHASE_OBJECT, MAX_THREADS_PHASE_OBJECT, 1);							// размер квадрата
		dim3 grid_phase( (int) nx / MAX_THREADS_PHASE_OBJECT, (int) ny / MAX_THREADS_PHASE_OBJECT, 1 );		// сколько квадратов нужно чтобы покрыть все изображение

		phaseObject<<<grid_phase, threads_phase>>>(potential + nx * ny * kz, (cusp::complex<double>*) pfftw_in, nx, ny, microscope->getSigma());
		cudaThreadSynchronize();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		///// PHI(k) = FT [ phi(x, y) ]
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		fftw_plan fftw_forward = fftw_plan_dft_2d( (int) nx, (int) ny, pfftw_in, pfftw_out, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_forward);
		fftw_destroy_plan(fftw_forward);

		const unsigned int MAX_THREADS_NORMALIZE = 1024;
		dim3 threads_normalize(MAX_THREADS_NORMALIZE, 1, 1);
		dim3 grid_normalize( (int) nx * ny / MAX_THREADS_NORMALIZE, 1, 1 );

		normalize<<<grid_normalize, threads_normalize>>>((cusp::complex<double>*) pfftw_out, nx);
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////
		/// Rearrangement 
		///	4 3  to 2 1 
		/// 1 2     3 4
		/////////////////////////////////////////////////////////////////////////////////////////////////
		const unsigned int MAX_THREADS_REARRANGEMENT = 16;
		dim3 threads_rearrangement(MAX_THREADS_REARRANGEMENT, MAX_THREADS_REARRANGEMENT, 1);									// размер квадрата
		dim3 grid_rearrangement( (int) nx / 2 / MAX_THREADS_REARRANGEMENT, (int) ny / 2 / MAX_THREADS_REARRANGEMENT, 1 );		// сколько квадратов нужно чтобы покрыть все изображение

		rearrangement<<<grid_rearrangement, threads_rearrangement>>>((cusp::complex<double>*) pfftw_out);
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// p(x, y, dZ) * [ t(x, y) * phi(x, y) ]
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		const unsigned int MAX_THREADS_PROPAGATION = 16;
		dim3 threads_propagation(MAX_THREADS_PROPAGATION, MAX_THREADS_PROPAGATION, 1);							// размер квадрата
		dim3 grid_propagation( (int) nx / MAX_THREADS_PROPAGATION, (int) ny / MAX_THREADS_PROPAGATION, 1 );		// сколько квадратов нужно чтобы покрыть все изображение
		
		propagate<<<grid_propagation, threads_propagation>>>((cusp::complex<double>*) pfftw_out, microscope->getLambda(), kz * dz, nx / dpa );
		cudaThreadSynchronize();

		// 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	 	/// H(k) * PHI(k)
	 	/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	 	const double imageSizeAngstrems = nx / dpa; // размер всего изображения в ангстремах
// 	 	const double lambda = microscope->getLambda();
// 	 	const double Cs = microscope->getCs();
// 	 	const double aperture = microscope->getAperture();
// 	 	const double defocus = microscope->getDefocus();
// 	 	//double Z = 0;
// 	 
// 	 	const unsigned int MAX_THREADS_OBJECT_LENS = 16;
// 	 	dim3 threads_object_lens(MAX_THREADS_OBJECT_LENS, MAX_THREADS_OBJECT_LENS, 1);							// размер квадрата
// 	 	dim3 grid_object_lens( (int) nx / MAX_THREADS_OBJECT_LENS, (int) ny / MAX_THREADS_OBJECT_LENS, 1 );		// сколько квадратов нужно чтобы покрыть все изображение
// 	 
// 	 	objectLens<<<grid_object_lens, threads_object_lens>>>((cusp::complex<double>*) pfftw_out, lambda, Cs, aperture, defocus, imageSizeAngstrems);
// 	 	cudaThreadSynchronize();
	 	

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// phi(x, y) = FT^(-1) { PHI(k) }
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		fftw_plan fftw_backward = fftw_plan_dft_2d((int) nx, (int) ny, pfftw_out, pfftw_in, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_backward);
		fftw_destroy_plan(fftw_backward);
		
		normalize<<<grid_normalize, threads_normalize>>>((cusp::complex<double>*) pfftw_in, nx);
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		Image::copyFFTtoImage<double>(result, pfftw_in, kz);

		/// !!!!!!!!!!!!!!!!!!!!
		//Z = 10;
	}



	cudaFree(pfftw_in);
	cudaFree(pfftw_out);
	return 0;
}