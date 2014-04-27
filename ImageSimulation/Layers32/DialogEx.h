#if !defined(AFX_DIALOGEX_H__4A1B5304_0C68_4B31_B056_FA91AAFC6098__INCLUDED_)
#define AFX_DIALOGEX_H__4A1B5304_0C68_4B31_B056_FA91AAFC6098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogEx dialog

class CDialogEx : public CDialog
{
// Construction
public:
	CDialogEx(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogEx)
	enum { IDD = IDD_EXAMPLE };
	BOOL	m_at;
	BOOL	m_ft;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogEx)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEX_H__4A1B5304_0C68_4B31_B056_FA91AAFC6098__INCLUDED_)
