// DialogInfo.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogInfo dialog


CDialogInfo::CDialogInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogInfo)
	m_str = _T("");
	//}}AFX_DATA_INIT
}


void CDialogInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogInfo)
	DDX_Text(pDX, IDC_STATIC1, m_str);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogInfo, CDialog)
	//{{AFX_MSG_MAP(CDialogInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogInfo message handlers
