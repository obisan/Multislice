#include "stdafx.h"
#include "Dispatcher.h"
//#include "ModelPotential.h"
#include "ModelSimulated.h"

Dispatcher::Dispatcher(void) {

}

Dispatcher::~Dispatcher(void) {

}

bool Dispatcher::isFileExist(const char *fname) {
	_finddata_t data;
	intptr_t nFind = _findfirst(fname,&data);
	if (nFind != -1) {
		// Если этого не сделать, то произойдет утечка ресурсов
		_findclose(nFind);
		return true;
	}
	return false;
}

bool Dispatcher::isDirectoryExist(const char* dirname) {
	struct stat statbuf;
	if(stat(dirname,&statbuf)) {
		return false;
	}
	return true;
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
		std::cerr << "Empty \"file input\" field!" << std::endl;
		return -1;
	}

	strcpy(command.potentialDirectory, doc.child("action").child("io").child("fileNameOutput").child_value());
	if( strlen(command.fileNameOutput) == 0 ) {
		std::cerr << "Empty \"file output\" field!" << std::endl;
		return -1;
	}

	strcpy(command.fileNameOutput, doc.child("action").child("io").child("PotentialDirectory").child_value());
	if( strlen(command.potentialDirectory) == 0 ) {
		std::cerr << "Empty \"Potential Directory\" field!" << std::endl;
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	strcpy(buffer, doc.child("action").child("image").child("nx").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"nx\" field!" << std::endl;
		return -1;
	} try {
		command.nx = atoi(buffer);
	} catch(...) {
		std::cerr << "Convert \"nx\" problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("image").child("ny").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"ny\" field!" << std::endl;
		return -1;
	} try {
		command.ny = atoi(buffer);
	} catch(...) {
		std::cerr << "Convert \"ny\" problems!" << std::endl;
		return -1;
	}
	
	strcpy(buffer, doc.child("action").child("image").child("dpa").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"dpa\" field!" << std::endl;
		return -1;
	} try {
		command.dpa = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert \"dpa\" problems!" << std::endl;
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	strcpy(buffer, doc.child("action").child("slicing").child("radius").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"radius\" field!" << std::endl;
		return -1;
	} try {
		command.radius = (float) atof(buffer);

		if(command.radius > 20.0f) {
			std::cerr << "Radius is too big, it was changed on 20A!" << std::endl;
			command.bindim = 20.0f;
		}
	} catch(...) {
		std::cerr << "Convert \"radius\" problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("slicing").child("bindim").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"binsize\" field!" << std::endl;
		return -1;
	} try {
		command.bindim = atoi(buffer);

		if(command.bindim < 10.0f) {
			std::cerr << "Bin size too small, it was changed on 10A!" << std::endl;
			command.bindim = 10.0f;
		}
	} catch(...) {
		std::cerr << "Convert \"bindim\" problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("slicing").child("numberslices").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"numberslices\" field!" << std::endl;
		return -1;
	} try {
		command.numberSlices = atoi(buffer);
	} catch(...) {
		std::cerr << "Convert \"numberslices\" problems!" << std::endl;
		return -1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	strcpy(buffer, doc.child("action").child("microscope").child("aperture").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"aperture\" field!" << std::endl;
		return -1;
	} try {
		command.aperture = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert \"aperture\" problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("microscope").child("cs").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"cs\" field!" << std::endl;
		return -1;
	} try {
		command.cs = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert \"cs\" problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("microscope").child("defocus").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"defocus\" field!" << std::endl;
		return -1;
	} try {
		command.defocus = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert \"defocus\" problems!" << std::endl;
		return -1;
	}

	strcpy(buffer, doc.child("action").child("microscope").child("keV").child_value() );
	if( strlen(buffer) == 0 ) {
		std::cerr << "Empty \"keV\" field!" << std::endl;
		return -1;
	} try {
		command.keV = (float) atof(buffer);
	} catch(...) {
		std::cerr << "Convert \"keV\" problems!" << std::endl;
		return -1;
	}


	return 0;
}

int Dispatcher::Run(const char* fileNameXML) {
	if(!isFileExist(fileNameXML)) {
		std::cerr << "XML File with name [" << fileNameXML << "] doesn't exist." << std::endl;
		return -1;
	} else {
		std::cout << "XML File with name [" << fileNameXML << "] exist." << std::endl;
	}
	
	if( parseCommand(fileNameXML, command) == -1) {
		return -1;
	}
	
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

	AModel::Model *model = getModelType(command.fileNameInput);
	
	if(!isDirectoryExist(command.potentialDirectory)) {
		if( model->read(command.fileNameInput) == -1 ) {
			std::cout << "Can not read file [" << command.fileNameInput << "] !!!" << std::endl;
			return -1;
		} else {
			std::cout << "Read file model [" << command.fileNameInput << "] successful." << std::endl;
		} 

		//////////////////////////////////////////////////////////////////////////
		// Calculating map potentials	//////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		PotentialBuilder::ModelPotential *modelPotential 
			= new PotentialBuilder::ModelPotential(model, command.nx, command.ny, command.numberSlices, command.radius, command.bindim, command.potentialDirectory);
		if(modelPotential->calculatePotentialGrid() == -1) 
			return -1;
		modelPotential->savePotentialStack(command.fileNameOutput, command.potentialDirectory);
		delete modelPotential;
	} else {
		if( model->readhead(command.fileNameInput) == -1 ) {
			std::cout << "Can not read file [" << command.fileNameInput << "] !!!" << std::endl;
			return -1;
		} else {
			std::cout << "Read file model [" << command.fileNameInput << "] successful." << std::endl;
		} 
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	std::cout << std::endl;
	std::cout << "Image size		= " << command.nx << "x" << command.ny << std::endl;
	std::cout << "Number of slices	= " << command.numberSlices << std::endl;
	std::cout << "Number of atoms	= " << model->getNumberAtoms() << std::endl;
	std::cout << "Dots per atom		= " << command.dpa << std::endl;
	

	ModelSimulated *modelSimulated = new ModelSimulated(command.potentialDirectory, model, command.nx, command.ny, command.numberSlices, command.dpa);
	Microscope *microscope = new Microscope(command.keV, command.cs, command.aperture, command.defocus);
	Image *result = new Image(command.nx, command.ny, 1, sizeof(double), 2);
	
	modelSimulated->imageCalculation(result, microscope);
	Image *result_module = result->getModule();
	result_module->saveMRC(command.fileNameOutput, model, command.nx, command.ny, 1, mrc_FLOAT);

	delete result_module;
	delete result;
	delete microscope;
	delete modelSimulated;

	delete model;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	cudaDeviceReset();

	std::cout	<< "Calculation for [" << fileNameXML <<  "] finished successful." << std::endl << std::endl;

	return 0;
}