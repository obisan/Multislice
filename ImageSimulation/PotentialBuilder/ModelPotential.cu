#include "stdafx.h"
#include "ModelPotential.h"

namespace PotentialBuilder {
	ModelPotential::ModelPotential(void) {

	}

	ModelPotential::ModelPotential(AModel::Model *model, size_t nx, size_t ny, size_t nz, double radius, double bindim, const char* fileNameOutput) {
		this->model = model;
		this->nx = nx;
		this->ny = ny;
		this->nz = nz;
		this->radius = radius;
		this->bindim = bindim;

		strcpy(this->fileNameOutput, fileNameOutput);
	}

	ModelPotential::~ModelPotential(void) {
		if(this->model != nullptr) { model = nullptr; }
	}

	int ModelPotential::calculatePotentialGrid() {
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		struct stat statbuf;
		if(stat(fileNameOutput,&statbuf)) {
			wchar_t wzfileNameOutput[256];
			mbstowcs(wzfileNameOutput, fileNameOutput, 256);
			if (CreateDirectory(wzfileNameOutput,NULL))
				std::cout << "Directory [" << fileNameOutput << "] created." << std::endl;
			else {
				std::cout << "Error create [" << fileNameOutput << "] directory." << std::endl << std::endl;
				return -1;
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		const size_t nAtoms = model->getNumberAtoms();
		const double a_h = model->getA();
		const double b_h = model->getB();
		const double c_h = model->getC();
		const double dx = a_h / this->nx;
		const double dy = b_h / this->ny;
		const double dz = c_h / this->nz;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
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

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		dim3 threads(BLOCKSIZEX, BLOCKSIZEY, 1);		
		dim3 grid(this->nx / BLOCKSIZEX, this->ny / BLOCKSIZEY, 1 );

		AModel::Cortege *pAtoms = model->getTableCell();
		std::sort(pAtoms, pAtoms + nAtoms);

		std::vector<atom> slice;
		
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

		for(size_t kz = 0; kz < nz; kz++) {
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////	Divide on slices /////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////

			for(size_t i = 0; i < nAtoms; i++) {
				if( kz * dz <= pAtoms[i].element.xsCoordinate.z * c_h && pAtoms[i].element.xsCoordinate.z * c_h <= (kz + 1) * dz ) {
					atom buff;
					buff.id = i + 1;
					//buff.num = model->getNumberByName(pAtoms[i].element.Atom) - 1;
					buff.num = pAtoms[i].element.Atom - 1;
					buff.x = pAtoms[i].element.xsCoordinate.x;
					buff.y = pAtoms[i].element.xsCoordinate.y;

					slice.push_back(buff);
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////	atoms			//////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			
			atom *SliceAtoms;
			checkCudaErrors( cudaMallocManaged(&(SliceAtoms), slice.size() * sizeof(atom)));
			for(size_t l = 0; l < slice.size(); l++) {
				SliceAtoms[l] = slice[l];
			}

			//memcpy(SliceAtoms, &slice, slice.size() * sizeof(atom));

			cudaEvent_t start,stop;
			float ctime = 0.0f;
			cudaEventCreate(&start);
			cudaEventCreate(&stop);
			cudaEventRecord(start,0);
			
			calculatePotentialGridGPU<<<grid, threads>>>(potentialSlice, SliceAtoms, slice.size());
			CUERR

			checkCudaErrors( cudaThreadSynchronize() );

			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&ctime, start, stop);
			time_kernel += ctime;

			std::cout << "slice: " << kz << std::endl << "calculated atoms: " << slice.size() << std::endl;
			
			char slicename[256];
			sprintf(slicename, "%s/slice%003u.slc", fileNameOutput, kz);
			FILE *pFile;
			pFile = fopen(slicename, "wb");
			fwrite(potentialSlice, sizeof(double), nx * ny, pFile);
			fclose(pFile);

			slice.clear();
			cudaFree(SliceAtoms);
		}

		cudaEventRecord(stop_total,0);
		cudaEventSynchronize(stop_total);
		cudaEventElapsedTime(&time_total, start_total, stop_total);

		pAtoms = nullptr;

		std::cout << std::endl;
		std::cout << "Kernel time calculating potential grid: " << time_kernel	<< "ms." << std::endl;
		std::cout << "Total  time calculating potential grid: " << time_total	<< "ms." << std::endl << std::endl;


		return 0;
	}

	__global__ void calculatePotentialGridGPU(double *potential, atom* atoms, unsigned int n) {
		const int ix = __umul24(blockDim.x, blockIdx.x) + threadIdx.x;
		const int iy = __umul24(blockDim.y, blockIdx.y) + threadIdx.y;
		const int is = __umul24(blockDim.x, threadIdx.y) + threadIdx.x;
		const int LINESIZE = __umul24(gridDim.x, blockDim.x);

		double latticex = ix * dx_d; // lattice x
		double latticey = iy * dy_d; // lattice y

		__shared__ double imageval[BLOCKSIZEX*BLOCKSIZEY];
		imageval[is] = 0.0;

		for(unsigned int i = 0; i < n; i++) {
			int numberAtom = atoms[i].num;
			double x = fabs(atoms[i].x * a_d - latticex);
			double y = fabs(atoms[i].y * b_d - latticey);

			x = ( x >= a_d / 2.0 ) ? x - a_d : x;
			y = ( y >= b_d / 2.0 ) ? y - b_d : y;

			double r = __dsqrt_rn(x * x + y * y);
			
			//if(r > radius_d) continue;
			r = (r < 1e-20) ? 1e-20 : r;
			double dR1 = 6.2831853071796 * r; // 2 * PI * r;

			imageval[is] += ( 
						FParamsDevice[(numberAtom) * 12 + 0 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 1]))
					+	FParamsDevice[(numberAtom) * 12 + 1 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 1]))
					+	FParamsDevice[(numberAtom) * 12 + 2 * 2 + 0] * bessk0(dR1 * __dsqrt_rn(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 1])) 
					) * 300.73079394295
					+ (
					(	FParamsDevice[(numberAtom) * 12 + 0 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7])
					+	(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7])
					+	(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7])
					) * 150.36539697148;
		
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
		//memcpy(image->imageData, this->potential, nx * ny * nz * sizeof(double));
		image->saveMRC(filenamept, model, nx, ny, nz, mrc_FLOAT);
		delete image;

		return 0;
	}

	int	ModelPotential::savePotentialStack(const char* filename, const char* stackDirectory) {
		Image *image = new Image(nx, ny, nz, sizeof(double), 1);
		char filenamept[256];
		strcpy(filenamept, filename);
		strcat(filenamept, "_pt");
		image->saveStackMRC(filenamept, stackDirectory, model, nx, ny, nz, mrc_FLOAT);
		delete image;

		return 0;
	}


	AModel::Model* ModelPotential::getModel() {
		return model;
	}
}