// SpectrumFrameLayers.cpp: implementation of the CSpectrumFrameLayers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "layers32.h"
#include "SpectrumFrameLayers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpectrumFrameLayers::CSpectrumFrameLayers(int dem,struct Complex *arroy)
{
	int dd;
	Kvadrant=1;
	m_iDem=dem;
	dd=dem*dem;
	m_bArroy=new BYTE[m_iDem*m_iDem];
	m_fArroy=new fi_complex[m_iDem*m_iDem];
	for(int i=0;i<dd;i++)
	{
		m_fArroy[i].re=arroy[i].re;
		m_fArroy[i].im=arroy[i].im;
	}
	m_iMin=0;
	m_iMax=255;
	m_fPos_Const=0.001;
	//m_fPos_Const=0.05;
	m_fPos=1;
	CreatePallete();
	this->ScaleView();
	//GetSpectrum();
}

CSpectrumFrameLayers::~CSpectrumFrameLayers()
{

}
