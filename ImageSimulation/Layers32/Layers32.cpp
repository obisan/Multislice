// Layers32.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Layers32.h"
#include "MainFrm.h"
#include "ModelFrame.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayers32App

BEGIN_MESSAGE_MAP(CLayers32App, CWinApp)
	//{{AFX_MSG_MAP(CLayers32App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayers32App construction

CLayers32App::CLayers32App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	i_database=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLayers32App object

CLayers32App theApp;

double DLayer;
double LLayer;

/////////////////////////////////////////////////////////////////////////////
// CLayers32App initialization

BOOL CLayers32App::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	SetRegistryKey(_T("Studio Image Proccessing - Crazy Layers"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need. 

	HINSTANCE hInst = AfxGetResourceHandle();
	//m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_LAYERSTYPE));
	//m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_LAYERSTYPE));
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));


	// The main window has been initialized, so show and update it.
	pFrame->SetTitle("Crazy Layers");
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLayers32App message handlers

int CLayers32App::ExitInstance() 
{
	//TODO: handle additional resources you may have added
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);
	


	return CWinApp::ExitInstance();
}

/*void CLayers32App::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	// create a new MDI child window
	pFrame->CreateNewChild(RUNTIME_CLASS(CModelFrame),IDR_LAYERSTYPE, m_hMDIMenu, m_hMDIAccel);
}*/



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CLayers32App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CLayers32App message handlers

/////////////////////////////////////////////////////////////////////////////
/////подключение к базе
//BOOL DipProjectActivate (double idi, IUnknown* iface);
BOOL L2DB (double idi, IUnknown* iface);

extern "C" _declspec(dllexport)  int PluginInfoAbout(IUnknown* iFace)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	IPluginCtrl*	iface = NULL;
	iFace->QueryInterface(IID_IPluginCtrl,(void**)&iface);

	DWORD  idmenu = NULL;
	idmenu = iface->SetMenuCommand("Моделирование изображений","Моделирование изображений");
	iface->SetMenuFunction(idmenu,L2DB);
	
	idmenu = iface->SetMenuDatabase("Моделирование изображений","Моделирование изображений");
	iface->SetMenuFunction(idmenu,L2DB);

	iface->Release();
	return 1;
}


//	вызывается при уничтожении плагина

extern "C" _declspec(dllexport)  BOOL CanTerminatePlugin(void* param)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}

extern "C" _declspec(dllexport)  BOOL TerminatePlugin(void* param)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//////
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	if (app->m_pMainWnd)
		if (app->m_pMainWnd->m_hWnd)	app->m_pMainWnd->DestroyWindow();
	app->ExitInstance();
	///////
	return TRUE;
}


BOOL L2DB (double idi, IUnknown* iface)
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	//	<- Обязательно !!!

	//	активизируем окно приложения
	CLayers32App*  app = (CLayers32App*) AfxGetApp();
	if (app == NULL)
	{
		MessageBox(0,"Странно...  Функция есть, а приложения нету","DipProject Error!",0);
		return FALSE;
	}

	CMainFrame*  pMainFrame = (CMainFrame*) app->m_pMainWnd;
	
	
	if (pMainFrame == NULL)
	{
		pMainFrame = new CMainFrame;
		app->m_pMainWnd = pMainFrame;
		if (pMainFrame->LoadFrame(IDR_MAINFRAME)==FALSE)
		{
			MessageBox(0,"Не могу создать окно приложения","DipProject Error!",0);
			return FALSE;
		}
	}
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->ActivateFrame();

	//	получаем интерфейсы на БД и на изображение
	IDatabase*	 database = NULL;
	iface->QueryInterface(IID_IDatabase,(void**)&database);
	if (database == NULL)		return FALSE;
	
    if(app->i_database!=NULL)
	{
		app->i_database->Release();
		app->i_database=NULL;
	}
	app->i_database=database;
	database->CreateImage(NULL,"aa",10,10,0);

	//	i_database->Release();		<- не вызывать; вызов будет в CImageDoc::DeleteContext()
	return TRUE;
}
