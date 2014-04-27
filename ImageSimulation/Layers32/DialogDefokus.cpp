// DialogDefokus.cpp : implementation file
//

#include "stdafx.h"

#include "layers32.h"
#include "DialogDefokus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define E 2.71
/////////////////////////////////////////////////////////////////////////////
// CDialogDefokus dialog


CDialogDefokus::CDialogDefokus(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDefokus::IDD, pParent)
{
	m_shift=FALSE;
	m_delta=12;
	m_pos_prob=0;
	//{{AFX_DATA_INIT(CDialogDefokus)
	m_Ccf = 1800000*pow(10.0,-6.0);
	m_Cxp = 1.2*10;//равно 12 амгстрен, при JEM-100В
	m_cos = FALSE;
	m_sin = TRUE;
	m_n = 1;
	m_D_for_view = 0.0;
	m_ap_diaf_view = 0.0;
	m_znak_def = 1;
	m_auto_defoc = FALSE;
	//}}AFX_DATA_INIT
	Function();
}


void CDialogDefokus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDefokus)
	DDX_Control(pDX, IDC_GRAF, m_graf);
	DDX_Text(pDX, IDC_CCF, m_Ccf);
	DDX_Text(pDX, IDC_CXP, m_Cxp);
	DDX_Check(pDX, IDC_COS, m_cos);
	DDX_Check(pDX, IDC_SIN, m_sin);
	DDX_Text(pDX, IDC_N, m_n);
	DDX_Text(pDX, IDC_D, m_D_for_view);
	DDX_Text(pDX, IDC_AP_DIAF, m_ap_diaf_view);
	DDX_Check(pDX, IDC_ZNAK_DEF, m_znak_def);
	DDX_Check(pDX, IDC_AUTO_DEFOC, m_auto_defoc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDefokus, CDialog)
	//{{AFX_MSG_MAP(CDialogDefokus)
	ON_WM_DRAWITEM()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SIN, OnSin)
	ON_BN_CLICKED(IDC_COS, OnCos)
	ON_BN_CLICKED(IDC_REDRAW, OnRedraw)
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_BN_CLICKED(IDC_AUTO_DEFOC, OnAutoDefoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDefokus message handlers

void CDialogDefokus::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	CRect Rect;
	//double Kmax;
	HDC hdc;
	HPEN BlackHPEN,RedHPEN,BlueHPEN,GreenHPEN,WhiteHPEN,WhiteDashHPEN;
	char str[100];
	int i;
	int h  =350, x = 0, y = 0;
	int Xs = 30;
	int Ys = 30;
	int Xc = 260;
	int Yc = 130;
	int Xe = 490;
	int Ye = 230;
	if(lpDrawItemStruct->CtlID == IDC_GRAF && lpDrawItemStruct->itemAction == ODA_DRAWENTIRE) {
		hdc = lpDrawItemStruct->hDC;
		//////////////////////
		BlackHPEN = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		RedHPEN = CreatePen(PS_SOLID, 1, RGB(255,0,0));
		GreenHPEN = CreatePen(PS_SOLID, 1, RGB(0,255,0));
		BlueHPEN = CreatePen(PS_SOLID, 1, RGB(0,0,255));
		WhiteHPEN = CreatePen(PS_SOLID, 1, RGB(255,255,255));
		WhiteDashHPEN = CreatePen(PS_DOT, 1, RGB(255,255,255));
		SelectObject(hdc, BlackHPEN);
		for(i = 0; i < h; i++) {
			MoveToEx(hdc, 0, i, NULL); LineTo(hdc, 600, i);
		}
		
		if(m_cos == TRUE) {
			SelectObject(hdc,GreenHPEN);		
			MoveToEx(hdc, Xs, Yc - (int)(m_aCos[0] * (Yc - Ys)), NULL);
			for(i=0;i<460;i++) {
				LineTo(hdc, i + Xs, Yc - (int)(m_aCos[i] * (Yc - Ys)));
			}
		}
		
		if(m_sin == TRUE) {
			SelectObject(hdc, RedHPEN);
			MoveToEx(hdc, Xs, Yc, NULL);
			for(i=0;i<460;i++) {
				LineTo(hdc, i + Xs, Yc - (int)(m_aSin[i] * (Yc - Ys)));
			}
		}
		
		//отрисовка бегунка
		SelectObject(hdc, WhiteHPEN);		
		MoveToEx(hdc, Xs + m_pos_prob, Ys, NULL);
		LineTo(hdc, Xs + m_pos_prob, Ye);
		
	
		///отрисовка сетки
		SelectObject(hdc,WhiteHPEN);
		int tt = 0;;
		for(i = Xs; i < Xe; i += 4) {
			MoveToEx(hdc, Xs + tt * 4, Yc, NULL);
			LineTo(hdc, Xs + tt * 4 + 2,Yc);
			tt++;
		}
		
		
		MoveToEx(hdc, Xs, Ys, NULL);
		LineTo(hdc, Xs, Ye);
		
		MoveToEx(hdc, Xs, Ys, NULL);
		LineTo(hdc, Xe, Ys);

		MoveToEx(hdc, Xs, Ye, NULL);
		LineTo(hdc, Xe, Ye);

		MoveToEx(hdc, Xs, Yc, NULL);
		LineTo(hdc, Xs + 10, Yc);
		
		MoveToEx(hdc, Xc, Ys, NULL);
		LineTo(hdc, Xc, Ys + 10);
		MoveToEx(hdc, Xc, Ye, NULL);
		LineTo(hdc, Xc, Ye - 10);

		MoveToEx(hdc, Xe, Ys, NULL);
		LineTo(hdc, Xe, Ys + 10);
		MoveToEx(hdc, Xe, Ye, NULL);
		LineTo(hdc, Xe, Ye - 10);

		MoveToEx(hdc, Xs + (Xc - Xs) / 2, Ys, NULL);
		LineTo(hdc, Xs + (Xc - Xs) / 2, Ys + 10);
		MoveToEx(hdc, Xs + (Xc - Xs) / 2, Ye, NULL);
		LineTo(hdc, Xs + (Xc - Xs) / 2, Ye - 10);

		MoveToEx(hdc, Xc + (Xc - Xs) / 2, Ys, NULL);
		LineTo(hdc, Xc + (Xc - Xs) / 2, Ys + 10);
		MoveToEx(hdc, Xc + (Xc - Xs) / 2, Ye, NULL);
		LineTo(hdc, Xc + (Xc - Xs) / 2, Ye - 10);

		SetBkMode(hdc, OPAQUE | TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));

		Rect.left = Xe - 20;
		Rect.top = Ys + 10;
		Rect.right = Xe + 20;
		Rect.bottom = Ys + 50;		
		DrawText(hdc, "K", -1, &Rect, DT_CENTER | DT_TOP);
		
		Rect.left = Xe - 20;
		Rect.top = Ye - 50;
		Rect.right = Xe + 21;
		Rect.bottom = Ye - 10;		
		DrawText(hdc, "T", -1, &Rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

		Rect.left = 0;
		Rect.top = 0;
		Rect.right = Xs - 2;
		Rect.bottom = Ys + (Ys);		
		DrawText(hdc, "1", -1, &Rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

		Rect.left = 0;
		Rect.top = Ye - 20;
		Rect.right = Xs - 2;
		Rect.bottom = Ye + 20;		
		DrawText(hdc, "-1", -1, &Rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

		Rect.left = 0;
		Rect.top = Yc - 20;
		Rect.right = Xs - 2;
		Rect.bottom = Yc + 20;		
		DrawText(hdc, "0", -1, &Rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		
		//отрисовка K
		sprintf(str, "%0.3lf", (this->m_iDem / (2 * this->m_apertura)) * (0.25));
		Rect.left = Xs + (Xc - Xs) / 2 - 20;
		Rect.top = 0;
		Rect.right = Xs + (Xc - Xs) / 2 + 21;
		Rect.bottom = Ys - 2;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

		sprintf(str, "%0.3lf", (this->m_iDem / (2 * this->m_apertura)) * (0.5));
		Rect.left = Xc - 20;
		Rect.top = 0;
		Rect.right = Xc + 21;
		Rect.bottom = Ys - 2;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

		sprintf(str, "%0.3lf", (this->m_iDem / (2 * this->m_apertura)) * (0.75));
		Rect.left = Xc + (Xc - Xs) / 2 - 20;
		Rect.top = 0;
		Rect.right = Xc + (Xc - Xs) / 2 + 21;
		Rect.bottom = Ys - 2;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

		sprintf(str, "%0.3lf", this->m_iDem / (2 * this->m_apertura));
		Rect.left = Xe - 20;
		Rect.top = 0;
		Rect.right = Xe + 21;
		Rect.bottom = Ys - 2;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		//отрисовка T (1/K)
		sprintf(str, "%0.3lf", 1 / ((this->m_iDem / (2 * this->m_apertura)) * (0.25)));
		Rect.left = Xs + (Xc - Xs) / 2 - 20;
		Rect.top = Ye + 2;
		Rect.right = Xs + (Xc - Xs) / 2 + 21;
		Rect.bottom = 350;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

		sprintf(str, "%0.3lf", 1 / ((this->m_iDem / (2 * this->m_apertura)) * (0.5)));
		Rect.left = Xc - 20;
		Rect.top = Ye + 2;
		Rect.right = Xc + 21;
		Rect.bottom = 350;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

		sprintf(str, "%0.3lf", 1 / ((this->m_iDem / (2 * this->m_apertura)) * (0.75)));
		Rect.left = Xc + (Xc - Xs) / 2 - 20;
		Rect.top = Ye + 2;
		Rect.right = Xc + (Xc - Xs) / 2 + 21;
		Rect.bottom = 350;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

		sprintf(str, "%0.3lf",1.0 / (this->m_iDem / (2.0 * this->m_apertura)));
		Rect.left = Xe - 20;
		Rect.top = Ye + 2;
		Rect.right = Xe + 21;
		Rect.bottom = 350;		
		DrawText(hdc, str, -1, &Rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

		if(m_cos == TRUE && m_sin == TRUE) {
			if(m_pos_prob > 459) {
				sprintf(str, "К=%0.4f; T=%0.4f\nsin=%0.6f\ncos=%0.6f", this->m_iDem / (2.0 * this->m_apertura), 1.0 / (this->m_iDem / (2 * this->m_apertura)), m_aSin[459], m_aCos[459]);
			} else {
				sprintf(str, "К=%0.4f; T=%0.4f\nsin=%0.6f\ncos=%0.6f", this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob, 1.0 / (this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob), m_aSin[m_pos_prob], m_aCos[m_pos_prob]);
			}
		} else if(m_cos == FALSE && m_sin == TRUE) {
			if(m_pos_prob > 459) {
				sprintf(str, "К=%0.4f; T=%0.4f\nsin=%0.6f", this->m_iDem / (2.0 * this->m_apertura), 1.0 / (this->m_iDem / (2.0 * this->m_apertura)), m_aSin[459]);
			} else {
				sprintf(str, "К=%0.4f; T=%0.4f\nsin=%0.6f", this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob, 1.0 / (this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob), m_aSin[m_pos_prob]);
			}
		} else if(m_cos == TRUE && m_sin == FALSE) {
			if(m_pos_prob > 459) {
				sprintf(str, "К=%0.4f; T=%0.4f\n\ncos=%0.6f", this->m_iDem / (2.0 * this->m_apertura), 1.0 / (this->m_iDem / (2.0 * this->m_apertura)), m_aCos[459]);
			} else {
				sprintf(str, "К=%0.4f; T=%0.4f\n\ncos=%0.6f", this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob, 1.0 / (this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob), m_aCos[m_pos_prob]);
			}
		} else if(m_cos == FALSE && m_sin == FALSE) {
			if(m_pos_prob > 459) {
				sprintf(str, "", this->m_iDem / (2.0 * this->m_apertura));
			} else {
				sprintf(str, "", this->m_iDem / (2.0 * this->m_apertura) / 460.0 * m_pos_prob);
			}
		}

		Rect.left = Xs;
		Rect.top = Ye + 20;
		Rect.right = Xe;
		Rect.bottom = 350;		
		DrawText(hdc, str, -1, &Rect, DT_LEFT | DT_TOP);
		
		sprintf(str, "апертурная диафрагма: %0.6f\nдлина волны[амгстрены]: %0.6f", this->m_k_ap_diaf, this->m_length_wave);
		Rect.left = Xc;
		Rect.top = Ye + 20;
		Rect.right = Xe;
		Rect.bottom = 350;		
		DrawText(hdc, str, -1, &Rect, DT_LEFT | DT_TOP);
		//////отрисовка апертурной диафрагмы
		//m_k_ap_diaf
		
		SelectObject(hdc, WhiteHPEN);		
		int k_ap_diaf;
		k_ap_diaf = (int)((460.0 * m_k_ap_diaf) / (this->m_iDem / (2.0 * this->m_apertura)));
		tt = 0;
		for(i = Ys; i <Ye; i += 4) {
			MoveToEx(hdc, Xs + k_ap_diaf, Ys + tt * 4, NULL);
			LineTo(hdc, Xs + k_ap_diaf, Ys + tt * 4 + 3);
			tt++;
		}

		//////////////////////
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDialogDefokus::Function()
{
	int i;
	static double Kmax;
	static double step;
	static double e, t, temp;
	Kmax = this->m_iDem / (2 * this->m_apertura);
	step = Kmax / 460;
	for(i=0;i<460;i++) {
		e = pow(E, (-1.2 * pow(M_PI, 2) * pow(m_Cxp, 2) * pow(m_length_wave, 2) * pow(i * step, 4)));
		t = (2 * M_PI / m_length_wave) * (0.25 * m_Ccf * pow(m_length_wave, 4.0) * pow(10.0, 7.0) * pow(i * step, 4.0) - 0.5 * m_D * pow(m_length_wave, 2.0) * pow(i * step, 2.0));
		m_aCos[i] = e * cos(t);
		m_aSin[i] = e * sin(t);
	}
}

void CDialogDefokus::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMUpDown->iDelta == -1) {
		UpdateData();
		m_n += 1;
		if(m_n > 1000) {
			m_n = 1000;
		}
		UpdateD();
		UpdateData(false);
		this->Function();
		m_graf.RedrawWindow();
	} else {
		UpdateData();
		m_n -= 1;
		if(m_n < 1) {
			m_n=1;
		}
		UpdateD();
		UpdateData(false);
		this->Function();
		m_graf.RedrawWindow();
	}
	
	*pResult = 0;
}

void CDialogDefokus::UpdateD() {
	char str[20];
	m_D = pow(((2 * m_n - 0.5) * m_Ccf * 10000000 * this->m_length_wave), 0.5);
	sprintf(str, "%0.4f", m_D);
	this->m_D_for_view = atof(str);
	
	sprintf(str,"%0.4f",m_k_ap_diaf);
	this->m_ap_diaf_view = atof(str);
}

void CDialogDefokus::OnOK() 
{
	// TODO: Add extra validation here
	char str[20];	
	UpdateData();

	
	
	sprintf(str,"%0.4f",m_D);
	if(atof(str)!=m_D_for_view)
	{
		m_D=m_D_for_view;
	}

	if(this->m_znak_def==TRUE)
	{
		m_D*=-1;
	}
	
	/////////////
	if(this->m_auto_defoc==TRUE)
	{
		this->m_k_ap_diaf= pow((6.0/(this->m_Ccf*pow(10.0,7.0)*pow(this->m_length_wave,3))),0.25) ;
		//sprintf(str,"%0.4f",m_k_ap_diaf);
		//this->m_ap_diaf_view=atof(str);		
	}
	else
	{
		
		sprintf(str,"%0.4f",m_k_ap_diaf);
		if(atof(str)!=m_ap_diaf_view)
		{
			m_k_ap_diaf=m_ap_diaf_view;
		}
	}
	/////////////
	
	
	CDialog::OnOK();
}



int CDialogDefokus::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	this->Function();
	this->RedrawWindow();	
	return 0;
}
void CDialogDefokus::OnSin() 
{
	char str[20];
	UpdateData();
	sprintf(str,"%0.4f",m_D);
	if(atof(str)!=m_D_for_view)
	{
		m_D=m_D_for_view;
	}
	this->Function();
	this->m_graf.RedrawWindow();
	
}

void CDialogDefokus::OnCos() 
{
	char str[20];
	UpdateData();
	sprintf(str,"%0.4f",m_D);
	if(atof(str)!=m_D_for_view)
	{
		m_D=m_D_for_view;
	}
	this->Function();
	this->m_graf.RedrawWindow();}

void CDialogDefokus::OnRedraw() 
{
	char str[20];

	UpdateData();
	
	sprintf(str,"%0.4f",m_D);
	if(atof(str)!=m_D_for_view)
	{
		m_D=m_D_for_view;
	}

	if(this->m_auto_defoc==TRUE)
	{
		this->m_k_ap_diaf= pow((6.0/(this->m_Ccf*pow(10.0,7)*pow(this->m_length_wave,3))),0.25) ;
		sprintf(str,"%0.4f",m_k_ap_diaf);
		this->m_ap_diaf_view=atof(str);		
		UpdateData(FALSE);
	}
	else
	{
		
		sprintf(str,"%0.4f",m_k_ap_diaf);
		if(atof(str)!=m_ap_diaf_view)
		{
			m_k_ap_diaf=m_ap_diaf_view;
		}
	}

	this->Function();
	this->m_graf.RedrawWindow();
}



void CDialogDefokus::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
		// TODO: Add your message handler code here and/or call default
	CScrollBar *ScrollBar=(CScrollBar *)GetDlgItem(IDC_SCROLLBAR);
	if (pScrollBar==ScrollBar)
	{
		switch(nSBCode)
		{
		case SB_LINERIGHT:
			this->m_pos_prob+=1;
			break;
		case SB_LINELEFT:
			this->m_pos_prob-=1;
			break;
		case SB_PAGELEFT:
			this->m_pos_prob-=10;
			break;
		case SB_PAGERIGHT:
			this->m_pos_prob+=10;
			break;
		case SB_THUMBPOSITION:
			this->m_pos_prob=nPos;
			break;
		case SB_THUMBTRACK:
			this->m_pos_prob=nPos;
			break;
			
		}
		if(this->m_pos_prob >459) this->m_pos_prob=459;
		if(this->m_pos_prob <0) this->m_pos_prob=0;
		ScrollBar->SetScrollPos(m_pos_prob);
		if(m_shift==TRUE)
		{
			int t=0;
		}
		UpdateData(FALSE);
		m_graf.RedrawWindow();
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CDialogDefokus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CScrollBar *ScrollBar=(CScrollBar *)GetDlgItem(IDC_SCROLLBAR);
	ASSERT(ScrollBar);
	ScrollBar->SetScrollRange(0,459);
	ScrollBar->SetScrollPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDefokus::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_SHIFT:
		m_shift=TRUE;
		break;
	}
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDialogDefokus::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_SHIFT:
		m_shift=FALSE;
		break;
	}	
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}



void CDialogDefokus::OnAutoDefoc() 
{
	char str[100];
	CEdit* wnd = (CEdit*) GetDlgItem(IDC_AP_DIAF);
	UpdateData();	
	if(this->m_auto_defoc==TRUE)
	{
		this->m_k_ap_diaf= pow((6.0/(this->m_Ccf*pow(10.0,7)*pow(this->m_length_wave,3))),0.25) ;
		sprintf(str,"%0.4f",m_k_ap_diaf);
		this->m_ap_diaf_view=atof(str);
		this->Function();
		this->m_graf.RedrawWindow();
		wnd->EnableWindow(FALSE);
	}
	else if(this->m_auto_defoc==FALSE)
	{
		wnd->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}
