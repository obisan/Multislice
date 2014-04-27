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
