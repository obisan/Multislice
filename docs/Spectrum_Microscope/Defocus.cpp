// Defocus.cpp : Implementation of CDefocus
#include "stdafx.h"
#include "Defocus.h"
#include ".\defocus.h"


// CDefocus

STDMETHODIMP CDefocus::Calculate (long iteration)
{
	m_iteration = iteration;
	HRESULT  hr = S_OK;

	//	проверяем входные параметры и 
	//	запрашиваем интерфейс на исходное изображение
	CComQIPtr<ILocalImage>	 cplx = (LPUNKNOWN) NULL;
	cplx = (m_srcObject.vt == VT_UNKNOWN) ? m_srcObject.punkVal : NULL;
	cplx = (m_srcObject.vt == VT_DISPATCH) ? m_srcObject.pdispVal : cplx;
	if (cplx == NULL)	return E_INVALIDARG;

	ImageInfo	ii;
	cplx->GetBitmapPtr (0, &ii);

	//	получаем указатель на изображение или создаем новое
	CComQIPtr<ILocalImage>	 dstImage;
	dstImage = (m_dstObject.vt == VT_UNKNOWN) ? m_dstObject.punkVal : NULL;
	dstImage = (m_dstObject.vt == VT_DISPATCH) ? m_dstObject.pdispVal : dstImage;
	if (dstImage == NULL)
		hr = dstImage.CoCreateInstance(L"Improc.ImageObject");
	if (hr == S_OK)
		hr = dstImage->CreateImage (ii.width, ii.height, (m_type==3) ? ImageCplx : ImageCplxRe);

	ImageInfo	iire, iiim;
	dstImage->GetBitmapPtr (0, &iire);
	dstImage->GetBitmapPtr (1, &iiim);
		
	float	N = ii.width;
	float	k = (2.f * M_PI * m_alfa) / (N * N);
	for (long y=ii.height-1; y>=0; y--)
	{
		for (long x=0; x<ii.width; x++)
		{
			float	i2 = (x-N/2) * (x-N/2);
			float	j2 = (y-N/2) * (y-N/2);
			if (m_type == 1)
				((float*) iire.pBits)[x] = cos(k*(i2+j2));
			if (m_type == 2)
			{
				//if(y == ii.height/2 && x==ii.width/2)
				//	((float*) iire.pBits)[x] =2+ sin(k*(i2+j2));
				//else
					((float*) iire.pBits)[x] = sin(k*(i2+j2));
			}
			if (m_type == 3)
				((float*) iire.pBits)[x] = cos(k*(i2+j2)),
				((float*) iiim.pBits)[x] = sin(k*(i2+j2));
		}
		iire.pBits += iire.bpWidth;
		iiim.pBits += iiim.bpWidth;
		ii.pBits += ii.bpWidth;
	}

	//	сохраняем все свойства у изображения
	CComQIPtr<IImageProperty>	prop = dstImage;
	CComQIPtr<IDataScience>		ds = dstImage;
	if (prop != NULL)
		prop->UpdateProperties (m_srcObject, 0);
	if (ds != NULL)
		ds->put_Name (m_bstrName);
	
	m_dstObject.Clear();
	m_dstObject = CComVariant((LPUNKNOWN) dstImage);
	return S_OK;
}

LRESULT CDefocus::OnBnClickedRadio1 (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_type = 1;
	GetDlgItem (IDC_EDIT2).EnableWindow(0);
	return 1;
}

LRESULT CDefocus::OnBnClickedRadio2 (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_type = 2;
	GetDlgItem (IDC_EDIT2).EnableWindow(1);
	return 1;
}

LRESULT CDefocus::OnBnClickedRadio3 (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_type = 3;
	GetDlgItem (IDC_EDIT2).EnableWindow(0);
	return 1;
}

LRESULT CDefocus::OnEnKillfocusEdit1(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	char	str[32];
	memset (str, 0, 32);
	if(wID == IDC_EDIT1)	GetDlgItemText (IDC_EDIT1, str, 31),
							m_alfa = atof(str);
	if(wID == IDC_EDIT2)	GetDlgItemText (IDC_EDIT2, str, 31),
							m_constant = atof(str);
	return 1;
}

LRESULT CDefocus::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_type == 1)	CheckDlgButton (IDC_RADIO1, 1);
	if (m_type == 2)	CheckDlgButton (IDC_RADIO2, 1);
	if (m_type == 3)	CheckDlgButton (IDC_RADIO3, 1);

	GetDlgItem (IDC_EDIT2).EnableWindow(0);

	CString  str;
	str.Format ("%lf", m_alfa);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT1, str);
	str.Format ("%lf", m_constant);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT2, str);
	return TRUE;
}
