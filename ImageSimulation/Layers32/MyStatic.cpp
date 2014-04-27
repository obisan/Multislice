// MyStatic.cpp : implementation file
//

#include "stdafx.h"

#include "MyStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyStatic

CMyStatic::CMyStatic()
{
	m_bInFocus=FALSE;
	m_dExtraVisibleArea=0;
	m_dSizeXRe=5;
	m_dSizeYRe=5;

	m_BiBits=NULL;

	m_binfo=new MYBITMAPINFO;
	//m_binfo->hdr.biWidth=nWidth;
	//m_binfo->hdr.biHeight=nHeight;
	//int rx;
	//rx=nWidth/4;
	//if (nWidth%4) rx++;
	//rx*=4;
	//m_binfo->hdr.biSizeImage=nHeight*rx;
	m_binfo->hdr.biBitCount=8;
	m_binfo->hdr.biClrImportant=0;
	m_binfo->hdr.biClrUsed=0;
	m_binfo->hdr.biCompression=BI_RGB;
	m_binfo->hdr.biPlanes=1;
	m_binfo->hdr.biXPelsPerMeter=0;
	m_binfo->hdr.biYPelsPerMeter=0;
	m_binfo->hdr.biSize=sizeof(BITMAPINFOHEADER);
	//image=new UCHAR [rx*nHeight];
	for (short z=0;z<256;z++)
		m_binfo->colors[z]=RGB(z,z,z);

}

CMyStatic::~CMyStatic()
{
	delete m_binfo;
	if (m_BiBits)
		delete[] m_BiBits;
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	//{{AFX_MSG_MAP(CMyStatic)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyStatic message handlers

//DEL BOOL CMyStatic::PreTranslateMessage(MSG* pMsg) 
//DEL {
//DEL 	// TODO: Add your specialized code here and/or call the base class
//DEL 	if (pMsg->message==WM_PAINT || pMsg->message==0x101)
//DEL 	{
//DEL 		return CStatic::PreTranslateMessage(pMsg);
//DEL 	}
//DEL 	switch(pMsg->message)
//DEL 	{
//DEL 	
//DEL 	case WM_KEYDOWN:
//DEL 		{
//DEL 			//26 - 48  1
//DEL 			//
//DEL 			//27 - 4d  3
//DEL 			//
//DEL 			//28 - 50  5
//DEL 			//
//DEL 			//25 - 4b  7
//DEL 			LPARAM lParam=(pMsg->lParam>>16)&0xFF;
//DEL 			switch(lParam)
//DEL 			{
//DEL 			case 0x48:
//DEL 				break;
//DEL 			case 0x4d:
//DEL 				break;
//DEL 			case 0x50:
//DEL 				break;
//DEL 			case 0x4b:
//DEL 				break;
//DEL 			default:
//DEL 				return CStatic::PreTranslateMessage(pMsg);
//DEL 			}
//DEL 			return CStatic::PreTranslateMessage(pMsg);
//DEL 			return TRUE;
//DEL 		}
//DEL 	default:
//DEL 		return CStatic::PreTranslateMessage(pMsg);
//DEL 	}
//DEL 	return TRUE;
//DEL }

void CMyStatic::OnSetFocus(CWnd* pOldWnd) 
{
	CStatic::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
	m_bInFocus=TRUE;
	Invalidate(FALSE);
}

void CMyStatic::OnKillFocus(CWnd* pNewWnd) 
{
	CStatic::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	m_bInFocus=FALSE;
	Invalidate(FALSE);
}
void CMyStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDC *pDC=&dc;
	CDC DCComp;
	DCComp.CreateCompatibleDC(pDC);
	CRect Rect;
	GetClientRect(&Rect);
	
	CBitmap BitmapComp;
	BitmapComp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
	CBitmap *BitmapOld=DCComp.SelectObject(&BitmapComp);
	DCComp.FillSolidRect(&Rect,RGB(0,0,0));
	if (m_BiBits)
	{
		StretchDIBits(DCComp.GetSafeHdc(),  // handle of device context  
			0, 		// x-coordinate of upper-left corner of dest. rect. 			
			0, 			// y-coordinate of upper-left corner of dest. rect. 			
			Rect.Width(), 			// width of destination rectangle 			
			Rect.Height(), 			// height of destination rectangle 			
			0, 			// x-coordinate of upper-left corner of source rect. 			
			0, 			// y-coordinate of upper-left corner of source rect. 			
			m_binfo->hdr.biWidth, 			// width of source rectangle 			
			m_binfo->hdr.biHeight, 			// height of source rectangle 			
			m_BiBits, 			// address of bitmap bits 			
			(BITMAPINFO*)m_binfo, 			// address of bitmap data 			
			DIB_RGB_COLORS, 			// usage 			
			SRCCOPY 			// raster operation code 			
			); 
		
		
	}
	if (m_bInFocus)
	{
		CPen *oldPen;
		//Rect.DeflateRect(1,1);
		oldPen=DCComp.SelectObject(&CPen(PS_DOT,1,RGB(255,255,255)));
		int PrevMode=DCComp.SetROP2(R2_XORPEN);
		DCComp.MoveTo(Rect.left,Rect.top);
		DCComp.LineTo(Rect.right-1,Rect.top);
		DCComp.LineTo(Rect.right-1,Rect.bottom-1);
		DCComp.LineTo(Rect.left,Rect.bottom-1);
		DCComp.LineTo(Rect.left,Rect.top);
		DCComp.SetROP2(PrevMode);
		DCComp.SelectObject(oldPen);
		//Rect.InflateRect(1,1);
	}
	
	if (m_bInFocus)
	{
		Rect.DeflateRect(1,1);
		DCComp.DrawFocusRect(&Rect);
		Rect.InflateRect(1,1);
	}
	
	if (m_dExtraVisibleArea>0)
	{
		
		int OffsX=(int)(((double)Rect.Width()/(m_dExtraVisibleArea*2+m_dSizeXRe))*m_dExtraVisibleArea);
		int OffsY=(int)(((double)Rect.Height()/(m_dExtraVisibleArea*2+m_dSizeYRe))*m_dExtraVisibleArea);
		CPen Pen;
		Pen.CreatePen(PS_DOT,1,RGB(0,0,200));
		CPen *PenOld=DCComp.SelectObject(&Pen);
		
		DCComp.MoveTo(OffsX,0);
		DCComp.LineTo(OffsX,Rect.Height());
		DCComp.MoveTo(Rect.Width()-OffsX,0);
		DCComp.LineTo(Rect.Width()-OffsX,Rect.Height());
		
		DCComp.MoveTo(0,OffsY);
		DCComp.LineTo(Rect.Width(),OffsY);
		DCComp.MoveTo(0,Rect.Height()-OffsY);
		DCComp.LineTo(Rect.Width(),Rect.Height()-OffsY);
		
		DCComp.SelectObject(PenOld);
		
	}
	{
		int OffsX=Rect.Width()/2;
		int OffsY=Rect.Height()/2;
		CPen Pen;
		Pen.CreatePen(PS_SOLID,1,RGB(70,70,70));
		CPen *PenOld=DCComp.SelectObject(&Pen);
		
		DCComp.MoveTo(OffsX,0);
		DCComp.LineTo(OffsX,Rect.Height());
		
		DCComp.MoveTo(0,OffsY);
		DCComp.LineTo(Rect.Width(),OffsY);
		
		DCComp.SelectObject(PenOld);
		DCComp.SetBkMode(TRANSPARENT); 
		DCComp.SetTextColor(RGB(150,150,150));
		CString String;

		OffsX=(int)(((double)Rect.Width()/(m_dExtraVisibleArea*2+m_dSizeXRe))*m_dExtraVisibleArea);
		OffsY=(int)(((double)Rect.Height()/(m_dExtraVisibleArea*2+m_dSizeYRe))*m_dExtraVisibleArea);
	
		String.Format("%g",m_dSizeYRe/2);
		if (OffsY>16 && Rect.Width()/2>String.GetLength()*8)
		{
			DCComp.TextOut(Rect.Width()/2+3,OffsY-16-3,String);
		}

		String.Format("%g",m_dSizeXRe/2);
		if (OffsX>String.GetLength()*8 && Rect.Height()/2>16)
		{
			DCComp.TextOut(Rect.Width()-OffsX+3,Rect.Height()/2-16-3,String);
		}
	}
	
	pDC->BitBlt(0,0,Rect.right+1,Rect.bottom+1,&DCComp,0,0,SRCCOPY);
	DCComp.SelectObject(BitmapOld);
	
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CMyStatic::SetImage(CMyImage<double> *Image)
{
	m_binfo->hdr.biWidth=Image->Width();
	m_binfo->hdr.biHeight=Image->Height();
	int rx;
	rx=Image->Width()/4;
	if (Image->Width()%4)
		rx++;
	rx*=4;
	m_binfo->hdr.biSizeImage=Image->Height()*rx;
	if (m_BiBits)
	{
		delete[] m_BiBits;
		m_BiBits=NULL;
	}
	m_BiBits=new UCHAR [m_binfo->hdr.biSizeImage];

	for (int y=0;y<Image->Height();y++)
	{
		for(int x=0;x<Image->Width();x++)
		{
			if (Image->m_lpImage[y*Image->Width()+x]>255)
				m_BiBits[y*rx+x]=0xFF;
			else
				if (Image->m_lpImage[y*Image->Width()+x]<0)
					m_BiBits[y*rx+x]=0;
				else
					m_BiBits[y*rx+x]=(UCHAR)Image->m_lpImage[y*Image->Width()+x];
		}
	}


	return TRUE;
}
