// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "Layers32.h"
#include "MainFrm.h"
#include "ModelFrame.h"
#include "ModelFrameLayers.h"
#include "SpectrumFrame.h"
#include "SpectrumFrameLayers.h"
#include "dib.h"
#include "LatGen.h"
#include "DialogInfo.h"
#include "htmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CLayers32App theApp;
extern double DLayer;
extern double LLayer;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_PROCESSING, OnProcessing)
	ON_COMMAND(ID_CONSTANT, OnConstant)
	ON_COMMAND(ID_WAVE, OnWave)
	ON_COMMAND(ID_CHESS, OnChess)
	ON_COMMAND(ID_RECT, OnRect)
	ON_COMMAND(ID_CIRCLE, OnCircle)
	ON_COMMAND(ID_NOISE, OnNoise)
	ON_COMMAND(ID_KLIN, OnKlin)
	ON_COMMAND(ID_FRACTAL, OnFractal)
	ON_COMMAND(ID_KVAZI, OnKvazi)
	ON_COMMAND(ID_OBOB, OnObob)
	ON_COMMAND(ID_ANIZOTROP, OnAnizotrop)
	ON_COMMAND(ID_TURBO, OnTurbo)
	ON_COMMAND(ID_EXAMPLE, OnExample)
	ON_COMMAND(ID_KVAZI2, OnKvazi2)
	ON_COMMAND(ID_SP2TYPE, OnSp2type)
	ON_COMMAND(ID_START, OnStart)
	ON_COMMAND(ID_ATOMS, OnAtoms)
	ON_COMMAND(ID_SHOW_WIN, OnShowWin)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_type_of_system = PAM;
	if(m_type_of_system == KOS) {
		PntLayer = DialogPath.m_count;
		DifPhaseModul = DialogPath.m_modul;
		m_length_wave = DialogPath.m_wave;
		m_distance_layer = DialogPath.m_layers;
		m_apertura = DialogPath.m_apertura;
	} else {
		PntLayer = DialogPath.m_count_pam;
		DifPhaseModul = DialogPath.m_modul_pam;
		m_length_wave = DialogPath.m_wave_pam;
		m_distance_layer = DialogPath.m_layers_pam;
		m_apertura = DialogPath.m_aperture_pam;
		m_napr = DialogPath.m_napr_pam;
	}
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	////
	m_iDem_fasa=m_iDem_ampl=512;
	m_cd_fasa.i1=30;
	m_cd_fasa.d1=1;
	m_cd_fasa.d2=0.25;
	//по умолчанию клин
	m_type_fasa=7;
	char str[300];
	m_fasa = new CModelFrameLayers(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);

	sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
	if (!m_fasa->Create(NULL,_TEXT(str),0, rectDefault, NULL))
		return 0;
	m_fasa->MoveWindow(0,0,10000,10000);
	
	m_cd_ampl.i1=25;
	m_cd_ampl.i2=5;
	m_cd_ampl.i3=255;
	m_cd_ampl.i4=0;
	m_cd_ampl.i5=1;
	//по умолчанию круги
	m_type_ampl=5;
	sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
	m_ampl = new CModelFrameLayers(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
	if (!m_ampl->Create(NULL,_TEXT(str),0, rectDefault, NULL))
		return 0;
	m_ampl->MoveWindow(40,40,10000,10000);
	

	m_ampl->ShowWindow(SW_HIDE);
	m_fasa->ShowWindow(SW_HIDE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


/*void CMainFrame::OnFileNew() 
{
	// TODO: Add your command handler code here
	m_fasa = new CModelFrameLayers();
	if (!m_fasa->Create(NULL,_TEXT("Модель фазы"),0, rectDefault, NULL))
		return;
		m_ampl = new CModelFrameLayers();
	if (!m_ampl->Create(NULL,_TEXT("Модель амплитуды"),0, rectDefault, NULL))
		return;
	
	CMDIFrameWnd* pFrame1 = new CMainFrame;
	theApp.m_pMainWnd = pFrame1;
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pFrame->CreateNewChild(RUNTIME_CLASS(CModelFrameLayers),IDR_LAYERSTYPE, NULL,NULL);
	
	CModelFrameLayers *pWnd = new CModelFrameLayers();
	if (!pWnd->Create(NULL,
		_TEXT("БПФ: Амплитудный объект"),
		0, rectDefault, NULL))
		return;
}*/

void CMainFrame::OnProcessing() {
	// TODO: Add your command handler code here
	if(m_type_of_system == KOS) {
		DialogPath.m_kos = TRUE;
		DialogPath.m_pam = FALSE;
	} else {
		DialogPath.m_kos = FALSE;
		DialogPath.m_pam = TRUE;
	}

	int temp = 0;
		
	if(DialogPath.DoModal()==IDOK) {
		if(DialogPath.m_kos == TRUE) {
			m_type_of_system = KOS;
				
			PntLayer = DialogPath.m_count;
			DifPhaseModul = DialogPath.m_modul;
			m_length_wave = DialogPath.m_wave * pow(10.0,3);//переводим мкм в нм
			m_distance_layer = DialogPath.m_layers * pow(10.0,6);//переводим мм в нм
			m_apertura = DialogPath.m_apertura * pow(10.0,6);//переводим мм в нм
		} else {
			m_type_of_system = PAM;
				
			PntLayer = DialogPath.m_count_pam;
			DifPhaseModul = DialogPath.m_modul_pam;
			m_length_wave = DialogPath.m_wave_pam;
			m_distance_layer = DialogPath.m_layers_pam;
			m_apertura = DialogPath.m_aperture_pam;
			m_napr = DialogPath.m_napr_pam;
		}
		//Processing();
	}
	
}

void CMainFrame::OnConstant() 
{
	CDialogConstanta Dialog;
	// TODO: Add your command handler code here
	if(Dialog.DoModal()==IDOK)
	{
		if(Dialog.m_ft==TRUE)
		{
			m_iDem_fasa=Dialog.m_dem;
			m_type_fasa=1;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();

		}
		if(Dialog.m_at==TRUE)
		{
			m_iDem_ampl=Dialog.m_dem;
			m_type_ampl=1;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
}

void CMainFrame::OnWave() 
{
	// TODO: Add your command handler code here
	if(DialogWave.DoModal()==IDOK)
	{
		if(DialogWave.m_ft==TRUE)
		{
			m_iDem_fasa=DialogWave.m_dem;
			m_cd_fasa.i1=DialogWave.m_amp;
			m_cd_fasa.i2=DialogWave.m_period;
			m_cd_fasa.i3=DialogWave.m_direct;
			m_type_fasa=2;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogWave.m_at==TRUE)
		{
			m_iDem_ampl=DialogWave.m_dem;
			m_cd_ampl.i1=DialogWave.m_amp;
			m_cd_ampl.i2=DialogWave.m_period;
			m_cd_ampl.i3=DialogWave.m_direct;
			m_type_ampl=2;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnChess() 
{
	// TODO: Add your command handler code here
	if(DialogChess.DoModal()==IDOK)
	{
		if(DialogChess.m_ft==TRUE)
		{
			m_iDem_fasa=DialogChess.m_dem;
			m_cd_fasa.i1=DialogChess.m_period;
			m_cd_fasa.i2=DialogChess.m_dark;
			m_cd_fasa.i3=DialogChess.m_light;
			m_type_fasa=3;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogChess.m_at==TRUE)
		{
			m_iDem_ampl=DialogChess.m_dem;
			m_cd_ampl.i1=DialogChess.m_period;
			m_cd_ampl.i2=DialogChess.m_dark;
			m_cd_ampl.i3=DialogChess.m_light;
			m_type_ampl=3;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
}

void CMainFrame::OnRect() 
{
	// TODO: Add your command handler code here
	if(DialogRect.DoModal()==IDOK)
	{
		if(DialogRect.m_ft==TRUE)
		{
			m_iDem_fasa=DialogRect.m_dem;
			m_cd_fasa.i1=DialogRect.m_demrect;
			m_cd_fasa.i2=DialogRect.m_count;
			m_cd_fasa.i3=DialogRect.m_fon;
			m_cd_fasa.i4=DialogRect.m_rect;
			m_type_fasa=4;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogRect.m_at==TRUE)
		{
			m_iDem_ampl=DialogRect.m_dem;
			m_cd_ampl.i1=DialogRect.m_demrect;
			m_cd_ampl.i2=DialogRect.m_count;
			m_cd_ampl.i3=DialogRect.m_fon;
			m_cd_ampl.i4=DialogRect.m_rect;
			m_type_ampl=4;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnCircle() 
{
	// TODO: Add your command handler code here
	if(DialogCircle.DoModal()==IDOK)
	{
		if(DialogCircle.m_ft==TRUE)
		{
			m_iDem_fasa=DialogCircle.m_dem;
			m_cd_fasa.i1=DialogCircle.m_diametr;
			m_cd_fasa.i2=DialogCircle.m_count;
			m_cd_fasa.i3=DialogCircle.m_inten_fon;
			m_cd_fasa.i4=DialogCircle.m_inten_circle;
			m_cd_fasa.i5=DialogCircle.m_ton;
			m_type_fasa=5;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogCircle.m_at==TRUE)
		{
			m_iDem_ampl=DialogCircle.m_dem;
			m_cd_ampl.i1=DialogCircle.m_diametr;
			m_cd_ampl.i2=DialogCircle.m_count;
			m_cd_ampl.i3=DialogCircle.m_inten_fon;
			m_cd_ampl.i4=DialogCircle.m_inten_circle;
			m_cd_ampl.i5=DialogCircle.m_ton;
			m_type_ampl=5;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnNoise() 
{
	// TODO: Add your command handler code here
	if(DialogNoise.DoModal()==IDOK)
	{
		if(DialogNoise.m_ft==TRUE)
		{
			m_iDem_fasa=DialogNoise.m_dem;
			m_cd_fasa.i1=DialogNoise.m_min;
			m_cd_fasa.i2=DialogNoise.m_max;
			m_type_fasa=6;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogNoise.m_at==TRUE)
		{
			m_iDem_ampl=DialogNoise.m_dem;
			m_cd_ampl.i1=DialogNoise.m_min;
			m_cd_ampl.i2=DialogNoise.m_max;
			m_type_ampl=6;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnKlin() 
{
	// TODO: Add your command handler code here
	if(DialogKlin.DoModal() == IDOK) {
		if(DialogKlin.m_ft == TRUE) {
			m_iDem_fasa = DialogKlin.m_dem;
			m_cd_fasa.i1 = DialogKlin.m_direct;
			m_cd_fasa.d1 = DialogKlin.m_krut;
			m_type_fasa = 7;
			m_fasa->ReModeling(m_iDem_fasa, m_type_fasa, m_cd_fasa, 1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]", m_iDem_fasa, m_iDem_fasa);
			m_fasa->SetWindowText(str);
			m_fasa->BringWindowToTop();
		}
		if(DialogKlin.m_at == TRUE) {
			m_iDem_ampl = DialogKlin.m_dem;
			m_cd_ampl.i1 = DialogKlin.m_direct;
			m_cd_ampl.d1 = DialogKlin.m_krut;
			m_type_ampl = 7;
			m_ampl->ReModeling(m_iDem_ampl, m_type_ampl, m_cd_ampl, 1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str, "Модель амплитуды [%dx%d]", m_iDem_ampl, m_iDem_ampl);
			m_ampl->SetWindowText(str);
			m_ampl->BringWindowToTop();
		}
	}
}

void CMainFrame::OnFractal() 
{
	// TODO: Add your command handler code here
	if(DialogFractal.DoModal() == IDOK) {
		if(DialogFractal.m_ft == TRUE) {
			m_iDem_fasa = DialogFractal.m_dem;
			m_cd_fasa.d1 = DialogFractal.m_h;
			m_cd_fasa.i1 = DialogFractal.m_x;
			m_cd_fasa.i2 = DialogFractal.m_y;
			m_type_fasa = 8;
			m_fasa->ReModeling(m_iDem_fasa, m_type_fasa, m_cd_fasa, 1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str, "Модель фазы [%dx%d]", m_iDem_fasa, m_iDem_fasa);
			m_fasa->SetWindowText(str);
			m_fasa->BringWindowToTop();
		}
		if(DialogFractal.m_at==TRUE) {
			m_iDem_ampl = DialogFractal.m_dem;
			m_cd_ampl.d1 = DialogFractal.m_h;
			m_cd_ampl.i1 = DialogFractal.m_x;
			m_cd_ampl.i2 = DialogFractal.m_y;
			m_type_ampl = 8;
			m_ampl->ReModeling(m_iDem_ampl, m_type_ampl, m_cd_ampl, 1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str, "Модель амплитуды [%dx%d]", m_iDem_ampl, m_iDem_ampl);
			m_ampl->SetWindowText(str);
			m_ampl->BringWindowToTop();
		}
	}
}

void CMainFrame::Processing() {
	//m_length_wave -> длина волны
	//m_distance_layer -> расстояние между слоями
	//m_apertura ->размер аппертуры
	LLayer = m_length_wave;
	DLayer = m_apertura;
	
	CSpectrumFrameLayers *m_image;
	CSpectrumFrameLayers *m_spectr;
	
	int Flag = 0;
	double a, u, v, fi, ph;
	size_t ImageSize = m_iDem_fasa;
	double step = 222.0 / ImageSize;
	struct Complex z1, z2;
	char str[300];
	
	// массив изображения
	BYTE *Image_R = new BYTE[ImageSize * ImageSize];
	// спектр конечной реализации
	struct Complex *R = new struct Complex[ImageSize * ImageSize];
	// массив ЧХ
	struct Complex *W = new struct Complex[ImageSize * ImageSize];
	// временный комплексный массив
	struct Complex *TempComp = new struct Complex[ImageSize * ImageSize];     
	
	for(size_t i = 0; i < ImageSize; i++) {
		for(size_t j = 0; j < ImageSize; j++) {
			R[i * ImageSize + j].re = 1.0;
			R[i * ImageSize + j].re = 0.0;
			Image_R[i * ImageSize + j] = 1;
		}
	}
	//вывод коментария
	sprintf(str,"Погодтовка частотной характеристики");
	m_wndStatusBar.SetPaneText(0,str,TRUE);
	
	//подготовка частотной характеристики
	a = ((-1) * M_PI * m_length_wave * m_distance_layer) / (m_apertura * m_apertura);		
	for(size_t i = 0; i < ImageSize; i++) {
		if (i < (size_t) ImageSize / 2) 
			u = i * i;
		else                    
			u = (ImageSize - i) * (ImageSize - i);
		
		for(size_t j = 0; j < ImageSize; j++) {
			if(j < (size_t) ImageSize / 2) 
				v = j * j;
			else                   
				v = (ImageSize - j) * (ImageSize - j);
			fi = a * (u + v);
			cexp(fi, W[i * ImageSize + j]);
		}
	}
	//------------------------------------------------------------------------
	//  Начало реализации "слоевого" метода
	//------------------------------------------------------------------------
	//PntLayer количество слоев
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	for(size_t k = 0; k < (unsigned) PntLayer; k++) {
		sprintf(str, "Идет расчет слоя #%d из %d", k+1, PntLayer);
		m_wndStatusBar.SetPaneText(0, str, TRUE);

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//DifPhaseModul  глубина фазовой модуляции
		//////////////////////////////////////////////////////////////////////////////////////////////////
		m_fasa->ReModeling(m_iDem_fasa, m_type_fasa, m_cd_fasa, 0);
		m_ampl->ReModeling(m_iDem_ampl, m_type_ampl, m_cd_ampl, 0);

		ph = 2.0 * M_PI * DifPhaseModul / (double) ImageSize;
		for(size_t i = 0; i < ImageSize; i++) {
			for(size_t j = 0; j < ImageSize; j++) {
				fi = (m_fasa->m_bArroy[i * ImageSize + j]) * ph;
				z1.re = (float) (m_ampl->m_bArroy[i * ImageSize + j] * cos(fi) / (float) ImageSize);
				z1.im = (float) (m_ampl->m_bArroy[i * ImageSize + j] * sin(fi) / (float) ImageSize);
				z2 = R[i * ImageSize + j];
				cmul(z1, z2, R[i * ImageSize + j]);
				TempComp[i * ImageSize + j] = R[i * ImageSize + j];
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//преобразование фурье 1-ое
		//////////////////////////////////////////////////////////////////////////////////////////////////
		Fft_forward(TempComp);

		//////////////////////////////////////////////////////////////////////////////////////////////////
		// Умножение на частотную характеристику свободного пространства
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if (k != PntLayer - 1) {
			for(size_t i = 0; i < ImageSize; i++) {
				for(size_t j = 0; j < ImageSize; j++) {
					cmul(TempComp[i * ImageSize + j], W[i * ImageSize + j], z1);
					TempComp[i * ImageSize + j] = z1;
				}
			}
		} else {
			for(size_t i = 0; i < ImageSize; i++) {
				for(size_t j = 0; j < ImageSize; j++) {
					z2 = TempComp[i * ImageSize + j];
					cmul(TempComp[i * ImageSize + j], W[i * ImageSize + j], z1);
					TempComp[i * ImageSize + j] = z1;
					W[i * ImageSize + j] = z2;
				}			
			}
			sprintf(str, "Изображение:");
			m_wndStatusBar.SetPaneText(0, str, TRUE);
			m_spectr = new CSpectrumFrameLayers(m_iDem_ampl, TempComp);
			m_spectr->m_fPos_Const = 0.0000001;
			m_spectr->ScaleView();
			m_spectr->m_type_spectr = SPECTR;
			m_spectr->m_type_plant = KOS;
			m_spectr->m_apertura = m_apertura;
			m_spectr->m_length_wave = m_length_wave;
			sprintf(str, "Спектр изображения [%dx%d]", m_iDem_ampl, m_iDem_ampl);
			if (!m_spectr->Create(NULL, _TEXT(str), 0, rectDefault, NULL))
				return;
			m_spectr->MoveWindow(120, 120, 10000, 10000);
			
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// Обратное преобразование Фурье
		//////////////////////////////////////////////////////////////////////////////////////////////////
		Fft_back(TempComp);
	}
	sprintf(str,"Спектр изображения:");
	m_wndStatusBar.SetPaneText(0,str,TRUE);

	m_image = new CSpectrumFrameLayers(m_iDem_ampl, TempComp);
	m_image->Kvadrant = 0;
	m_image->m_fPos_Const = 0.00000000001;
	m_image->ScaleView();
	m_image->m_type_spectr=IMAGE;
	sprintf(str, "Изображение [%dx%d]", m_iDem_ampl, m_iDem_ampl);
	if (!m_image->Create(NULL,_TEXT(str), 0, rectDefault, NULL))
		return;
	m_image->MoveWindow(80, 80, 10000, 10000);
	m_spectr->BringWindowToTop();
	delete[] W;
	delete[] R;
	delete[] TempComp;
	
}

void CMainFrame::Fft_forward(struct Complex *array, int dem) {
	int m_iDem = dem;

	fftw_complex *in = new fftw_complex[m_iDem * m_iDem];
	size_t dxdy = m_iDem * m_iDem;
	
	for(size_t i = 0; i < dxdy; i++) {
		in[i].re = array[i].re;
		in[i].im = array[i].im;
	}
	fftwnd_plan p = fftw2d_create_plan(m_iDem,m_iDem, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
	fftwnd_one(p, in, NULL);
	fftwnd_destroy_plan(p);
	
	for(size_t i = 0; i < dxdy; i++) {
		array[i].re = (float) in[i].re;
		array[i].im = (float) in[i].im;
	}
	delete[] in;
}
void CMainFrame::Fft_forward(struct Complex *array) {
	int m_iDem = m_iDem_ampl;

	fftw_complex *in = new fftw_complex[m_iDem * m_iDem];
	size_t dxdy = m_iDem * m_iDem;
	
	for(size_t i = 0; i < dxdy; i++) {
		in[i].re = array[i].re;
		in[i].im = array[i].im;
	}

	fftwnd_plan p = fftw2d_create_plan(m_iDem,m_iDem, FFTW_FORWARD,FFTW_ESTIMATE|FFTW_IN_PLACE);
	fftwnd_one(p, in, NULL);
	fftwnd_destroy_plan(p);
	
	for(size_t i = 0; i < dxdy; i++) {
		array[i].re = (float) in[i].re;
		array[i].im = (float) in[i].im;
	}
	delete[] in;
}
void CMainFrame::Fft_back(Complex *array, int dem) {
	int m_iDem = dem;
	fftw_complex *in = new fftw_complex[m_iDem * m_iDem];
	size_t dxdy = m_iDem * m_iDem;
	
	for(size_t i = 0; i < dxdy; i++) {
		in[i].re = array[i].re;
		in[i].im = array[i].im;
	}

	fftwnd_plan p = fftw2d_create_plan(m_iDem, m_iDem, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
	fftwnd_one(p, in, NULL);
	fftwnd_destroy_plan(p);
	
	for(size_t i = 0; i < dxdy; i++) {
		array[i].re = (float) in[i].re;
		array[i].im = (float) in[i].im;
	}
	delete[] in;
}
void CMainFrame::Fft_back(Complex *array) {
	int m_iDem = m_iDem_ampl;
	fftw_complex *in = new fftw_complex[m_iDem * m_iDem];
	size_t dxdy = m_iDem * m_iDem;
	
	for(size_t i = 0; i < dxdy; i++) {
		in[i].re = array[i].re;
		in[i].im = array[i].im;
	}

	fftwnd_plan p = fftw2d_create_plan(m_iDem, m_iDem, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
	fftwnd_one(p, in, NULL);
	fftwnd_destroy_plan(p);
	
	for(size_t i = 0; i < dxdy; i++) {
		array[i].re = (float) in[i].re;
		array[i].im = (float) in[i].im;
	}
	delete[] in;
}

void CMainFrame::OnKvazi() 
{
	// TODO: Add your command handler code here
	if(DialogKvazi.DoModal()==IDOK)
	{
		if(DialogKvazi.m_ft==TRUE)
		{
			m_iDem_fasa=DialogKvazi.m_dem;
			m_cd_fasa.d1=DialogKvazi.m_A1;
			m_cd_fasa.d2=DialogKvazi.m_f_A1;
			m_cd_fasa.d3=DialogKvazi.m_y_A1;
			m_cd_fasa.d4=DialogKvazi.m_a;
			m_cd_fasa.d5=DialogKvazi.m_f_a;
			m_cd_fasa.d6=DialogKvazi.m_y_a;
			m_cd_fasa.d7=DialogKvazi.m_A2;
			m_cd_fasa.d8=DialogKvazi.m_f_A2;
			m_cd_fasa.d9=DialogKvazi.m_y_A2;
			m_cd_fasa.d10=DialogKvazi.m_b;
			m_cd_fasa.d11=DialogKvazi.m_f_b;
			m_cd_fasa.d12=DialogKvazi.m_y_b;
			m_cd_fasa.d13=DialogKvazi.m_c;
			m_cd_fasa.d14=DialogKvazi.m_f_c;
			m_cd_fasa.d15=DialogKvazi.m_y_c;
			m_cd_fasa.i1=DialogKvazi.m_x;
			m_cd_fasa.i2=DialogKvazi.m_y;
			m_type_fasa=9;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogKvazi.m_at==TRUE)
		{
			m_iDem_ampl=DialogKvazi.m_dem;
			m_cd_ampl.d1=DialogKvazi.m_A1;
			m_cd_ampl.d2=DialogKvazi.m_f_A1;
			m_cd_ampl.d3=DialogKvazi.m_y_A1;
			m_cd_ampl.d4=DialogKvazi.m_a;
			m_cd_ampl.d5=DialogKvazi.m_f_a;
			m_cd_ampl.d6=DialogKvazi.m_y_a;
			m_cd_ampl.d7=DialogKvazi.m_A2;
			m_cd_ampl.d8=DialogKvazi.m_f_A2;
			m_cd_ampl.d9=DialogKvazi.m_y_A2;
			m_cd_ampl.d10=DialogKvazi.m_b;
			m_cd_ampl.d11=DialogKvazi.m_f_b;
			m_cd_ampl.d12=DialogKvazi.m_y_b;
			m_cd_ampl.d13=DialogKvazi.m_c;
			m_cd_ampl.d14=DialogKvazi.m_f_c;
			m_cd_ampl.d15=DialogKvazi.m_y_c;
			m_cd_ampl.i1=DialogKvazi.m_x;
			m_cd_ampl.i2=DialogKvazi.m_y;
			m_type_ampl=9;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}

	
}

void CMainFrame::OnObob() 
{
	// TODO: Add your command handler code here
	if(DialogObob.DoModal()==IDOK)
	{
		if(DialogObob.m_ft==TRUE)
		{
			m_iDem_fasa=DialogObob.m_dem;
			m_cd_fasa.d1=DialogObob.m_A1;
			m_cd_fasa.d2=DialogObob.m_f_A1;
			m_cd_fasa.d3=DialogObob.m_y_A1;
			m_cd_fasa.d4=DialogObob.m_a;
			m_cd_fasa.d5=DialogObob.m_f_a;
			m_cd_fasa.d6=DialogObob.m_y_a;
			m_cd_fasa.d7=DialogObob.m_A2;
			m_cd_fasa.d8=DialogObob.m_f_A2;
			m_cd_fasa.d9=DialogObob.m_y_A2;
			m_cd_fasa.d10=DialogObob.m_al;
			m_cd_fasa.d11=DialogObob.m_f_al;
			m_cd_fasa.d12=DialogObob.m_y_al;
			m_cd_fasa.d13=DialogObob.m_be;
			m_cd_fasa.d14=DialogObob.m_f_be;
			m_cd_fasa.d15=DialogObob.m_y_be;
			m_cd_fasa.d16=DialogObob.m_n;
			m_cd_fasa.d17=DialogObob.m_f_n;
			m_cd_fasa.d18=DialogObob.m_y_n;
			m_cd_fasa.i1=DialogObob.m_x;
			m_cd_fasa.i2=DialogObob.m_y;
			m_type_fasa=10;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogObob.m_at==TRUE)
		{
			m_iDem_ampl=DialogObob.m_dem;
			m_cd_ampl.d1=DialogObob.m_A1;
			m_cd_ampl.d2=DialogObob.m_f_A1;
			m_cd_ampl.d3=DialogObob.m_y_A1;
			m_cd_ampl.d4=DialogObob.m_a;
			m_cd_ampl.d5=DialogObob.m_f_a;
			m_cd_ampl.d6=DialogObob.m_y_a;
			m_cd_ampl.d7=DialogObob.m_A2;
			m_cd_ampl.d8=DialogObob.m_f_A2;
			m_cd_ampl.d9=DialogObob.m_y_A2;
			m_cd_ampl.d10=DialogObob.m_al;
			m_cd_ampl.d11=DialogObob.m_f_al;
			m_cd_ampl.d12=DialogObob.m_y_al;
			m_cd_ampl.d13=DialogObob.m_be;
			m_cd_ampl.d14=DialogObob.m_f_be;
			m_cd_ampl.d15=DialogObob.m_y_be;
			m_cd_ampl.d16=DialogObob.m_n;
			m_cd_ampl.d17=DialogObob.m_f_n;
			m_cd_ampl.d18=DialogObob.m_y_n;
			m_cd_ampl.i1=DialogObob.m_x;
			m_cd_ampl.i2=DialogObob.m_y;
			m_type_ampl=10;
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnAnizotrop() 
{
	// TODO: Add your command handler code here
	if(DialogAniz.DoModal()==IDOK)
	{
		if(DialogAniz.m_ft==TRUE)
		{
			m_iDem_ampl=DialogAniz.m_dem;
			m_cd_fasa.d1=DialogAniz.m_a;
			m_cd_fasa.d2=DialogAniz.m_b;
			m_cd_fasa.d3=DialogAniz.m_c;
			m_cd_fasa.d4=DialogAniz.m_d;
			m_cd_fasa.d5=DialogAniz.m_al1;
			m_cd_fasa.d6=DialogAniz.m_al2;
			m_cd_fasa.d7=DialogAniz.m_be1;
			m_cd_fasa.d8=DialogAniz.m_be2;
			m_cd_fasa.i1=DialogAniz.m_x;
			m_cd_fasa.i2=DialogAniz.m_y;
			m_type_fasa=11;
			
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogAniz.m_at==TRUE)
		{
			m_iDem_ampl=DialogAniz.m_dem;
			m_cd_ampl.d1=DialogAniz.m_a;
			m_cd_ampl.d2=DialogAniz.m_b;
			m_cd_ampl.d3=DialogAniz.m_c;
			m_cd_ampl.d4=DialogAniz.m_d;
			m_cd_ampl.d5=DialogAniz.m_al1;
			m_cd_ampl.d6=DialogAniz.m_al2;
			m_cd_ampl.d7=DialogAniz.m_be1;
			m_cd_ampl.d8=DialogAniz.m_be2;
			m_cd_ampl.i1=DialogAniz.m_x;
			m_cd_ampl.i2=DialogAniz.m_y;
			m_type_ampl=11;
			
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}

	
}

void CMainFrame::OnTurbo() 
{
	// TODO: Add your command handler code here
	if(DialogTurbo.DoModal()==IDOK)
	{
		if(DialogTurbo.m_ft==TRUE)
		{
			m_iDem_ampl=DialogTurbo.m_dem;
			//////////////////////
			m_cd_fasa.d1=DialogTurbo.m_A;
			m_cd_fasa.d2=DialogTurbo.m_Afi;
			m_cd_fasa.d3=DialogTurbo.m_Agam;
			m_cd_fasa.d4=DialogTurbo.m_alfa;
			m_cd_fasa.d5=DialogTurbo.m_afi;
			m_cd_fasa.d6=DialogTurbo.m_agam;
			m_cd_fasa.d7=DialogTurbo.m_B;
			m_cd_fasa.d8=DialogTurbo.m_Bfi;
			m_cd_fasa.d9=DialogTurbo.m_Bgam;
			m_cd_fasa.d10=DialogTurbo.m_gamma;
			m_cd_fasa.d11=DialogTurbo.m_gfi;
			m_cd_fasa.d12=DialogTurbo.m_ggam;
			m_cd_fasa.d13=DialogTurbo.m_W;
			m_cd_fasa.d14=DialogTurbo.m_Wfi;
			m_cd_fasa.d15=DialogTurbo.m_Wgam;
			m_cd_fasa.i1=DialogTurbo.m_x;
			m_cd_fasa.i2=DialogTurbo.m_y;
			//////////////////////
			m_type_fasa=12;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogTurbo.m_at==TRUE)
		{
			m_iDem_ampl=DialogTurbo.m_dem;
			//////////////////////
			m_cd_ampl.d1=DialogTurbo.m_A;
			m_cd_ampl.d2=DialogTurbo.m_Afi;
			m_cd_ampl.d3=DialogTurbo.m_Agam;
			m_cd_ampl.d4=DialogTurbo.m_alfa;
			m_cd_ampl.d5=DialogTurbo.m_afi;
			m_cd_ampl.d6=DialogTurbo.m_agam;
			m_cd_ampl.d7=DialogTurbo.m_B;
			m_cd_ampl.d8=DialogTurbo.m_Bfi;
			m_cd_ampl.d9=DialogTurbo.m_Bgam;
			m_cd_ampl.d10=DialogTurbo.m_gamma;
			m_cd_ampl.d11=DialogTurbo.m_gfi;
			m_cd_ampl.d12=DialogTurbo.m_ggam;
			m_cd_ampl.d13=DialogTurbo.m_W;
			m_cd_ampl.d14=DialogTurbo.m_Wfi;
			m_cd_ampl.d15=DialogTurbo.m_Wgam;
			m_cd_ampl.i1=DialogTurbo.m_x;
			m_cd_ampl.i2=DialogTurbo.m_y;
			//////////////////////
			m_type_ampl=12;			
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnExample() 
{
//	FILE *in;
	int temp;
	CDib *bmp_file;
	TCHAR title[]="Открытие BMP";
	CFileDialog fileDlg(TRUE);
	CString str("Все файлы (*.*)"); str += (TCHAR)NULL;
	str += "*.*"; str += (TCHAR)NULL;
	str += "BMP файлы (*.bmp)"; str += (TCHAR)NULL;
	str += "*.bmp"; str += (TCHAR)NULL;
	fileDlg.m_ofn.lpstrFilter = str;
	fileDlg.m_ofn.nFilterIndex = 2;
	TCHAR strName[_MAX_PATH];
	strName[0] = (TCHAR)NULL;
	fileDlg.m_ofn.lpstrFile = strName;
	fileDlg.m_ofn.lpstrTitle = title;
	
	
	if(DialogExample.DoModal()==IDOK)
	{
		if(DialogExample.m_ft==TRUE)
		{
			if (fileDlg.DoModal() == IDOK)
			{
				bmp_file=new CDib(fileDlg.m_ofn.lpstrFile);
				temp=bmp_file->InfoHeader.biHeight;
				if(temp!=512)
				{
					AfxMessageBox("Размер изображения должен быть равен 512, 1024, 2048");
				}
				else
				{
					
					m_type_fasa=15;
					m_cd_fasa.i1=0;
					m_cd_fasa.i2=255;
					strcpy(m_fasa->NameFile,fileDlg.m_ofn.lpstrFile);
					m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
					m_fasa->ShowWindow(SW_SHOW);
					char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
				}
		}
		
	}
		if(DialogExample.m_at==TRUE)
		{
			if (fileDlg.DoModal() == IDOK)
			{
				bmp_file=new CDib(fileDlg.m_ofn.lpstrFile);
				temp=bmp_file->InfoHeader.biHeight;
				if(temp!=512)
				{
					AfxMessageBox("Размер изображения должен быть равен 512, 1024, 2048");
				}
				else
				{
					
					m_type_ampl=15;
					m_cd_ampl.i1=0;
					m_cd_ampl.i2=255;
					strcpy(m_ampl->NameFile,fileDlg.m_ofn.lpstrFile);
					m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
					m_ampl->ShowWindow(SW_SHOW);
					char str[300];
					sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
					m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
				}
			}
		}
	}
	// TODO: Add your command handler code here
}

void CMainFrame::OnKvazi2() 
{
	// TODO: Add your command handler code here
	if(DialogKvazi2.DoModal()==IDOK)
	{
		if(DialogKvazi2.m_ft==TRUE)
		{
			m_iDem_ampl=DialogKvazi2.m_dem;
			//////////////////////
			m_cd_fasa.d1=DialogKvazi2.m_A1;
			m_cd_fasa.d2=DialogKvazi2.m_f_A1;
			m_cd_fasa.d3=DialogKvazi2.m_y_A1;
			m_cd_fasa.d4=DialogKvazi2.m_alfa1;
			m_cd_fasa.d5=DialogKvazi2.m_f_alfa1;
			m_cd_fasa.d6=DialogKvazi2.m_y_alfa1;
			m_cd_fasa.d7=DialogKvazi2.m_A2;
			m_cd_fasa.d8=DialogKvazi2.m_f_A2;
			m_cd_fasa.d9=DialogKvazi2.m_y_A2;
			m_cd_fasa.d10=DialogKvazi2.m_A3;
			m_cd_fasa.d11=DialogKvazi2.m_f_A3;
			m_cd_fasa.d12=DialogKvazi2.m_y_A3;
			m_cd_fasa.d13=DialogKvazi2.m_a;
			m_cd_fasa.d14=DialogKvazi2.m_f_a;
			m_cd_fasa.d15=DialogKvazi2.m_y_a;
			m_cd_fasa.d16=DialogKvazi2.m_b;
			m_cd_fasa.d17=DialogKvazi2.m_f_b;
			m_cd_fasa.d18=DialogKvazi2.m_y_b;
			m_cd_fasa.d19=DialogKvazi2.m_c;
			m_cd_fasa.d20=DialogKvazi2.m_f_c;
			m_cd_fasa.d21=DialogKvazi2.m_y_c;
			m_cd_fasa.d22=DialogKvazi2.m_alfa2;
			m_cd_fasa.d23=DialogKvazi2.m_f_alfa2;
			m_cd_fasa.d24=DialogKvazi2.m_y_alfa2;
			m_cd_fasa.d25=DialogKvazi2.m_beta;
			m_cd_fasa.d26=DialogKvazi2.m_f_beta;
			m_cd_fasa.d27=DialogKvazi2.m_y_beta;
			//////////////////////
			m_cd_fasa.i1=DialogKvazi2.m_x;
			m_cd_fasa.i2=DialogKvazi2.m_y;
			//////////////////////
			m_type_fasa=13;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogKvazi2.m_at==TRUE)
		{
			m_iDem_ampl=DialogKvazi2.m_dem;
			//////////////////////
			m_cd_ampl.d1=DialogKvazi2.m_A1;
			m_cd_ampl.d2=DialogKvazi2.m_f_A1;
			m_cd_ampl.d3=DialogKvazi2.m_y_A1;
			m_cd_ampl.d4=DialogKvazi2.m_alfa1;
			m_cd_ampl.d5=DialogKvazi2.m_f_alfa1;
			m_cd_ampl.d6=DialogKvazi2.m_y_alfa1;
			m_cd_ampl.d7=DialogKvazi2.m_A2;
			m_cd_ampl.d8=DialogKvazi2.m_f_A2;
			m_cd_ampl.d9=DialogKvazi2.m_y_A2;
			m_cd_ampl.d10=DialogKvazi2.m_A3;
			m_cd_ampl.d11=DialogKvazi2.m_f_A3;
			m_cd_ampl.d12=DialogKvazi2.m_y_A3;
			m_cd_ampl.d13=DialogKvazi2.m_a;
			m_cd_ampl.d14=DialogKvazi2.m_f_a;
			m_cd_ampl.d15=DialogKvazi2.m_y_a;
			m_cd_ampl.d16=DialogKvazi2.m_b;
			m_cd_ampl.d17=DialogKvazi2.m_f_b;
			m_cd_ampl.d18=DialogKvazi2.m_y_b;
			m_cd_ampl.d19=DialogKvazi2.m_c;
			m_cd_ampl.d20=DialogKvazi2.m_f_c;
			m_cd_ampl.d21=DialogKvazi2.m_y_c;
			m_cd_ampl.d22=DialogKvazi2.m_alfa2;
			m_cd_ampl.d23=DialogKvazi2.m_f_alfa2;
			m_cd_ampl.d24=DialogKvazi2.m_y_alfa2;
			m_cd_ampl.d25=DialogKvazi2.m_beta;
			m_cd_ampl.d26=DialogKvazi2.m_f_beta;
			m_cd_ampl.d27=DialogKvazi2.m_y_beta;
			//////////////////////
			m_cd_ampl.i1=DialogKvazi2.m_x;
			m_cd_ampl.i2=DialogKvazi2.m_y;
			//////////////////////
			m_type_ampl=13;			
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}

	
}

void CMainFrame::OnSp2type() 
{
	// TODO: Add your command handler code here
	if(DialogSP2Type.DoModal()==IDOK)
	{
		if(DialogSP2Type.m_ft==TRUE)
		{
			m_iDem_ampl=DialogSP2Type.m_dem;
			//////////////////////
			m_cd_fasa.d1=DialogSP2Type.m_A1;
			m_cd_fasa.d2=DialogSP2Type.m_f_A1;
			m_cd_fasa.d3=DialogSP2Type.m_y_A1;

			m_cd_fasa.d4=DialogSP2Type.m_A2;
			m_cd_fasa.d5=DialogSP2Type.m_f_A2;
			m_cd_fasa.d6=DialogSP2Type.m_y_A2;

			m_cd_fasa.d7=DialogSP2Type.m_A3;
			m_cd_fasa.d8=DialogSP2Type.m_f_A3;
			m_cd_fasa.d9=DialogSP2Type.m_y_A3;

			m_cd_fasa.d10=DialogSP2Type.m_alfa1;
			m_cd_fasa.d11=DialogSP2Type.m_f_alfa1;
			m_cd_fasa.d12=DialogSP2Type.m_y_alfa1;

			m_cd_fasa.d13=DialogSP2Type.m_alfa2;
			m_cd_fasa.d14=DialogSP2Type.m_f_alfa2;
			m_cd_fasa.d15=DialogSP2Type.m_y_alfa2;

			m_cd_fasa.d16=DialogSP2Type.m_alfa3;
			m_cd_fasa.d17=DialogSP2Type.m_f_alfa3;
			m_cd_fasa.d18=DialogSP2Type.m_y_alfa3;

			m_cd_fasa.d19=DialogSP2Type.m_a;
			m_cd_fasa.d20=DialogSP2Type.m_f_a;
			m_cd_fasa.d21=DialogSP2Type.m_y_a;

			m_cd_fasa.d22=DialogSP2Type.m_b;
			m_cd_fasa.d23=DialogSP2Type.m_f_b;
			m_cd_fasa.d24=DialogSP2Type.m_y_b;

			m_cd_fasa.d25=DialogSP2Type.m_c;
			m_cd_fasa.d26=DialogSP2Type.m_f_c;
			m_cd_fasa.d27=DialogSP2Type.m_y_c;

			m_cd_fasa.d28=DialogSP2Type.m_beta;
			m_cd_fasa.d29=DialogSP2Type.m_f_beta;
			m_cd_fasa.d30=DialogSP2Type.m_y_beta;

			m_cd_fasa.d31=DialogSP2Type.m_gamma;
			m_cd_fasa.d32=DialogSP2Type.m_f_gamma;
			m_cd_fasa.d33=DialogSP2Type.m_y_gamma;
			//////////////////////
			m_cd_fasa.i1=DialogSP2Type.m_x;
			m_cd_fasa.i2=DialogSP2Type.m_y;
			//////////////////////
			m_type_fasa=14;
			m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,1);
			m_fasa->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель фазы [%dx%d]",m_iDem_fasa,m_iDem_fasa);
			m_fasa->SetWindowText(str);m_fasa->BringWindowToTop();
		}
		if(DialogSP2Type.m_at==TRUE)
		{
			m_iDem_ampl=DialogSP2Type.m_dem;
			//////////////////////
			m_cd_ampl.d1=DialogSP2Type.m_A1;
			m_cd_ampl.d2=DialogSP2Type.m_f_A1;
			m_cd_ampl.d3=DialogSP2Type.m_y_A1;

			m_cd_ampl.d4=DialogSP2Type.m_A2;
			m_cd_ampl.d5=DialogSP2Type.m_f_A2;
			m_cd_ampl.d6=DialogSP2Type.m_y_A2;

			m_cd_ampl.d7=DialogSP2Type.m_A3;
			m_cd_ampl.d8=DialogSP2Type.m_f_A3;
			m_cd_ampl.d9=DialogSP2Type.m_y_A3;

			m_cd_ampl.d10=DialogSP2Type.m_alfa1;
			m_cd_ampl.d11=DialogSP2Type.m_f_alfa1;
			m_cd_ampl.d12=DialogSP2Type.m_y_alfa1;

			m_cd_ampl.d13=DialogSP2Type.m_alfa2;
			m_cd_ampl.d14=DialogSP2Type.m_f_alfa2;
			m_cd_ampl.d15=DialogSP2Type.m_y_alfa2;

			m_cd_ampl.d16=DialogSP2Type.m_alfa3;
			m_cd_ampl.d17=DialogSP2Type.m_f_alfa3;
			m_cd_ampl.d18=DialogSP2Type.m_y_alfa3;

			m_cd_ampl.d19=DialogSP2Type.m_a;
			m_cd_ampl.d20=DialogSP2Type.m_f_a;
			m_cd_ampl.d21=DialogSP2Type.m_y_a;

			m_cd_ampl.d22=DialogSP2Type.m_b;
			m_cd_ampl.d23=DialogSP2Type.m_f_b;
			m_cd_ampl.d24=DialogSP2Type.m_y_b;

			m_cd_ampl.d25=DialogSP2Type.m_c;
			m_cd_ampl.d26=DialogSP2Type.m_f_c;
			m_cd_ampl.d27=DialogSP2Type.m_y_c;

			m_cd_ampl.d28=DialogSP2Type.m_beta;
			m_cd_ampl.d29=DialogSP2Type.m_f_beta;
			m_cd_ampl.d30=DialogSP2Type.m_y_beta;

			m_cd_ampl.d31=DialogSP2Type.m_gamma;
			m_cd_ampl.d32=DialogSP2Type.m_f_gamma;
			m_cd_ampl.d33=DialogSP2Type.m_y_gamma;
			//////////////////////
			m_cd_ampl.i1=DialogSP2Type.m_x;
			m_cd_ampl.i2=DialogSP2Type.m_y;
			//////////////////////
			m_type_ampl=14;			
			m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,1);
			m_ampl->ShowWindow(SW_SHOW);
			char str[300];
			sprintf(str,"Модель амплитуды [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			m_ampl->SetWindowText(str);m_ampl->BringWindowToTop();
		}
	}
	
}

void CMainFrame::OnStart() 
{
	// TODO: Add your command handler code here
	m_ampl->ShowWindow(SW_SHOW);
	m_fasa->ShowWindow(SW_SHOW);
	char str[300];
	if(m_iDem_ampl!=m_iDem_fasa)
	{
		sprintf(str,"Размер модели амплитуды (%d px) не равен размеру модели фазы (%d px).\nМодели должны быть одинакового размера.",m_iDem_ampl,m_iDem_fasa);
		MessageBox(str,MB_OK);
	}
	else
	{
		if(m_type_of_system==KOS)
		{
			Processing();	
		}
		else
		{
			ProcessingPAM();	
		}
	}
}

void CMainFrame::ProcessingPAM()
{
	//m_length_wave -> длина волны
	//m_distance_layer -> расстояние между слоями
	//m_apertura ->размер аппертуры
	CDialogInfo*  DialogInfo = new CDialogInfo(NULL);
	DialogInfo->Create(IDD_INFO,NULL);
	
				
	LLayer=m_length_wave;
	DLayer=m_apertura;
	
	CSpectrumFrameLayers *m_image;
	CSpectrumFrameLayers *m_spectr;
	
	int i, j, k, Flag = 0;
	double a, u, v, fi, ph;
	int ImageSize=m_iDem_fasa;
	double step = 222./ImageSize;
	struct Complex z1, z2;
	char str[300];
	
	// массив изображения
	BYTE *Image_R =new BYTE[ImageSize*ImageSize];
	// спектр конечной реализации
	struct Complex *R=new struct Complex[ImageSize*ImageSize];
	// массив ЧХ
	struct Complex *W=new struct Complex[ImageSize*ImageSize];
	// временный комплексный массив
	struct Complex *TempComp=new struct Complex[ImageSize*ImageSize];     
	
	for(i=0; i<ImageSize; i++)
	{
		for(j=0; j<ImageSize; j++) 
		{
			R[i*ImageSize+j].re=1;
			R[i*ImageSize+j].re=0;
			Image_R[i*ImageSize+j] = 1;
		}
	}
	//вывод коментария
	sprintf(str,"Погодтовка частотной характеристики");
	m_wndStatusBar.SetPaneText(0,str,TRUE);
	
	DialogInfo->ShowWindow(SW_SHOW);
	DialogInfo->m_str.Format("Погодтовка частотной характеристики");
	DialogInfo->UpdateData(FALSE);
	
	//подготовка частотной характеристики
	a = (M_PI*m_length_wave*m_distance_layer*(-1))/(m_apertura*m_apertura);		
	
	for(i=0; i<ImageSize; i++) 
	{
		if (i<(int)ImageSize/2) 
			u = i*i;
		else                    
			u = (ImageSize-i)*(ImageSize-i);
		
		for(j=0; j<ImageSize; j++) 
		{
			if(j<(int)ImageSize/2) 
				v = j*j;
			else                   
				v = (ImageSize-j)*(ImageSize-j);
			fi = a*(u+v);
			cexp(fi, W[i*ImageSize+j]);
		}
	}
	//------------------------------------------------------------------------
	//  Начало реализации "слоевого" метода
	//------------------------------------------------------------------------
	//PntLayer количество слоев
	for(k=0; k<PntLayer; k++) 
	{
		sprintf(str,"Идет расчет слоя #%d из %d",k+1,PntLayer);
		m_wndStatusBar.SetPaneText(0,str,TRUE);

		DialogInfo->ShowWindow(SW_SHOW);
		DialogInfo->m_str.Format("Идет расчет слоя #%d из %d",k+1,PntLayer);
		DialogInfo->UpdateData(FALSE);


		//DifPhaseModul  глубина фазовой модуляции
		m_fasa->ReModeling(m_iDem_fasa,m_type_fasa,m_cd_fasa,0);
		m_ampl->ReModeling(m_iDem_ampl,m_type_ampl,m_cd_ampl,0);

		ph = 2.*M_PI*DifPhaseModul/(double)ImageSize;
		for(i=0; i<ImageSize; i++) 
		{
			for(j=0; j<ImageSize; j++) 
			{
				fi = (m_fasa->m_bArroy[i*ImageSize+j])*ph;
				z1.re =(float)(m_ampl->m_bArroy[i*ImageSize+j]*cos(fi)/(float)ImageSize);
				z1.im = (float)(m_ampl->m_bArroy[i*ImageSize+j]*sin(fi)/(float)ImageSize);
				z2 = R[i*ImageSize+j];
				cmul(z1, z2, R[i*ImageSize+j]);
				TempComp[i*ImageSize+j] = R[i*ImageSize+j];
			}
		}

		//преобразование фурье 1-ое
		Fft_forward(TempComp);
		// Умножение на частотную характеристику свободного пространства
		if (k != PntLayer-1) 
		{
			for(i=0; i<ImageSize; i++) 
			{
				for(j=0; j<ImageSize; j++) 
				{
					cmul(TempComp[i*ImageSize+j], W[i*ImageSize+j], z1);
					TempComp[i*ImageSize+j] = z1;
				}
			}
		}
		else 
		{
			for(i=0; i<ImageSize; i++) 
			{
				for(j=0; j<ImageSize; j++) 
				{
					z2 = TempComp[i*ImageSize+j];
					cmul(TempComp[i*ImageSize+j], W[i*ImageSize+j], z1);
					TempComp[i*ImageSize+j] = z1;
					W[i*ImageSize+j]        = z2;
				}			
			}
			sprintf(str,"Спектр изображения:");
			m_wndStatusBar.SetPaneText(0,str,TRUE);
			
			DialogInfo->ShowWindow(SW_SHOW);
			DialogInfo->m_str.Format("Погодтовка cпектра изображения.");
			DialogInfo->UpdateData(FALSE);

			
			m_spectr = new CSpectrumFrameLayers(m_iDem_ampl,TempComp);
		
			m_spectr->m_apertura=m_apertura;
			m_spectr->m_length_wave=m_length_wave;

			m_spectr->GetOptimumCoefficient();
			m_spectr->m_fPos_Const*=100000;
			m_spectr->ScaleView();			
			m_spectr->m_type_spectr=SPECTR;
			m_spectr->m_type_plant=PAM;
			sprintf(str,"Спектр изображения [%dx%d]",m_iDem_ampl,m_iDem_ampl);
			
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return;
			m_spectr->MoveWindow(120,120,10000,10000);
			
		}
		// Обратное преобразование Фурье
		Fft_back(TempComp);
	}
	sprintf(str,"Изображение:");
	m_wndStatusBar.SetPaneText(0,str,TRUE);
	DialogInfo->ShowWindow(SW_SHOW);
	DialogInfo->m_str.Format("Погодтовка изображения.");
	DialogInfo->UpdateData(FALSE);

	m_image = new CSpectrumFrameLayers(m_iDem_ampl,TempComp);
	m_image->Kvadrant=0;
	m_image->m_fPos_Const=0.00000000001;
	m_image->ScaleView();
	m_image->m_type_spectr=IMAGE;
	sprintf(str,"Изображение [%dx%d]",m_iDem_ampl,m_iDem_ampl);
	
	if (!m_image->Create(NULL,_TEXT(str),0, rectDefault, NULL))
		return;
	DialogInfo->DestroyWindow();
	m_image->MoveWindow(80,80,10000,10000);
	m_spectr->BringWindowToTop();
	
	delete[] W;
	delete[] R;
	delete[] TempComp;

}

void CMainFrame::OnAtoms() 
{
	m_ampl->ShowWindow(SW_HIDE);
	m_fasa->ShowWindow(SW_HIDE);
	CSpectrumFrame *m_image;
	CSpectrumFrame *m_spectr;
	char str[300];

	if(DialogAtoms.DoModal()==IDOK) {
		CLatGenerator	LatGen;
		
		int m_iDem_temp = DialogAtoms.m_dem;
		int n2 = m_iDem_temp * m_iDem_temp;
		
		struct Complex *TempComp = new struct Complex[m_iDem_temp * m_iDem_temp];
		
		if (LatGen.ShowDialog()) {
			fi_complex *pfi_complex = new fi_complex[m_iDem_temp * m_iDem_temp];
			float *pfImage = new float[m_iDem_temp * m_iDem_temp];
			
			SRenderParam RenderParam;
			RenderParam.m_pfi_complex = pfi_complex;
			RenderParam.m_nWidth = m_iDem_temp;
			RenderParam.m_nHeight = m_iDem_temp;
			RenderParam.m_dLambda = m_length_wave;
			RenderParam.m_dSigma = M_PI / (m_length_wave * m_napr);
			
			LatGen.RenderLayer(&RenderParam);
			RenderParam.m_dWidthAng;
			/////////////////////////
			for(size_t i = 0; i < n2; i++) {
				TempComp[i].im = pfi_complex[i].im;
				TempComp[i].re = pfi_complex[i].re;
			}
			
			//////////////////////////////////////////
			m_image = new CSpectrumFrameLayers(m_iDem_temp, TempComp);
			m_image->Kvadrant = 0;
			m_image->m_fPos_Const = 0.0005;
			m_image->ScaleView();
			m_image->m_type_spectr = IMAGE;
			sprintf(str, "Изображение атомной структуры [%dx%d]", m_iDem_temp, m_iDem_temp);
			if (!m_image->Create(NULL, _TEXT(str), 0, rectDefault, NULL))
				return;
			m_image->MoveWindow(80, 80, 10000, 10000);

			Fft_forward(TempComp,m_iDem_temp);
			m_spectr = new CSpectrumFrameLayers(m_iDem_temp, TempComp);
			m_spectr->m_apertura = RenderParam.m_dWidthAng;
			m_spectr->m_length_wave = m_length_wave;
			m_spectr->GetOptimumCoefficient();
			m_spectr->m_fPos_Const *= 100000;
			m_spectr->ScaleView();
			m_spectr->m_type_spectr = SPECTR;
			m_spectr->m_type_plant = PAM;
			sprintf(str, "Спектр атомной структуры [%dx%d]", m_iDem_temp, m_iDem_temp);
			if (!m_spectr->Create(NULL,_TEXT(str), 0, rectDefault, NULL))
				return;
			m_spectr->MoveWindow(120, 120, 10000, 10000);
			delete[] TempComp;
			///////////////////////
		}
	}
}

void CMainFrame::OnShowWin() 
{
	// TODO: Add your command handler code here
	m_fasa->ShowWindow(SW_SHOW);
	m_ampl->ShowWindow(SW_SHOW);
}

BOOL CMainFrame::DestroyWindow() 
{
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	ASSERT(app);

	// TODO: Add your specialized code here and/or call the base class
	if(app->i_database!=NULL)
	{
		app->i_database->Release();
		app->i_database=NULL;
	}
	
	return CMDIFrameWnd::DestroyWindow();
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	m_wndToolBar.OnUpdateCmdUI(this,TRUE);
	return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnHelpIndex() 
{
	FILE *f_help;
	f_help = fopen("help.bat", "w");
	fprintf(f_help, "start CrazyLayers.chm\ndel help.bat");
	fclose(f_help);
	if(WinExec("help.bat", SW_RESTORE) == ERROR_FILE_NOT_FOUND) {
		AfxMessageBox("Не найден файл help.bat", MB_OK, NULL);
	}
	//remove("help.bat");
	//void *p=HtmlHelp((HWND)GetDesktopWindow(),"CrazyLayers.chm",HH_DISPLAY_TOPIC,NULL) ;
}