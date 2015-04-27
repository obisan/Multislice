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
		
		double *potentialSlice;
		potentialSlice = (double*) malloc(nx * ny * sizeof(double));
		memset(potentialSlice, 0, nx * ny * sizeof(double));
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////

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
			
			std::vector<atom> &atoms = slice;

			cudaEvent_t start,stop;
			float ctime = 0.0f;
			cudaEventCreate(&start);
			cudaEventCreate(&stop);
			cudaEventRecord(start,0);
			
			for(size_t iy = 0; iy < ny; iy++) {
				for(size_t ix = 0; ix < nx; ix++) {

					double latticex = ix * dx; // lattice x
					double latticey = iy * dy; // lattice y

					double imageval = 0.0;

					for(unsigned int i = 0; i < slice.size(); i++) {
						int numberAtom = atoms[i].num;
						double x = fabs(atoms[i].x * a_h - latticex);
						double y = fabs(atoms[i].y * b_h - latticey);

						x = ( x >= a_h / 2.0 ) ? x - a_h : x;
						y = ( y >= b_h / 2.0 ) ? y - b_h : y;

						double r = sqrt(x * x + y * y);
			
						if(r > radius) continue;
						r = (r < 1e-20) ? 1e-20 : r;
						double dR1 = 6.2831853071796 * r; // 2 * PI * r;

						imageval += ( 
									FParamsDevice[(numberAtom) * 12 + 0 * 2 + 0] * bessk0(dR1 * sqrt(FParamsDevice[(numberAtom) * 12 + 0 * 2 + 1]))
								+	FParamsDevice[(numberAtom) * 12 + 1 * 2 + 0] * bessk0(dR1 * sqrt(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 1]))
								+	FParamsDevice[(numberAtom) * 12 + 2 * 2 + 0] * bessk0(dR1 * sqrt(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 1])) 
								) * 300.73079394295
								+ (
								(	FParamsDevice[(numberAtom) * 12 + 0 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 0 * 2 + 7])
								+	(FParamsDevice[(numberAtom) * 12 + 1 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 1 * 2 + 7])
								+	(FParamsDevice[(numberAtom) * 12 + 2 * 2 + 6] / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7]) * exp(-(6.2831853071796 * r * r) / FParamsDevice[(numberAtom) * 12 + 2 * 2 + 7])
								) * 150.36539697148;
		
					}
					potentialSlice[ nx * ny * iy + ix ] = imageval; 
				}				
			}

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
			free(potentialSlice);
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

	/*-------------------- bessk0() ---------------*/
	/*
		modified Bessel function K0(x)
		see Abramowitz and Stegun page 380
    
		Note: K0(0) is not define and this function
			returns 1E20
 
		x = (double) real arguments
    
		this routine calls bessi0() = Bessel function I0(x)
    
		12-feb-1997 E. Kirkland
	 */
	double bessk0( double x )
	{
		double bessi0(double);
	
		int i;
		double ax, x2, sum;
		double k0a[] = { -0.57721566, 0.42278420, 0.23069756,
			0.03488590, 0.00262698, 0.00010750, 0.00000740};
	
		double k0b[] = { 1.25331414, -0.07832358, 0.02189568,
			-0.01062446, 0.00587872, -0.00251540, 0.00053208};
	
		ax = fabs( x );
		if( (ax > 0.0)  && ( ax <=  2.0 ) ) {
			x2 = ax / 2.0;
			x2 = x2 * x2;
			sum = k0a[6];
			for( i=5; i>=0; i--) sum = sum*x2 + k0a[i];
			sum = -log(ax/2.0) * bessi0(x) + sum;
		} else if( ax > 2.0 ) {
			x2 = 2.0/ax;
			sum = k0b[6];
			for( i=5; i>=0; i--) sum = sum*x2 + k0b[i];
			sum = exp( -ax ) * sum / sqrt( ax );
		} else sum = 1.0e20;
		return ( sum );
	
	}  /* end bessk0() */


	/*-------------------- bessi0() ---------------*/
	/*
		modified Bessel function I0(x)
		see Abramowitz and Stegun page 379

		x = (double) real arguments

		12-feb-1997 E. Kirkland
	 */
	 double bessi0( double x )	 {
 		int i;
 		double ax, sum, t;
 	
 		double i0a[] = { 1.0, 3.5156229, 3.0899424, 1.2067492,
			0.2659732, 0.0360768, 0.0045813 };

 		double i0b[] = { 0.39894228, 0.01328592, 0.00225319,
 			-0.00157565, 0.00916281, -0.02057706, 0.02635537,
 			-0.01647633, 0.00392377};

		ax = fabs( x );
		if( ax <= 3.75 ) {
			t = x / 3.75;
			t = t * t;
			sum = i0a[6];
			for( i=5; i>=0; i--) sum = sum*t + i0a[i]; 
		} else {
			t = 3.75 / ax;
			sum = i0b[8];
			for( i=7; i>=0; i--) sum = sum*t + i0b[i];
			sum = exp( ax ) * sum / sqrt( ax );
		}
		return( sum );

	}  /* end bessi0() */

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