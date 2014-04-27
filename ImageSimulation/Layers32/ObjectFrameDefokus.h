// ObjectFrameDefokus.h: interface for the CObjectFrameDefokus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTFRAMEDEFOKUS_H__22A7A97C_E9F4_4CB1_8BD0_1D4CA453E2C6__INCLUDED_)
#define AFX_OBJECTFRAMEDEFOKUS_H__22A7A97C_E9F4_4CB1_8BD0_1D4CA453E2C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectFrame.h"

class CObjectFrameDefokus : public CObjectFrame  
{
public:
	CObjectFrameDefokus(fi_complex *arroy, int Dem,double d,double ccf,double cxp,double ap_diaf);
	virtual ~CObjectFrameDefokus();

protected:
	double m_ap_diaf;
	double m_d;
	double m_ccf;
	double m_cxp;
	int Calc();
};

#endif // !defined(AFX_OBJECTFRAMEDEFOKUS_H__22A7A97C_E9F4_4CB1_8BD0_1D4CA453E2C6__INCLUDED_)
