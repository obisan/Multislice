#if !defined(AFX_MODELFRAME_H__CB3B67E1_8F0A_11D4_A6D4_00A0C99E2B24__INCLUDED_)
#define AFX_MODELFRAME_H__CB3B67E1_8F0A_11D4_A6D4_00A0C99E2B24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelFrame.h : header file
//
#include "layers32.h"
/////////////////////////////////////////////////////////////////////////////
// CModelFrame frame

class CModelFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CModelFrame)
protected:

// Attributes
public:
	struct common_datas m_cd;
	LPBITMAPINFO Pallete;
	//CChildView m_wndView;
	int m_iDem;
	BYTE *m_bArroy;

	CModelFrame();           // protected constructor used by dynamic creation

// Operations
public:
	void ReGenerationScale();
	void NewArroy(int i);
	MINMAXINFO FAR* lpMinMaxInfo;
	
	virtual int GetModel();
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	int xPos;
	int yPos;
	int m_sX;
	int m_sY;
	int m_Limited;
	void SaveBmp();
	int m_type;
	virtual ~CModelFrame();

	// Generated message map functions
	//{{AFX_MSG(CModelFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnSaveSpectrBmp();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnL2db();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELFRAME_H__CB3B67E1_8F0A_11D4_A6D4_00A0C99E2B24__INCLUDED_)
