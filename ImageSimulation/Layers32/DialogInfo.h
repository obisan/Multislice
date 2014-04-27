#if !defined(AFX_DIALOGINFO_H__5D297D95_7685_4251_817C_2853D5A5C0BD__INCLUDED_)
#define AFX_DIALOGINFO_H__5D297D95_7685_4251_817C_2853D5A5C0BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogInfo dialog

class CDialogInfo : public CDialog
{
// Construction
public:
	CDialogInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogInfo)
	enum { IDD = IDD_INFO };
	CString	m_str;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGINFO_H__5D297D95_7685_4251_817C_2853D5A5C0BD__INCLUDED_)
