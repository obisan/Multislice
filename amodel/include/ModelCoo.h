#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AMODEL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AMODEL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef AMODEL_EXPORTS
#define AMODEL_API __declspec(dllexport)
#else
#define AMODEL_API __declspec(dllimport)
#endif

#include "Model.h"

namespace AModel {

	class AMODEL_API ModelCoo : public Model
	{
	public:
		ModelCoo(void);
		ModelCoo(const char* filename);
		ModelCoo(const Model& matrix, const Model& insertion);
		~ModelCoo(void);

		int		read(const char* filename);
		int		readhead(const char* filename);

	private:
		
	};
}