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
