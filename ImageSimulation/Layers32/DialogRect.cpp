// DialogRect.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRect dialog


CDialogRect::CDialogRect(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRect)
	m_ft = FALSE;
	m_at = FALSE;
	m_count = 50;
	m_dem = 512;
	m_demrect = 16;
	m_fon = 255;
	m_rect = 0;
	//}}AFX_DATA_INIT
}


void CDialogRect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRect)
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Text(pDX, IDC_COUNTRECT, m_count);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_DEMRECT, m_demrect);
	DDX_Text(pDX, IDC_FON, m_fon);
	DDX_Text(pDX, IDC_RECT, m_rect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRect, CDialog)
	//{{AFX_MSG_MAP(CDialogRect)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRect message handlers

void CDialogRect::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
