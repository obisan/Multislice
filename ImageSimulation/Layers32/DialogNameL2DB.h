#if !defined(AFX_DIALOGNAMEL2DB_H__E2F58060_403A_44FC_A5B3_26BEC054826F__INCLUDED_)
#define AFX_DIALOGNAMEL2DB_H__E2F58060_403A_44FC_A5B3_26BEC054826F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogNameL2DB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogNameL2DB dialog

class CDialogNameL2DB : public CDialog
{
// Construction
public:
	CDialogNameL2DB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogNameL2DB)
	enum { IDD = IDD_NAME_L2DB };
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogNameL2DB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogNameL2DB)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGNAMEL2DB_H__E2F58060_403A_44FC_A5B3_26BEC054826F__INCLUDED_)
