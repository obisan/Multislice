#include "stdafx.h"
#include "ModelPotential.h"
#include "kernel.cuh"

ModelPotential::ModelPotential(void) {

}

ModelPotential::ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double dpa, double radius) {
	this->model = model;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	this->dpa = dpa;
	this->radius = radius;
	
	this->potential = (double*) malloc(nx * ny * nz * sizeof(double));
	memset(this->potential, 0, nx * ny * nz * sizeof(double));
}

ModelPotential::~ModelPotential(void) {
	if(this->model != nullptr) { model = nullptr; }
	if(this->potential != nullptr) { free(this->potential); }
}

int ModelPotential::calculatePotentialGrid() {
	const size_t nAtoms = model->getNumberAtoms();
	const double dk = 1.0 / dpa;
	const double a = model->getA();
	const double b = model->getB();
	const double c = model->getC();
	const double dx = a / this->nx;
	const double dy = b / this->ny;
	const double dz = c / this->nz;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	checkCudaErrors( cudaMemcpyToSymbol(radius_d, &radius, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(dx_d, &dx, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(dy_d, &dy, sizeof(double)) );
	
	double a_h = model->getA();
	double b_h = model->getB();
	double c_h = model->getC();
	checkCudaErrors( cudaMemcpyToSymbol(a_d, &a_h, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(b_d, &b_h, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(c_d, &c_h, sizeof(double)) );

	double bindimx = 15; // angstrem
	double bindimy = 15; // angstrem
	checkCudaErrors( cudaMemcpyToSymbol(bindimx_d, &bindimx, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(bindimy_d, &bindimy, sizeof(double)) );

	int	binx = ceil(a / bindimx);
	int	biny = ceil(b / bindimy);
	checkCudaErrors( cudaMemcpyToSymbol(binx_d, &binx, sizeof(int)) );
	checkCudaErrors( cudaMemcpyToSymbol(biny_d, &biny, sizeof(int)) );

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	double *potentialSlice;
	checkCudaErrors( cudaMallocManaged(&(potentialSlice), nx * ny * sizeof(double)));
	memset(potentialSlice, 0, nx * ny * sizeof(double));
	CUERR
			
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	dim3 threads(BLOCKSIZEX, BLOCKSIZEY, 1);		
	dim3 grid(this->nx / BLOCKSIZEX, this->ny / BLOCKSIZEY, 1 );

	AModel::Cortege *pAtoms = model->getTableCell();
	std::sort(pAtoms, pAtoms + nAtoms);

	std::vector<atom> slice;
	std::vector<atom> *bins = new std::vector<atom> [biny * binx];
	
	int *bins_num;
	cudaMallocManaged(&(bins_num), binx * biny * sizeof(int));
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	float time_kernel = 0.0f;
	float time_total = 0.0f;
	cudaEvent_t start_total,stop_total;
	cudaEventCreate(&start_total);
	cudaEventCreate(&stop_total);
	cudaEventRecord(start_total,0);

	for(size_t kz = 0; kz < nz; kz++) {
		for(size_t i = 0; i < nAtoms; i++) {
			if( kz * dz <= pAtoms[i].element.xsCoordinate.z * c && pAtoms[i].element.xsCoordinate.z * c <= (kz + 1) * dz ) {
				atom buff;
 				buff.id = i + 1;
				buff.num = model->getNumberByName(pAtoms[i].element.Atom) - 1;
				buff.x = pAtoms[i].element.xsCoordinate.x;
				buff.y = pAtoms[i].element.xsCoordinate.y;

				slice.push_back(buff);
			}
		}

		for(size_t iy = 0; iy < biny; iy++) {
			for(size_t jx = 0; jx < binx; jx++) {
				for(auto t : slice) {
					if( t.x * a >= jx * bindimx && t.x * a < (jx + 1) * bindimx )
						if( t.y * b >= iy * bindimy && t.y * b < (iy + 1) * bindimy )
							bins[binx * iy + jx].push_back(t);
				}
			}
		}

		int summ_atoms_in_bins = 0;
		for(size_t i = 0; i < binx * biny; i++) {
			summ_atoms_in_bins += bins[i].size();
		}

		atom *bins_d;
		cudaMallocManaged(&(bins_d), slice.size() * sizeof(atom));

		for(size_t i = 0, k = 0; i < binx * biny; i++) {
			bins_num[i] = bins[i].size();
			for(auto s : bins[i]) {
				bins_d[k] = s;
				k++;
			}
			bins[i].clear();
		}

		// make number to offset
		for(int i = 0; i < binx * biny - 1; i++) {
			bins_num[i+1] += bins_num[i];
		}
		bins_num[0] = 0;

		cudaEvent_t start,stop;
		float ctime = 0.0f;
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventRecord(start,0);

		calculatePotentialGridGPU<<<grid, threads>>>(potentialSlice, bins_num, bins_d);
		cudaThreadSynchronize();

		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&ctime, start, stop);
		time_kernel += ctime;

		std::cout << "slice: " << kz << std::endl << "calculated atoms: " << slice.size() << std::endl;

		cudaMemcpy(potential + nx * ny * kz, potentialSlice, nx * ny * sizeof(double), cudaMemcpyDeviceToHost);
		
		slice.clear();

		cudaFree(bins_d);
	}

	cudaEventRecord(stop_total,0);
	cudaEventSynchronize(stop_total);
	cudaEventElapsedTime(&time_total, start_total, stop_total);

	pAtoms = nullptr;
	
	std::cout << std::endl;
	std::cout << "Kernel time calculating potential grid: " << time_kernel << "ms." << std::endl;
	std::cout << "Total  time calculating potential grid: " << time_total << "ms." << std::endl << std::endl;
	
	return 0;
}

// __global__ void calculatePotentialGridGPU(double *potential, int *bin_num, atom *bin_d) {
// 	const int ix = __umul24(blockDim.x, blockIdx.x) + threadIdx.x;
// 	const int iy = __umul24(blockDim.y, blockIdx.y) + threadIdx.y;
// 	const int is = __umul24(blockDim.x, threadIdx.y) + threadIdx.x;
// 	const int LINESIZE = __umul24(gridDim.x, blockDim.x);
// 
// 	int i,j,kb;
// 	i = j = kb = 0;
// 
// 	int bins[32];
// 		
// 	double coordx = ix * dx_d;
// 	double coordy = iy * dy_d;
// 	
// 	double dRadiusX = (radius_d + 0.866 * bindimx_d);// 6.928 = 4 * sqrt(3) // 0.866 = sqrt(3)/2
// 	double dRadiusY = (radius_d + 0.866 * bindimy_d);// 6.928 = 4 * sqrt(3) // 0.866 = sqrt(3)/2
// 
// 	int numberofbinsX = ceil(dRadiusX / bindimx_d);
// 	int numberofbinsY = ceil(dRadiusY / bindimy_d);
// 
// 	int coordbinx = ceil(coordx / bindimx_d);
// 	int coordbiny = ceil(coordy / bindimy_d);
// 	
// 	for(i = 0; i < biny_d && kb < 32; i++) {
// 		double dY1 = fabs(coordy - (i + 0) * bindimy_d);
// 		double dY2 = fabs(coordy - (i + 1) * bindimy_d);
// 		
// 		dY1 = ( dY1 >= b_d / 2.0 ) ? fabs(dY1 - b_d) : dY1;
// 		dY2 = ( dY2 >= b_d / 2.0 ) ? fabs(dY2 - b_d) : dY2;
// 
// 		if(dY1 <= dRadiusY || dY2 <= dRadiusY) { 
// 			for(j = 0; j < binx_d && kb < 32; j++) {
// 				double dX1 = fabs(coordx - (j + 0) * bindimx_d);
// 				double dX2 = fabs(coordx - (j + 1) * bindimx_d);
// 			
// 				dX1 = ( dX1 >= a_d / 2.0 ) ? fabs(dX1 - a_d) : dX1;
// 				dX2 = ( dX2 >= a_d / 2.0 ) ? fabs(dX2 - a_d) : dX2;
// 
// 				if(dX1 <= dRadiusX || dX2 <= dRadiusX) { 
// 					bins[kb] = binx_d * i + j;
// 					kb++;
// 				}				
// 			}
// 		}
// 	}
// 
// 	__syncthreads();
// 
// 	__shared__ double imageval[BLOCKSIZEX*BLOCKSIZEY];
// 	imageval[is] = 0.0;
// 	
// 	for(i = 0; i < kb - 1; i++) {
// 		int n = bin_num[bins[i] + 1] - bin_num[bins[i]];
// 		int offset = bin_num[bins[i]];
// 		for(j = 0; j < n; j++) {
// 			double x = fabs(bin_d[offset + j].x * a_d - ix * dx_d);
// 			double y = fabs(bin_d[offset + j].y * b_d - iy * dy_d);
// 
// 			x = ( x >= a_d / 2.0 ) ? x - a_d : x;
// 			y = ( y >= b_d / 2.0 ) ? y - b_d : y;
// 
// 			double r = __dsqrt_rn(x * x + y * y);
// 
// 			imageval[is] += calculateProjectedPotential(bin_d[offset + j].num, r);
// 		}
// 	}
// 
// 	__syncthreads();
// 
// 	potential[ LINESIZE * iy + ix ] = imageval[blockDim.x * threadIdx.y + threadIdx.x]; 
// 	
// }

__global__ void calculatePotentialGridGPU(double *potential, int *bin_num, atom *bin_d) {
	const int ix = __umul24(blockDim.x, blockIdx.x) + threadIdx.x;
	const int iy = __umul24(blockDim.y, blockIdx.y) + threadIdx.y;
	const int is = __umul24(blockDim.x, threadIdx.y) + threadIdx.x;
	const int LINESIZE = __umul24(gridDim.x, blockDim.x);

	int i,j,kb;
	i = j = kb = 0;

	int bins[32];
		
	double coordx = ix * dx_d;
	double coordy = iy * dy_d;
	
	double dRadiusX = (radius_d + 0.866 * bindimx_d);// 6.928 = 4 * sqrt(3) // 0.866 = sqrt(3)/2
	double dRadiusY = (radius_d + 0.866 * bindimy_d);// 6.928 = 4 * sqrt(3) // 0.866 = sqrt(3)/2

	int numberofbinsX = ceil(dRadiusX / bindimx_d);
	int numberofbinsY = ceil(dRadiusY / bindimy_d);

	int coordbinx = ceil(coordx / bindimx_d);
	int coordbiny = ceil(coordy / bindimy_d);
	
	for(i = 0; i < biny_d && kb < 32; i++) {
		double dY1 = fabs(coordy - (i + 0) * bindimy_d);
		double dY2 = fabs(coordy - (i + 1) * bindimy_d);
		
		dY1 = ( dY1 >= b_d / 2.0 ) ? fabs(dY1 - b_d) : dY1;
		dY2 = ( dY2 >= b_d / 2.0 ) ? fabs(dY2 - b_d) : dY2;

		if(dY1 <= dRadiusY || dY2 <= dRadiusY) { 
			for(j = 0; j < binx_d && kb < 32; j++) {
				double dX1 = fabs(coordx - (j + 0) * bindimx_d);
				double dX2 = fabs(coordx - (j + 1) * bindimx_d);
			
				dX1 = ( dX1 >= a_d / 2.0 ) ? fabs(dX1 - a_d) : dX1;
				dX2 = ( dX2 >= a_d / 2.0 ) ? fabs(dX2 - a_d) : dX2;

				if(dX1 <= dRadiusX || dX2 <= dRadiusX) { 
					bins[kb] = binx_d * i + j;
					kb++;
				}				
			}
		}
	}

	__syncthreads();

	__shared__ double imageval[BLOCKSIZEX*BLOCKSIZEY];
	
	imageval[is] = 0.0;
	
	for(i = 0; i < kb - 1; i++) {
		int n = bin_num[bins[i] + 1] - bin_num[bins[i]];
		int offset = bin_num[bins[i]];
		for(j = 0; j < n; j++) {
			int numberAtom = bin_d[offset + j].num;
			double x = fabs(bin_d[offset + j].x * a_d - ix * dx_d);
			double y = fabs(bin_d[offset + j].y * b_d - iy * dy_d);

			x = ( x >= a_d / 2.0 ) ? x - a_d : x;
			y = ( y >= b_d / 2.0 ) ? y - b_d : y;

			double r = __dsqrt_rn(x * x + y * y); 			
			double dR1 = 6.2831853071796 * r; // 2 * PI * r;
		
			imageval[is] += ( 
				FParamsDevice[(numberAtom) * 12 + 0 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 1]))
				+ FParamsDevice[(numberAtom) * 12 + 1 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 1]))
				+ FParamsDevice[(numberAtom) * 12 + 2 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 1])) 
				) * 300.73079394295
				+ (
				(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7])
				+ (FParamsDevice[(numberAtom) * 12 + 1 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7])
				+ (FParamsDevice[(numberAtom) * 12 + 2 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7])
				) * 150.36539697148;

		}
	}

	__syncthreads();

	potential[ LINESIZE * iy + ix ] = imageval[is]; 
	
}


__device__ double	calculateProjectedPotential(int numberAtom, double r) {
	double sumf = 0.0;
	double sums = 0.0;
 	double dR1 = 6.2831853071796 * r; // 2 * PI * r;
		
	sumf += FParamsDevice[(numberAtom) * 12 + 0 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 1]));  
	sumf += FParamsDevice[(numberAtom) * 12 + 1 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 1]));  
	sumf += FParamsDevice[(numberAtom) * 12 + 2 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 1]));  
	sumf *= 300.73079394295; // 4 * PI * PI *a0 * e

	sums += (FParamsDevice[(numberAtom) * 12 + 0 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7]);
	sums += (FParamsDevice[(numberAtom) * 12 + 1 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7]);
	sums += (FParamsDevice[(numberAtom) * 12 + 2 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7]);
	sums *= 150.36539697148; // 2 * PI * PI * a0 * e

	return (sumf + sums);
}

__device__ double	bessk0( double ax ) {
	double x2;
	double sum;
		
	if( (ax > 0.0)  && ( ax <=  2.0 ) ) {
		x2 = __ddiv_rn(ax, 2.0);
		x2 = __dmul_rd(x2, x2);
		sum = __fma_rn(k0a[6], x2, k0a[5]);
		sum = __fma_rn(sum, x2, k0a[4]);
		sum = __fma_rn(sum, x2, k0a[3]);
		sum = __fma_rn(sum, x2, k0a[2]);
		sum = __fma_rn(sum, x2, k0a[1]);
		sum = __fma_rn(sum, x2, k0a[0]);

		sum = -log(ax / 2.0) * bessi0( ax ) + sum;
	} else if( ax > 2.0 ) {
		x2 = __ddiv_rn(2.0, ax);
		sum = __fma_rn(k0b[6], x2, k0b[5]);
		sum = __fma_rn(sum, x2, k0b[4]);
		sum = __fma_rn(sum, x2, k0b[3]);
		sum = __fma_rn(sum, x2, k0b[2]);
		sum = __fma_rn(sum, x2, k0b[1]);
		sum = __fma_rn(sum, x2, k0b[0]);

		sum = exp( -ax ) * sum / __dsqrt_rn(ax);
	} else sum = 1.0e20;
	return ( sum );
}

__device__ double	bessi0( double ax ) {
 	double sum;
	double t;
 	
	if( ax <= 3.75 ) {
		t = __ddiv_rn(ax, 3.75);
		t = __dmul_rd(t, t);
		sum = __fma_rn(i0a[6], t, i0a[5]);
		sum = __fma_rn(sum, t, i0a[4]);
		sum = __fma_rn(sum, t, i0a[3]);
		sum = __fma_rn(sum, t, i0a[2]);
		sum = __fma_rn(sum, t, i0a[1]);
		sum = __fma_rn(sum, t, i0a[0]);
	} else {
		t = __ddiv_rn(3.75, ax);
		sum = __fma_rn(i0b[8], t, i0a[7]);
		sum = __fma_rn(sum, t, i0a[6]);
		sum = __fma_rn(sum, t, i0a[5]);
		sum = __fma_rn(sum, t, i0a[4]);
		sum = __fma_rn(sum, t, i0a[3]);
		sum = __fma_rn(sum, t, i0a[2]);
		sum = __fma_rn(sum, t, i0a[1]);
		sum = __fma_rn(sum, t, i0a[0]);

		sum = exp( ax ) * sum / __dsqrt_rn( ax );
	}
	return( sum );
}

int	ModelPotential::savePotential(const char* filename) {
	Image *image = new Image(nx, ny, nz, sizeof(double), 1);
	char filenamept[256];
	strcpy(filenamept, filename);
	strcat(filenamept, "_pt");
	memcpy(image->imageData, this->potential, nx * ny * nz * sizeof(double));
	image->saveMRC(filenamept, model, nx, ny, nz, mrc_FLOAT);
	delete image;

	return 0;
}

AModel::Model* ModelPotential::getModel() {
	return model;
}

size_t ModelPotential::getNx() {
	return nx;
}

size_t ModelPotential::getNy() {
	return ny;
}

size_t ModelPotential::getNz() {
	return nz;
}

