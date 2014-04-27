// Image.cpp: определяет экспортированные функции для приложения DLL.
//

// #include "stdafx.h"
// #include "Image.h"
// 
// 
// // Пример экспортированной переменной
// IMAGE_API int nImage=0;
// 
// // Пример экспортированной функции.
// IMAGE_API int fnImage(void)
// {
// 	return 42;
// }
// 
// // Конструктор для экспортированного класса.
// // см. определение класса в Image.h
// CImage::CImage()
// {
// 	return;
// }

#include "stdafx.h"
#include "Image.h"
#include "MRC.h"
#include "otherEnums.h"

namespace ImageSpace {
	Image::Image() {

	}

	Image::Image(size_t nx, size_t ny, size_t nz, size_t depth, size_t nChannels) {
		this->width			= (int)nx;
		this->height		= (int)ny;
		this->thickness		= (int)nz;
		this->depth			= (int)depth;
		this->nChannels		= (int)nChannels;
		this->nSize			= this->width * this->height * this->thickness * this->depth * this->nChannels;
		this->widthStep		= this->width;

		this->imageData = new char [ this->nSize ];
		memset(this->imageData, 0, this->nSize);
	}

	Image::Image(size_t nx, size_t ny, size_t depth, size_t nChannels) {
		Image(nx, ny, 1, depth, nChannels);
	}

	Image::Image(const Image *image) {
		this->width			= image->width;
		this->height		= image->height;
		this->thickness		= image->thickness;
		this->depth			= image->depth;
		this->nChannels		= image->nChannels;
		this->nSize			= this->width * this->height * this->thickness * this->depth * this->nChannels;
		this->widthStep		= this->width;

		this->imageData = new char [ this->nSize ];
		memcpy(this->imageData, image->imageData, image->nSize);
	}

	Image::~Image() {
		if(imageData != nullptr) {
			delete[] imageData;
			imageData = nullptr;
		}
	}

	void Image::zernike(Image* image, int transformationStatus) {
		if(transformationStatus == transformationStatus::transformed) {
			double *p = (double*) image->imageData;
			std::complex<double> z(p[0], p[1]);
			std::complex<double> z1(0, 1);
			p[0] = (z * z1).real();
			p[1] = (z * z1).imag();
		} else 
			if (transformationStatus == transformationStatus::notTransformed) {
				Image::fft(image, 0, fft::forward);
				Image::normalize(image, 0, image->width);
				double *p = (double*) image->imageData;
				std::complex<double> z(p[0], p[1]);
				std::complex<double> z1(0, 1);
				p[0] = (z * z1).real();
				p[1] = (z * z1).imag();
				Image::fft(image, 0, fft::backward);
				Image::normalize(image, 0, image->width);
			}
	}

	void Image::fft(Image* image, int slide, int direction) {
		fftw_complex *pfftw_in = fftw_alloc_complex(image->width * image->height);
		fftw_complex *pfftw_out = fftw_alloc_complex(image->width * image->height);

		Image::copy<double>(image, pfftw_in, slide, copyDirection::copyImagetoFFT);

		fftw_plan fftw_ward;
		fftw_ward = (direction == fft::forward) ? 
			fftw_plan_dft_2d(image->width, image->height, pfftw_in, pfftw_out, FFTW_FORWARD, FFTW_ESTIMATE) : 
			fftw_plan_dft_2d(image->width, image->height, pfftw_in, pfftw_out, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftw_execute(fftw_ward);
		fftw_destroy_plan(fftw_ward);

		Image::copy<double>(image, pfftw_out, slide, copyDirection::copyFFTtoImage);

		fftw_free(pfftw_in);
		fftw_free(pfftw_out);
	}	

	void Image::normalize(Image* image, int slide, int n) {
		size_t nx = image->width;
		size_t ny = image->height;
		size_t nChannels = image->nChannels;
		double *p = 0;
		for(size_t i = 0; i < ny; i++) {
			p = image->getPointer<double>(slide, i);
			for(size_t j = 0; j < nx; j++) {
				p[nChannels * j + 0] /= (double) n;
				p[nChannels * j + 1] /= (double) n;
			}
		}
	}

	void Image::save(const char* filenamesave) {
		double *pp = 0;
		size_t nnx = this->width;
		size_t nny = this->height;
		size_t nnz = this->thickness;
		size_t nChannels = this->nChannels;

		char *filenamesave_ext = new char[ strlen(filenamesave) + 8 ];
		strcpy(filenamesave_ext, filenamesave);
		strcat(filenamesave_ext,".txt");

		std::ofstream fout(filenamesave_ext);
		for(size_t k = 0; k < nnz;k++) {
			for(size_t i = 0; i < nny; i++) {
				pp = this->getPointer<double>(k, i);
				for(size_t j = 0; j < nnx; j++) {
					for(size_t l = 0; l < nChannels; l++)
						fout << pp[nChannels * j + l] << "\t";
				} fout << std::endl;
			} fout << std::endl;
		} fout.close();

		delete[] filenamesave_ext;
	}

	void Image::saveMRC(const char* filenamesave, AModel::Model* model) {
		float tiltangles[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
				
		MRC *imageMRC = new MRC(
			filenamesave, this->width, this->height, this->thickness, 4, 
			0, 0, 0, 
			0, 0, 0, 
			model->getA(), model->getB(), model->getC(), model->getAlpha(), model->getBeta(), model->getGamma(), 
			1, 2, 3, 0, 0, 0,
			0, 0, 
			0, "",
			0, 0, "",
			0, 0,
			1, 0, 0, 0, 0, 0,
			tiltangles,
			0.0f, 0.0f, 0.0f,
			"", "", 
			0.0f,
			0,
			"",
			this->imageData);
		
		imageMRC->save(filenamesave);

		delete imageMRC;
	}

	void Image::saveQtSpectrum(const char* filenamesave, size_t numberSlice) {
		double *pp = 0;
		size_t nnx = this->width;
		size_t nny = this->height;
		size_t nnz = this->thickness;
		size_t nChannels = this->nChannels;

		std::ofstream fout(filenamesave);
		fout << "0" << "\t" << "1" << "\t" << nnx << "\t" << nny << "\n";
		for(size_t i = 0; i < nny; i++) {
			pp = this->getPointer<double>(numberSlice, i);
			for(size_t j = 0; j < nnx; j++) {
				for(size_t l = 0; l < nChannels; l++)
					fout << pp[nChannels * j + l] << "\t";
			} fout << std::endl;
		} fout << std::endl;
		fout.close();
	}

	Image* Image::getRealPart() {
		Image *img = new Image(this->width, this->height, this->thickness, this->depth, 1);

		double *pThis = 0;
		double *pSave = 0;

		size_t nnx = this->width;
		size_t nny = this->height;
		size_t nnz = this->thickness;
		size_t nChannels = this->nChannels;

		for(size_t kz = 0; kz < nnz; kz++) {
			for(size_t iy = 0; iy < nny; iy++) {
				pThis = this->getPointer<double>(kz, iy);
				pSave = img->getPointer<double>(kz, iy);
				for(size_t jx = 0; jx < nnx; jx++) {
					pSave[jx] = pThis[nChannels * jx];
				}
			}
		}
		return img;
	}

	Image* Image::getImagePart() {
		Image *img = new Image(this->width, this->height, this->thickness, this->depth, 1);
		const size_t nChannels = this->nChannels;

		if(nChannels == 1) {
			std::cerr << "Image have only one channel! Can not get image part!" << std::endl;
			return nullptr;
		}

		double *pThis = 0;
		double *pSave = 0;
		size_t nnx = this->width;
		size_t nny = this->height;
		size_t nnz = this->thickness;

		for(size_t kz = 0; kz < nnz; kz++) {
			for(size_t iy = 0; iy < nny; iy++) {
				pThis = this->getPointer<double>(kz, iy);
				pSave = img->getPointer<double>(kz, iy);
				for(size_t jx = 0; jx < nnx; jx++) {
					pSave[jx] = pThis[nChannels * jx + 1];
				}
			}
		}
		return img;
	}

	Image* Image::getModule() {
		Image *img = new Image(this->width, this->height, this->thickness, this->depth, 1);

		size_t nChannels = this->nChannels;
		size_t nnx = this->width;
		size_t nny = this->height;
		size_t nnz = this->thickness;

		if(nChannels == 1)
			return getRealPart();

		double *pThis = 0;
		double *pSave = 0;
		for(size_t kz = 0; kz < nnz; kz++) {
			for(size_t iy = 0; iy < nny; iy++) {
				pThis = this->getPointer<double>(kz, iy);
				pSave = img->getPointer<double>(kz, iy);
				for(size_t jx = 0; jx < nnx; jx++) {
					pSave[jx] = sqrt( pow(pThis[nChannels * jx], 2.0) + pow(pThis[nChannels * jx + 1], 2.0) );
				}
			}
		}
		return img;
	}
}