// DialogNameL2DB.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogNameL2DB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogNameL2DB dialog


CDialogNameL2DB::CDialogNameL2DB(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogNameL2DB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogNameL2DB)
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void CDialogNameL2DB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogNameL2DB)
	DDX_Text(pDX, IDC_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 500);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogNameL2DB, CDialog)
	//{{AFX_MSG_MAP(CDialogNameL2DB)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogNameL2DB message handlers
