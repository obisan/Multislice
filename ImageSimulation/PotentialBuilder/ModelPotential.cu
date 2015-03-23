#include "stdafx.h"
#include "ModelPotential.h"

namespace PotentialBuilder {
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
		const double a = model->getA();
		const double b = model->getB();
		const double c = model->getC();
		const double dx = a / this->nx;
		const double dy = b / this->ny;
		const double dz = c / this->nz;
	
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		double *potentialSlice;
		cudaMallocManaged(&(potentialSlice), nx * ny * sizeof(double));
		memset(potentialSlice, 0, nx * ny * sizeof(double));
		CUERR

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		cudaEvent_t start_total,stop_total;
		float time_total = 0.0f;
		cudaEventCreate(&start_total);
		cudaEventCreate(&stop_total);
		cudaEventRecord(start_total,0);
	
		dim3 threads(BLOCKSIZEX, BLOCKSIZEY, 1);										// размер квардатика
		dim3 grid(this->nx / BLOCKSIZEX / UNROLLX, this->ny / BLOCKSIZEY, 1 );		// сколько квадратиков нужно чтобы покрыть все изображение


		AModel::Cortege *pAtoms = model->getTableCell();
		std::sort(pAtoms, pAtoms + nAtoms);
	
		int		atominfoid_host[ATOMS_IN_CONST_MEMORY];
		float	atominfoxy_host[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * ATOMS_IN_CONST_MEMORY];
	
		int j = 0;
		float time_kernel = 0.0f;
		for(size_t kz = 0; kz * dz < c; kz++) {
			for(size_t i = 0; i < nAtoms; i++) {
				if( kz * dz <= pAtoms[i].element.xsCoordinate.z * c && pAtoms[i].element.xsCoordinate.z * c < (kz + 1) * dz ) {
 					atominfoid_host[j] = model->getNumberByName(pAtoms[i].element.Atom) - 1;
 					atominfoxy_host[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * j + 0] = pAtoms[i].element.xsCoordinate.x;
 					atominfoxy_host[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * j + 1] = pAtoms[i].element.xsCoordinate.y;
					j = j + 1;
				}

				if(j == ATOMS_IN_CONST_MEMORY || i == nAtoms - 1) {
 					checkCudaErrors( cudaMemcpyToSymbol(atominfoid, atominfoid_host, ATOMS_IN_CONST_MEMORY * sizeof(int), 0, cudaMemcpyHostToDevice));
 					checkCudaErrors( cudaMemcpyToSymbol(atominfoxy, atominfoxy_host, ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * ATOMS_IN_CONST_MEMORY * sizeof(float), 0, cudaMemcpyHostToDevice));
				
					cudaEvent_t start_kernel,stop_kernel;
					float ctime = 0.0f;
					cudaEventCreate(&start_kernel);
					cudaEventCreate(&stop_kernel);
					cudaEventRecord(start_kernel,0);

 					calculatePotentialGridGPU<<<grid, threads>>>(j, a, b, c, dx, dy, potentialSlice, radius);
 					checkCudaErrors( cudaThreadSynchronize() );
					
					cudaEventRecord(stop_kernel,0);
					cudaEventSynchronize(stop_kernel);
					cudaEventElapsedTime(&ctime, start_kernel, stop_kernel);

					time_kernel += ctime;

					std::cout << "slice: " << kz << "calculated atoms: " << j << " current atom: " << i << std::endl;
					
					j = 0;				
				}
			}

			// Device to Host
			memcpy(potential + nx * ny * kz, potentialSlice, nx * ny * sizeof(double));
			memset(potentialSlice, 0, nx * ny * sizeof(double));
		}

		pAtoms = nullptr;
	
		cudaEventRecord(stop_total,0);
		cudaEventSynchronize(stop_total);
		cudaEventElapsedTime(&time_total, start_total, stop_total);
	
		std::cout << std::endl;
		std::cout << "Kernel time calculating potential grid: " << time_kernel	<< "ms." << std::endl;
		std::cout << "Total  time calculating potential grid: " << time_total	<< "ms." << std::endl << std::endl;
	
		return 0;
	}

	__global__ void calculatePotentialGridGPU(int nAtoms, double a, double b, double c, double dx, double dy, double *potential, double r) {
		const int ix = blockDim.x * blockIdx.x * UNROLLX + threadIdx.x;
		const int iy = blockDim.y * blockIdx.y + threadIdx.y;
		const int LINESIZE = UNROLLX * gridDim.x * blockDim.x;

		int l;
	
		double imageval1 = 0.0;
		double imageval2 = 0.0;
		double imageval3 = 0.0;
		double imageval4 = 0.0;
		double imageval5 = 0.0;
		double imageval6 = 0.0;
		double imageval7 = 0.0;
		double imageval8 = 0.0;

		for(l = 0; l < nAtoms; l++) {
			int atomid = atominfoid[l];
			float dY = fabsf(atominfoxy[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * l + 1] * b - (iy * dy));
			float x = atominfoxy[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * l + 0] * a;

			dY = ( dY >= b / 2.0 ) ? dY - b : dY;
			dY = dY * dY;

		

	// 		float dX1 = ix * dx - x;
	// 		float dX2 = dX1 + gridspacing_u;
	// 		float dX3 = dX2 + gridspacing_u;
	// 		float dX4 = dX3 + gridspacing_u;
	// 		float dX5 = dX4 + gridspacing_u;
	// 		float dX6 = dX5 + gridspacing_u;
	// 		float dX7 = dX6 + gridspacing_u;
	// 		float dX8 = dX7 + gridspacing_u;

			float dX1 = fabsf(x - (ix + 0 * blockDim.x) * dx);
			float dX2 = fabsf(x - (ix + 1 * blockDim.x) * dx);
 			float dX3 = fabsf(x - (ix + 2 * blockDim.x) * dx);
			float dX4 = fabsf(x - (ix + 3 * blockDim.x) * dx);
 			float dX5 = fabsf(x - (ix + 4 * blockDim.x) * dx);
 			float dX6 = fabsf(x - (ix + 5 * blockDim.x) * dx);
 			float dX7 = fabsf(x - (ix + 6 * blockDim.x) * dx);
 			float dX8 = fabsf(x - (ix + 7 * blockDim.x) * dx);

			dX1 = ( dX1 >= a / 2.0 ) ? dX1 - a : dX1;
			dX2 = ( dX2 >= a / 2.0 ) ? dX2 - a : dX2;
			dX3 = ( dX3 >= a / 2.0 ) ? dX3 - a : dX3;
			dX4 = ( dX4 >= a / 2.0 ) ? dX4 - a : dX4;
			dX5 = ( dX5 >= a / 2.0 ) ? dX5 - a : dX5;
			dX6 = ( dX6 >= a / 2.0 ) ? dX6 - a : dX6;
			dX7 = ( dX7 >= a / 2.0 ) ? dX7 - a : dX7;
			dX8 = ( dX8 >= a / 2.0 ) ? dX8 - a : dX8;
		
			float dR1 = sqrtf(dX1 * dX1 + dY);
			float dR2 = sqrtf(dX2 * dX2 + dY);
			float dR3 = sqrtf(dX3 * dX3 + dY);
			float dR4 = sqrtf(dX4 * dX4 + dY);
			float dR5 = sqrtf(dX5 * dX5 + dY);
			float dR6 = sqrtf(dX6 * dX6 + dY);
			float dR7 = sqrtf(dX7 * dX7 + dY);
			float dR8 = sqrtf(dX8 * dX8 + dY);

			if(dR1 < r) {
				dR1 = (dR1 < 1.0e-10) ? 1.0e-10 : dR1;
				imageval1 += calculateProjectedPotential(atomid, dR1);
			}
			if(dR2 < r) {
				dR2 = (dR2 < 1.0e-10) ? 1.0e-10 : dR2;
				imageval2 += calculateProjectedPotential(atomid, dR2);
			}
			if(dR3 < r) {
				dR3 = (dR3 < 1.0e-10) ? 1.0e-10 : dR3;
				imageval3 += calculateProjectedPotential(atomid, dR3);
			}
			if(dR4 < r) {
				dR4 = (dR4 < 1.0e-10) ? 1.0e-10 : dR4;
				imageval4 += calculateProjectedPotential(atomid, dR4);
			}
			if(dR5 < r) {
				dR5 = (dR5 < 1.0e-10) ? 1.0e-10 : dR5;
				imageval5 += calculateProjectedPotential(atomid, dR5);
			}
			if(dR6 < r) {
				dR6 = (dR6 < 1.0e-10) ? 1.0e-10 : dR6;
				imageval6 += calculateProjectedPotential(atomid, dR6);
			}
			if(dR7 < r) {
				dR7 = (dR7 < 1.0e-10) ? 1.0e-10 : dR7;
				imageval7 += calculateProjectedPotential(atomid, dR7);
			}
			if(dR8 < r) {
				dR8 = (dR8 < 1.0e-10) ? 1.0e-10 : dR8;
				imageval8 += calculateProjectedPotential(atomid, dR8);
			}
		}

		potential[ LINESIZE * iy + ix					] = potential[ LINESIZE * iy + ix					] + imageval1;
  		potential[ LINESIZE * iy + ix + 1 * blockDim.x	] = potential[ LINESIZE * iy + ix + 1 * blockDim.x	] + imageval2; 
  		potential[ LINESIZE * iy + ix + 2 * blockDim.x	] = potential[ LINESIZE * iy + ix + 2 * blockDim.x	] + imageval3;
  		potential[ LINESIZE * iy + ix + 3 * blockDim.x	] = potential[ LINESIZE * iy + ix + 3 * blockDim.x	] + imageval4;
  		potential[ LINESIZE * iy + ix + 4 * blockDim.x	] = potential[ LINESIZE * iy + ix + 4 * blockDim.x	] + imageval5;
  		potential[ LINESIZE * iy + ix + 5 * blockDim.x	] = potential[ LINESIZE * iy + ix + 5 * blockDim.x	] + imageval6;
  		potential[ LINESIZE * iy + ix + 6 * blockDim.x	] = potential[ LINESIZE * iy + ix + 6 * blockDim.x	] + imageval7;
  		potential[ LINESIZE * iy + ix + 7 * blockDim.x	] = potential[ LINESIZE * iy + ix + 7 * blockDim.x	] + imageval8;

	}

	__device__ double	calculateProjectedPotential(int numberAtom, double r) {
		double sumf;
		double sums;
 		double dR1;

		sumf = 0.0;
		sums = 0.0;
		dR1 = 6.2831853071796 * r; // 2 * PI * r

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
}