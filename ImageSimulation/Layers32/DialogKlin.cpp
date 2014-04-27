// DialogKlin.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogKlin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogKlin dialog


CDialogKlin::CDialogKlin(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogKlin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogKlin)
	m_at = FALSE;
	m_ft = FALSE;
	m_dem = 512;
	m_direct = 30;
	m_krut = 1.0;
	//}}AFX_DATA_INIT
}


void CDialogKlin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogKlin)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_DIRECT, m_direct);
	DDX_Text(pDX, IDC_KRUT, m_krut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogKlin, CDialog)
	//{{AFX_MSG_MAP(CDialogKlin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogKlin message handlers

void CDialogKlin::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta==-1)
	{
		UpdateData();
		m_dem*=2;
		if(m_dem>4096)
		{
			m_dem=4096;
		}
		UpdateData(false);
	}
	else
	{
		UpdateData();
		m_dem/=2;
		if(m_dem<512)
		{
			m_dem=512;
		}
		UpdateData(false);
	}
	*pResult = 0;
}
