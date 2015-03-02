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

	double *potentialSlice;
	cudaMallocManaged(&(potentialSlice), nx * ny * sizeof(double));
	memset(potentialSlice, 0, nx * ny * sizeof(double));
	CUERR

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	cudaEvent_t start,stop;
	float time = 0.0f;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
	
	dim3 threads(BLOCKSIZEX, BLOCKSIZEY, 1);									// размер квардатика
	//dim3 grid(this->nx / BLOCKSIZEX / UNROLLX, this->ny / BLOCKSIZEY, 1 );		// сколько квадратиков нужно чтобы покрыть все изображение
	dim3 grid(this->nx / BLOCKSIZEX, this->ny / BLOCKSIZEY, 1 );		// сколько квадратиков нужно чтобы покрыть все изображение


	AModel::Cortege *pAtoms = model->getTableCell();
	std::sort(pAtoms, pAtoms + nAtoms);
	
	float bindimx = 15; // angstrem
	float bindimy = 15; // angstrem

	int	binx = ceil(a / bindimx);
	int	biny = ceil(b / bindimy);

	std::vector<atom> *bins = new std::vector<atom> [biny * binx];
	
	int *bins_num;
	cudaMallocManaged(&(bins_num), binx * biny * sizeof(int));
	
	std::vector<atom> slice;

	for(size_t kz = 0; kz * dz < c; kz++) {
		for(size_t i = 0; i < nAtoms; i++) {
			if( kz * dz <= pAtoms[i].element.xsCoordinate.z * c && pAtoms[i].element.xsCoordinate.z * c < (kz + 1) * dz ) {
				atom buff;
 				buff.id = i;
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

		std::cout << "atoms in slice / bins: " << slice.size() << " / " << summ_atoms_in_bins << std::endl;
		
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

		calculatePotentialGridGPU<<<grid, threads>>>(potentialSlice, bins_num, bins_d, a, b, c, dx, dy, binx, biny, bindimx, bindimy, radius);
		cudaThreadSynchronize();

		cudaMemcpy(potential + nx * ny * kz, potentialSlice, nx * ny * sizeof(double), cudaMemcpyDeviceToHost);
		memset(potentialSlice, 0, nx * ny * sizeof(double));

		slice.clear();

		cudaFree(bins_d);
	}

	pAtoms = nullptr;
	
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);
	
	std::cout << std::endl << "Kernel time calculating potential grid: " << time << "ms." << std::endl << std::endl;
	
	return 0;
}

__global__ void calculatePotentialGridGPU(double *potential, int *bin_num, atom *bin_d, double a, double b, double c, double dx, double dy, double binx, double biny, double bindimx, double bindimy, double radius) {
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
	const int LINESIZE = gridDim.x * blockDim.x;

	int i,j,kb;
	i = j = kb = 0;

	int bins[32] = {0};
	
	float coordx = ix * dx;
	float coordy = iy * dy;

	for(i = 0; i < biny; i++) {
		float dY1 = fabsf(coordy - i * bindimy);
		float dY2 = fabsf(coordy - (i + 1) * bindimy);
		
		dY1 = ( dY1 >= b / 2.0 ) ? fabsf(dY1 - b) : dY1;
		dY2 = ( dY2 >= b / 2.0 ) ? fabsf(dY2 - b) : dY2;

		if(dY1 < bindimy || dY2 < bindimy) {
			for(j = 0; j < binx; j++) {
				float dX1 = fabsf(coordx - j * bindimx);
				float dX2 = fabsf(coordx - (j + 1) * bindimx);
			
				dX1 = ( dX1 >= a / 2.0 ) ? fabsf(dX1 - a) : dX1;
				dX2 = ( dX2 >= a / 2.0 ) ? fabsf(dX2 - a) : dX2;

				if(dX1 < bindimx || dX2 < bindimx) {
					bins[kb] = binx * i + j;
					kb++;
				}				
			}
		}
	}

	float imageval = 0.0f;

	for(i = 0; i < kb - 1; i++) {
		int n = bin_num[bins[i]+1] - bin_num[bins[i]];
		int offset = bin_num[bins[i]];
		for(j = 0; j < n; j++) {
			float x = fabsf(bin_d[offset + j].x * a - ix * dx);
			float y = fabsf(bin_d[offset + j].y * b - iy * dy);

			x = ( x >= a / 2.0 ) ? x - a : x;
			y = ( y >= b / 2.0 ) ? y - b : y;

			float r = sqrtf(x * x + y * y);

			imageval += calculateProjectedPotential(bin_d[offset + j].num, r);
		}
	}

	potential[ LINESIZE * iy + ix ] = potential[ LINESIZE * iy + ix ] + imageval; 
	
}

// __global__ void calculatePotentialGridGPU(int nAtoms, double a, double b, double c, double dx, double dy, double *potential, double r, double dk) {
// 	const int ix = blockDim.x * blockIdx.x * UNROLLX + threadIdx.x;
// 	const int iy = blockDim.y * blockIdx.y + threadIdx.y;
// 	const int LINESIZE = UNROLLX * gridDim.x * blockDim.x;
// 
// 	int l;
// 	
// 	double imageval1 = 0.0;
// 	double imageval2 = 0.0;
// 	double imageval3 = 0.0;
// 	double imageval4 = 0.0;
// 	double imageval5 = 0.0;
// 	double imageval6 = 0.0;
// 	double imageval7 = 0.0;
// 	double imageval8 = 0.0;
// 
// 	for(l = 0; l < nAtoms; l++) {
// 		float dY = fabsf(atominfoxy[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * l + 1] * b - (iy * dy));
// 		dY = ( dY >= b / 2.0 ) ? dY - b : dY;
// 		dY = dY * dY;
// 
// 		float x = atominfoxy[ATOMS_IN_CONST_MEMORY_MULTIPLICATOR * l + 0] * a;
// 
// // 		float dX1 = ix * dx - x;
// // 		float dX2 = dX1 + gridspacing_u;
// // 		float dX3 = dX2 + gridspacing_u;
// // 		float dX4 = dX3 + gridspacing_u;
// // 		float dX5 = dX4 + gridspacing_u;
// // 		float dX6 = dX5 + gridspacing_u;
// // 		float dX7 = dX6 + gridspacing_u;
// // 		float dX8 = dX7 + gridspacing_u;
// 
// 		float dX1 = fabsf(x - (ix + 0 * blockDim.x) * dx);
// 		float dX2 = fabsf(x - (ix + 1 * blockDim.x) * dx);
//  		float dX3 = fabsf(x - (ix + 2 * blockDim.x) * dx);
// 		float dX4 = fabsf(x - (ix + 3 * blockDim.x) * dx);
//  		float dX5 = fabsf(x - (ix + 4 * blockDim.x) * dx);
//  		float dX6 = fabsf(x - (ix + 5 * blockDim.x) * dx);
//  		float dX7 = fabsf(x - (ix + 6 * blockDim.x) * dx);
//  		float dX8 = fabsf(x - (ix + 7 * blockDim.x) * dx);
// 
// 		dX1 = ( dX1 >= a / 2.0 ) ? dX1 - a : dX1;
// 		dX2 = ( dX2 >= a / 2.0 ) ? dX2 - a : dX2;
// 		dX3 = ( dX3 >= a / 2.0 ) ? dX3 - a : dX3;
// 		dX4 = ( dX4 >= a / 2.0 ) ? dX4 - a : dX4;
// 		dX5 = ( dX5 >= a / 2.0 ) ? dX5 - a : dX5;
// 		dX6 = ( dX6 >= a / 2.0 ) ? dX6 - a : dX6;
// 		dX7 = ( dX7 >= a / 2.0 ) ? dX7 - a : dX7;
// 		dX8 = ( dX8 >= a / 2.0 ) ? dX8 - a : dX8;
// 		
// 		float dR1 = sqrtf(dX1 * dX1 + dY);
// 		float dR2 = sqrtf(dX2 * dX2 + dY);
// 		float dR3 = sqrtf(dX3 * dX3 + dY);
// 		float dR4 = sqrtf(dX4 * dX4 + dY);
// 		float dR5 = sqrtf(dX5 * dX5 + dY);
// 		float dR6 = sqrtf(dX6 * dX6 + dY);
// 		float dR7 = sqrtf(dX7 * dX7 + dY);
// 		float dR8 = sqrtf(dX8 * dX8 + dY);
// 
// 		
// 		int atomid = atominfoid[l];
// 		
// 		if(dR1 < r) {
// 			dR1 = (dR1 < 1.0e-10) ? 1.0e-10 : dR1;
// 			imageval1 += calculateProjectedPotential(atomid, dR1);
// 		}
// 		if(dR2 < r) {
// 			dR2 = (dR2 < 1.0e-10) ? 1.0e-10 : dR2;
// 			imageval2 += calculateProjectedPotential(atomid, dR2);
// 		}
// 		if(dR3 < r) {
// 			dR3 = (dR3 < 1.0e-10) ? 1.0e-10 : dR3;
// 			imageval3 += calculateProjectedPotential(atomid, dR3);
// 		}
// 		if(dR4 < r) {
// 			dR4 = (dR4 < 1.0e-10) ? 1.0e-10 : dR4;
// 			imageval4 += calculateProjectedPotential(atomid, dR4);
// 		}
// 		if(dR5 < r) {
// 			dR5 = (dR5 < 1.0e-10) ? 1.0e-10 : dR5;
// 			imageval5 += calculateProjectedPotential(atomid, dR5);
// 		}
// 		if(dR6 < r) {
// 			dR6 = (dR6 < 1.0e-10) ? 1.0e-10 : dR6;
// 			imageval6 += calculateProjectedPotential(atomid, dR6);
// 		}
// 		if(dR7 < r) {
// 			dR7 = (dR7 < 1.0e-10) ? 1.0e-10 : dR7;
// 			imageval7 += calculateProjectedPotential(atomid, dR7);
// 		}
// 		if(dR8 < r) {
// 			dR8 = (dR8 < 1.0e-10) ? 1.0e-10 : dR8;
// 			imageval8 += calculateProjectedPotential(atomid, dR8);
// 		}
// 	}
// 
// 	potential[ LINESIZE * iy + ix					] = potential[ LINESIZE * iy + ix					] + imageval1;
//   	potential[ LINESIZE * iy + ix + 1 * blockDim.x	] = potential[ LINESIZE * iy + ix + 1 * blockDim.x	] + imageval2; 
//   	potential[ LINESIZE * iy + ix + 2 * blockDim.x	] = potential[ LINESIZE * iy + ix + 2 * blockDim.x	] + imageval3;
//   	potential[ LINESIZE * iy + ix + 3 * blockDim.x	] = potential[ LINESIZE * iy + ix + 3 * blockDim.x	] + imageval4;
//   	potential[ LINESIZE * iy + ix + 4 * blockDim.x	] = potential[ LINESIZE * iy + ix + 4 * blockDim.x	] + imageval5;
//   	potential[ LINESIZE * iy + ix + 5 * blockDim.x	] = potential[ LINESIZE * iy + ix + 5 * blockDim.x	] + imageval6;
//   	potential[ LINESIZE * iy + ix + 6 * blockDim.x	] = potential[ LINESIZE * iy + ix + 6 * blockDim.x	] + imageval7;
//   	potential[ LINESIZE * iy + ix + 7 * blockDim.x	] = potential[ LINESIZE * iy + ix + 7 * blockDim.x	] + imageval8;
// 
// }

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

__device__ double	bessk0( double x ) {
	double ax, x2, sum;

	
	ax = fabs( x );
	if( (ax > 0.0)  && ( ax <=  2.0 ) ) {
		x2 = ax / 2.0;
		x2 = x2 * x2;
		sum = k0a[6];
		for( int i = 5; i >= 0; i--) sum = sum * x2 + k0a[i];
		sum = -log(ax / 2.0) * bessi0( x ) + sum;
	} else if( ax > 2.0 ) {
		x2 = 2.0/ax;
		sum = k0b[6];
		for( int i=5; i>=0; i--) sum = sum*x2 + k0b[i];
		sum = exp( -ax ) * sum / sqrt( ax );
	} else sum = 1.0e20;
	return ( sum );
}

__device__ double	bessi0( double x ) {
 	double ax, sum, t;
 	
	ax = fabs( x );
	if( ax <= 3.75 ) {
		t = x / 3.75;
		t = t * t;
		sum = i0a[6];
		for( int  i = 5; i >= 0; i--) sum = sum * t + i0a[i]; 
	} else {
		t = 3.75 / ax;
		sum = i0b[8];
		for( int i = 7; i >= 0; i--) sum = sum * t + i0b[i];
		sum = exp( ax ) * sum / sqrt( ax );
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

