#if !defined(AFX_DIALOGCIRCLE_H__D2583490_8348_444E_B5D7_244AA9F6A404__INCLUDED_)
#define AFX_DIALOGCIRCLE_H__D2583490_8348_444E_B5D7_244AA9F6A404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogCircle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogCircle dialog

class CDialogCircle : public CDialog
{
// Construction
public:
	void ShowContextHelp(CWnd* pWnd, POINT pt, int iCtrlID);
	CDialogCircle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogCircle)
	enum { IDD = IDD_CIRCLE };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_count;
	int		m_dem;
	int		m_diametr;
	int		m_inten_circle;
	int		m_inten_fon;
	int		m_ton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogCircle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogCircle)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCIRCLE_H__D2583490_8348_444E_B5D7_244AA9F6A404__INCLUDED_)
