// DialogSP2Type.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogSP2Type.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogSP2Type dialog


CDialogSP2Type::CDialogSP2Type(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSP2Type::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSP2Type)
	m_at = FALSE;
	m_ft = FALSE;
	m_x = 0;
	m_y = 255;
	m_dem = 512;
	m_a = 1.5;
	m_A1 = 45;
	m_A2 = 20;
	m_A3 = 0;
	m_alfa1 = 7.4827;
	m_alfa2 = 34.9279;
	m_alfa3 = 0.1371;
	m_b = 3.5;
	m_beta = 1;
	m_c = 3.5;
	m_f_a = 0.0;
	m_f_A1 = 0.0;
	m_f_A2 = 0.0;
	m_f_A3 = 0.0;
	m_f_alfa2 = 0.0;
	m_f_alfa3 = 0.0;
	m_f_b = 0.0;
	m_f_beta = 0.0;
	m_f_c = 0.0;
	m_f_gamma = 0.0;
	m_gamma = 1;
	m_y_a = 1.0;
	m_y_A1 = 1.0;
	m_y_A2 = 1.0;
	m_y_A3 = 1.0;
	m_y_alfa1 = 1.0;
	m_y_alfa2 = 1.0;
	m_y_alfa3 = 1.0;
	m_y_b = 1.0;
	m_y_beta = 1.0;
	m_y_c = 1.0;
	m_y_gamma = 1.0;
	m_f_alfa1 = 0.0;
	//}}AFX_DATA_INIT
}


void CDialogSP2Type::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSP2Type)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_A, m_a);
	DDX_Text(pDX, IDC_A1_, m_A1);
	DDX_Text(pDX, IDC_A2_, m_A2);
	DDX_Text(pDX, IDC_A3_, m_A3);
	DDX_Text(pDX, IDC_alfa1_, m_alfa1);
	DDX_Text(pDX, IDC_alfa2_, m_alfa2);
	DDX_Text(pDX, IDC_alfa3_, m_alfa3);
	DDX_Text(pDX, IDC_B_, m_b);
	DDX_Text(pDX, IDC_beta_, m_beta);
	DDX_Text(pDX, IDC_C_, m_c);
	DDX_Text(pDX, IDC_F_a__, m_f_a);
	DDX_Text(pDX, IDC_F_A1__, m_f_A1);
	DDX_Text(pDX, IDC_F_A2__, m_f_A2);
	DDX_Text(pDX, IDC_F_A3__, m_f_A3);
	DDX_Text(pDX, IDC_F_alfa2__, m_f_alfa2);
	DDX_Text(pDX, IDC_F_alfa3__, m_f_alfa3);
	DDX_Text(pDX, IDC_F_b__, m_f_b);
	DDX_Text(pDX, IDC_F_beta__, m_f_beta);
	DDX_Text(pDX, IDC_F_c__, m_f_c);
	DDX_Text(pDX, IDC_F_gamma_, m_f_gamma);
	DDX_Text(pDX, IDC_gamma_, m_gamma);
	DDX_Text(pDX, IDC_Y_a__, m_y_a);
	DDX_Text(pDX, IDC_Y_A1__, m_y_A1);
	DDX_Text(pDX, IDC_Y_A2__, m_y_A2);
	DDX_Text(pDX, IDC_Y_A3__, m_y_A3);
	DDX_Text(pDX, IDC_Y_alfa1__, m_y_alfa1);
	DDX_Text(pDX, IDC_Y_alfa2__, m_y_alfa2);
	DDX_Text(pDX, IDC_Y_alfa3__, m_y_alfa3);
	DDX_Text(pDX, IDC_Y_b__, m_y_b);
	DDX_Text(pDX, IDC_Y_beta__, m_y_beta);
	DDX_Text(pDX, IDC_Y_c__, m_y_c);
	DDX_Text(pDX, IDC_Y_gamma_, m_y_gamma);
	DDX_Text(pDX, IDC_F_alfa1__, m_f_alfa1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSP2Type, CDialog)
	//{{AFX_MSG_MAP(CDialogSP2Type)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSP2Type message handlers

void CDialogSP2Type::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
