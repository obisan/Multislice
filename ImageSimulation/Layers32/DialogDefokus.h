#if !defined(AFX_DIALOGDEFOKUS_H__87B99D35_AC2B_4BF1_A36E_81FB02FF3957__INCLUDED_)
#define AFX_DIALOGDEFOKUS_H__87B99D35_AC2B_4BF1_A36E_81FB02FF3957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDefokus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDefokus dialog

class CDialogDefokus : public CDialog
{
// Construction
public:
	int m_iDem;
	void UpdateD();
	double m_length_wave;
	double m_apertura;
	CDialogDefokus(CWnd* pParent = NULL);   // standard constructor
	double m_D;
	double m_k_ap_diaf;

// Dialog Data
	//{{AFX_DATA(CDialogDefokus)
	enum { IDD = IDD_DEFOKUS };
	CStatic	m_graf;
	double	m_Ccf;
	double	m_Cxp;
	BOOL	m_cos;
	BOOL	m_sin;
	int		m_n;
	double	m_D_for_view;
	double	m_ap_diaf_view;
	BOOL	m_znak_def;
	BOOL	m_auto_defoc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDefokus)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_shift;
	
	int m_pos_prob;
	double m_delta;
	
	double m_aSin[460];
	double m_aCos[460];
	
	void Function();

	// Generated message map functions
	//{{AFX_MSG(CDialogDefokus)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSin();
	afx_msg void OnCos();
	afx_msg void OnRedraw();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAutoDefoc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDEFOKUS_H__87B99D35_AC2B_4BF1_A36E_81FB02FF3957__INCLUDED_)
