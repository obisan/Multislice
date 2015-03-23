// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа POTENTIALBUILDER_EXPORTS,
// в командной строке. Этот символ не должен быть определен в каком-либо проекте
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции POTENTIALBUILDER_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
#ifdef POTENTIALBUILDER_EXPORTS
#define POTENTIALBUILDER_API __declspec(dllexport)
#else
#define POTENTIALBUILDER_API __declspec(dllimport)
#endif

#include "Dispatcher.h"

// Этот класс экспортирован из PotentialBuilder.dll
class POTENTIALBUILDER_API CPotentialBuilder {
public:
	CPotentialBuilder(void);
	// TODO: Добавьте здесь свои методы.
};

extern POTENTIALBUILDER_API int nPotentialBuilder;

POTENTIALBUILDER_API int fnPotentialBuilder(void);
POTENTIALBUILDER_API int fnPotentialBuilderRun(int fnPotentialBuilderRun(const char *fileNameOutput, AModel::Model* model, int nx, int ny, int numberSlices, double radius, double bindim));
