// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__DE26E635_F772_11D3_B050_000001260696__INCLUDED_)
#define AFX_DIB_H__DE26E635_F772_11D3_B050_000001260696__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDib  
{
public:
	int GetFlag();
	BITMAPFILEHEADER BmpHeader;  // заголовки
	BITMAPINFOHEADER InfoHeader; // файла

	RGBQUAD* lpRGB;
	BYTE* lpBMP;
	CDib(CString filename);
	CDib();
	virtual ~CDib();
	int LoadFile(CString filename);
	UINT GetNumColor();
	CString m_FileName;


protected:
	int m_Flag;
	BOOL TypeBmpFile(FILE *In);
	void GetFileNameLoad();
};

#endif // !defined(AFX_DIB_H__DE26E635_F772_11D3_B050_000001260696__INCLUDED_)
