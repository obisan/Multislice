// ����������� ���� ���� ifdef - ��� ����������� ����� �������� ��������, ���������� ��������� 
// �������� �� ��������� DLL. ��� ����� ������ DLL �������������� � �������������� ������� POTENTIALBUILDER_EXPORTS,
// � ��������� ������. ���� ������ �� ������ ���� ��������� � �����-���� �������
// ������������ ������ DLL. ��������� ����� ����� ������ ������, ��� �������� ����� �������� ������ ����, ����� 
// ������� POTENTIALBUILDER_API ��� ��������������� �� DLL, ����� ��� ������ DLL ����� �������,
// ������������ ������ ��������, ��� ����������������.
#ifdef POTENTIALBUILDER_EXPORTS
#define POTENTIALBUILDER_API __declspec(dllexport)
#else
#define POTENTIALBUILDER_API __declspec(dllimport)
#endif

#include "Dispatcher.h"

// ���� ����� ������������� �� PotentialBuilder.dll
class POTENTIALBUILDER_API CPotentialBuilder {
public:
	CPotentialBuilder(void);
	// TODO: �������� ����� ���� ������.
};

extern POTENTIALBUILDER_API int nPotentialBuilder;

POTENTIALBUILDER_API int fnPotentialBuilder(void);
POTENTIALBUILDER_API int fnPotentialBuilderRun(int fnPotentialBuilderRun(const char *fileNameOutput, AModel::Model* model, int nx, int ny, int numberSlices, double radius, double bindim));
