#ifndef _TEST_H_
#define _TEST_H_

#include <iostream>
#include <string>
#include <assert.h>
#include <io.h>
#include <iomanip>

#include "ModelFragmented.h"
#include "ModelPotential.h"
#include "ModelSimulated.h"

bool CheckFileExist(const char *fname) {
	_finddata_t data;
	intptr_t nFind = _findfirst(fname,&data);
	if (nFind != -1)
	{
		// Если этого не сделать, то произойдет утечка ресурсов
		_findclose(nFind);
		return true;
	}
	return false;
}

void test(const char* filename, const char* filenamesave, int numberSlices, size_t nx, size_t ny, double dpa, double radiuc, double keV, double Cs, double aperture, double defocus) {
	if(!CheckFileExist(filename)) {
		std::cerr << "File with name %" << filename << "% doesn't exist." << std::endl;
		system("Pause");
		return;
	} else {
		std::cout << "File with name%" << filename << "% exist." << std::endl;
	}

	
}

#endif