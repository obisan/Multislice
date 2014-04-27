#if !defined(AFX_DIALOGKLIN_H__B7629DFE_AF35_4B44_B8AB_19CB13CE0AD7__INCLUDED_)
#define AFX_DIALOGKLIN_H__B7629DFE_AF35_4B44_B8AB_19CB13CE0AD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogKlin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogKlin dialog

class CDialogKlin : public CDialog
{
// Construction
public:
	CDialogKlin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogKlin)
	enum { IDD = IDD_KLIN };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_dem;
	int		m_direct;
	double	m_krut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogKlin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogKlin)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGKLIN_H__B7629DFE_AF35_4B44_B8AB_19CB13CE0AD7__INCLUDED_)
