// LatGen.h: interface for the CLatGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LATGEN_H__E6B76FE1_CAC0_11D4_AA18_00A0C99E2AB9__INCLUDED_)
#define AFX_LATGEN_H__E6B76FE1_CAC0_11D4_AA18_00A0C99E2AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MyImage.h"
#include "AtomsArray.h"
#include "SpectrumFrame.h"

struct SRenderParam
{
	SRenderParam()
	{
		m_dLambda=0.037;
		m_dSigma=0.8486;
	}
	fi_complex *m_pfi_complex;
	int m_nWidth;
	int m_nHeight;
	double m_dLambda;
	double m_dSigma;
	double m_dWidthAng;
};

class CLatGenerator  
{
public:
	BOOL RenderLayer(SRenderParam *RenderParam);
	CLatGenerator();
	virtual ~CLatGenerator();
	BOOL ShowDialog();

protected:
	int m_nNumLayers;
	double m_dDimension;
	BOOL m_bGenerate;
	double	m_dSizeXRe;
	double	m_dSizeYRe;
	double  m_dMaxAtomOffs;
	double  m_dMaxCellOffs;
	CLayersArray m_LayersArray;
};

#endif // !defined(AFX_LATGEN_H__E6B76FE1_CAC0_11D4_AA18_00A0C99E2AB9__INCLUDED_)
