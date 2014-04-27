#if !defined(AFX_DIALOGRECT_H__885049DF_0761_4DD4_8DB1_F8D880CC1127__INCLUDED_)
#define AFX_DIALOGRECT_H__885049DF_0761_4DD4_8DB1_F8D880CC1127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogRect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogRect dialog

class CDialogRect : public CDialog
{
// Construction
public:
	CDialogRect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRect)
	enum { IDD = IDD_RECT };
	BOOL	m_ft;
	BOOL	m_at;
	int		m_count;
	int		m_dem;
	int		m_demrect;
	int		m_fon;
	int		m_rect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRect)
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGRECT_H__885049DF_0761_4DD4_8DB1_F8D880CC1127__INCLUDED_)
