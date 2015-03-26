// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа IMAGE_EXPORTS,
// в командной строке. Этот символ не должен быть определен в каком-либо проекте
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции IMAGE_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.

#pragma once

#ifdef IMAGE_EXPORTS
#define IMAGE_API __declspec(dllexport)
#else
#define IMAGE_API __declspec(dllimport)
#endif

// Этот класс экспортирован из Image.dll
// class IMAGE_API CImage {
// public:
// 	CImage(void);
// 	// TODO: Добавьте здесь свои методы.
// };
// 
// extern IMAGE_API int nImage;
// 
// IMAGE_API int fnImage(void);

#include "otherEnums.h"

namespace ImageSpace {
	class IMAGE_API Image {
	public:
		Image();
		Image(size_t nx, size_t ny, size_t nz, size_t depth, size_t nChannels);
		Image(size_t nx, size_t ny, size_t depth, size_t nChannels);
		Image(const Image* image);
		~Image();

		size_t              nSize;			// sizeof(IplImage)
		int                 nChannels;		// Число каналов
		int                 depth;			// Глубина в битах
		int                 width;			// Ширина
		int                 height;			// Высота
		int					thickness;		// Толщина
		char*				imageData;		// Данные изображения
		int                 widthStep;		// Число байт в одной строке изображения

		template<class T> T* getPointer(size_t slide, size_t line) {
			return (T*) (this->imageData + nChannels * width * height * sizeof(T) * slide + nChannels * widthStep * sizeof(T) * line);
		}

		static void zernike(Image* image, int transformationStatus);
		static void normalize(Image *image, int slide, int n);

		void		save(const char*);
		int			saveMRC(const char* filenamesave, AModel::Model* model, int width, int height, int thickness, int mode);
		int			saveStackMRC(const char* filenamesave, const char* StackDirectory, AModel::Model* model, int width, int height, int thickness, int mode);
		void		saveQtSpectrum(const char*, size_t n = 0);

		Image*		getRealPart();
		Image*		getImagePart();
		Image*		getModule();

		template<class T> static void copyFFTtoImage(Image* image, fftw_complex* pfftw, int slide) {
			size_t nChannels = image->nChannels;
			size_t nx = image->width;
			size_t ny = image->height;
			for(size_t i = 0; i < ny; i++) {
				T* pImage = image->getPointer<T>(slide, i);
				for(size_t j = 0; j < nx; j++) {
					pImage[nChannels * j + 0]	= pfftw[nx * i + j][0];
					pImage[nChannels * j + 1]	= pfftw[nx * i + j][1];
				}
			}
		}

		template<class T> static void copyImagetoFFT(Image* image, fftw_complex* pfftw, int slide) {
			size_t nChannels = image->nChannels;
			size_t nx = image->width;
			size_t ny = image->height;
			for(size_t i = 0; i < ny; i++) {
				T* pImage = image->getPointer<T>(slide, i);
				for(size_t j = 0; j < nx; j++) {
					pfftw[nx * i + j][0] = pImage[nChannels * j + 0];
					pfftw[nx * i + j][1] = pImage[nChannels * j + 1];
				}
			}
		}

	};
}