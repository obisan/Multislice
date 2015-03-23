#include "stdafx.h"
#include "Dispatcher.h"
#include "ModelPotential.h"

namespace PotentialBuilder {
	Dispatcher::Dispatcher(const char *fileNameOutput, AModel::Model* model, int nx, int ny, int numberSlices, double radius, double bindim) {
		strcpy(this->fileNameOutput, fileNameOutput);
		this->model = model;
		this->nx = nx;
		this->ny = ny;
		this->numberSlices = numberSlices;
		this->radius = radius;
		this->bindim = bindim;
	}

	Dispatcher::~Dispatcher(void) {
		if(model != nullptr) delete model;
		model = nullptr;
	}

	int Dispatcher::iRun() {
		ModelPotential *a = new ModelPotential(model, nx, ny, numberSlices, radius, bindim);
		int err = a->calculatePotentialGrid();
		if(err == -1) return err;

		a->savePotential(fileNameOutput);

		delete a;
		return err;
	}
}