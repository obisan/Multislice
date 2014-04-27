// DialogNoise.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogNoise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogNoise dialog


CDialogNoise::CDialogNoise(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogNoise::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogNoise)
	m_at = FALSE;
	m_ft = FALSE;
	m_dem = 512;
	m_max = 255;
	m_min = 0;
	//}}AFX_DATA_INIT
}


void CDialogNoise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogNoise)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_MAX, m_max);
	DDX_Text(pDX, IDC_MIN, m_min);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogNoise, CDialog)
	//{{AFX_MSG_MAP(CDialogNoise)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogNoise message handlers

void CDialogNoise::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
