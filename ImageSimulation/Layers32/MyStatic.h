#if !defined(AFX_MYSTATIC_H__4B12DEC4_CB7F_11D4_AA18_00A0C99E2AB9__INCLUDED_)
#define AFX_MYSTATIC_H__4B12DEC4_CB7F_11D4_AA18_00A0C99E2AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyStatic.h : header file
//

#include "MyImage.h"

/////////////////////////////////////////////////////////////////////////////
// CMyStatic window

class CMyStatic : public CStatic
{
	struct MYBITMAPINFO
    {
        BITMAPINFOHEADER hdr;
        DWORD colors[256];
    } *m_binfo;

	// Construction
public:
	CMyStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyStatic)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetImage(CMyImage<double> *Image);
	double m_dSizeXRe;
	double m_dSizeYRe;
	double m_dExtraVisibleArea;
	BOOL m_bInFocus;
	virtual ~CMyStatic();

	// Generated message map functions
protected:
	UCHAR* m_BiBits;
	//{{AFX_MSG(CMyStatic)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSTATIC_H__4B12DEC4_CB7F_11D4_AA18_00A0C99E2AB9__INCLUDED_)
