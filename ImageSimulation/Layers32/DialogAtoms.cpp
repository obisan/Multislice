// DialogAtoms.cpp : implementation file
//

#include "stdafx.h"
#include "layers32.h"
#include "DialogAtoms.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAtoms dialog


CDialogAtoms::CDialogAtoms(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAtoms::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAtoms)
	m_dem = 512;
	//}}AFX_DATA_INIT
}


void CDialogAtoms::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAtoms)
	DDX_Text(pDX, IDC_DEM, m_dem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAtoms, CDialog)
	//{{AFX_MSG_MAP(CDialogAtoms)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAtoms message handlers

void CDialogAtoms::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMUpDown->iDelta == -1) {
		UpdateData();
		m_dem *= 2;
		if(m_dem > 4096) {
			m_dem = 4096;
		}
		UpdateData(false);
	} else {
		UpdateData();
		m_dem /= 2;
		if(m_dem < 512) {
			m_dem = 512;
		}
		UpdateData(false);
	}
	*pResult = 0;
}

BOOL CDialogAtoms::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	ShowContextHelp(CWnd::FromHandle((HWND)pHelpInfo->hItemHandle), 
		pHelpInfo->MousePos, pHelpInfo->iCtrlId);
	
	return CDialog::OnHelpInfo(pHelpInfo);
}

void CDialogAtoms::ShowContextHelp(CWnd *pWnd, POINT pt, int iCtrlID)
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
