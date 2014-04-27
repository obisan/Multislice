// DialogConstanta.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogConstanta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConstanta dialog


CDialogConstanta::CDialogConstanta(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConstanta::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConstanta)
	m_at = FALSE;
	m_ft = FALSE;
	m_dem = 512;
	//}}AFX_DATA_INIT
}


void CDialogConstanta::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConstanta)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_DEM, m_dem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConstanta, CDialog)
	//{{AFX_MSG_MAP(CDialogConstanta)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConstanta message handlers

void CDialogConstanta::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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

BOOL CDialogConstanta::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShowContextHelp(CWnd::FromHandle((HWND)pHelpInfo->hItemHandle), 
		pHelpInfo->MousePos, pHelpInfo->iCtrlId);
	
	return CDialog::OnHelpInfo(pHelpInfo);
}

void CDialogConstanta::ShowContextHelp(CWnd *pWnd, POINT pt, int iCtrlID)
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
