#if !defined(AFX_DIALOGKVAZI2_H__A8CECC96_689F_4B15_9E3C_BE5BCE890558__INCLUDED_)
#define AFX_DIALOGKVAZI2_H__A8CECC96_689F_4B15_9E3C_BE5BCE890558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogKvazi2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogKvazi2 dialog

class CDialogKvazi2 : public CDialog
{
// Construction
public:
	CDialogKvazi2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogKvazi2)
	enum { IDD = IDD_KVAZI2 };
	double	m_a;
	double	m_A2;
	double	m_A1;
	double	m_A3;
	double	m_alfa1;
	double	m_b;
	int		m_dem;
	BOOL	m_ft;
	int		m_x;
	int		m_y;
	double	m_c;
	double	m_alfa2;
	double	m_beta;
	BOOL	m_at;
	double	m_f_a;
	double	m_f_A1;
	double	m_f_A2;
	double	m_f_A3;
	double	m_f_alfa1;
	double	m_f_alfa2;
	double	m_f_b;
	double	m_f_beta;
	double	m_f_c;
	double	m_y_a;
	double	m_y_A1;
	double	m_y_A2;
	double	m_y_A3;
	double	m_y_alfa1;
	double	m_y_alfa2;
	double	m_y_b;
	double	m_y_beta;
	double	m_y_c;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogKvazi2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogKvazi2)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGKVAZI2_H__A8CECC96_689F_4B15_9E3C_BE5BCE890558__INCLUDED_)
