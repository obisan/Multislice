// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#include "targetver.h"

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

// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������

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

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
//#include "C:/ProgramData/NVIDIA Corporation/CUDA Samples/v6.5/common/inc/helper_cuda.h"
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