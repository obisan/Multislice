#if !defined(AFX_DIALOGTURBO_H__6B0DEF80_D7C8_4666_87A1_1CE1D0B1A2EB__INCLUDED_)
#define AFX_DIALOGTURBO_H__6B0DEF80_D7C8_4666_87A1_1CE1D0B1A2EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogTurbo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogTurbo dialog

class CDialogTurbo : public CDialog
{
// Construction
public:
	CDialogTurbo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogTurbo)
	enum { IDD = IDD_TURBO };
	BOOL	m_at;
	BOOL	m_ft;
	int		m_dem;
	int		m_x;
	int		m_y;
	double	m_A;
	double	m_Agam;
	double	m_Afi;
	double	m_B;
	double	m_Bfi;
	double	m_Bgam;
	double	m_gamma;
	double	m_gfi;
	double	m_ggam;
	double	m_W;
	double	m_Wfi;
	double	m_Wgam;
	double	m_afi;
	double	m_agam;
	double	m_alfa;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTurbo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogTurbo)
	afx_msg void OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTURBO_H__6B0DEF80_D7C8_4666_87A1_1CE1D0B1A2EB__INCLUDED_)
