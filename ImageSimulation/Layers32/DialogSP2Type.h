#if !defined(AFX_DIALOGSP2TYPE_H__26BD98A2_0209_4CCC_BEE1_CBC078F9DEC9__INCLUDED_)
#define AFX_DIALOGSP2TYPE_H__26BD98A2_0209_4CCC_BEE1_CBC078F9DEC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogSP2Type.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogSP2Type dialog

class CDialogSP2Type : public CDialog
{
// Construction
public:
	CDialogSP2Type(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogSP2Type)
	enum { IDD = IDD_SP_2_TYPE };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_x;
	int		m_y;
	int		m_dem;
	double	m_a;
	double	m_A1;
	double	m_A2;
	double	m_A3;
	double	m_alfa1;
	double	m_alfa2;
	double	m_alfa3;
	double	m_b;
	double	m_beta;
	double	m_c;
	double	m_f_a;
	double	m_f_A1;
	double	m_f_A2;
	double	m_f_A3;
	double	m_f_alfa2;
	double	m_f_alfa3;
	double	m_f_b;
	double	m_f_beta;
	double	m_f_c;
	double	m_f_gamma;
	double	m_gamma;
	double	m_y_a;
	double	m_y_A1;
	double	m_y_A2;
	double	m_y_A3;
	double	m_y_alfa1;
	double	m_y_alfa2;
	double	m_y_alfa3;
	double	m_y_b;
	double	m_y_beta;
	double	m_y_c;
	double	m_y_gamma;
	double	m_f_alfa1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSP2Type)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSP2Type)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGSP2TYPE_H__26BD98A2_0209_4CCC_BEE1_CBC078F9DEC9__INCLUDED_)
