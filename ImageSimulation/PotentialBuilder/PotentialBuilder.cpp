// PotentialBuilder.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "PotentialBuilder.h"


// Пример экспортированной переменной
POTENTIALBUILDER_API int nPotentialBuilder=0;

// Пример экспортированной функции.
POTENTIALBUILDER_API int fnPotentialBuilder(void)
{
	return 42;
}

// Конструктор для экспортированного класса.
// см. определение класса в PotentialBuilder.h
CPotentialBuilder::CPotentialBuilder()
{
	return;
}

int fnPotentialBuilderRun(const char *fileNameInput, const char *fileNameOutput, AModel::Model* model, int nx, int ny, int numberSlices, double radius, double bindim) {
	PotentialBuilder::Dispatcher *a = new PotentialBuilder::Dispatcher(fileNameOutput, model, nx, ny, numberSlices, radius, bindim);
	int err = a->iRun();
	delete a;
	return err;
}
