#if !defined(AFX_LATPROGR_H__82F6AEFB_3F17_11D5_A3EE_C06F64C100D1__INCLUDED_)
#define AFX_LATPROGR_H__82F6AEFB_3F17_11D5_A3EE_C06F64C100D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LatProgr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLatProgr dialog
#include "resource.h"
class CLatProgr : public CDialog
{
// Construction
public:
	void SetPos(int nPos);
	void SetRange(int min,int max);
	CLatProgr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLatProgr)
	enum { IDD = IDD_LATDLG_PROGRESS };
	CProgressCtrl	m_ProgressCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLatProgr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLatProgr)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LATPROGR_H__82F6AEFB_3F17_11D5_A3EE_C06F64C100D1__INCLUDED_)
