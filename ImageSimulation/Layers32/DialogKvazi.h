#if !defined(AFX_DIALOGKVAZI_H__07C11951_8804_4988_87F4_D58FE75E12B9__INCLUDED_)
#define AFX_DIALOGKVAZI_H__07C11951_8804_4988_87F4_D58FE75E12B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogKvazi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogKvazi dialog

class CDialogKvazi : public CDialog
{
// Construction
public:
	CDialogKvazi(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogKvazi)
	enum { IDD = IDD_KVAZI };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_x;
	int		m_y;
	int		m_dem;
	double	m_a;
	double	m_A1;
	double	m_A2;
	double	m_b;
	double	m_c;
	double	m_f_a;
	double	m_f_A1;
	double	m_f_A2;
	double	m_f_b;
	double	m_f_c;
	double	m_y_a;
	double	m_y_A1;
	double	m_y_A2;
	double	m_y_b;
	double	m_y_c;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogKvazi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogKvazi)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGKVAZI_H__07C11951_8804_4988_87F4_D58FE75E12B9__INCLUDED_)
