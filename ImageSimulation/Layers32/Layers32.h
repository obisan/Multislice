// Layers32.h : main header file for the LAYERS32 application
//

#if !defined(AFX_LAYERS32_H__56BD0749_8AC6_4EE2_B05A_F9DBDC33E153__INCLUDED_)
#define AFX_LAYERS32_H__56BD0749_8AC6_4EE2_B05A_F9DBDC33E153__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "other.h"
#include "Interfaces.h"
#include "htmlhelp.h"

/////////////////////////////////////////////////////////////////////////////
// CLayers32App:
// See Layers32.cpp for the implementation of this class
//

//////////////////////////////////////////////
//////////////////////////////////////////////
class CLayers32App : public CWinApp
{
public:
	CLayers32App();
	IDatabase*	i_database;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayers32App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	

public:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;
	//{{AFX_MSG(CLayers32App)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERS32_H__56BD0749_8AC6_4EE2_B05A_F9DBDC33E153__INCLUDED_)
