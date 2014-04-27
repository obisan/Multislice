// SpectrumFrameLayers.h: interface for the CSpectrumFrameLayers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPECTRUMFRAMELAYERS_H__7B70E76A_2100_461B_8B4B_3325B5A14CDD__INCLUDED_)
#define AFX_SPECTRUMFRAMELAYERS_H__7B70E76A_2100_461B_8B4B_3325B5A14CDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SpectrumFrame.h"

class CSpectrumFrameLayers : public CSpectrumFrame  
{
public:
	
	CSpectrumFrameLayers(int dem,struct Complex *arroy);
	virtual ~CSpectrumFrameLayers();

};

#endif // !defined(AFX_SPECTRUMFRAMELAYERS_H__7B70E76A_2100_461B_8B4B_3325B5A14CDD__INCLUDED_)
