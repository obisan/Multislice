#if !defined(AFX_SPECTRUMFRAME_H__0CAE1761_907B_11D4_A6D4_00A0C99E2B24__INCLUDED_)
#define AFX_SPECTRUMFRAME_H__0CAE1761_907B_11D4_A6D4_00A0C99E2B24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpectrumFrame.h : header file
//

#include <math.h>

#include "ModelFrame.h"
#include "Ampl.h"

#define KOS 0001
#define PAM 0002

struct fi_complex {
	float re;
	float im;
};

/////////////////////////////////////////////////////////////////////////////
// CSpectrumFrame frame

class CSpectrumFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSpectrumFrame)
protected:
	virtual void GetSpectrum(BYTE *arroy);

// Attributes
public:
	CSpectrumFrame();           // protected constructor used by dynamic creation
	CSpectrumFrame(BYTE *arroy,int Dem);           // protected constructor used by dynamic creation
	CSpectrumFrame(int dem,struct Complex *arroy);
	virtual ~CSpectrumFrame();

// Operations
public:
	void GetOptimumCoefficient();
	double m_apertura;
	double m_length_wave;
	int m_type_spectr;
	int m_type_plant;
	int Kvadrant;
	void ScaleView();
	double m_fPos_Const;
	void FindMinMax(BYTE *arroy);
	int m_iMax;
	
	MINMAXINFO FAR* lpMinMaxInfo;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectrumFrame)
	public:
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ReGenerationScale();
	int xPos;
	int yPos;
	int m_sX;
	int m_sY;
	int m_Limited;
	////
	void SaveBmp();
	void SaveSp();
	void OptimizeScale();
	
	
	double m_fPredel;
	double m_fPos;
	virtual void Fft(BYTE *arroy);
	int m_iMin;
	
	void CreatePallete();
	LPBITMAPINFO Pallete;
	struct fi_complex *m_fArroy;
	BYTE *m_bArroy;
	int m_iDem;
	CAmpl Dialog;
	
	

	// Generated message map functions
	//{{AFX_MSG(CSpectrumFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSaveSpectrSp();
	afx_msg void OnSaveSpectrBmp();
	afx_msg void OnCoeffS();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCernic();
	afx_msg void OnDefokus();
	afx_msg void OnAber();
	afx_msg void OnL2db();
	afx_msg void OnUpdateL2db(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRUMFRAME_H__0CAE1761_907B_11D4_A6D4_00A0C99E2B24__INCLUDED_)
