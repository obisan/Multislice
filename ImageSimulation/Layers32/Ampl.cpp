// Ampl.cpp : implementation file
//

#include "stdafx.h"
#include "layers32.h"
#include "Ampl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAmpl dialog


CAmpl::CAmpl(CWnd* pParent /*=NULL*/)
	: CDialog(CAmpl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAmpl)
	m_pos = 0.0;
	//}}AFX_DATA_INIT
}


void CAmpl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAmpl)
	DDX_Text(pDX, IDC_POS, m_pos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAmpl, CDialog)
	//{{AFX_MSG_MAP(CAmpl)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAmpl message handlers
