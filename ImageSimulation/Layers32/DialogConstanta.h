#if !defined(AFX_DIALOGCONSTANTA_H__1C97A5E3_3089_40C7_83E9_EEFEB9A03E40__INCLUDED_)
#define AFX_DIALOGCONSTANTA_H__1C97A5E3_3089_40C7_83E9_EEFEB9A03E40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConstanta.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConstanta dialog

class CDialogConstanta : public CDialog
{
// Construction
public:
	void ShowContextHelp(CWnd* pWnd, POINT pt, int iCtrlID);
	CDialogConstanta(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogConstanta)
	enum { IDD = IDD_CONSTANTA };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_dem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConstanta)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConstanta)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONSTANTA_H__1C97A5E3_3089_40C7_83E9_EEFEB9A03E40__INCLUDED_)
