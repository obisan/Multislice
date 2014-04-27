// Microscope.cpp : Implementation of CMicroscope

#include "stdafx.h"
#include "Microscope.h"


// CMicroscope


STDMETHODIMP CMicroscope::Calculate (long iteration)
{
	//m_iteration = iteration;
	HRESULT  hr = S_OK;

	////	получаем указатель на изображение или создаем новое
	CComQIPtr<ILocalImage>	 dstImage;
	hr = dstImage.CoCreateInstance(L"Improc.ImageObject");
	if (hr == S_OK)
		hr = dstImage->CreateImage (m_size, m_size, (m_type==3) ? ImageCplx : ImageCplxRe);

	ImageInfo	iire, iiim;
	dstImage->GetBitmapPtr (0, &iire);
	dstImage->GetBitmapPtr (1, &iiim);

	float Cs = m_Cs*1000;000;
	float	N = m_size/2;
	float lambda = 12.398/(sqrt(m_V*(m_V+1022)))/10;
	if(m_defoc_type == 0) m_deltaF =  sqrt((2*m_n - 0.5)*Cs*lambda);

	m_Tmin = (2*m_D)/N;
	m_Tmax = 2*sqrt(lambda*m_deltaF);

	for (long y=m_size-1; y>=0; y--)
	{
		for (long x=0; x<m_size; x++)
		{
			float	kx2 = (x-N) * (x-N);
			float	ky2 = (y-N) * (y-N);
			long	k   =  sqrt(kx2 + ky2);
			float	u	= float(k/m_D);
			float   alfa = (M_PI/2*Cs*pow(lambda, 3)*pow(u,4)-M_PI*m_deltaF*lambda*u*u);
			float	Es = exp(-pow( ((M_PI*m_alfa)/(1000*lambda)),2)*pow(Cs*pow(lambda, 3)*pow(u,3)-m_deltaF*lambda*u, 2));
			
			if (m_type == 1)
				((float*) iire.pBits)[x] = Es*cos(alfa);
			if (m_type == 2)
				((float*) iire.pBits)[x] = Es*sin(alfa);
			if (m_type == 3)
				((float*) iire.pBits)[x] = Es*cos(alfa),
				((float*) iiim.pBits)[x] = Es*sin(alfa);
		}
		iire.pBits += iire.bpWidth;
		iiim.pBits += iiim.bpWidth;
	
	}

	////	сохраняем все свойства у изображения
	CComQIPtr<IImageProperty>	prop = dstImage;
	CComQIPtr<IDataScience>		ds = dstImage;
	if (prop != NULL)
		prop->UpdateProperties (m_srcObject, 0);
	if (ds != NULL)
		ds->put_Name (m_bstrName);
	
	m_dstObject.Clear();
	m_dstObject = CComVariant((LPUNKNOWN) dstImage);

	CString  str;
	str.Format ("%lf", m_deltaF);
	SetDlgItemText (IDC_EDIT6, str);
	return S_OK;
}

LRESULT CMicroscope::OnBnClickedRadio1 (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_type = 1;
	return 1;
}

LRESULT CMicroscope::OnBnClickedRadio2 (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_type = 2;
	return 1;
}

LRESULT CMicroscope::OnBnClickedRadio3 (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_type = 3;
	return 1;
}

LRESULT CMicroscope::OnEnKillfocusEdit1(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	char  buf[256];
	memset (buf, 0, sizeof(buf));
	GetDlgItemText (wID, buf, sizeof(buf));

	float	val = atof (buf);
	if (wID == IDC_EDIT1)	m_size = val;
	if (wID == IDC_EDIT2)	m_V = val;
	if (wID == IDC_EDIT4)	m_D = val;
	if (wID == IDC_EDIT3)	m_Cs = val;
	if (wID == IDC_EDIT5)	m_n = val;
	if (wID == IDC_EDIT6)	m_deltaF = val;
	if (wID == IDC_EDIT7)	m_alfa = val;
	//char	str[32];
	//memset (str, 0, 32);
	//GetDlgItemText (IDC_EDIT1, str, 31);
	//m_alfa = atof(str);
	return 1;
}

LRESULT CMicroscope::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_type == 1)	CheckDlgButton (IDC_RADIO1, 1);
	if (m_type == 2)	CheckDlgButton (IDC_RADIO2, 1);
	if (m_type == 3)	CheckDlgButton (IDC_RADIO3, 1);
	if (m_defoc_type == 1)	CheckDlgButton (IDC_RADIO4, 1);
	if (m_defoc_type == 0)	CheckDlgButton (IDC_RADIO5, 1);
	
	CString  str;
	str.Format ("%d", m_size);
	SetDlgItemText (IDC_EDIT1, str);
	str.Format ("%lf", m_V);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT2, str);
	str.Format ("%lf", m_Cs);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT3, str);
	str.Format ("%lf", m_D);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT4, str);
	str.Format ("%d", m_n);
	SetDlgItemText (IDC_EDIT5, str);
	str.Format ("%lf", m_deltaF);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT6, str);
	str.Format ("%lf", m_alfa);
	str.TrimRight ('0');
	str.TrimRight ('.');
	SetDlgItemText (IDC_EDIT7, str);
	return TRUE;
}

LRESULT CMicroscope::OnBnClickedRadio4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	m_defoc_type = 1;
	GetDlgItem(IDC_EDIT6).EnableWindow(1);
	return 0;
}

LRESULT CMicroscope::OnBnClickedRadio5(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT6).EnableWindow(0);
	m_defoc_type = 0;
	return 0;
}
