#pragma once

namespace PotentialBuilder {

	class Dispatcher {
	private:
		char fileNameOutput[256];

		AModel::Model *model;
		int nx;
		int ny;
		int numberSlices;
		double radius;
		double bindim;


	public:
		Dispatcher(const char *fileNameOutput, AModel::Model* model, int nx, int ny, int numberSlices, double radius, double bindim);
		~Dispatcher(void);

		int iRun();

	};

}