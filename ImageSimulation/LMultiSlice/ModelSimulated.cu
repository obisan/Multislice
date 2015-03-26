#include "stdafx.h"
#include "ModelSimulated.h"
#include "kernel.cuh"

ModelSimulated::ModelSimulated(void) {
	
}

ModelSimulated::~ModelSimulated(void) {
	if(this->model != nullptr) this->model = nullptr;
}

ModelSimulated::ModelSimulated(const char* potentialDirectory, AModel::Model* model, int nx, int ny, int countSlices, double dpa) {
	this->nx = nx;
	this->ny = ny;
	this->countSlices = countSlices;
	this->dpa = dpa;

	this->model = model;

	for(size_t i = 0; i < countSlices; i++) {
		char slicename[256];
		sprintf(slicename, "%s/slice%003u.slc", potentialDirectory, i);
		std::string slice(slicename);
		slices.push_back(slicename);
	}
}

int ModelSimulated::imageCalculation(Image *result, Microscope *microscope) {
	fftw_complex *wave_in; 
 	fftw_complex *wave_out; 
	fftw_complex *wave_next;
 	cudaMallocManaged(&(wave_in),	nx * ny * sizeof(fftw_complex));
	CUERR
 	cudaMallocManaged(&(wave_out),	nx * ny * sizeof(fftw_complex));
	CUERR
	cudaMallocManaged(&(wave_next), nx * ny * sizeof(fftw_complex));
	CUERR

	double *potentialSlice;
	cudaMallocManaged(&(potentialSlice), nx * ny * sizeof(double));
	memset(potentialSlice, 0, nx * ny * sizeof(double));
	CUERR


	for(size_t i = 0; i < nx * ny; i++) {
		wave_in[i][0] = 1.0;	
		wave_in[i][1] = 0.0;
	}
		
	cudaEvent_t start,stop;
	float time = 0.0f;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
	

	double dz = this->model->getC() / countSlices;
	for(size_t kz = 0; kz < countSlices; kz++) {	
		FILE *pFile;
		pFile = fopen(slices[kz].c_str(), "rb");
		fread(potentialSlice, sizeof(double), nx * ny, pFile);
		fclose(pFile);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// t(x, y) = exp(sigma * potential(x, y))
		/// [ t(x, y) * phi(x, y) ]
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		const unsigned int MAX_THREADS_PHASE_OBJECT = 16;
		dim3 threads_phase(MAX_THREADS_PHASE_OBJECT, MAX_THREADS_PHASE_OBJECT, 1);							// ðàçìåð êâàäðàòà
		dim3 grid_phase( (int) nx / MAX_THREADS_PHASE_OBJECT, (int) ny / MAX_THREADS_PHASE_OBJECT, 1 );		// ñêîëüêî êâàäðàòîâ íóæíî ÷òîáû ïîêðûòü âñå èçîáðàæåíèå

		phaseObject<<<grid_phase, threads_phase>>>(potentialSlice, (cusp::complex<double>*) wave_in, nx, ny, microscope->getSigma());
		cudaThreadSynchronize();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		///// PHI(k) = FFT [ t_n(x, y) * phi_n(x, y) ]
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		fftw_plan fftw_forward = fftw_plan_dft_2d( (int) nx, (int) ny, wave_in, wave_out, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_forward);
		fftw_destroy_plan(fftw_forward);

		const unsigned int MAX_THREADS_NORMALIZE = 1024;
		dim3 threads_normalize(MAX_THREADS_NORMALIZE, 1, 1);
		dim3 grid_normalize( (int) nx * ny / MAX_THREADS_NORMALIZE, 1, 1 );

		normalize<<<grid_normalize, threads_normalize>>>((cusp::complex<double>*) wave_out, nx);
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////
		/// Rearrangement 
		///	4 3  to 2 1 
		/// 1 2     3 4
		/////////////////////////////////////////////////////////////////////////////////////////////////
		const unsigned int MAX_THREADS_REARRANGEMENT = 16;
		dim3 threads_rearrangement(MAX_THREADS_REARRANGEMENT, MAX_THREADS_REARRANGEMENT, 1);									// ðàçìåð êâàäðàòà
		dim3 grid_rearrangement( (int) nx / 2 / MAX_THREADS_REARRANGEMENT, (int) ny / 2 / MAX_THREADS_REARRANGEMENT, 1 );		// ñêîëüêî êâàäðàòîâ íóæíî ÷òîáû ïîêðûòü âñå èçîáðàæåíèå

		rearrangement<<<grid_rearrangement, threads_rearrangement>>>((cusp::complex<double>*) wave_out);
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// p_n(x, y, dZ) * FFT [ t_n(x, y) * phi_n(x, y) ]
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		const unsigned int MAX_THREADS_PROPAGATION = 16;
		dim3 threads_propagation(MAX_THREADS_PROPAGATION, MAX_THREADS_PROPAGATION, 1);							// ðàçìåð êâàäðàòà
		dim3 grid_propagation( (int) nx / MAX_THREADS_PROPAGATION, (int) ny / MAX_THREADS_PROPAGATION, 1 );		// ñêîëüêî êâàäðàòîâ íóæíî ÷òîáû ïîêðûòü âñå èçîáðàæåíèå
		
		propagate<<<grid_propagation, threads_propagation>>>((cusp::complex<double>*) wave_out, (cusp::complex<double>*) wave_next, microscope->getLambda(), (kz + 1) * dz, nx / dpa );
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// phi_n+1(x, y) = FFT^(-1) { p_n(x, y, dZ) * [ t_n(x, y) * phi_n(x, y) ] }
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		fftw_plan fftw_backward_propagate = fftw_plan_dft_2d((int) nx, (int) ny, wave_next, wave_in, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_backward_propagate);
		fftw_destroy_plan(fftw_backward_propagate);
		
		normalize<<<grid_normalize, threads_normalize>>>((cusp::complex<double>*) wave_in, nx);
		cudaThreadSynchronize();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Image::copyFFTtoImage<double>(result, wave_in + nx * ny * kz, kz);

		/// !!!!!!!!!!!!!!!!!!!!
		//Z = 10;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// PHI(k) = FFT [ phi_exit(x, y) ]
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	fftw_plan fftw_forward = fftw_plan_dft_2d( (int) nx, (int) ny, wave_in, wave_out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fftw_forward);
	fftw_destroy_plan(fftw_forward);

	const unsigned int MAX_THREADS_NORMALIZE = 1024;
	dim3 threads_normalize(MAX_THREADS_NORMALIZE, 1, 1);
	dim3 grid_normalize( (int) nx * ny / MAX_THREADS_NORMALIZE, 1, 1 );

	normalize<<<grid_normalize, threads_normalize>>>((cusp::complex<double>*) wave_out, nx);
	cudaThreadSynchronize();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/// Rearrangement 
	///	4 3  to 2 1 
	/// 1 2     3 4
	/////////////////////////////////////////////////////////////////////////////////////////////////
	const unsigned int MAX_THREADS_REARRANGEMENT = 16;
	dim3 threads_rearrangement(MAX_THREADS_REARRANGEMENT, MAX_THREADS_REARRANGEMENT, 1);									// размер квадрата
	dim3 grid_rearrangement( (int) nx / 2 / MAX_THREADS_REARRANGEMENT, (int) ny / 2 / MAX_THREADS_REARRANGEMENT, 1 );		// сколько квадратов нужно чтобы покрыть все изображение

	rearrangement<<<grid_rearrangement, threads_rearrangement>>>((cusp::complex<double>*) wave_out);
	cudaThreadSynchronize();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/// H(k) * PHI(k)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	const double imageSizeAngstrems = nx / dpa; // ðàçìåð âñåãî èçîáðàæåíèÿ â àíãñòðåìàõ
	const double lambda = microscope->getLambda();
	const double Cs = microscope->getCs();
	const double aperture = microscope->getAperture();
	const double defocus = microscope->getDefocus();
	//double Z = 0;
	 
	const unsigned int MAX_THREADS_OBJECT_LENS = 16;
	dim3 threads_object_lens(MAX_THREADS_OBJECT_LENS, MAX_THREADS_OBJECT_LENS, 1);							// ðàçìåð êâàäðàòà
	dim3 grid_object_lens( (int) nx / MAX_THREADS_OBJECT_LENS, (int) ny / MAX_THREADS_OBJECT_LENS, 1 );		// ñêîëüêî êâàäðàòîâ íóæíî ÷òîáû ïîêðûòü âñå èçîáðàæåíèå
	 
	objectLens<<<grid_object_lens, threads_object_lens>>>((cusp::complex<double>*) wave_out, lambda, Cs, aperture, defocus, imageSizeAngstrems);
	cudaThreadSynchronize();
 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// phi(x, y) = FFT^(-1) { PHI(k) }
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	fftw_plan fftw_backward = fftw_plan_dft_2d((int) nx, (int) ny, wave_out, wave_in, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(fftw_backward);
	fftw_destroy_plan(fftw_backward);
		
	normalize<<<grid_normalize, threads_normalize>>>((cusp::complex<double>*) wave_in, nx);
	cudaThreadSynchronize();

	Image::copyFFTtoImage<double>(result, wave_in, 0);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);
	
	std::cout << std::endl << "Kernel time calculating electron image image: " << time << "ms." << std::endl << std::endl;
	

	cudaFree(wave_in);
	cudaFree(wave_out);
	cudaFree(wave_next);
	


	return 0;
}