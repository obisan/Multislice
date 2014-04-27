#if !defined(AFX_DIALOGANIZ_H__3A7CF16D_65D0_48AF_AE60_8792A560AC75__INCLUDED_)
#define AFX_DIALOGANIZ_H__3A7CF16D_65D0_48AF_AE60_8792A560AC75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogAniz.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogAniz dialog

class CDialogAniz : public CDialog
{
// Construction
public:
	void ShowContextHelp(CWnd* pWnd, POINT pt, int iCtrlID);
	CDialogAniz(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAniz)
	enum { IDD = IDD_ANIZOTROP };
	BOOL	m_ft;
	BOOL	m_at;
	int		m_dem;
	int		m_x;
	int		m_y;
	double	m_a;
	double	m_al1;
	double	m_al2;
	double	m_b;
	double	m_be1;
	double	m_be2;
	double	m_c;
	double	m_d;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAniz)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAniz)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGANIZ_H__3A7CF16D_65D0_48AF_AE60_8792A560AC75__INCLUDED_)
