// PotentialBuilder.cpp: ���������� ���������������� ������� ��� ���������� DLL.
//

#include "stdafx.h"
#include "PotentialBuilder.h"


// ������ ���������������� ����������
POTENTIALBUILDER_API int nPotentialBuilder=0;

// ������ ���������������� �������.
POTENTIALBUILDER_API int fnPotentialBuilder(void)
{
	return 42;
}

// ����������� ��� ����������������� ������.
// ��. ����������� ������ � PotentialBuilder.h
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
