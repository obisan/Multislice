#if !defined(AFX_DIALOGWAVE_H__A86B835E_CC64_4F8A_A057_3E04F73A9A6E__INCLUDED_)
#define AFX_DIALOGWAVE_H__A86B835E_CC64_4F8A_A057_3E04F73A9A6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogWave.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogWave dialog

class CDialogWave : public CDialog
{
// Construction
public:
	CDialogWave(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogWave)
	enum { IDD = IDD_WAVE };
	int		m_amp;
	BOOL	m_at;
	BOOL	m_ft;
	int		m_period;
	int		m_direct;
	int		m_dem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogWave)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogWave)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGWAVE_H__A86B835E_CC64_4F8A_A057_3E04F73A9A6E__INCLUDED_)
