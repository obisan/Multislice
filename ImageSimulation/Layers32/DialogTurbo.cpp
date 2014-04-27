// DialogTurbo.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogTurbo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogTurbo dialog


CDialogTurbo::CDialogTurbo(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTurbo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogTurbo)
	m_at = FALSE;
	m_ft = FALSE;
	m_dem = 512;
	m_x = 0;
	m_y = 255;
	m_A = 1.0;
	m_Agam = 1.0;
	m_Afi = 0.0;
	m_B = 0.02;
	m_Bfi = 0.0;
	m_Bgam = 1.0;
	m_gamma = 1.66;
	m_gfi = 0.0;
	m_ggam = 1.0;
	m_W = 0.0;
	m_Wfi = 0.0;
	m_Wgam = 1.0;
	m_afi = 0.0;
	m_agam = 1.0;
	m_alfa = 0.9;
	//}}AFX_DATA_INIT
}


void CDialogTurbo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTurbo)
	DDX_Check(pDX, IDC_AT3, m_at);
	DDX_Check(pDX, IDC_FT3, m_ft);
	DDX_Text(pDX, IDC_DEM3, m_dem);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC__A, m_A);
	DDX_Text(pDX, IDC__Agam, m_Agam);
	DDX_Text(pDX, IDC__Afi, m_Afi);
	DDX_Text(pDX, IDC__B, m_B);
	DDX_Text(pDX, IDC__Bfi, m_Bfi);
	DDX_Text(pDX, IDC__Bgam, m_Bgam);
	DDX_Text(pDX, IDC__gamma, m_gamma);
	DDX_Text(pDX, IDC__gfi, m_gfi);
	DDX_Text(pDX, IDC__ggam, m_ggam);
	DDX_Text(pDX, IDC__W, m_W);
	DDX_Text(pDX, IDC__Wfi, m_Wfi);
	DDX_Text(pDX, IDC__Wgam, m_Wgam);
	DDX_Text(pDX, IDC_afi, m_afi);
	DDX_Text(pDX, IDC_agam, m_agam);
	DDX_Text(pDX, IDC_alfa, m_alfa);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogTurbo, CDialog)
	//{{AFX_MSG_MAP(CDialogTurbo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, OnDeltaposSpin3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogTurbo message handlers

void CDialogTurbo::OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult) 
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
