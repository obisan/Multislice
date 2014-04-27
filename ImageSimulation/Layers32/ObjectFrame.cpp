// ObjectFrame.cpp: implementation of the CObjectFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "aber32.h"
//#include "MDI.h"

#include "ObjectFrame.h"
#include "SpectrumFrame.h"

//extern CMDIApp theApp;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CObjectFrame::CObjectFrame()
{
	::CSpectrumFrame();
	//m_fPos_Const=0.001;
	m_fPos_Const=0.05;
}

CObjectFrame::~CObjectFrame()
{
	delete[] zz;
}

void CObjectFrame::Fft()
{
	fftw_complex *in=new fftw_complex[m_iDem*m_iDem];
	int dxdy=m_iDem*m_iDem;//,j;
	m_fPos_Const=0.0003;
	//////
	//long u;
	
	for(int i=0;i<dxdy;i++)
	{
		
		in[i].re=m_fArroy[i].re;
		in[i].im=m_fArroy[i].im;
	}
	fftwnd_plan p;
	p = fftw2d_create_plan(m_iDem,m_iDem, FFTW_BACKWARD,FFTW_ESTIMATE|FFTW_IN_PLACE);
	fftwnd_one(p,in, NULL);
	fftwnd_destroy_plan(p);
	
	for(int i=0;i<dxdy;i++)
	{
		m_fArroy[i].re=(float)in[i].re;
		m_fArroy[i].im=(float)in[i].im;
	}
	delete[] in;
	OptimizeScale();
	ScaleView();
}

void CObjectFrame::GetSpectrum()
{
	//Preparation();
	Calc();
	Fft();
}

CObjectFrame::CObjectFrame(fi_complex *arroy, int Dem)
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




int CObjectFrame::Calc()
{
	return 0;
}
