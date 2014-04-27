// DialogKvazi2.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogKvazi2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogKvazi2 dialog


CDialogKvazi2::CDialogKvazi2(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogKvazi2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogKvazi2)
	m_a = 1.5;
	m_A2 = 60;
	m_A1 = 10.0;
	m_A3 = 0.0;
	m_alfa1 = 3.0;
	m_b = 0.75;
	m_dem = 512;
	m_ft = FALSE;
	m_x = 0;
	m_y = 255;
	m_c = 0.75;
	m_alfa2 = 7.0;
	m_beta = 40.0;
	m_at = FALSE;
	m_f_a = 0.0;
	m_f_A1 = 0.0;
	m_f_A2 = 0.0;
	m_f_A3 = 0.0;
	m_f_alfa1 = 0.0;
	m_f_alfa2 = 0.0;
	m_f_b = 0.0;
	m_f_beta = 0.0;
	m_f_c = 0.0;
	m_y_a = 1.0;
	m_y_A1 = 1.0;
	m_y_A2 = 1.0;
	m_y_A3 = 1.0;
	m_y_alfa1 = 1.0;
	m_y_alfa2 = 1.0;
	m_y_b = 1.0;
	m_y_beta = 1.0;
	m_y_c = 1.0;
	//}}AFX_DATA_INIT
}


void CDialogKvazi2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogKvazi2)
	DDX_Text(pDX, IDC_A, m_a);
	DDX_Text(pDX, IDC_A2, m_A2);
	DDX_Text(pDX, IDC_A1, m_A1);
	DDX_Text(pDX, IDC_A3, m_A3);
	DDX_Text(pDX, IDC_alfa1, m_alfa1);
	DDX_Text(pDX, IDC_B, m_b);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_C, m_c);
	DDX_Text(pDX, IDC_alfa2, m_alfa2);
	DDX_Text(pDX, IDC_beta, m_beta);
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Text(pDX, IDC_F_a_l, m_f_a);
	DDX_Text(pDX, IDC_F_A1, m_f_A1);
	DDX_Text(pDX, IDC_F_A2_, m_f_A2);
	DDX_Text(pDX, IDC_F_A3, m_f_A3);
	DDX_Text(pDX, IDC_F_alf1, m_f_alfa1);
	DDX_Text(pDX, IDC_F_alfa2, m_f_alfa2);
	DDX_Text(pDX, IDC_F_b_, m_f_b);
	DDX_Text(pDX, IDC_F_beta, m_f_beta);
	DDX_Text(pDX, IDC_F_c_, m_f_c);
	DDX_Text(pDX, IDC_Y_a_l, m_y_a);
	DDX_Text(pDX, IDC_Y_A1, m_y_A1);
	DDX_Text(pDX, IDC_Y_A2_, m_y_A2);
	DDX_Text(pDX, IDC_Y_A3, m_y_A3);
	DDX_Text(pDX, IDC_Y_alfa1, m_y_alfa1);
	DDX_Text(pDX, IDC_Y_alfa2, m_y_alfa2);
	DDX_Text(pDX, IDC_Y_b_, m_y_b);
	DDX_Text(pDX, IDC_Y_beta, m_y_beta);
	DDX_Text(pDX, IDC_Y_c_, m_y_c);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogKvazi2, CDialog)
	//{{AFX_MSG_MAP(CDialogKvazi2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogKvazi2 message handlers

void CDialogKvazi2::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
