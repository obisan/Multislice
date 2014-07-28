// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>

#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <map>

// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������

#include <cuda_runtime_api.h>
#include <cuda_runtime.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define CUFFTW

#ifdef CUFFTW
	#include <cufftw.h>
#else
	#include <fftw3.h>
#endif

#include <AModel.h>
#include <BaseStructures.h>
#include <Model.h>
#include <ModelAce.h>
#include <ModelCoo.h>
