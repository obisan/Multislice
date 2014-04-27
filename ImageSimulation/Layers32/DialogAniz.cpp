// DialogAniz.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogAniz.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAniz dialog


CDialogAniz::CDialogAniz(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAniz::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAniz)
	m_ft = FALSE;
	m_at = FALSE;
	m_dem = 512;
	m_x = 0;
	m_y = 255;
	m_a = 1.0;
	m_al1 = 2.0;
	m_al2 = 5.0;
	m_b = 10.0;
	m_be1 = 20.0;
	m_be2 = 50.0;
	m_c = 20.0;
	m_d = 10.0;
	//}}AFX_DATA_INIT
}


void CDialogAniz::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAniz)
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_A1, m_a);
	DDX_Text(pDX, IDC_al1, m_al1);
	DDX_Text(pDX, IDC_al2, m_al2);
	DDX_Text(pDX, IDC_b1, m_b);
	DDX_Text(pDX, IDC_be1, m_be1);
	DDX_Text(pDX, IDC_be2, m_be2);
	DDX_Text(pDX, IDC_c1, m_c);
	DDX_Text(pDX, IDC_d1, m_d);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAniz, CDialog)
	//{{AFX_MSG_MAP(CDialogAniz)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAniz message handlers

void CDialogAniz::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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

BOOL CDialogAniz::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShowContextHelp(CWnd::FromHandle((HWND)pHelpInfo->hItemHandle), 
		pHelpInfo->MousePos, pHelpInfo->iCtrlId);
	
	return CDialog::OnHelpInfo(pHelpInfo);
}

void CDialogAniz::ShowContextHelp(CWnd *pWnd, POINT pt, int iCtrlID)
{
CString s;

 // Загружаем текст помощи из String Table
 if(s.LoadString(iCtrlID))
 {
  HH_POPUP hPop; // структура всплывающей HTML справки

  // Инициализируем структуру знаками NULL (нулями)	
  memset(&hPop, 0, sizeof(hPop)); 

  // Устанавливаем размер структуры
  hPop.cbStruct         = sizeof(hPop);		

  // Цвет фона - жёлтый
  hPop.clrBackground    = RGB(255, 255, 255);	

  hPop.clrForeground    = -1; // Font color	
  hPop.rcMargins.left   = 10;
  hPop.rcMargins.bottom = 10;
  hPop.rcMargins.right  = 10;
  hPop.rcMargins.top  = 5;
  hPop.pt               = pt;
  hPop.pszText          = s; // Текст из String Table
  hPop.pszFont          = NULL; // Шрифт

#ifndef _WIN64
  ::HtmlHelpA(pWnd->GetSafeHwnd(), NULL, HH_DISPLAY_TEXT_POPUP, (DWORD)&hPop);
#endif
 } // Если строка справки для данного запроса найдена, то завершаем

}
