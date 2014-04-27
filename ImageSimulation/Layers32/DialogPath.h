#if !defined(AFX_DIALOGPATH_H__46F73AE9_10A4_4B36_90FD_827B06DE40E6__INCLUDED_)
#define AFX_DIALOGPATH_H__46F73AE9_10A4_4B36_90FD_827B06DE40E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogPath.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogPath dialog

class CDialogPath : public CDialog
{
// Construction
public:
	CDialogPath(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPath)
	enum { IDD = IDD_PATH };
	double	m_apertura;
	int		m_count;
	double	m_layers;
	double	m_wave;
	double	m_formula;
	double	m_modul;
	BOOL	m_kos;
	BOOL	m_pam;
	
	double	m_aperture_pam;
	int		m_count_pam;
	double	m_layers_pam;
	double	m_modul_pam;
	double	m_napr_pam;
	double	m_wave_pam;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPath)
	afx_msg void OnUp();
	afx_msg void OnKos();
	afx_msg void OnPam();
	afx_msg void OnChangeApertura();
	afx_msg void OnChangeNaprPam();
	afx_msg void OnChangeWavePam();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPATH_H__46F73AE9_10A4_4B36_90FD_827B06DE40E6__INCLUDED_)
