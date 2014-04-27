// SpectrumFrame.cpp : implementation file
//

#include "stdafx.h"

//#include "aber32.h"
#include "SpectrumFrame.h"
#include "ObjectFrameCernic.h"
#include "ObjectFrameDefokus.h"
#include "DialogDefokus.h"
#include "DialogInfo.h"
#include "DialogNameL2DB.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCTSTR lpszTextClass;
extern double DLayer;
extern double LLayer;

#define     GlobalAllocPtr(flags, cb)        \
(GlobalLock(GlobalAlloc((flags), (cb))))

/////////////////////////////////////////////////////////////////////////////
// CSpectrumFrame

IMPLEMENT_DYNCREATE(CSpectrumFrame, CMDIChildWnd)

CSpectrumFrame::CSpectrumFrame()
{
	Kvadrant = 1;
	m_iDem = 1;
	m_bArroy = new BYTE[1];
	m_fArroy = new fi_complex[1];
	m_iMin = 0;
	m_iMax = 255;
	m_fPos = 1;
	m_fPos_Const = 1;
	m_fPredel = 100;
	CreatePallete();
}

CSpectrumFrame::~CSpectrumFrame()
{
	delete[] m_bArroy;
	delete[] m_fArroy;
}


BEGIN_MESSAGE_MAP(CSpectrumFrame, CMDIChildWnd)
//{{AFX_MSG_MAP(CSpectrumFrame)
ON_WM_CREATE()
ON_WM_PAINT()
ON_WM_GETMINMAXINFO()
ON_COMMAND(ID_SAVE_SPECTR_SP, OnSaveSpectrSp)
ON_COMMAND(ID_SAVE_SPECTR_BMP, OnSaveSpectrBmp)
ON_COMMAND(ID_COEFF_S, OnCoeffS)
ON_WM_RBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_COMMAND(ID_CERNIC, OnCernic)
ON_COMMAND(ID_DEFOKUS, OnDefokus)
ON_COMMAND(ID_ABER, OnAber)
ON_COMMAND(ID_L2DB, OnL2db)
ON_UPDATE_COMMAND_UI(ID_L2DB, OnUpdateL2db)
ON_WM_SETFOCUS()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectrumFrame message handlers

CSpectrumFrame::CSpectrumFrame(BYTE *arroy,int Dem)
{
	m_iDem=Dem;
	m_bArroy=new BYTE[m_iDem*m_iDem];
	m_fArroy=new fi_complex[m_iDem*m_iDem];
	m_iMin=0;
	m_iMax=255;
	m_fPos=1;
	m_fPos_Const=1;
	//m_fPos_Const=50;
	m_fPredel=100;
	CreatePallete();
	/////
	/////GetSpectrum(arroy);
	
}
CSpectrumFrame::CSpectrumFrame(int dem,struct Complex *arroy)
{
	int dd;
	Kvadrant=1;
	m_iDem=dem;
	dd=dem*dem;
	m_bArroy=new BYTE[m_iDem*m_iDem];
	m_fArroy=new fi_complex[m_iDem*m_iDem];
	for(int i=0;i<dd;i++)
	{
		m_fArroy[i].re=arroy[i].re;
		m_fArroy[i].im=arroy[i].im;
	}
	m_iMin=0;
	m_iMax=255;
	m_fPos_Const=0.001;
	m_fPos=1;
	CreatePallete();
	this->ScaleView();
}
int CSpectrumFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	this->MoveWindow(20,20,m_iDem+4+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME),m_iDem+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME),TRUE);
	ReGenerationScale();
	
	
	
	return 0;
}

BOOL CSpectrumFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	cs.style |= WS_CHILD | WS_VISIBLE |WS_OVERLAPPEDWINDOW;
	
	
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

void CSpectrumFrame::GetSpectrum(BYTE *arroy)
{
}

void CSpectrumFrame::CreatePallete()
{
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
	
}

void CSpectrumFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
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

void CSpectrumFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMinMaxInfo = (MINMAXINFO FAR*)lpMMI;
	
	lpMinMaxInfo->ptMaxTrackSize.x  = m_iDem+2+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
	lpMinMaxInfo->ptMaxTrackSize.y = m_iDem+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYEDGE)+2*GetSystemMetrics(SM_CYFIXEDFRAME);
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

void CSpectrumFrame::ScaleView()
{
	int dx2=m_iDem/2;
	int dy2=m_iDem/2;
	int idx,dy2pidx,offs1,offs2;
	int i,j;
	double buffer=0;
	double average=0;
	
	
	if(Kvadrant==1)
	{
		for(i=0;i<dy2;i++)
		{
			idx=i*m_iDem;
			dy2pidx=(dy2+i)*m_iDem;
			
			for(j=0;j<dx2;j++)
			{
				offs1=dy2pidx+dx2+j;
				offs2=idx+j;
				buffer=m_fPos_Const*m_fPos * sqrt(m_fArroy[offs1].re*m_fArroy[offs1].re+m_fArroy[offs1].im*m_fArroy[offs1].im);
				if(buffer>255.){buffer=255.;}
				if(buffer<0.0){buffer=0.0;}
				m_bArroy[offs2]=(BYTE)buffer;
				buffer=m_fPos_Const*m_fPos * sqrt(m_fArroy[offs2].re*m_fArroy[offs2].re+m_fArroy[offs2].im*m_fArroy[offs2].im);
				if(buffer>255.){buffer=255.;}
				if(buffer<0.0){buffer=0.0;}
				m_bArroy[offs1]=(BYTE)buffer;
				offs1=idx+dx2+j;
				offs2=dy2pidx+j;
				buffer=m_fPos_Const*m_fPos * sqrt(m_fArroy[offs1].re*m_fArroy[offs1].re+m_fArroy[offs1].im*m_fArroy[offs1].im);
				if(buffer>255.){buffer=255.;}
				if(buffer<0.0){buffer=0.0;}
				m_bArroy[offs2]=(BYTE)buffer;
				buffer=m_fPos_Const*m_fPos * sqrt(m_fArroy[offs2].re*m_fArroy[offs2].re+m_fArroy[offs2].im*m_fArroy[offs2].im);
				if(buffer>255.){buffer=255.;}
				if(buffer<0.0){buffer=0.0;}
				m_bArroy[offs1]=(BYTE)buffer;
			}
		}
	}
	else
	{
		int DD=m_iDem*m_iDem;
		for(i=0;i<DD;i++)
		{
			buffer=m_fPos_Const*m_fPos * sqrt(m_fArroy[i].re*m_fArroy[i].re+m_fArroy[i].im*m_fArroy[i].im);
			if(buffer>255.){buffer=255.;}
			if(buffer<0.0){buffer=0.0;}
			
			m_bArroy[i]=(BYTE)buffer;
			
		}
	}
}






void CSpectrumFrame::FindMinMax(BYTE *arroy)
{
	m_iMax=arroy[1];
	m_iMin=arroy[1];
	for(int i=0;i<m_iDem*m_iDem;i++)
	{
		if(arroy[i]>m_iMax) m_iMax=arroy[i];
		if(arroy[i]<m_iMin) m_iMin=arroy[i];
	}
	
}

void CSpectrumFrame::Fft(BYTE *arroy)
{
/*fftw_complex *in=new fftw_complex[m_iDem*m_iDem];
int dxdy=m_iDem*m_iDem;

  for(int i=0;i<dxdy;i++)
  {
		
		  in[i].re=arroy[i];
		  in[i].im=0;
		  }
		  fftwnd_plan p;
		  p = fftw2d_create_plan(m_iDem,m_iDem, FFTW_FORWARD,FFTW_ESTIMATE|FFTW_IN_PLACE);
		  fftwnd_one(p,in, NULL);
		  fftwnd_destroy_plan(p);
		  
			for(i=0;i<dxdy;i++)
			{
			m_fArroy[i].re=(float)in[i].re;
			m_fArroy[i].im=(float)in[i].im;
			}
			delete[] in;
			OptimizeScale();
	ScaleView();*/
}

/*void CSpectrumFrame::OnCoeffS() 
{
// TODO: Add your command handler code here
//Dialog.DoModal();
//m_fPos=m_fPredel=10;
Dialog.GetCoeff(m_fPos,m_fPredel);
ScaleView();
this->RedrawWindow();
}*/



void CSpectrumFrame::OptimizeScale()
{
/*int DD=m_iDem*m_iDem;
double buffer=0;
char string[20];
for(int i=0;i<DD;i++)
{
buffer+=m_fPos_Const*m_fPos * sqrt(m_fArroy[i].r*m_fArroy[i].r+m_fArroy[i].i*m_fArroy[i].i);
//m_ProgBar.StepIt();

  }
  buffer=buffer/DD;
	m_fPos_Const=126/(buffer*2);*/
	
	/*sprintf(string,"buffer=%f",buffer);
	AfxMessageBox(string,MB_OK,NULL);
	sprintf(string,"m_fPos_Const=%f",m_fPos_Const);
	AfxMessageBox(string,MB_OK,NULL);*/
}

void CSpectrumFrame::OnSaveSpectrSp() 
{
	// TODO: Add your command handler code here
	SaveSp();
	
}

void CSpectrumFrame::OnSaveSpectrBmp() 
{
	// TODO: Add your command handler code here
	SaveBmp();
	
}

void CSpectrumFrame::SaveSp()
{
	int i;
	/////////диалог сохранения файла
	char Tmp_File_Name[256];
	// Создаем объект блока диалога
	CFileDialog fileDlg(FALSE);
	fileDlg.m_ofn.lpstrInitialDir="NULL";
	// Определяем расширение, которое будет присваиваться
	// имени файла по умолчанию
	CString ext("sp");
	fileDlg.m_ofn.lpstrDefExt = ext;
	//устанавливаю фильтры на файлы
	CString str("Все файлы (*.*)"); str += (TCHAR)NULL;
	str += "*.*"; str += (TCHAR)NULL;
	str += "SP файлы (*.sp,*.dat)"; str += (TCHAR)NULL;
	str += "*.sp;*.dat"; str += (TCHAR)NULL;
	fileDlg.m_ofn.lpstrFilter = str;
	fileDlg.m_ofn.nFilterIndex=2;
	//имя по умолчанию
	Tmp_File_Name[0] ='\0' ;
	strcpy(Tmp_File_Name,"SP-file.sp");
	fileDlg.m_ofn.lpstrFile	= Tmp_File_Name;
	// Заголовок блока диалога
	TCHAR title[] = "Сохранить SP файл";
	fileDlg.m_ofn.lpstrTitle = title;
	// Задаем вывод флажка "Только для чтения" (Read Only) и
	// снимаем режим Проводник (Explorer)
	//fileDlg.m_ofn.Flags &= ~(OFN_HIDEREADONLY  | OFN_EXPLORER);
	
	// Выводим блок диалога на экран
	if(fileDlg.DoModal() == IDOK)
	{
		FILE *out;
		out=fopen(fileDlg.m_ofn.lpstrFile,"wb");
		for(i=0; i<m_iDem*m_iDem; i++)
		{
			if((fwrite(&m_fArroy[i],sizeof(struct fi_complex),1,out)) == NULL)
			{
				AfxMessageBox("Ошибка при записи в файл",MB_OK,NULL);
				fclose(out);
				return;
			}
		}
		fclose(out);
		
	}
	//запись в файл
}


void CSpectrumFrame::SaveBmp()
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
/*void CSpectrumFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
// TODO: Add your message handler code here and/or call default

  CMDIChildWnd::OnHScroll(nSBCode, nPos, pScrollBar);
  }
  
	void CSpectrumFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	
	  CMDIChildWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}*/

void CSpectrumFrame::OnCoeffS() 
{
	// TODO: Add your command handler code here
	//Dialog.GetCoeff(m_fPos,m_fPredel);
	Dialog.m_pos=m_fPos;
	Dialog.DoModal();
	m_fPos=Dialog.m_pos;
	ScaleView();
	this->RedrawWindow();
	
}

void CSpectrumFrame::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	Dialog.m_pos=m_fPos;
	Dialog.DoModal();
	m_fPos=Dialog.m_pos;
	ScaleView();
	this->RedrawWindow();	
	CMDIChildWnd::OnRButtonUp(nFlags, point);
}

void CSpectrumFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	xPos=point.x;
	yPos=point.y;	
	CMDIChildWnd::OnLButtonDown(nFlags, point);
}

void CSpectrumFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	static int xTemp = 0, yTemp = 0, xProg = 0, yProg = 0;
	
	CMDIChildWnd::OnMouseMove(nFlags, point);
	if(nFlags == MK_LBUTTON) {
		////
		xProg=xPos-point.x;
		yProg=yPos-point.y;
		
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
			if(m_sX<xProg) {
				m_sX = m_sX - xProg;
			} else {
				m_sX = 0;
			}
		}
		
		if(yProg > 2) {
			if(yTemp > yProg) {
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
	CMDIChildWnd::OnMouseMove(nFlags, point);
}

void CSpectrumFrame::ReGenerationScale() {
	int temp = GetSystemMetrics(SM_CYSCREEN);
	switch(temp) {
	case 480:
		m_Limited = 350;
		break;
	case 600:
		m_Limited = 450;
		break;
	case 768:
		m_Limited = 600;
		break;
	case 864:
		m_Limited = 750;
		break;
	case 1024:
		m_Limited = 850;
		break;
	default:
		m_Limited = 1000;
	}
	int xPos = 0;
	int yPos = 0;
	m_sX = 0;
	m_sY = 0;
	UpdateWindow();
}

void CSpectrumFrame::OnCernic() {
	char str[300];
	if(m_type_spectr == SPECTR) {
		CObjectFrameCernic *m_object;
		m_object = new CObjectFrameCernic(m_fArroy, m_iDem);
		m_object->Kvadrant = 0;
		m_object->GetOptimumCoefficient();
		m_object->m_fPos_Const *= 0.8;
		m_object->ScaleView();
		sprintf(str,"Визуализация Цернике [%dx%d]", m_iDem, m_iDem);
		if (!m_object->Create(NULL, _TEXT(str), 0, rectDefault, NULL))
			return;	
		m_object->MoveWindow(80, 80, 10000, 10000);
	} else {
		AfxMessageBox("Визуализация находится только со спектра!!!",MB_OK);
	}
}

void CSpectrumFrame::OnDefokus() {
	char str[300];
	if(m_type_spectr == SPECTR) {
		if(this->m_type_plant == PAM) {
			CDialogDefokus dialog;
			dialog.m_apertura = m_apertura;
			dialog.m_length_wave = m_length_wave;
			dialog.m_iDem = this->m_iDem;
			dialog.m_k_ap_diaf = pow((6.0 / (dialog.m_Ccf * pow(10.0,7.0) * pow(dialog.m_length_wave, 3.0))), 0.25);
			dialog.UpdateD();

			if(dialog.DoModal() == IDOK) {
				LLayer = m_length_wave;
				DLayer = dialog.m_apertura;
				CObjectFrameDefokus *m_object;
				
				CDialogInfo* DialogInfo = new CDialogInfo(NULL);
				DialogInfo->Create(IDD_INFO,NULL);
				DialogInfo->ShowWindow(SW_SHOW);
				DialogInfo->m_str.Format("Идет расчет дефокусировки...");
				DialogInfo->UpdateData(FALSE);
				
				m_object = new CObjectFrameDefokus(m_fArroy, m_iDem, dialog.m_D, dialog.m_Ccf, dialog.m_Cxp, dialog.m_k_ap_diaf);
				m_object->Kvadrant = 0;
				m_object->GetOptimumCoefficient();
				m_object->m_fPos_Const *= 0.8;
				m_object->ScaleView();
				sprintf(str, "Визуализация Дефокусировка [%dx%d]", m_iDem, m_iDem);
				if (!m_object->Create(NULL,_TEXT(str),0, rectDefault, NULL))
					return;	
				m_object->MoveWindow(80,80,10000,10000);
				
				DialogInfo->CloseWindow();
				delete DialogInfo;
			}
		}
	} else {
		AfxMessageBox("Визуализация находится только со спектра!!!",MB_OK);
	}	
}

void CSpectrumFrame::OnAber() {
	if(m_type_spectr == SPECTR) {
		FILE *out;
		out = fopen("Sp.tmp", "wb");
		for(int i=0; i<m_iDem*m_iDem; i++) {
			if((fwrite(&m_fArroy[i],sizeof(struct fi_complex),1,out)) == NULL) {
				AfxMessageBox("Ошибка при записи в файл",MB_OK,NULL);
				fclose(out);
				return;
			}
		}
		fclose(out);
		if(WinExec("aber32.exe Sp.tmp",SW_RESTORE)==ERROR_FILE_NOT_FOUND) {
			AfxMessageBox("Не найден файл aber32.exe",MB_OK,NULL);
		}
		if(WinExec("start CrazyLayers.chm",SW_RESTORE)==ERROR_FILE_NOT_FOUND) {
			AfxMessageBox("Не найден файл aber32.exe",MB_OK,NULL);
		}
	} else {
		AfxMessageBox("Визуализация находится только со спектра!!!",MB_OK);
	}
}

void CSpectrumFrame::GetOptimumCoefficient() {
	double Max,Min,temp;
	int i;
	int DD = m_iDem * m_iDem;
	Max = Min = sqrt( m_fArroy[1].re * m_fArroy[1].re + m_fArroy[1].im * m_fArroy[1].im );
	for(i = 0; i < DD; i++) {
		temp = sqrt(m_fArroy[i].re * m_fArroy[i].re + m_fArroy[i].im*  m_fArroy[i].im);
		if(temp>Max) Max = temp;
		if(temp<Min) Min = temp;
	}
	temp = 0;
	m_fPos_Const = 255 / Max;
	temp = 0;
	temp = 0;
}

void CSpectrumFrame::OnL2db() {
	// TODO: Add your command handler code here
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	if(app->i_database != NULL) {
		CDialogNameL2DB DialogNameL2DB;
		DialogNameL2DB.m_name = "Слоевой метод";
		DialogNameL2DB.DoModal();
		double idi;
		app->i_database->CreateImage(&idi,DialogNameL2DB.m_name,m_iDem,m_iDem,2);
		if(idi != NULL) {
			IImageComp*  iimage = NULL;
			app->i_database->OpenImage(idi,IID_IImageComp,(void**)&iimage);
			if (iimage != NULL) {
				iimage->SetImageData((void*)m_bArroy,0,m_iDem,m_iDem);
				iimage->Release();
			}
		} else {
			AfxMessageBox("Ошибка при создании изображения в базе!",MB_OK);
		}
	} else {
		AfxMessageBox("Программа запущена как самостоятельное приложение.",MB_OK);
	}	
}

void CSpectrumFrame::OnUpdateL2db(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CSpectrumFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	
	if(app->i_database!=NULL)
	{
		CMainFrame*  pMainFrame = (CMainFrame*) app->m_pMainWnd;
		pMainFrame->m_wndToolBar.OnUpdateCmdUI(this,FALSE);
	}
}