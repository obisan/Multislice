// ObjectFrameDefokus.cpp: implementation of the CObjectFrameDefokus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "layers32.h"
#include "ObjectFrameDefokus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
extern double DLayer;
extern double LLayer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectFrameDefokus::CObjectFrameDefokus(fi_complex *arroy, int Dem,double d,double ccf,double cxp,double ap_diaf)
{
	int dxdy;
	Kvadrant=0;
	m_d=d;
	m_ap_diaf=ap_diaf;
	m_ccf=ccf;
	m_cxp=cxp;
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

CObjectFrameDefokus::~CObjectFrameDefokus()
{

}

int CObjectFrameDefokus::Calc()
{
	int ImageSize=m_iDem;
	double Kmax,K;
	int i, j, n2 = (int)ImageSize/2;
	double k1, k2, k3, kC, ka, u, v, uv, fi;
	struct Complex z1, z2;
	double mi;
	//double step = 222./ImageSize;
	//m_d*=-1;

	k1 = (M_PI*LLayer*m_d)/(DLayer*DLayer);
	k2 = (.5*m_ccf*LLayer*LLayer*LLayer)/(DLayer*DLayer*DLayer*DLayer);
	k3=0;
	kC = .5*M_PI*M_PI*m_cxp*m_cxp*LLayer*LLayer/(DLayer*DLayer*DLayer*DLayer);


	// Обратное преобразование Фурье
	for(i=0; i<ImageSize; i++) 
	{
		if (i>=n2) u = (double)(ImageSize-i)*(ImageSize-i);
		else       u = (double)i*i;
		for(j=0; j<ImageSize; j++) 
		{
			if (j>=n2) v = (double)(ImageSize-j)*(ImageSize-j);
			else       v = (double)j*j;
			uv = u + v;
			K=m_iDem/(2*DLayer);
			if(K<0) K*=-1;
			Kmax=(m_ap_diaf*m_iDem)/K;
			
			if(sqrt(uv/pow(DLayer,2))<this->m_ap_diaf)
			{
				ka=1;
			}
			else
			{
				ka=0;
			}
			
			fi = uv*k1 + uv*uv*k2;
			cexp(fi, z1);
			z2.im = m_fArroy[i*m_iDem+j].im;
			z2.re = m_fArroy[i*m_iDem+j].re;
			cmul(z1, z2, m_fArroy[i*m_iDem+j]);
			
			//if (kC > 0.) 
			//{
				mi = ka*exp(-kC*uv*uv);
				//mi = exp(-kC*uv*uv);
				m_fArroy[i*m_iDem+j].re *= (float)mi;
				m_fArroy[i*m_iDem+j].im *= (float)mi;
			//}


		}
	}
	return 0;
}
