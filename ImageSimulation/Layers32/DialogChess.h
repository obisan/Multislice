#if !defined(AFX_DIALOGCHESS_H__84844C33_4304_43C7_95FC_F0287187C1B8__INCLUDED_)
#define AFX_DIALOGCHESS_H__84844C33_4304_43C7_95FC_F0287187C1B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogChess.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogChess dialog

class CDialogChess : public CDialog
{
// Construction
public:
	void ShowContextHelp(CWnd* pWnd, POINT pt, int iCtrlID);
	CDialogChess(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogChess)
	enum { IDD = IDD_CHESS };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_dark;
	int		m_dem;
	int		m_light;
	int		m_period;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogChess)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogChess)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCHESS_H__84844C33_4304_43C7_95FC_F0287187C1B8__INCLUDED_)
