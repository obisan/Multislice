#if !defined(AFX_DIALOGATOMS_H__4127B839_202A_4AE8_8721_FD243ADE28A1__INCLUDED_)
#define AFX_DIALOGATOMS_H__4127B839_202A_4AE8_8721_FD243ADE28A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogAtoms.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogAtoms dialog

class CDialogAtoms : public CDialog
{
// Construction
public:
	void ShowContextHelp(CWnd* pWnd, POINT pt, int iCtrlID);
	CDialogAtoms(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAtoms)
	enum { IDD = IDD_ATOMS };
	int		m_dem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAtoms)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAtoms)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGATOMS_H__4127B839_202A_4AE8_8721_FD243ADE28A1__INCLUDED_)
