// LatDlg.cpp : implementation file
//

#include "stdafx.h"

#include "LatDlg.h"
#include "Blt.h"
#include "mend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	LatShowWindow(nItemID) {pWnd=GetDlgItem(nItemID);ASSERT(pWnd);pWnd->ShowWindow(nCmdShow);}

#define MacroMoveToNextToken(buf,pos) {while(isspace(buf[pos]))pos++;}
#define MacroMoveToNextLine(buf,pos) {while(buf[pos]!=0){ pos++; if (buf[pos-1]=='\n') break;}}

//#define GetToolTips GetTooltips


/////////////////////////////////////////////////////////////////////////////
// CLatDlg dialog


CLatDlg::CLatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLatDlg)
	m_nNumLayers = 0;
	//}}AFX_DATA_INIT
	m_dSizeXRe = 5;
	m_dSizeYRe = 5;
	m_dDimension = 20;
	m_dExtraVisibleArea=1;
	m_dMaxAtomOffs=0.0;
	m_dMaxCellOffs=0.;
	m_nCurSelection=0;
	m_nCurLayer=0;
	m_bInsertMode=FALSE;
	m_pLayersArray=NULL;
}


void CLatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLatDlg)
	DDX_Control(pDX, IDC_STATIC_CELL, m_MyStatic);
	DDX_Text(pDX, IDC_ALL_EDIT_NUMLAYERS, m_nNumLayers);
	DDV_MinMaxInt(pDX, m_nNumLayers, 1, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLatDlg, CDialog)
	//{{AFX_MSG_MAP(CLatDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_ATOMS, OnSelchangeComboAtoms)
	ON_EN_CHANGE(IDC_EDIT_ATOM_X, OnChangeEditAtom)
	ON_BN_CLICKED(IDC_ADD_ATOM, OnAddAtom)
	ON_BN_CLICKED(IDC_ADD_LAYER, OnAddLayer)
	ON_CBN_SELCHANGE(IDC_COMBO_LAYERS, OnSelchangeComboLayers)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_BN_CLICKED(IDC_L2_DEL_LAYER, OnDelLayer)
	ON_EN_CHANGE(IDC_L3_X, OnChangeL3)
	ON_EN_CHANGE(IDC_L2_EDIT_Z, OnChangeL2)
	ON_EN_CHANGE(IDC_L4_ATOMOFFS, OnChangeL4)
	ON_EN_CHANGE(IDC_ALL_EDIT_NUMLAYERS, OnChangeLAll)
	ON_BN_CLICKED(IDC_L5_LOAD, OnL5Load)
	ON_EN_CHANGE(IDC_L3_Y, OnChangeL3)
	ON_EN_CHANGE(IDC_L3_DIMENSION, OnChangeL3)
	ON_EN_CHANGE(IDC_L3_EXTRA_AREA, OnChangeL3)
	ON_EN_CHANGE(IDC_EDIT_ATOM_Y, OnChangeEditAtom)
	ON_EN_CHANGE(IDC_L4_CELLOFFS, OnChangeL4)
	ON_BN_CLICKED(IDC_L5_SAVE, OnL5Save)
	//}}AFX_MSG_MAP

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnTipText)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLatDlg message handlers

BOOL CLatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

	CMyStatic *MyStatic=(CMyStatic *)GetDlgItem(IDC_STATIC_CELL);
	ASSERT(MyStatic);

	m_dSizeXRe = 5;
	m_dSizeYRe = 5;
	m_dDimension = 10;
	MyStatic->m_dSizeXRe=m_dSizeXRe;
	MyStatic->m_dSizeYRe=m_dSizeYRe;
	MyStatic->m_dExtraVisibleArea=m_dExtraVisibleArea;


	CComboBox *ComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_ATOMS);
	ASSERT(ComboBox);
	CString String;
	for(size_t i = 0; ; i++) {
		if (g_pcElementNames[i]==NULL)
			break;
		String.Format("%-3d %s  (%s)",i+1,g_pcElementNames[i],g_pcElementNamesRu[i]);
		ComboBox->AddString(String);
	}
	ComboBox->SetDroppedWidth(150);

	ComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_LAYERS);
	ASSERT(ComboBox);

	ASSERT(!(m_pLayersArray->GetSize()<1));
	for(size_t i = 0; i < m_pLayersArray->GetSize(); i++) {
		//String.Format("Слой %d",i+1);
		ComboBox->AddString(m_pLayersArray->GetAt(i)->m_sName);
	}
	ComboBox->SetCurSel(0);

	CTabCtrl *pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_TAB);
	ASSERT(pTabCtrl);
	TC_ITEM tc_item;
	memset(&tc_item,0,sizeof(TC_ITEM));
    tc_item.mask=TCIF_TEXT;
    
	
	tc_item.pszText="Атом";
	pTabCtrl->InsertItem(0,&tc_item);
    tc_item.pszText="Слой";
	pTabCtrl->InsertItem(1,&tc_item);
    tc_item.pszText="Ячейка";
	pTabCtrl->InsertItem(2,&tc_item);
    tc_item.pszText="Случайные характеристики";
	pTabCtrl->InsertItem(3,&tc_item);
    tc_item.pszText="Сохранить/Загрузить";
	pTabCtrl->InsertItem(4,&tc_item);

	pTabCtrl->SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	CRect rcRect;
	GetWindowRect(rcRect);
	rcRect.left+=150;
	MoveWindow(rcRect,FALSE);
	rcRect.right-=150;
	MoveWindow(rcRect,TRUE);

	UpdateSelection(FALSE);
	UpdateCell();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLatDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMyStatic *MyStatic=(CMyStatic *)GetDlgItem(IDC_STATIC_CELL);
	ASSERT(MyStatic);
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	CRect Rect;
	MyStatic->GetWindowRect(&Rect);
	CPoint Point(point);
	ClientToScreen(&Point);
	if (Rect.PtInRect(Point))
	{
		if (GetFocus()!=MyStatic)
		{
			MyStatic->SetFocus();
		}
		MyStatic->ScreenToClient(&Point);
		
		double dxPos=((double)Point.x*(m_dSizeXRe+m_dExtraVisibleArea*2))/(double)Rect.Width()-m_dSizeXRe/2.-m_dExtraVisibleArea;
		double dyPos=-(((double)Point.y*(m_dSizeYRe+m_dExtraVisibleArea*2))/(double)Rect.Height()-m_dSizeYRe/2.-m_dExtraVisibleArea);
		if (m_bInsertMode)
		{
			if (dxPos>0)
			{
				if (dxPos>m_dSizeXRe/2)
					dxPos=m_dSizeXRe/2;
			}else
			{
				if (dxPos<(-m_dSizeXRe/2))
					dxPos=-m_dSizeXRe/2;
			}
			if (dyPos>0)
			{
				if (dyPos>m_dSizeYRe/2)
					dyPos=m_dSizeYRe/2;
			}else
			{
				if (dyPos<(-m_dSizeYRe/2))
					dyPos=-m_dSizeYRe/2;
			}
			CAtomParams *AtomParamsCur=pAtomsArray->GetAt(m_nCurSelection);
			CAtomParams *pAtomParams=new CAtomParams();
			pAtomParams->SetXPos(dxPos);
			pAtomParams->SetYPos(dyPos);
			pAtomParams->SetZ(AtomParamsCur->GetZ());
			m_nCurSelection=pAtomsArray->Add(pAtomParams);
			UpdateCell();
			m_bInsertMode=FALSE;
		}else
		{
			double dMinLen=(m_dSizeXRe>m_dSizeYRe)?m_dSizeXRe:m_dSizeYRe;
			double dLen;
			CAtomParams* pAtomParams;
			for(int i=0;i<pAtomsArray->GetSize();i++)
			{
				pAtomParams=(CAtomParams *)pAtomsArray->GetAt(i);
				dLen=sqrt((dxPos-pAtomParams->GetXPos())*(dxPos-pAtomParams->GetXPos())+
					(dyPos-pAtomParams->GetYPos())*(dyPos-pAtomParams->GetYPos()));
				
				if ((dLen)<dMinLen)
				{
					m_nCurSelection=i;
					dMinLen=dLen;
				}
			}
		}
		UpdateSelection(FALSE);
	}
	else
		CDialog::OnLButtonDown(nFlags, point);
}

void CLatDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x=300;
	lpMMI->ptMinTrackSize.y=400;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CLatDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here


	CWnd *pTabCtrl=GetDlgItem(IDC_TAB);
	if (pTabCtrl)
	{
		CPoint ptOffset;
		CRect  rcChild;

		CWnd* pwndChild;
		pwndChild=GetDlgItem(IDCANCEL);
		ASSERT(pwndChild);
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		ptOffset.x=cx-rcChild.right-10;
		ptOffset.y=cy-rcChild.bottom-2;
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild,TRUE);
		
		int nCancelBtnHeight=rcChild.Height();

		MoveChild(IDOK,&ptOffset);
		MoveChild(IDC_ALL_EDIT_NUMLAYERS,&ptOffset);
		MoveChild(IDC_ALL_STATIC_NUMLAYERS,&ptOffset);

		
		rcChild.bottom=cy-nCancelBtnHeight-4;
		rcChild.right=cx-10;
		rcChild.left=10;
		rcChild.top=10;
		pTabCtrl->MoveWindow(rcChild, TRUE);
		

		ptOffset.y=0;
		// All Layers
		pwndChild=GetDlgItem(IDC_GUIDE_ALL);
		ASSERT(pwndChild);
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		ptOffset.x=cx-rcChild.right-15;
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild,TRUE);

		MoveChild(IDC_STATIC_SCALE,&ptOffset);
		MoveChild(IDC_COMBO_LAYERS,&ptOffset);
		
		// Layer 1
		CTabCtrl *pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_TAB);
		ASSERT(pTabCtrl);
		//if (pTabCtrl->GetCurSel()==0)
		{
			pwndChild=GetDlgItem(IDC_GUIDE_1);
			ASSERT(pwndChild);
			pwndChild->GetWindowRect(rcChild);
			ScreenToClient(rcChild);
			ptOffset.x=cx-rcChild.right-15;
			rcChild.OffsetRect(ptOffset);
			pwndChild->MoveWindow(rcChild,TRUE);
			
			MoveChild(IDC_COMBO_ATOMS,&ptOffset);
			MoveChild(IDC_EDIT_ATOM_X,&ptOffset);
			MoveChild(IDC_EDIT_ATOM_Y,&ptOffset);
			MoveChild(IDC_ADD_ATOM,&ptOffset);
			MoveChild(IDC_L1_STATIC_X,&ptOffset);
			MoveChild(IDC_L1_STATIC_Y,&ptOffset);
		}
		
		
		// Layer 2
		//if (pTabCtrl->GetCurSel()==1)
		{
			pwndChild=GetDlgItem(IDC_GUIDE_2);
			ASSERT(pwndChild);
			pwndChild->GetWindowRect(rcChild);
			ScreenToClient(rcChild);
			ptOffset.x=cx-rcChild.right-15;
			rcChild.OffsetRect(ptOffset);
			pwndChild->MoveWindow(rcChild,TRUE);
			
			MoveChild(IDC_ADD_LAYER,&ptOffset);
			MoveChild(IDC_L2_DEL_LAYER,&ptOffset);
			MoveChild(IDC_L2_STATIC_Z,&ptOffset);
			MoveChild(IDC_L2_EDIT_Z,&ptOffset);
			
		}
		// Layer 3
		//if (pTabCtrl->GetCurSel()==2)
		{
			pwndChild=GetDlgItem(IDC_GUIDE_3);
			ASSERT(pwndChild);
			pwndChild->GetWindowRect(rcChild);
			ScreenToClient(rcChild);
			ptOffset.x=cx-rcChild.right-15;
			rcChild.OffsetRect(ptOffset);
			pwndChild->MoveWindow(rcChild,TRUE);
			
			
			MoveChild(IDC_L3_STATIC_LATOPT,&ptOffset);
			MoveChild(IDC_L3_STATIC_X,&ptOffset);
			MoveChild(IDC_L3_X,&ptOffset);
			MoveChild(IDC_L3_STATIC_Y,&ptOffset);
			MoveChild(IDC_L3_Y,&ptOffset);
			MoveChild(IDC_L3_STATIC_DIM,&ptOffset);
			MoveChild(IDC_L3_DIMENSION,&ptOffset);
			MoveChild(IDC_L3_STATIC_VIEWAREA,&ptOffset);
			MoveChild(IDC_L3_STATIC_EXTRA_AREA,&ptOffset);
			MoveChild(IDC_L3_EXTRA_AREA,&ptOffset);
		}
		// Layer 4
		pwndChild=GetDlgItem(IDC_GUIDE_4);
		ASSERT(pwndChild);
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		ptOffset.x=cx-rcChild.right-15;
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild,TRUE);
		
		
		MoveChild(IDC_L4_STATIC_ATOMOFFS,&ptOffset);
		MoveChild(IDC_L4_STATIC_CELLOFFS,&ptOffset);
		MoveChild(IDC_L4_ATOMOFFS,&ptOffset);
		MoveChild(IDC_L4_CELLOFFS,&ptOffset);

		// Layer 5
		pwndChild=GetDlgItem(IDC_GUIDE_5);
		ASSERT(pwndChild);
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		ptOffset.x=cx-rcChild.right-15;
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild,TRUE);
		
		
		MoveChild(IDC_L5_LOAD,&ptOffset);
		MoveChild(IDC_L5_SAVE,&ptOffset);

		ReScale();
		Invalidate(FALSE);
	}	

}

void CLatDlg::ReScale()
{
	try
	{
		double dSizeXRe=m_dSizeXRe;
		double dSizeYRe=m_dSizeYRe;
		double dDimension=m_dDimension;
		double dExtraVisibleArea=m_dExtraVisibleArea;

		CMyStatic *MyStatic=(CMyStatic *)GetDlgItem(IDC_STATIC_CELL);
		if (MyStatic==NULL)
			return;

		CRect rcTabCtrl;
		CTabCtrl *pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_TAB);
		ASSERT(pTabCtrl);
		pTabCtrl->GetWindowRect(rcTabCtrl);
		ScreenToClient(rcTabCtrl);
		
		int cx=rcTabCtrl.Width(),cy=rcTabCtrl.Height();

		
		int nSizeX=(int)((dSizeXRe+dExtraVisibleArea*2)*dDimension);
		int nSizeY=(int)((dSizeYRe+dExtraVisibleArea*2)*dDimension);
		if (nSizeX<1)
			nSizeX=1;
		if (nSizeY<1)
			nSizeY=1;
		
		CRect rcTabItem;
		pTabCtrl->GetItemRect(0,rcTabItem);
		cy-=rcTabItem.Height();
		cx-=150;
		float k;
		if ((float)cx/(float)nSizeX>(float)cy/(float)nSizeY)
		{
			k=(float)(cy-10)/(float)nSizeY;
		}else
		{
			k=(float)(cx-10)/(float)nSizeX;
		}
		if (k<0)
			k=0;
		
		int nSizeX1=(int)(nSizeX*k);
		int nSizeY1=(int)(nSizeY*k);
		CStatic *Static=(CStatic *)GetDlgItem(IDC_STATIC_SCALE);
		ASSERT(Static);
		if (nSizeX1>=nSizeX)
		{
			nSizeX=(nSizeX1/nSizeX)*nSizeX;
			nSizeY=(nSizeY1/nSizeY)*nSizeY;
			CString String;
			int nWidth=(int)(dDimension*dSizeXRe);
			if (nWidth<1)
				nWidth=1;
			String.Format("%d:1",nSizeX/nWidth);
			Static->SetWindowText(String);
		}else
		{
			int i = 2;
			for(;;i++)
			{
				if (nSizeX/i<=nSizeX1)
					break;
			}
			nSizeX/=i;
			nSizeY/=i;
			CString String;
			String.Format("1:%d",i);
			Static->SetWindowText(String);
		}
		
		MyStatic->MoveWindow(rcTabCtrl.left+cx/2-(nSizeX/2),rcTabCtrl.top+rcTabItem.Height()+cy/2-(nSizeY/2),nSizeX,nSizeY,TRUE);
		MyStatic->Invalidate(FALSE);
	}catch(...)
	{
		ASSERT(0);
	}
}

void CLatDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}


BOOL CLatDlg::UpdateCell()
{
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	double dSizeXRe=m_dSizeXRe;
	double dSizeYRe=m_dSizeYRe;
	double dDimension=m_dDimension;
	double dExtraVisibleArea=m_dExtraVisibleArea;

	int nWidth=(int)((dSizeXRe+dExtraVisibleArea*2)*dDimension);
	int nHeight=(int)((dSizeYRe+dExtraVisibleArea*2)*dDimension);
	if (nWidth<1)
		nWidth=1;
	if (nHeight<1)
		nHeight=1;
	CMyImage<double> *MyImage=new CMyImage<double>();
	MyImage->Create(nWidth,nHeight);
	memset(MyImage->m_lpImage,0,nWidth*nHeight*sizeof(double));

	CAtomParams *pAtomParams=(CAtomParams *)pAtomsArray->GetAt(0);
	pAtomParams->m_dDimension=dDimension;

	for(int i=0;i<pAtomsArray->GetSize();i++)
	{
		pAtomParams=(CAtomParams *)pAtomsArray->GetAt(i);
		pAtomParams->Render();
		SumBlt(MyImage,
			(int)((pAtomParams->GetXPos()+dSizeXRe/2+dExtraVisibleArea-ATOM_IMAGE_SIZE/2)*dDimension),
			(int)((pAtomParams->GetYPos()+dSizeYRe/2+dExtraVisibleArea-ATOM_IMAGE_SIZE/2)*dDimension),
			pAtomParams->m_Image
			);
	}


	CMyStatic *MyStatic=(CMyStatic *)GetDlgItem(IDC_STATIC_CELL);
	ASSERT(MyStatic);
	MyStatic->SetImage(MyImage);
	MyStatic->m_dSizeXRe=dSizeXRe;
	MyStatic->m_dSizeYRe=dSizeYRe;
	MyStatic->m_dExtraVisibleArea=dExtraVisibleArea;

	delete MyImage;

	ReScale();

	return TRUE;
}

BOOL CLatDlg::OnTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];
	CString strTipText;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		UINT nID = pNMHDR->idFrom;
		// idFrom is actually the HWND of the tool
		nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
		if (nID != 0) // will be zero on a separator
		{
			AfxLoadString(nID, szFullText);
			// this is the command id, not the button index
			AfxExtractSubString(strTipText, szFullText, 1, '\n');
		}
	}else
	{
		CTabCtrl *pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_TAB);
		ASSERT(pTabCtrl);
		ASSERT(pTabCtrl->GetToolTips());
		if ((HWND)pNMHDR->hwndFrom==pTabCtrl->GetToolTips()->GetSafeHwnd())
		{
			switch(pNMHDR->idFrom)
			{
			case 0:
				AfxLoadString(IDS_TAB_TIP_1, szFullText);
				strTipText=szFullText;
				break;
			case 1:
				AfxLoadString(IDS_TAB_TIP_2, szFullText);
				strTipText=szFullText;
				break;
			case 2:
				AfxLoadString(IDS_TAB_TIP_3, szFullText);
				strTipText=szFullText;
				break;
			case 3:
				AfxLoadString(IDS_TAB_TIP_4, szFullText);
				strTipText=szFullText;
				break;
			case 4:
				AfxLoadString(IDS_TAB_TIP_5, szFullText);
				strTipText=szFullText;
				break;
			}
		}
	}

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText,
			(sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	else
		_mbstowcsz(pTTTW->szText, strTipText,
			(sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText,
			(sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	else
		lstrcpyn(pTTTW->szText, strTipText,
			(sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#endif
	*pResult = 0;

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	return TRUE;    // message was handled
}



void CLatDlg::CfgUpdate()
{
	if (!UpdateCell())
 		ReScale();
}

void CLatDlg::OnSelchangeComboAtoms() 
{
	// TODO: Add your control notification handler code here
	if (!m_bInit)
	{
		UpdateSelection(TRUE);
		UpdateCell();
	}
}

void CLatDlg::UpdateSelection(BOOL bParamsToImage)
{
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	ASSERT(!(m_nCurSelection<0));
	ASSERT(!(m_nCurSelection>=pAtomsArray->GetSize()));
	
	if (m_nCurSelection<0 || m_nCurSelection>=pAtomsArray->GetSize())
		return;
	m_bInit=TRUE;
	CAtomParams *pAtomParams=(CAtomParams *)pAtomsArray->GetAt(m_nCurSelection);

	CComboBox *ComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_ATOMS);
	ASSERT(ComboBox);

	CString String;
	if (bParamsToImage)
	{		
		double dVal=0;
		pAtomParams->SetZ(ComboBox->GetCurSel());
		GetDlgItemText(IDC_EDIT_ATOM_X,String);
		if (sscanf(String,"%lf",&dVal))
		{
			if (dVal<(-m_dSizeXRe/2.))
				dVal=(-m_dSizeXRe/2.);
			if (dVal>(m_dSizeXRe/2.))
				dVal=(m_dSizeXRe/2.);
			pAtomParams->SetXPos(dVal);
		}
		GetDlgItemText(IDC_EDIT_ATOM_Y,String);
		if (sscanf(String,"%lf",&dVal))
		{
			if (dVal<(-m_dSizeYRe/2.))
				dVal=(-m_dSizeYRe/2.);
			if (dVal>(m_dSizeYRe/2.))
				dVal=(m_dSizeYRe/2.);
			pAtomParams->SetYPos(dVal);
		}
	}else
	{
		ComboBox->SetCurSel(pAtomParams->GetZ());
		String.Format("%g",pAtomParams->GetXPos());
		SetDlgItemText(IDC_EDIT_ATOM_X,String);
		String.Format("%g",pAtomParams->GetYPos());
		SetDlgItemText(IDC_EDIT_ATOM_Y,String);
	}
	m_bInit=FALSE;
}

void CLatDlg::OnChangeEditAtom() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (!m_bInit)
	{
		UpdateSelection(TRUE);	
		UpdateCell();
	}
}

BOOL CLatDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);

	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			//26 - 48  1
			//
			//27 - 4d  3
			//
			//28 - 50  5
			//
			//25 - 4b  7
			if (pMsg->hwnd==m_MyStatic.GetSafeHwnd())
			{
				ASSERT(!(m_nCurSelection<0));
				ASSERT(!(m_nCurSelection>=pAtomsArray->GetSize()));
				
				if (m_nCurSelection<0 || m_nCurSelection>=pAtomsArray->GetSize())
					return TRUE;
				CAtomParams *pAtomParams=(CAtomParams *)pAtomsArray->GetAt(m_nCurSelection);
				if (pMsg->wParam==46)
				{
					if (pAtomsArray->GetSize()>1)
					{
						delete pAtomParams;
						pAtomsArray->RemoveAt(m_nCurSelection);
						m_nCurSelection=0;
					}
				}else
				{
					LPARAM lParam=(pMsg->lParam>>16)&0xFF;
					switch(lParam)
					{
					case 0x48:
						pAtomParams->SetYPos(pAtomParams->GetYPos()+(m_dSizeYRe/2.)*10./100.);
						break;
					case 0x4d:
						pAtomParams->SetXPos(pAtomParams->GetXPos()+(m_dSizeXRe/2.)*10./100.);
						break;
					case 0x50:
						pAtomParams->SetYPos(pAtomParams->GetYPos()-(m_dSizeYRe/2.)*10./100.);
						break;
					case 0x4b:
						pAtomParams->SetXPos(pAtomParams->GetXPos()-(m_dSizeXRe/2.)*10./100.);
						break;
					default:
						return CDialog::PreTranslateMessage(pMsg);
					}
				}
				UpdateSelection(FALSE);
				UpdateSelection(TRUE);
				UpdateCell();
				return TRUE;
			}
		}
	default:
		return CDialog::PreTranslateMessage(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLatDlg::OnAddAtom() 
{
	// TODO: Add your control notification handler code here

	m_bInsertMode=TRUE;
}

void CLatDlg::OnAddLayer() 
{
	// TODO: Add your control notification handler code here
	CComboBox *ComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_LAYERS);
	ASSERT(ComboBox);
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	CAtomParams *pAtomParams;
	pAtomParams=pAtomsArray->GetAt(m_nCurSelection);
	int Z=pAtomParams->GetZ();

	pAtomsArray = new CAtomsArray();
		
	m_nCurLayer=m_pLayersArray->Add(pAtomsArray);

	pAtomParams=new CAtomParams();
	pAtomParams->SetZ(Z);
	m_nCurSelection=pAtomsArray->Add(pAtomParams);
	CString String;

	String.Format("Слой %d",m_pLayersArray->GetSize());
	ComboBox->AddString(String);
	ComboBox->SetCurSel(m_pLayersArray->GetSize()-1);
	UpdateCell();
	UpdateSelection(FALSE);
}

void CLatDlg::OnSelchangeComboLayers() 
{
	// TODO: Add your control notification handler code here
	CComboBox *pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_LAYERS);
	ASSERT(pComboBox);
	m_nCurLayer=pComboBox->GetCurSel();
	m_nCurSelection=0;
	UpdateCell();
	UpdateSelection(FALSE);
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	CString String;
	char txt[64];
	sprintf(txt,"%g",pAtomsArray->m_dZ);
	SetDlgItemText(IDC_L2_EDIT_Z,txt);
}

void CLatDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTabCtrl *pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_TAB);
	CWnd *pWnd;
	m_bInsertMode=FALSE;
	int nCmdShow;

	if (pTabCtrl->GetCurSel()==0)
		nCmdShow=SW_SHOW;
	else
		nCmdShow=SW_HIDE;

	pWnd=GetDlgItem(IDC_COMBO_ATOMS);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_EDIT_ATOM_X);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_EDIT_ATOM_Y);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_ADD_ATOM);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
	
	pWnd=GetDlgItem(IDC_L1_STATIC_X);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
	
	pWnd=GetDlgItem(IDC_L1_STATIC_Y);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);

	if (pTabCtrl->GetCurSel()==1)
	{
		nCmdShow=SW_SHOW;
		CString String;
		String.Format("%g",m_pLayersArray->GetAt(m_nCurLayer)->m_dZ);
		SetDlgItemText(IDC_L2_EDIT_Z,String);
	}
	else
		nCmdShow=SW_HIDE;

	pWnd=GetDlgItem(IDC_ADD_LAYER);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_L2_DEL_LAYER);
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);

	LatShowWindow(IDC_L2_STATIC_Z);
	LatShowWindow(IDC_L2_EDIT_Z);

	if (pTabCtrl->GetCurSel()==2)
	{
		nCmdShow=SW_SHOW;
		m_bInit=TRUE;
		CString String;
		String.Format("%g",m_dSizeXRe);
		SetDlgItemText(IDC_L3_X,String);
		String.Format("%g",m_dSizeYRe);
		SetDlgItemText(IDC_L3_Y,String);
		String.Format("%g",m_dDimension);
		SetDlgItemText(IDC_L3_DIMENSION,String);
		String.Format("%g",m_dExtraVisibleArea);
		SetDlgItemText(IDC_L3_EXTRA_AREA,String);
		m_bInit=FALSE;
	}
	else
		nCmdShow=SW_HIDE;
	
	LatShowWindow(IDC_L3_STATIC_LATOPT);
	LatShowWindow(IDC_L3_STATIC_X);
	LatShowWindow(IDC_L3_X);
	LatShowWindow(IDC_L3_STATIC_Y);
	LatShowWindow(IDC_L3_Y);
	LatShowWindow(IDC_L3_STATIC_DIM);
	LatShowWindow(IDC_L3_DIMENSION);
	LatShowWindow(IDC_L3_STATIC_VIEWAREA);
	LatShowWindow(IDC_L3_STATIC_EXTRA_AREA);
	LatShowWindow(IDC_L3_EXTRA_AREA);

	if (pTabCtrl->GetCurSel()==3)
	{
		nCmdShow=SW_SHOW;
		m_bInit=TRUE;
		CString String;
		String.Format("%g",m_dMaxAtomOffs);
		SetDlgItemText(IDC_L4_ATOMOFFS,String);
		String.Format("%g",m_dMaxCellOffs);
		SetDlgItemText(IDC_L4_CELLOFFS,String);
		m_bInit=FALSE;
	}
	else
		nCmdShow=SW_HIDE;
	
	LatShowWindow(IDC_L4_STATIC_ATOMOFFS);
	LatShowWindow(IDC_L4_STATIC_CELLOFFS);
	LatShowWindow(IDC_L4_ATOMOFFS);
	LatShowWindow(IDC_L4_CELLOFFS);

	if (pTabCtrl->GetCurSel()==4)
	{
		nCmdShow=SW_SHOW;
	}
	else
		nCmdShow=SW_HIDE;
	
	LatShowWindow(IDC_L5_LOAD);
	LatShowWindow(IDC_L5_SAVE);

	*pResult = 0;
}


void CLatDlg::OnDelLayer() 
{
	// TODO: Add your control notification handler code here
	if (m_pLayersArray->GetSize()<2)
		return;
	CComboBox *ComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_LAYERS);
	ASSERT(ComboBox);
	m_nCurLayer=ComboBox->GetCurSel();
	ComboBox->DeleteString(m_nCurLayer);
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	m_pLayersArray->RemoveAt(m_nCurLayer);
	ASSERT(pAtomsArray);
	delete pAtomsArray;
	if (m_nCurLayer>0)
		m_nCurLayer--;
	m_nCurSelection=0;


	ComboBox->SetCurSel(m_nCurLayer);

	UpdateCell();
	UpdateSelection(FALSE);
}

void CLatDlg::OnChangeL3() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (!m_bInit)
	{
		CString String;
		double dVal=0;
		GetDlgItemText(IDC_L3_X,String);
		sscanf(String,"%lf",&dVal);
		if (dVal>=1 && dVal<=100)
		{
			m_dSizeXRe=dVal;
		}
		GetDlgItemText(IDC_L3_Y,String);
		sscanf(String,"%lf",&dVal);
		if (dVal>=1 && dVal<=100)
		{
			m_dSizeYRe=dVal;
		}
		GetDlgItemText(IDC_L3_DIMENSION,String);
		sscanf(String,"%lf",&dVal);
		if (dVal>=.01 && dVal<=50)
		{
			m_dDimension=dVal;
		}
		GetDlgItemText(IDC_L3_EXTRA_AREA,String);
		double dMaxSize=(m_dSizeXRe>m_dSizeYRe)?m_dSizeXRe:m_dSizeYRe;
		sscanf(String,"%lf",&dVal);
		if (dVal>=0 && dVal<=(dMaxSize*40.)/100.)
		{
			m_dExtraVisibleArea=dVal;
		}
		CfgUpdate();
	}
}

void CLatDlg::OnChangeL2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(m_nCurLayer);
	CString String;
	double dVal=0;
	GetDlgItemText(IDC_L2_EDIT_Z,String);
	sscanf(String,"%lf",&dVal);
	if (dVal>=0)
	{
		pAtomsArray->m_dZ=dVal;
	}
}

void CLatDlg::OnChangeL4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (!m_bInit)
	{
		CString String;
		double dVal=0;
		GetDlgItemText(IDC_L4_ATOMOFFS,String);
		if (sscanf(String,"%lf",&dVal) && dVal>=0)
		{
			m_dMaxAtomOffs=dVal;
		}	
		dVal=0;
		GetDlgItemText(IDC_L4_CELLOFFS,String);
		if (sscanf(String,"%lf",&dVal) && dVal>=0)
		{
			m_dMaxCellOffs=dVal;
		}
	}
}

void CLatDlg::OnChangeLAll() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString String;
	int nVal=0;
	GetDlgItemText(IDC_ALL_EDIT_NUMLAYERS,String);
	if (sscanf(String,"%d",&nVal) && nVal>=0)
	{
		m_nNumLayers=nVal;
	}	
		
}

void CLatDlg::OnL5Load() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,"Файлы моделей (*.lat)|*.lat|All files (*.*)|*.*||\0\0",this);
	if (FileDlg.DoModal()==IDOK)
	{
		char *lpcWrongFile="Неверный формат файла.";
		char *lpcFile=NULL;
		char *lpcTxt=NULL;
		CFileException exError;
		CFile File;
		CString sError="";
		if (!File.Open(FileDlg.GetPathName(),CFile::modeRead|CFile::shareDenyWrite,&exError))
		{
			sError.GetBufferSetLength(1024);
			exError.GetErrorMessage(sError.GetBufferSetLength(1024),1020);
			AfxMessageBox(sError);
			return;
		}
		CLayersArray LayersArray;
		CAtomsArray *pAtomsArray=NULL;
		CAtomParams *pAtomParams=NULL;
		double dSizeXRe=0;
		double dSizeYRe=0;
		double dDimension=0;
		double dMaxAtomOffs=0;
		double dMaxCellOffs=0;
		int nNLayers=0;
		double dZ=0;
		double dXPos;
		double dYPos;
		int nAtomZ;
		int nNumLayers=0;
		int nNumAtoms=0;
		int nCurLayer;
		int nCurAtom;
		int nCurPos=0;
		DWORD dwLength=File.GetLength();
		if (dwLength<3)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		
		lpcFile=new char[dwLength+1];
		lpcTxt=new char[dwLength+1];
		if (lpcFile==NULL || lpcTxt==NULL)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		File.Read(lpcFile,dwLength);
		lpcFile[dwLength]=0;
		if (memcmp(lpcFile,"LAT",3)!=0)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"Width %lf",&dSizeXRe)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"Height %lf",&dSizeYRe)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"Dimension %lf",&dDimension)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"MaxAtomOffs %lf",&dMaxAtomOffs)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"MaxCellOffs %lf",&dMaxCellOffs)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"NumLayers %d",&nNumLayers)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		if (sscanf(&lpcFile[nCurPos],"NLayers %d",&nNLayers)!=1)
		{
			sError=lpcWrongFile;
			goto FileLoadErr;
		}
		MacroMoveToNextLine(lpcFile,nCurPos);
		for(nCurLayer=0;nCurLayer<nNLayers;nCurLayer++)
		{
			lpcTxt[0]=0;
			pAtomsArray=new CAtomsArray();
			LayersArray.Add(pAtomsArray);
			if (sscanf(&lpcFile[nCurPos],"Name %[^\r^\n]",lpcTxt)!=1)
			{
				sError=lpcWrongFile;
				break;
			}
			pAtomsArray->m_sName=lpcTxt;
			MacroMoveToNextLine(lpcFile,nCurPos);
			if (sscanf(&lpcFile[nCurPos],"Z %lf",&dZ)!=1)
			{
				sError=lpcWrongFile;
				break;
			}
			pAtomsArray->m_dZ=dZ;
			MacroMoveToNextLine(lpcFile,nCurPos);
			if (sscanf(&lpcFile[nCurPos],"NAtoms %d",&nNumAtoms)!=1)
			{
				sError=lpcWrongFile;
				break;
			}
			MacroMoveToNextLine(lpcFile,nCurPos);
			for(nCurAtom=0;nCurAtom<nNumAtoms;nCurAtom++)
			{
				if (sscanf(&lpcFile[nCurPos],"Atom %d X %lf Y %lf",&nAtomZ,&dXPos,&dYPos)!=3)
				{
					sError=lpcWrongFile;
					break;
				}
				pAtomParams=new CAtomParams();
				pAtomParams->SetZ(nAtomZ);
				pAtomParams->SetXPos(dXPos);
				pAtomParams->SetYPos(dYPos);
				pAtomsArray->Add(pAtomParams);
				MacroMoveToNextLine(lpcFile,nCurPos);
			}
			if (sError!="")
				break;
		}
FileLoadErr:
		delete[] lpcFile;
		delete[] lpcTxt;
		File.Close();
		if (sError.GetLength()>0)
		{
			for(int i=0;i<LayersArray.GetSize();i++)
			{
				pAtomsArray=LayersArray.GetAt(i);
				ASSERT(pAtomsArray);
				for(int j=0;j<pAtomsArray->GetSize();j++)
				{
					pAtomParams=pAtomsArray->GetAt(j);
					delete pAtomParams;
				}
				delete pAtomsArray;
			}
			AfxMessageBox(sError);
		}else
		{
			for(int i=0;i<m_pLayersArray->GetSize();i++)
			{
				pAtomsArray=m_pLayersArray->GetAt(i);
				ASSERT(pAtomsArray);
				for(int j=0;j<pAtomsArray->GetSize();j++)
				{
					pAtomParams=pAtomsArray->GetAt(j);
					delete pAtomParams;
				}
				delete pAtomsArray;
			}
			m_pLayersArray->RemoveAll();
			for(int i=0;i<LayersArray.GetSize();i++)
			{
				pAtomsArray=LayersArray.GetAt(i);
				ASSERT(pAtomsArray);
				m_pLayersArray->Add(pAtomsArray);
			}
			m_dSizeXRe=dSizeXRe;
			m_dSizeYRe=dSizeYRe;
			m_dDimension=dDimension;
			m_dMaxAtomOffs=dMaxAtomOffs;
			m_dMaxCellOffs=dMaxCellOffs;
			m_nNumLayers=nNumLayers;
			UpdateAllCtrls();
			UpdateData(FALSE);
		}
	}
}

void CLatDlg::OnL5Save() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(FALSE,"lat","Untitled.lat",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,"Файлы моделей (*.lat)|*.lat|All files (*.*)|*.*||\0\0",this);
	if (FileDlg.DoModal()==IDOK)
	{
		CString sStr;
		CFileException exError;
		CFile File;
		
		if (!File.Open(FileDlg.GetPathName(),CFile::modeWrite|CFile::shareDenyWrite| CFile::modeCreate,&exError))
		{
			sStr.GetBufferSetLength(1024);
			exError.GetErrorMessage(sStr.GetBufferSetLength(1024),1020);
			AfxMessageBox(sStr);
			return;
		}
		sStr="LAT\r\n";
		File.Write(sStr,sStr.GetLength());
		sStr.Format("Width %g\r\nHeight %g\r\nDimension %g\r\n",m_dSizeXRe,m_dSizeYRe,m_dDimension);
		File.Write(sStr,sStr.GetLength());
		sStr.Format("MaxAtomOffs %g\r\nMaxCellOffs %g\r\n",m_dMaxAtomOffs,m_dMaxCellOffs);
		File.Write(sStr,sStr.GetLength());
		sStr.Format("NumLayers %d\r\n",m_nNumLayers);
		File.Write(sStr,sStr.GetLength());
		sStr.Format("NLayers %d\r\n",m_pLayersArray->GetSize());
		File.Write(sStr,sStr.GetLength());
		for(int nCurLayer=0;nCurLayer<m_pLayersArray->GetSize();nCurLayer++)
		{
			CAtomsArray *pAtomsArray=m_pLayersArray->GetAt(nCurLayer);
			ASSERT(pAtomsArray);
			sStr.Format("Name %s\r\nZ %g\r\nNAtoms %d\r\n",pAtomsArray->m_sName,pAtomsArray->m_dZ,pAtomsArray->GetSize());
			File.Write(sStr,sStr.GetLength());
			for(int nCurAtom=0;nCurAtom<pAtomsArray->GetSize();nCurAtom++)
			{
				CAtomParams *pAtomParams=pAtomsArray->GetAt(nCurAtom);
				ASSERT(pAtomParams);
				sStr.Format("Atom %d X %g Y %g\r\n",pAtomParams->GetZ(),pAtomParams->GetXPos(),pAtomParams->GetYPos());
				File.Write(sStr,sStr.GetLength());
			}
		}
		File.Close();
		
	}
}

void CLatDlg::UpdateAllCtrls()
{
	CComboBox *ComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_LAYERS);
	ASSERT(ComboBox);
	ComboBox->ResetContent();
	for(int i=0;i<m_pLayersArray->GetSize();i++)
	{
		ComboBox->AddString(m_pLayersArray->GetAt(i)->m_sName);
	}
	m_nCurLayer=0;
	m_nCurSelection=0;
	ComboBox->SetCurSel(0);
	UpdateCell();
	UpdateSelection(FALSE);
}
