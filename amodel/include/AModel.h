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
#include "ModelAce.h"
#include "ModelCoo.h"
#include "ModelXyz.h"

// This class is exported from the AModel.dll
class AMODEL_API CAModel {
public:
	CAModel(void);
	// TODO: add your methods here.
};

extern AMODEL_API int nAModel;

AMODEL_API int fnAModel(void);

AMODEL_API enum fileFormat {
	extentionNoneExists = -1,
	extentionAce = 1,
	extentionCoo = 2,
	extentionXyz = 3
};

AMODEL_API enum kindAtoms {
	none	= -1,
	all		= 10000
};

AMODEL_API fileFormat getFileFormat(const char*);

AMODEL_API AModel::Model* getModelType(const char* filename);