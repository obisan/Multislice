// DialogObob.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogObob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogObob dialog


CDialogObob::CDialogObob(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogObob::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogObob)
	m_dem = 512;
	m_ft = FALSE;
	m_at = FALSE;
	m_a = 2.89;
	m_A1 = 25.35;
	m_A2 = 246.76;
	m_al = 6.168;
	m_be = 36.72;
	m_f_a = 0.0;
	m_f_A1 = 0.0;
	m_f_al = 0.0;
	m_f_A2 = 0.0;
	m_f_be = 0.0;
	m_f_n = 0.0;
	m_n = 1;
	m_x = 0;
	m_y = 255;
	m_y_a = 1.0;
	m_y_A1 = 1.0;
	m_y_A2 = 1.0;
	m_y_al = 1.0;
	m_y_be = 1.0;
	m_y_n = 1.0;
	//}}AFX_DATA_INIT
}


void CDialogObob::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogObob)
	DDX_Text(pDX, IDC_DEM2, m_dem);
	DDX_Check(pDX, IDC_FT2, m_ft);
	DDX_Check(pDX, IDC_AT2, m_at);
	DDX_Text(pDX, IDC_A, m_a);
	DDX_Text(pDX, IDC_A1, m_A1);
	DDX_Text(pDX, IDC_A22, m_A2);
	DDX_Text(pDX, IDC_AL, m_al);
	DDX_Text(pDX, IDC_BE, m_be);
	DDX_Text(pDX, IDC_F_A, m_f_a);
	DDX_Text(pDX, IDC_F_A1, m_f_A1);
	DDX_Text(pDX, IDC_F_AL, m_f_al);
	DDX_Text(pDX, IDC_F_A22, m_f_A2);
	DDX_Text(pDX, IDC_F_BE, m_f_be);
	DDX_Text(pDX, IDC_F_N, m_f_n);
	DDX_Text(pDX, IDC_N, m_n);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_Y_A, m_y_a);
	DDX_Text(pDX, IDC_Y_A1, m_y_A1);
	DDX_Text(pDX, IDC_Y_A22, m_y_A2);
	DDX_Text(pDX, IDC_Y_AL, m_y_al);
	DDX_Text(pDX, IDC_Y_BE, m_y_be);
	DDX_Text(pDX, IDC_Y_N, m_y_n);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogObob, CDialog)
	//{{AFX_MSG_MAP(CDialogObob)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogObob message handlers

void CDialogObob::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
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
