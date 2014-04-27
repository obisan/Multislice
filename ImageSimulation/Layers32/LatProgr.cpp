// LatProgr.cpp : implementation file
//

#include "stdafx.h"
#include "LatProgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLatProgr dialog


CLatProgr::CLatProgr(CWnd* pParent /*=NULL*/)
	: CDialog(CLatProgr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLatProgr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(CLatProgr::IDD,pParent);
	ShowWindow(SW_SHOW);
}


void CLatProgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLatProgr)
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLatProgr, CDialog)
	//{{AFX_MSG_MAP(CLatProgr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLatProgr message handlers

BOOL CLatProgr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ProgressCtrl.SetRange32(0,100);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLatProgr::SetRange(int min, int max)
{
	m_ProgressCtrl.SetRange32(min,max);
}

void CLatProgr::SetPos(int nPos)
{
	m_ProgressCtrl.SetPos(nPos);
}
