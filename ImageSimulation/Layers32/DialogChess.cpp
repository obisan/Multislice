// DialogChess.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogChess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogChess dialog


CDialogChess::CDialogChess(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogChess::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogChess)
	m_at = FALSE;
	m_ft = FALSE;
	m_dark = 0;
	m_dem = 512;
	m_light = 255;
	m_period = 32;
	//}}AFX_DATA_INIT
}


void CDialogChess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogChess)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_DARK, m_dark);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_LIGHT, m_light);
	DDX_Text(pDX, IDC_PERIOD, m_period);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogChess, CDialog)
	//{{AFX_MSG_MAP(CDialogChess)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogChess message handlers

void CDialogChess::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CDialogChess::ShowContextHelp(CWnd *pWnd, POINT pt, int iCtrlID)
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

BOOL CDialogChess::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShowContextHelp(CWnd::FromHandle((HWND)pHelpInfo->hItemHandle), 
		pHelpInfo->MousePos, pHelpInfo->iCtrlId);
	
	return CDialog::OnHelpInfo(pHelpInfo);
}
