// DialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "layers32.h"
#include "DialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEx dialog


CDialogEx::CDialogEx(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogEx)
	m_at = FALSE;
	m_ft = TRUE;
	//}}AFX_DATA_INIT
}


void CDialogEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEx)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogEx, CDialog)
	//{{AFX_MSG_MAP(CDialogEx)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEx message handlers
