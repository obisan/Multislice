#include "stdafx.h"
#include "ModelPotential.h"

struct __align__(16) atom {
	int id;
	int num;
	float x;
	float y;
};

__global__ void calculatePotentialGridGPU(double *potential, int *bins_offset, int *bins_num, atom *bins_d, unsigned short *bins_lattice);

__device__ double	bessk0( double x );
__device__ double	bessi0( double x );
__device__ void		swap2(double& a, double& b);


ModelPotential::ModelPotential(void) {

}

ModelPotential::ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double radius, double bindim) {
	this->model = model;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	this->radius = radius;
	this->bindim = bindim;
	
	this->potential = (double*) malloc(nx * ny * nz * sizeof(double));
	memset(this->potential, 0, nx * ny * nz * sizeof(double));
}

ModelPotential::~ModelPotential(void) {
	if(this->model != nullptr) { model = nullptr; }
	if(this->potential != nullptr) { free(this->potential); }
}

int ModelPotential::calculatePotentialGrid() {
	const size_t nAtoms = model->getNumberAtoms();
	const double a_h = model->getA();
	const double b_h = model->getB();
	const double c_h = model->getC();
	const double dx = a_h / this->nx;
	const double dy = b_h / this->ny;
	const double dz = c_h / this->nz;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	checkCudaErrors( cudaMemcpyToSymbol(radius_d, &radius, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(dx_d, &dx, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(dy_d, &dy, sizeof(double)) );
	
	checkCudaErrors( cudaMemcpyToSymbol(a_d, &a_h, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(b_d, &b_h, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(c_d, &c_h, sizeof(double)) );

	double bindimx = this->bindim; // angstrem
	double bindimy = this->bindim; // angstrem
	checkCudaErrors( cudaMemcpyToSymbol(bindimx_d, &bindimx, sizeof(double)) );
	checkCudaErrors( cudaMemcpyToSymbol(bindimy_d, &bindimy, sizeof(double)) );

	int	binx = ceil(a_h / bindimx); // dimensionless
	int	biny = ceil(b_h / bindimy); // dimensionless
	checkCudaErrors( cudaMemcpyToSymbol(binx_d, &binx, sizeof(int)) );
	checkCudaErrors( cudaMemcpyToSymbol(biny_d, &biny, sizeof(int)) );

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	double *potentialSlice;
	checkCudaErrors( cudaMallocManaged(&(potentialSlice), nx * ny * sizeof(double)));
	memset(potentialSlice, 0, nx * ny * sizeof(double));
	CUERR
	
	int *bins_offset;
	checkCudaErrors( cudaMallocManaged(&(bins_offset), (binx * biny + 1) * sizeof(int)));
	memset(bins_offset, 0, (binx * biny + 1) * sizeof(int));
	CUERR

	int *bins_num;
	checkCudaErrors( cudaMallocManaged(&(bins_num), (nx * ny) * sizeof(int)));
	memset(bins_num, 0, (nx * ny) * sizeof(int));
	CUERR


	//////////////////////////////////////////////////////////////////////////////////////////////////////

	dim3 threads(BLOCKSIZEX, BLOCKSIZEY, 1);		
	dim3 grid(this->nx / BLOCKSIZEX, this->ny / BLOCKSIZEY, 1 );

	AModel::Cortege *pAtoms = model->getTableCell();
	std::sort(pAtoms, pAtoms + nAtoms);

	std::vector<atom> slice;
	std::vector<atom> *bins = new std::vector<atom> [biny * binx];
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	float time_kernel = 0.0f;
	float time_total = 0.0f;
	cudaEvent_t start_total,stop_total;
	cudaEventCreate(&start_total);
	cudaEventCreate(&stop_total);
	cudaEventRecord(start_total,0);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	clock_t time_lattice = clock();
	
	unsigned short *bins_lattice;
	checkCudaErrors( cudaMallocManaged(&(bins_lattice), (nx * ny * MAX_BINS_PER_PX) * sizeof(unsigned short)));
	memset(bins_lattice, -1, (nx * ny * MAX_BINS_PER_PX) * sizeof(unsigned short));

	for(size_t iy = 0; iy < ny; iy++) {
		int coordbinstarty	= floor(iy * ((double) biny / ny) - radius / bindimy ) - 1;		// iy in px
		int coordbinendy	= ceil(iy * ((double) biny / ny) + radius / bindimy ) - 1;

		for(size_t ix = 0; ix < nx; ix++) {                                 // jx in px
			int coordbinstartx	= floor(ix * ((double) binx / nx) - radius / bindimx ) - 1;
			int coordbinendx	= ceil(ix * ((double) binx / nx) + radius / bindimx ) - 1;
			
			int k = 0;
			for(int i = coordbinstarty; i <= coordbinendy && k < MAX_BINS_PER_PX; i++) {
				int bincurr_y = (i + biny) % biny;
				for(int j = coordbinstartx; j <= coordbinendx && k < MAX_BINS_PER_PX; j++, k++) {
					int bincur_x = (j + binx) % binx;
					bins_lattice[ MAX_BINS_PER_PX * (nx * iy + ix) + k ] = binx * bincurr_y + bincur_x;
					bins_num[nx * iy + ix] = k + 1;
				}
			}
		}
	}

	std::cout << "calculate lattice-to-bin: " << (clock() - time_lattice) / CLOCKS_PER_SEC << "s." << std::endl;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	for(size_t kz = 0; kz < nz; kz++) {
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////	Divide on slices /////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		for(size_t i = 0; i < nAtoms; i++) {
			if( kz * dz <= pAtoms[i].element.xsCoordinate.z * c_h && pAtoms[i].element.xsCoordinate.z * c_h <= (kz + 1) * dz ) {
				atom buff;
 				buff.id = i + 1;
				buff.num = model->getNumberByName(pAtoms[i].element.Atom) - 1;
				buff.x = pAtoms[i].element.xsCoordinate.x;
				buff.y = pAtoms[i].element.xsCoordinate.y;

				slice.push_back(buff);
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////	Divide on bins	//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		for(size_t iy = 0; iy < biny; iy++) {
			for(size_t jx = 0; jx < binx; jx++) {
				for(auto t : slice) {
					if( t.x * a_h >= jx * bindimx && t.x * a_h <= (jx + 1) * bindimx )
						if( t.y * b_h >= iy * bindimy && t.y * b_h <= (iy + 1) * bindimy )
							bins[binx * iy + jx].push_back(t);
				}
			}
		}

		atom *bins_d;
		checkCudaErrors( cudaMallocManaged(&(bins_d), slice.size() * sizeof(atom)));

		bins_offset[0] = 0;
		for(size_t i = 0, k = 0; i < binx * biny; i++) {
			bins_offset[i + 1] = bins[i].size();
			for(auto s : bins[i]) {
				bins_d[k] = s;
				k++;
			}
			bins[i].clear();
		}

		// make number to offset
		for(int i = 2; i < binx * biny + 1; i++) {
			bins_offset[i] = bins_offset[i] + bins_offset[i - 1];
		}

		cudaEvent_t start,stop;
		float ctime = 0.0f;
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventRecord(start,0);

		calculatePotentialGridGPU<<<grid, threads>>>(potentialSlice, bins_offset, bins_num, bins_d, bins_lattice);
		checkCudaErrors( cudaThreadSynchronize() );

		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&ctime, start, stop);
		time_kernel += ctime;

		std::cout << "slice: " << kz << std::endl << "calculated atoms: " << slice.size() << std::endl;

		cudaMemcpy(potential + nx * ny * kz, potentialSlice, nx * ny * sizeof(double), cudaMemcpyDeviceToHost);
		
		slice.clear();	
		cudaFree(bins_d);
	}

	cudaFree(bins_num);
	cudaFree(bins_lattice);
	cudaFree(bins_offset);

	cudaEventRecord(stop_total,0);
	cudaEventSynchronize(stop_total);
	cudaEventElapsedTime(&time_total, start_total, stop_total);

	pAtoms = nullptr;
	
	std::cout << std::endl;
	std::cout << "Kernel time calculating potential grid: " << time_kernel	<< "ms." << std::endl;
	std::cout << "Total  time calculating potential grid: " << time_total	<< "ms." << std::endl << std::endl;
	

	return 0;
}

__global__ void calculatePotentialGridGPU(double *potential, int *bins_offset, int *bins_num, atom *bins_d, unsigned short *bins_lattice) {
	const int ix = __umul24(blockDim.x, blockIdx.x) + threadIdx.x;
	const int iy = __umul24(blockDim.y, blockIdx.y) + threadIdx.y;
	const int is = __umul24(blockDim.x, threadIdx.y) + threadIdx.x;
	const int LINESIZE = __umul24(gridDim.x, blockDim.x);
	
	int i,j;
	
	double latticex = ix * dx_d; // lattice x
	double latticey = iy * dy_d; // lattice y
	
	const int numberBins = bins_num[LINESIZE * iy + ix];

	__shared__ double imageval[BLOCKSIZEX*BLOCKSIZEY];
	imageval[is] = 0.0;
	
	for(i = 0; i < numberBins; i++) {
		int ibin = bins_lattice[ MAX_BINS_PER_PX * (LINESIZE * iy + ix) + i];
		
		int n = bins_offset[ibin + 1] - bins_offset[ibin];
		int offset = bins_offset[ibin];

		for(j = 0; j < n; j++) {
			int numberAtom = bins_d[offset + j].num;
			double x = fabs(bins_d[offset + j].x * a_d - latticex);
			double y = fabs(bins_d[offset + j].y * b_d - latticey);

			x = ( x >= a_d / 2.0 ) ? x - a_d : x;
			y = ( y >= b_d / 2.0 ) ? y - b_d : y;

			double r = __dsqrt_rn(x * x + y * y);
			r = (r < 1e-20) ? 1e-20 : r;
			double dR1 = 6.2831853071796 * r; // 2 * PI * r;
		
			imageval[is] += ( 
					FParamsDevice[(numberAtom) * 12 + 0 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 1]))
				+	FParamsDevice[(numberAtom) * 12 + 1 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 1]))
				+	FParamsDevice[(numberAtom) * 12 + 2 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 1])) 
				) * 300.73079394295
				+ (
					(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7])
				+	(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7])
				+	(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7])
				) * 150.36539697148;

		}
	}

	__syncthreads();

	potential[ LINESIZE * iy + ix ] = imageval[is]; 
	
}

__device__ void		swap2(double& a, double& b) {
		double buffer = a;
		a = b;
		b = buffer;
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

