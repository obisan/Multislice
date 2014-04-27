// DialogWave.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogWave dialog


CDialogWave::CDialogWave(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogWave::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogWave)
	m_amp = 100;
	m_at = FALSE;
	m_ft = FALSE;
	m_period = 32;
	m_direct = 45;
	m_dem = 512;
	//}}AFX_DATA_INIT
}


void CDialogWave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogWave)
	DDX_Text(pDX, IDC_AMP, m_amp);
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_PERIOD, m_period);
	DDX_Text(pDX, IDC_DIRECT, m_direct);
	DDX_Text(pDX, IDC_DEM, m_dem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogWave, CDialog)
	//{{AFX_MSG_MAP(CDialogWave)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogWave message handlers

void CDialogWave::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
