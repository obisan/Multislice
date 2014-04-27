// DialogCircle.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogCircle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogCircle dialog


CDialogCircle::CDialogCircle(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCircle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogCircle)
	m_at = FALSE;
	m_ft = FALSE;
	m_count = 5;
	m_dem = 512;
	m_diametr = 25;
	m_inten_circle = 0;
	m_inten_fon = 255;
	m_ton = 1;
	//}}AFX_DATA_INIT
}


void CDialogCircle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogCircle)
	DDX_Check(pDX, IDC_AT, m_at);
	DDX_Check(pDX, IDC_FT, m_ft);
	DDX_Text(pDX, IDC_COUNT, m_count);
	DDX_Text(pDX, IDC_DEM, m_dem);
	DDX_Text(pDX, IDC_DIAMETR, m_diametr);
	DDX_Text(pDX, IDC_INTEN_CIRCLE, m_inten_circle);
	DDX_Text(pDX, IDC_INTEN_FON, m_inten_fon);
	DDX_Radio(pDX, IDC_TON, m_ton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogCircle, CDialog)
	//{{AFX_MSG_MAP(CDialogCircle)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogCircle message handlers

void CDialogCircle::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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

BOOL CDialogCircle::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShowContextHelp(CWnd::FromHandle((HWND)pHelpInfo->hItemHandle), 
		pHelpInfo->MousePos, pHelpInfo->iCtrlId);
	
	return CDialog::OnHelpInfo(pHelpInfo);
}

void CDialogCircle::ShowContextHelp(CWnd *pWnd, POINT pt, int iCtrlID)
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
