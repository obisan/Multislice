// ModelFrame.cpp : implementation file
//

#include "stdafx.h"

#include "RESOURCE.H"
#include "ModelFrame.h"
#include "DialogNameL2DB.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define     GlobalAllocPtr(flags, cb)        \
(GlobalLock(GlobalAlloc((flags), (cb))))

extern LPCTSTR lpszTextClass;


/////////////////////////////////////////////////////////////////////////////
// CModelFrame

IMPLEMENT_DYNCREATE(CModelFrame, CMDIChildWnd)

CModelFrame::CModelFrame()
{
	m_Limited=600;
	m_iDem=512;
	m_bArroy=new BYTE[m_iDem];
	Pallete=(LPBITMAPINFO)GlobalAllocPtr(GHND,
		(sizeof (BITMAPINFO)+
		(sizeof (RGBQUAD)*255 )));
	Pallete->bmiHeader.biSize		= 40;
	Pallete->bmiHeader.biWidth		= m_iDem;
	Pallete->bmiHeader.biHeight		= m_iDem;
	Pallete->bmiHeader.biPlanes		= 1;
	Pallete->bmiHeader.biBitCount	= 8;
	Pallete->bmiHeader.biCompression=BI_RGB;
	Pallete->bmiHeader.biSizeImage  = 0;
	Pallete->bmiHeader.biXPelsPerMeter = 0;
	Pallete->bmiHeader.biYPelsPerMeter = 0;
	Pallete->bmiHeader.biClrUsed       = 0;
	Pallete->bmiHeader.biClrImportant  = 0;
	
	for(int i=0;i<256;i++)
	{
		Pallete->bmiColors[i].rgbRed =
			Pallete->bmiColors[i].rgbGreen =
			Pallete->bmiColors[i].rgbBlue = i;
	}
	GetModel();
}

CModelFrame::~CModelFrame()
{
	delete[] m_bArroy;
	GlobalFree(Pallete);
}


BEGIN_MESSAGE_MAP(CModelFrame, CMDIChildWnd)
//{{AFX_MSG_MAP(CModelFrame)
ON_WM_CREATE()
ON_WM_PAINT()
ON_WM_GETMINMAXINFO()
ON_WM_CLOSE()
ON_COMMAND(ID_SAVE_SPECTR_BMP, OnSaveSpectrBmp)
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_SETFOCUS()
ON_COMMAND(ID_L2DB, OnL2db)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelFrame message handlers

BOOL CModelFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_CHILD | WS_VISIBLE |
		WS_OVERLAPPEDWINDOW;
	return CMDIChildWnd::PreCreateWindow(cs);
}

int CModelFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	this->MoveWindow(10,10,m_iDem+2+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME),m_iDem+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME),TRUE);
	////
	ReGenerationScale();
	//GetModel();
	return 0;
}

int CModelFrame::GetModel()
{
	delete[] m_bArroy;
	m_bArroy=new BYTE[m_iDem*m_iDem];
	int Dem=m_iDem;
	int i,j;
	BYTE *Buf;
	Buf =new BYTE[Dem*Dem];
	//	randomize();///////////////
	for (i=0; i<Dem; i++)
	{
		for(j=0; j<Dem; j++)
		{
			m_bArroy[i*Dem+j] =0;//  Копируем с Учетом доли в сигнале
		}
	}
	delete Buf;
	return 0;
}

void CModelFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//CClientDC dc(&m_wndView);
	
	// TODO: Add your message handler code here
	Pallete->bmiHeader.biWidth=m_iDem;
	Pallete->bmiHeader.biHeight=m_iDem;
	StretchDIBits(dc.m_hDC,
		m_sX,
		m_sY,
		m_iDem,
		m_iDem,
		0,
		0,
		m_iDem,
		m_iDem,
		m_bArroy,
		Pallete,
		DIB_RGB_COLORS,
		SRCCOPY);
	
	// Do not call CMDIChildWnd::OnPaint() for painting messages
}

void CModelFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	char str[10];
	sprintf(str,"%d",GetSystemMetrics(SM_CYMENU));
	
	/*lpMinMaxInfo = (MINMAXINFO FAR*)lpMMI;
	lpMinMaxInfo->ptMaxTrackSize.x  = m_iDem+2+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
	lpMinMaxInfo->ptMaxTrackSize.y = m_iDem+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
	lpMinMaxInfo->ptMinTrackSize.x  = 256;
	lpMinMaxInfo->ptMinTrackSize.y = 256;*/
	if(m_iDem<=m_Limited)
	{
		lpMinMaxInfo = (MINMAXINFO FAR*)lpMMI;
		lpMinMaxInfo->ptMaxTrackSize.x  = m_iDem+2+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
		lpMinMaxInfo->ptMaxTrackSize.y = m_iDem+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
		lpMinMaxInfo->ptMinTrackSize.x  = 256;
		lpMinMaxInfo->ptMinTrackSize.y = 256;
	}
	else
	{
		lpMinMaxInfo = (MINMAXINFO FAR*)lpMMI;
		lpMinMaxInfo->ptMaxTrackSize.x  = m_Limited+2+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
		lpMinMaxInfo->ptMaxTrackSize.y = m_Limited+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
		lpMinMaxInfo->ptMinTrackSize.x  = 256;
		lpMinMaxInfo->ptMinTrackSize.y = 256;
		
	}
	
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
}

void CModelFrame::NewArroy(int i)
{
	m_iDem=i;
	delete[] m_bArroy;
	m_bArroy=new BYTE[i*i];
	
}



/*void CModelFrame::OnAmpSpectr() 
{
// TODO: Add your command handler code here
CAOSpectrumFrame *pSpectrumWnd = new CAOSpectrumFrame(m_bArroy,m_iDem);
if (!pSpectrumWnd->Create(lpszTextClass,
_TEXT("БПФ: Амплитудный объект"),
0, rectDefault, NULL))
return;

}*/
void CModelFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);
	
	//CMDIChildWnd::OnClose();
}

void CModelFrame::OnSaveSpectrBmp() 
{
	// TODO: Add your command handler code here
	//MessageBox("ghghghgh");
	SaveBmp();
	
}

void CModelFrame::SaveBmp()
{
	FILE *out;
	BITMAPFILEHEADER BmpHeader;
	/////////диалог сохранения файла
	char Tmp_File_Name[256];
	// Создаем объект блока диалога
	CFileDialog fileDlg(FALSE);
	fileDlg.m_ofn.lpstrInitialDir="NULL";
	// Определяем расширение, которое будет присваиваться
	// имени файла по умолчанию
	CString ext("bmp");
	fileDlg.m_ofn.lpstrDefExt = ext;
	//устанавливаю фильтры на файлы
	CString str("Bmp файлы (*.bmp)"); str += (TCHAR)NULL;
	str += "*.bmp"; str += (TCHAR)NULL;
	
	fileDlg.m_ofn.lpstrFilter = str;
	fileDlg.m_ofn.nFilterIndex=1;
	//имя по умолчанию
	Tmp_File_Name[0] ='\0' ;
	strcpy(Tmp_File_Name,"CrazyLayers.bmp");
	fileDlg.m_ofn.lpstrFile	= Tmp_File_Name;
	// Заголовок блока диалога
	TCHAR title[] = "Сохранить BMP файл";
	fileDlg.m_ofn.lpstrTitle = title;
	// Задаем вывод флажка "Только для чтения" (Read Only) и
	// снимаем режим Проводник (Explorer)
	//fileDlg.m_ofn.Flags &= ~(OFN_HIDEREADONLY  | OFN_EXPLORER);
	
	// Выводим блок диалога на экран
	if(fileDlg.DoModal() == IDOK)
	{
		
		out=fopen(fileDlg.m_ofn.lpstrFile,"wb");
		///////////////////////
		BmpHeader.bfType = 0x4D42;
		BmpHeader.bfSize = 0x10436L;
		BmpHeader.bfReserved1 =
			BmpHeader.bfReserved2 = 0;
		BmpHeader.bfOffBits = 0x436;
		
		fwrite(&BmpHeader, sizeof(BITMAPFILEHEADER),1,out);
		fwrite(Pallete, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255,1,out);
		for(int i=0; i<m_iDem*m_iDem; i++)
			fwrite(&m_bArroy[i], 1,1,out);
		///////////////////////
		fclose(out);
	}
}

void CModelFrame::ReGenerationScale()
{
	int temp=GetSystemMetrics(SM_CYSCREEN);
	switch(temp)
	{
	case 480:
		m_Limited=350;
		break;
	case 600:
		m_Limited=450;
		break;
	case 768:
		m_Limited=600;
		break;
	case 864:
		m_Limited=750;
		break;
	case 1024:
		m_Limited=850;
		break;
	default:
		m_Limited=1000;
	}
	int xPos=0;
	int yPos=0;
	m_sX=0;
	m_sY=0;
	UpdateWindow();
	//OnGetMinMaxInfo();
}

void CModelFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	static int xTemp = 0, yTemp = 0, xProg = 0, yProg = 0;
	
	CMDIChildWnd::OnMouseMove(nFlags, point);
	if(nFlags == MK_LBUTTON) {
		////
		xProg = xPos - point.x;
		yProg = yPos - point.y;
		
		//xProg,yProg - растояние, на которое переместилась мышь
		//Dem+(sX-Limited),Dem+(sX-Limited) - сколько имеем в запасе невидемой картинки,
		//при условии, что Dem > Limited
		
		xTemp = m_iDem + (m_sX - m_Limited);
		yTemp = m_iDem + (m_sY - m_Limited);
		
		if(xProg > 2) {
			if(xTemp > xProg) {
				m_sX = m_sX - xProg;
			} else if(xTemp > 0 && xTemp <= xProg) {
				m_sX = m_sX - xTemp;
			}
		} else if(xProg < -2) {
			if(m_sX < xProg) {
				m_sX = m_sX - xProg;
			} else {
				m_sX = 0;
			}
		}
		
		if(yProg > 2) {
			if(yTemp > yProg)	{
				m_sY = m_sY - yProg;
			} else if(yTemp > 0 && yTemp <= yProg) {
				m_sY = m_sY - yTemp;
			}
		} else if(yProg < -2) {
			if(m_sY < yProg) {
				m_sY = m_sY - yProg;
			} else {
				m_sY = 0;
			}
		}
		
		yProg = xProg = 0;
		
		xPos = point.x;
		yPos = point.y;
		
		////
		InvalidateRect(NULL, FALSE);
	}
}



void CModelFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	xPos=point.x;
	yPos=point.y;
	
	CMDIChildWnd::OnLButtonDown(nFlags, point);
}

void CModelFrame::OnL2db() 
{
	// TODO: Add your command handler code here
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	if(app->i_database != NULL) {
		CDialogNameL2DB DialogNameL2DB;
		DialogNameL2DB.m_name = "Слоевой метод";
		DialogNameL2DB.DoModal();		
		double idi;
		app->i_database->CreateImage(&idi, "Изо", m_iDem, m_iDem, 2);
		if(idi != NULL) {
			IImageComp*  iimage = NULL;
			app->i_database->OpenImage(idi, IID_IImageComp, (void**)&iimage);
			if (iimage != NULL) {
				iimage->SetImageData((void*)m_bArroy, 0, m_iDem, m_iDem);
				iimage->Release();
			}
		} else {
			AfxMessageBox("Ошибка при создании изображения в базе!",MB_OK);
		}
	} else {
		AfxMessageBox("Программа запущена как самостоятельное приложение.",MB_OK);
	}	
}

void CModelFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	
	if(app->i_database!=NULL)
	{
		CMainFrame*  pMainFrame = (CMainFrame*) app->m_pMainWnd;
		pMainFrame->m_wndToolBar.OnUpdateCmdUI(this,TRUE);
	}
	
}