#if !defined(AFX_AMPL_H__B65972F7_5429_477C_ADA1_A0C36CE45F76__INCLUDED_)
#define AFX_AMPL_H__B65972F7_5429_477C_ADA1_A0C36CE45F76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Ampl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAmpl dialog

class CAmpl : public CDialog
{
// Construction
public:
	CAmpl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAmpl)
	enum { IDD = IDD_AMPLIFIRE };
	double	m_pos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmpl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAmpl)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMPL_H__B65972F7_5429_477C_ADA1_A0C36CE45F76__INCLUDED_)
