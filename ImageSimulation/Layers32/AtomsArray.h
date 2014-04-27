// AtomsArray.h: interface for the CAtomsArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATOMSARRAY_H__94BD1D8A_1880_11D5_AA18_00A0C99E2AB9__INCLUDED_)
#define AFX_ATOMSARRAY_H__94BD1D8A_1880_11D5_AA18_00A0C99E2AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AtomParams.h"

class CAtomsArray : public CArray<CAtomParams*,CAtomParams*>  
{
public:
	CString m_sName;
	double m_dZ;
	CAtomsArray();
	virtual ~CAtomsArray();

};

typedef CArray<CAtomsArray*,CAtomsArray*> CLayersArray;


#endif // !defined(AFX_ATOMSARRAY_H__94BD1D8A_1880_11D5_AA18_00A0C99E2AB9__INCLUDED_)
