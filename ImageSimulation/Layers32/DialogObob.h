#if !defined(AFX_DIALOGOBOB_H__45D7239E_A4B7_4F6B_8919_FC80926D9CAF__INCLUDED_)
#define AFX_DIALOGOBOB_H__45D7239E_A4B7_4F6B_8919_FC80926D9CAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogObob.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogObob dialog

class CDialogObob : public CDialog
{
// Construction
public:
	CDialogObob(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogObob)
	enum { IDD = IDD_OBOB };
	int		m_dem;
	BOOL	m_ft;
	BOOL	m_at;
	double	m_a;
	double	m_A1;
	double	m_A2;
	double	m_al;
	double	m_be;
	double	m_f_a;
	double	m_f_A1;
	double	m_f_al;
	double	m_f_A2;
	double	m_f_be;
	double	m_f_n;
	double	m_n;
	int		m_x;
	int		m_y;
	double	m_y_a;
	double	m_y_A1;
	double	m_y_A2;
	double	m_y_al;
	double	m_y_be;
	double	m_y_n;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogObob)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogObob)
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGOBOB_H__45D7239E_A4B7_4F6B_8919_FC80926D9CAF__INCLUDED_)
