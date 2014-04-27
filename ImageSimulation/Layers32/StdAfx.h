// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DBA30A29_0662_479C_B566_79DB76EFB871__INCLUDED_)
#define AFX_STDAFX_H__DBA30A29_0662_479C_B566_79DB76EFB871__INCLUDED_

#define _WIN32_WINNT 0x601

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//add me
#include <afxtempl.h>
#include <afxpriv.h>
#include <math.h>
#include <stdio.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "AtomsArray.h"
#include <fftw.h>


#endif // !defined(AFX_STDAFX_H__DBA30A29_0662_479C_B566_79DB76EFB871__INCLUDED_)
