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

	int		*atomCountInSlice;
	int		*atomId;
	float	(*xyz)[3];
	
private:
	AModel::Model*		modelSource;
	int					numberSlices;
	std::vector<Slice>	slices;
};