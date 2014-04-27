// AtomParams.h: interface for the CAtomParams class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATOMPARAMS_H__DA9A0322_D35E_11D4_AA18_00A0C99E2AB9__INCLUDED_)
#define AFX_ATOMPARAMS_H__DA9A0322_D35E_11D4_AA18_00A0C99E2AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyImage.h"

#define ATOM_IMAGE_SIZE 10. //ang

class CAtomParams  
{
public:
	void SetXPos(double xPos);
	double GetXPos();
	void SetYPos(double xPos);
	double GetYPos();
	int GetZ();
	BOOL SetZ(int Z);
	BOOL Render();
	static double	m_dDimension;

	CMyImage<double> *m_Image;
	CAtomParams();
	virtual ~CAtomParams();
	unsigned m_bitTweenX : 1;
	unsigned m_bitTweenY : 1;

protected:
	double m_xPos,m_yPos;
	int m_Z;
	BOOL m_bNeedUpdate;
	//static CMap<int,int,CMyImage<double>*,CMyImage<double>*> m_MapOfImages;
};


#endif // !defined(AFX_ATOMPARAMS_H__DA9A0322_D35E_11D4_AA18_00A0C99E2AB9__INCLUDED_)
