#ifndef _TEST_H_
#define _TEST_H_

#define FILENAMESAVE(filenamesave, postfix) std::string(filenamesave).insert(strlen(filenamesave) - 4, postfix).c_str()

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

	AModel::Model *model = new AModel::ModelCoo();
	if( model->read(filename) == -1 ) {
		std::cout << "Can not read file " << filename << "!!!" << std::endl;
		system("Pause");
		return;
	} else {
		std::cout << "Read file %" << filename << "% successful." << std::endl;
	} 

	std::sort(model->getTableCell(), model->getTableCell() + model->getNumberAtoms());
	
	/************************************************************************/
	/* Fragmentation														*/
	/************************************************************************/
	ModelFragmented *modelFragmented = new ModelFragmented(model, numberSlices);
	modelFragmented->dividing();
	
	/************************************************************************/
	/* Calculating map potentials											*/
	/************************************************************************/
	ModelPotential *modelPotential = new ModelPotential(modelFragmented, nx, ny, dpa, radiuc);

	std::cout << std::endl;
	std::cout << "Image size = " << nx << "x" << ny << std::endl;
	std::cout << "Number slides = " << numberSlices << std::endl;
	std::cout << "dpa = " << dpa << std::endl;
	
	Image *res		= new Image(nx, ny, numberSlices, sizeof(double), 2);
	Image *v		= new Image(nx, ny, numberSlices, sizeof(double), 2);
	Image *t		= new Image(nx, ny, numberSlices, sizeof(double), 2);
	Image *TxPhi	= new Image(nx, ny, numberSlices, sizeof(double), 2);
	
	Microscope *microscope = new Microscope(keV, Cs, aperture, defocus);

	modelPotential->calculatePotentialGrid(res, v);

	ModelSimulated *modelSimulated = new ModelSimulated(modelPotential, modelFragmented, nx, ny, dpa);
	modelSimulated->imageCalculation(res, t, TxPhi, microscope);

	res->saveMRC(filenamesave, model);
	
	char filenamesaveV[64];
	strcpy(filenamesaveV, filenamesave); strcat(filenamesaveV, "_V");
	v->saveMRC(filenamesaveV, model);

	char filenamesaveT[64];
	strcpy(filenamesaveT, filenamesave); strcat(filenamesaveT, "_T");
	t->saveMRC(filenamesaveT, model);

	char filenamesaveTxPhi[64];
	strcpy(filenamesaveTxPhi, filenamesave); strcat(filenamesaveTxPhi, "_TxPhi");
	TxPhi->saveMRC(filenamesaveTxPhi, model);
	 	
	Image *Zernike = new Image(t);
	Image::zernike(Zernike, transformationStatus::notTransformed);

	char filenamesaveZernike[64];
	strcpy(filenamesaveZernike, filenamesave); strcat(filenamesaveZernike, "_Zernike");
	Zernike->saveMRC(filenamesaveZernike, model);
	
	delete Zernike;

	delete microscope;

	delete res;
	delete v;
	delete t;
	delete TxPhi;

	delete modelFragmented;
	delete modelPotential;
	delete modelSimulated;
	
	//delete model;


	/************************************************************************/
	/************************************************************************/
	/************************************************************************/

		
	//system("D:\\LEMOI\\Multislice\\improc-deploy\\improc-gui.exe -i D:\\LEMOI\\Multislice\\samples\\oneAtomInCenter_qt.txt");
	//system("D:\\LEMOI\\Multislice\\Spectrum\\release\\Spectrum.exe  -i D:\\LEMOI\\Multislice\\samples\\oneAtomInCenter_qt.txt");
	//system("D:\\LEMOI\\Multislice\\Spectrum\\release\\Spectrum.exe  -i D:\\LEMOI\\Multislice\\samples\\C_Si_Cu_Au_U_qt_TxPhi.txt");

	std::cout	<< "test for %" << filename <<  "% finished successful." << std::endl 
				<< "//----------------------------------------------------------------//" << std::endl 
				<< std::endl;
}

#endif