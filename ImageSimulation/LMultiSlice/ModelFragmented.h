#pragma once

#include "Model.h"
#include "Slice.h"

class ModelFragmented {
private:
	ModelFragmented(void);
public:
	ModelFragmented(AModel::Model* model, int numberSlices);
	~ModelFragmented(void);

	int					dividing();

	AModel::Model*		getModelSource();
	int					getNumberSlices();
	std::vector<Slice>&	getSlices();
	Slice*				getSliceByZ(double Z);

private:
	AModel::Model*		modelSource;
	int					numberSlices;
	std::vector<Slice>	slices;
};