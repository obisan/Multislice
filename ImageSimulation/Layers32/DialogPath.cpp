// DialogPath.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPath dialog


CDialogPath::CDialogPath(CWnd* pParent /*=NULL*/)
: CDialog(CDialogPath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPath)
	//m_apertura = 30.0;
	m_apertura = 50.0;
	////////////////////
	
	//m_layers = 1.0;
	m_layers = 1.0;
	////////////////////
	
	//m_wave = 0.004;
	m_wave = 0.63;
	////////////////////
	
	m_count = 2;
	m_formula = (m_apertura*m_apertura)/(m_wave*512);
	m_modul = 0.25;
	m_kos = FALSE;
	m_pam = FALSE;
	m_aperture_pam = 600.0;
	m_count_pam = 2;
	m_layers_pam = 10.0;
	
	m_napr_pam = 100;
	m_wave_pam = 12.26/(pow(m_napr_pam*1000,0.5)*pow((1+0.9788*m_napr_pam*pow(10.0,-3)),0.5));
	m_modul_pam=1/(2*m_wave_pam*m_napr_pam);

	//}}AFX_DATA_INIT
}


void CDialogPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPath)
	DDX_Text(pDX, IDC_APERTURA, m_apertura);
	DDX_Text(pDX, IDC_COUNT, m_count);
	DDX_Text(pDX, IDC_LAYERS, m_layers);
	DDX_Text(pDX, IDC_WAVE, m_wave);
	DDX_Text(pDX, IDC_FORMULA, m_formula);
	DDX_Text(pDX, IDC_MODULAC, m_modul);
	DDX_Check(pDX, IDC_KOS, m_kos);
	DDX_Check(pDX, IDC_PAM, m_pam);
	DDX_Text(pDX, IDC_APERTURA_PAM, m_aperture_pam);
	DDX_Text(pDX, IDC_COUNT_PAM, m_count_pam);
	DDX_Text(pDX, IDC_LAYERS_PAM, m_layers_pam);
	DDX_Text(pDX, IDC_MODULAC_PAM, m_modul_pam);
	DDX_Text(pDX, IDC_NAPR_PAM, m_napr_pam);
	DDX_Text(pDX, IDC_WAVE_PAM, m_wave_pam);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPath, CDialog)
//{{AFX_MSG_MAP(CDialogPath)
ON_BN_CLICKED(IDC_UP, OnUp)
ON_BN_CLICKED(IDC_KOS, OnKos)
ON_BN_CLICKED(IDC_PAM, OnPam)
ON_EN_CHANGE(IDC_APERTURA, OnChangeApertura)
ON_EN_CHANGE(IDC_NAPR_PAM, OnChangeNaprPam)
ON_EN_CHANGE(IDC_WAVE_PAM, OnChangeWavePam)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPath message handlers

/*void CDialogPath::OnChangeApertura() 
{
// TODO: If this is a RICHEDIT control, the control will not
// send this notification unless you override the CDialog::OnInitDialog()
// function and call CRichEditCtrl().SetEventMask()
// with the ENM_CHANGE flag ORed into the mask.

  // TODO: Add your control notification handler code here
  
}*/

void CDialogPath::OnUp() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_formula = (m_apertura*m_apertura)/(m_wave*512);
	UpdateData(FALSE);
}

void CDialogPath::OnKos() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_kos==FALSE)
		m_pam=TRUE;
	else
		m_pam=FALSE;
	UpdateData(FALSE);
	
}

void CDialogPath::OnPam() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_pam==FALSE)
		m_kos=TRUE;
	else
		m_kos=FALSE;
	UpdateData(FALSE);
	
}

void CDialogPath::OnChangeApertura() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CDialogPath::OnChangeNaprPam() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_napr_pam)
	{	
		if(m_napr_pam<10)
		{
			m_napr_pam=10;
		}
		else if(m_napr_pam>10000)
		{
			m_napr_pam=10000;
		}
		m_wave_pam = 12.26/(pow(m_napr_pam*1000,0.5)*pow((1+0.9788*m_napr_pam*pow(10.0,-3)),0.5));
		m_modul_pam=1/(2*m_wave_pam*m_napr_pam);
		UpdateData(FALSE);
	}
}

void CDialogPath::OnChangeWavePam() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	/*UpdateData();
	{
		if(m_wave_pam)
		{
			m_napr_pam=(150.3076/(pow(m_wave_pam,2)*1.0000009787999999999999999999997))/1000;
			UpdateData(FALSE);
		}
	}*/
}
