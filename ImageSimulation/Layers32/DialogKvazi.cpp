// DialogKvazi.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogKvazi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogKvazi dialog


CDialogKvazi::CDialogKvazi(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogKvazi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogKvazi)
	m_at = FALSE;
	m_ft = FALSE;
	m_x = 0;
	m_y = 255;
	m_dem = 512;
	m_a = 2.89;
	m_A1 = 25.35;
	m_A2 = 246.76;
	m_b = 6.168;
	m_c = 36.72;
	m_f_a = 0.0;
	m_f_A1 = 0.0;
	m_f_A2 = 0.0;
	m_f_b = 0.0;
	m_f_c = 0.0;
	m_y_a = 1.0;
	m_y_A1 = 1.0;
	m_y_A2 = 1.0;
	m_y_b = 1.0;
	m_y_c = 1.0;
	//}}AFX_DATA_INIT
}


void CDialogKvazi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogKvazi)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_A, m_a);
	DDX_Text(pDX, IDC_A1, m_A1);
	DDX_Text(pDX, IDC_A2, m_A2);
	DDX_Text(pDX, IDC_B, m_b);
	DDX_Text(pDX, IDC_C, m_c);
	DDX_Text(pDX, IDC_F_A, m_f_a);
	DDX_Text(pDX, IDC_F_A1, m_f_A1);
	DDX_Text(pDX, IDC_F_A2, m_f_A2);
	DDX_Text(pDX, IDC_F_B, m_f_b);
	DDX_Text(pDX, IDC_F_C, m_f_c);
	DDX_Text(pDX, IDC_Y_A, m_y_a);
	DDX_Text(pDX, IDC_Y_A1, m_y_A1);
	DDX_Text(pDX, IDC_Y_A2, m_y_A2);
	DDX_Text(pDX, IDC_Y_B, m_y_b);
	DDX_Text(pDX, IDC_Y_C, m_y_c);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogKvazi, CDialog)
	//{{AFX_MSG_MAP(CDialogKvazi)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogKvazi message handlers

void CDialogKvazi::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
