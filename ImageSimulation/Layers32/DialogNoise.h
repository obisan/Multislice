#if !defined(AFX_DIALOGNOISE_H__EB240DE2_AAD7_4CD6_87CB_C2E1BB55C629__INCLUDED_)
#define AFX_DIALOGNOISE_H__EB240DE2_AAD7_4CD6_87CB_C2E1BB55C629__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogNoise.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogNoise dialog

class CDialogNoise : public CDialog
{
// Construction
public:
	CDialogNoise(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogNoise)
	enum { IDD = IDD_NOISE };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_dem;
	int		m_max;
	int		m_min;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogNoise)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogNoise)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGNOISE_H__EB240DE2_AAD7_4CD6_87CB_C2E1BB55C629__INCLUDED_)
