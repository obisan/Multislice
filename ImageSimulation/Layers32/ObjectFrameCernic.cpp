// ObjectFrameCernic.cpp: implementation of the CObjectFrameCernic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "layers32.h"
#include "ObjectFrameCernic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CObjectFrameCernic::CObjectFrameCernic(fi_complex *arroy, int Dem)
{
	int dxdy;
	Kvadrant=0;
	m_iDem=Dem;
	dxdy=m_iDem*m_iDem;
	zz=new fi_complex[m_iDem];
	m_bArroy=new BYTE[m_iDem*m_iDem];
	m_fArroy=new fi_complex[m_iDem*m_iDem];
	m_iMin=0;
	m_iMax=255;
	for(int i=0;i<dxdy;i++)
	{
		m_fArroy[i].re=(float)arroy[i].re;
		m_fArroy[i].im=(float)arroy[i].im;
	}
	CreatePallete();
	GetSpectrum();
}
CObjectFrameCernic::~CObjectFrameCernic()
{

}

int CObjectFrameCernic::Calc()
{
	struct Complex z, z1;
	complex(0., 1., z);
	cmul(m_fArroy[0], z, z1);
	m_fArroy[0].re = z1.re;
	m_fArroy[0].im = z1.im;
	return 0;	
}
