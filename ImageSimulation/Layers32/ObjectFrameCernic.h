// ObjectFrameCernic.h: interface for the CObjectFrameCernic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTFRAMECERNIC_H__5E5749E0_53BD_4AAB_B2C6_2C38F987A6DF__INCLUDED_)
#define AFX_OBJECTFRAMECERNIC_H__5E5749E0_53BD_4AAB_B2C6_2C38F987A6DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectFrame.h"

class CObjectFrameCernic : public CObjectFrame  
{
public:
	CObjectFrameCernic(fi_complex *arroy, int Dem);

	virtual ~CObjectFrameCernic();

protected:
	int Calc();
};

#endif // !defined(AFX_OBJECTFRAMECERNIC_H__5E5749E0_53BD_4AAB_B2C6_2C38F987A6DF__INCLUDED_)
