// stdafx.h: включаемый файл для стандартных системных включаемых файлов
// или включаемых файлов для конкретного проекта, которые часто используются, но
// не часто изменяются
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows:
#include <windows.h>



// TODO: Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
#include <stdio.h>
#include <tchar.h>

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <complex>

#include <time.h> 

#include <assert.h>
#include <io.h>
#include <iomanip>

#include <omp.h>

// TODO: Установите здесь ссылки на дополнительные заголовки, требующиеся для программы

#include <AModel.h>
#include <BaseStructures.h>
#include <Model.h>
#include <ModelAce.h>
#include <ModelCoo.h>

#define CUFFTW

#ifdef CUFFTW
#include <cufftw.h>
#else
#include <fftw3.h>
#endif

#include <cuda_runtime_api.h>
#include <cuda_runtime.h>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "helper_cuda.h"

#include <cuComplex.h>
#include "cusp.cuh"

#include "../pugixml-1.2/src/pugixml.hpp"

#include "../Image/Image.h"
#include "../Image/MRC.h"

using namespace ImageSpace;
using namespace pugi;

#define M_PI	3.14159265358979323846

#define CUERR { cudaError_t err; \
	if ((err = cudaGetLastError()) != cudaSuccess) { \
	printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__); \
	system("pause"); \
	return -1; }}