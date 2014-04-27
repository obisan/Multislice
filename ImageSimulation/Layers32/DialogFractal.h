#if !defined(AFX_DIALOGFRACTAL_H__63D5DF3B_3D4C_42F3_A810_455C91E91684__INCLUDED_)
#define AFX_DIALOGFRACTAL_H__63D5DF3B_3D4C_42F3_A810_455C91E91684__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogFractal.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogFractal dialog

class CDialogFractal : public CDialog
{
// Construction
public:
	CDialogFractal(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogFractal)
	enum { IDD = IDD_FRACTAL };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_dem;
	double	m_h;
	int		m_x;
	int		m_y;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFractal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowContextHelp(CWnd* pWnd, POINT pt, int iCtrlID);

	// Generated message map functions
	//{{AFX_MSG(CDialogFractal)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGFRACTAL_H__63D5DF3B_3D4C_42F3_A810_455C91E91684__INCLUDED_)
