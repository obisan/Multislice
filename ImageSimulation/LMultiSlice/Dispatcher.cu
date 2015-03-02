#include "stdafx.h"
#include "Dispatcher.h"
#include "ModelPotential.h"
#include "ModelSimulated.h"

Dispatcher::Dispatcher(void) {

}

Dispatcher::~Dispatcher(void) {

}

bool Dispatcher::CheckFileExist(const char *fname) {
	_finddata_t data;
	intptr_t nFind = _findfirst(fname,&data);
	if (nFind != -1) {
		// Если этого не сделать, то произойдет утечка ресурсов
		_findclose(nFind);
		return true;
	}
	return false;
}

int Dispatcher::parseCommand(const char* fileNameXML, Command& command) {
	xml_document doc;
	xml_parse_result result = doc.load_file(fileNameXML);

	if (result)
		std::cout << "XML [" << fileNameXML << "] parsed without errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n\n";
	else {
		std::cout << "XML [" << fileNameXML << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
		std::cout << "Error description: " << result.description() << "\n";
		std::cout << "Error offset: " << result.offset << " (error at [..." << (fileNameXML + result.offset) << "]\n\n";
		return -1;
	}

	char buffer[256];

	strcpy(command.fileNameInput, doc.child("action").child("io").child("fileNameInput").child_value());
	if( strlen(this->command.fileNameInput) == 0 ) {
		std::cerr << "Empty file input field!" << std::endl;
		return -1;
	}

	strcpy(command.fileNameOutput, doc.child("action").child("io").child("fileNameOutput").child_value());
	if( strlen(command.fileNameOutput) == 0 ) {
		std::cerr << "Empty file output field!" << std::endl;
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	strcpy(buffer, doc.child("action").child("image").child("nx").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty nx field!" << std::endl;
		return -1;
	} try {
		command.nx = atoi(buffer);
	} catch(...) {
		std::cerr << "Convert nx problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("image").child("ny").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty ny field!" << std::endl;
		return -1;
	} try {
		command.ny = atoi(buffer);
	} catch(...) {
		std::cerr << "Convert ny problems!" << std::endl;
		return -1;
	}
	
	strcpy(buffer, doc.child("action").child("image").child("dpa").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty dpa field!" << std::endl;
		return -1;
	} try {
		command.dpa = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert dpa problems!" << std::endl;
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	strcpy(buffer, doc.child("action").child("slicing").child("radiuc").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty radiuc field!" << std::endl;
		return -1;
	} try {
		command.radiuc = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert radiuc problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("slicing").child("numberslices").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty numberslices field!" << std::endl;
		return -1;
	} try {
		command.numberSlices = atoi(buffer);
	} catch(...) {
		std::cerr << "Convert numberslices problems!" << std::endl;
		return -1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	strcpy(buffer, doc.child("action").child("microscope").child("aperture").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty aperture field!" << std::endl;
		return -1;
	} try {
		command.aperture = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert aperture problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("microscope").child("cs").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty cs field!" << std::endl;
		return -1;
	} try {
		command.cs = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert cs problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("microscope").child("defocus").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty defocus field!" << std::endl;
		return -1;
	} try {
		command.defocus = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert defocus problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("microscope").child("keV").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty keV field!" << std::endl;
		return -1;
	} try {
		command.keV = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert keV problems!" << std::endl;
		return -1;
	}


	return 0;
}

int Dispatcher::Run(const char* fileNameXML) {
	if(!CheckFileExist(fileNameXML)) {
		std::cerr << "XML File with name [" << fileNameXML << "] doesn't exist." << std::endl;
		return -1;
	} else {
		std::cout << "XML File with name [" << fileNameXML << "] exist." << std::endl;
	}
	
	if( parseCommand(fileNameXML, command) == -1) {
		return -1;
	}

	AModel::Model *model = getModelType(command.fileNameInput);
	if( model->read(command.fileNameInput) == -1 ) {
		std::cout << "Can not read file " << command.fileNameInput << "!!!" << std::endl;
		return -1;
	} else {
		std::cout << "Read file model [" << command.fileNameInput << "] successful." << std::endl;
	} 

	/************************************************************************/
	/* Calculating map potentials	*****************************************/
	/************************************************************************/
	std::cout << std::endl;
	std::cout << "Image size		= " << command.nx << "x" << command.ny << std::endl;
	std::cout << "Number of slices	= " << command.numberSlices << std::endl;
	std::cout << "Number of atoms	= " << model->getNumberAtoms() << std::endl;
	std::cout << "dots per atom		= " << command.dpa << std::endl;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	int deviceCount = 0;
	cudaGetDeviceCount(&deviceCount);
	printf("\nDetected %d CUDA accelerators:\n", deviceCount);
	int dev;
	for (dev=0; dev < deviceCount; dev++) {
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, dev);
		printf("  [%d]: '%s'  Clock: %.1f GHz  Mem: %dMB  Rev: %d.%d\n", 
			dev, deviceProp.name, 
			deviceProp.clockRate / 1000000.0f, deviceProp.totalGlobalMem / (1024*1024),
			deviceProp.major, deviceProp.minor);
	}

	int cudadev = 0;
	printf("  Single-threaded single-GPU test run.\n");
	printf("  Opening CUDA device %d...\n\n", cudadev);
	cudaSetDevice(cudadev);
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	ModelPotential *modelPotential = new ModelPotential(model, command.nx, command.ny, command.numberSlices, command.dpa, command.radiuc);
	if(modelPotential->calculatePotentialGrid() == -1) 
		return -1;
	
	modelPotential->savePotential(command.fileNameOutput);

	ModelSimulated *modelSimulated = new ModelSimulated(modelPotential, command.nx, command.ny, command.numberSlices, command.dpa);
	Microscope *microscope = new Microscope(command.keV, command.cs, command.aperture, command.defocus);
	Image *result = new Image(command.nx, command.ny, 1, sizeof(double), 2);
	
	modelSimulated->imageCalculation(result, microscope);
	Image *result_module = result->getModule();
	result_module->saveMRC(command.fileNameOutput, model, command.nx, command.ny, 1, mrc_FLOAT);

	delete result_module;
	delete result;
	delete microscope;
	delete modelSimulated;

	delete modelPotential;
	
	delete model;

	/************************************************************************/
	/************************************************************************/
	/************************************************************************/
	cudaDeviceReset();

	std::cout	<< "Calculation for [" << fileNameXML <<  "] finished successful." << std::endl << std::endl;

	return 0;
}