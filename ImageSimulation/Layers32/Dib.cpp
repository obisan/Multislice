// Dib.cpp: implementation of the CDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//#include "TestDib.h"
#include "windowsx.h"
#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDib::CDib()
{ 
	lpBMP=new BYTE[1];
	m_Flag=1;
	GetFileNameLoad();
	if(m_Flag==1)
	{
	LoadFile(m_FileName);
	}

}

CDib::CDib(CString filename)
{ 
	lpBMP=new BYTE[1];
	
	LoadFile(filename);
}


int CDib::LoadFile(CString filename)
{
	
	FILE *In;
	int H_W=1;
	In=fopen(filename, "rb");
	if(In==NULL) return FALSE;
	if (TypeBmpFile(In) == FALSE)
	{
		fclose(In);
		return FALSE;
	}
	fseek(In, 0x436, SEEK_SET);
	delete[] lpBMP;
	lpBMP=new BYTE[InfoHeader.biHeight*InfoHeader.biWidth];
	for(long i=0; i<InfoHeader.biHeight*InfoHeader.biWidth; i++)
	{
		fread(&lpBMP[i],1,1,In);
	}
	return 0;
}

UINT CDib::GetNumColor()
{
	return 0;
}

CDib::~CDib()
{
	delete[] lpBMP;
}



void CDib::GetFileNameLoad()
{
	TCHAR title[]="�������� BMP";
	CFileDialog fileDlg(TRUE);
	//������������ ������� �� �����
	CString str("��� ����� (*.*)"); str += (TCHAR)NULL;
	str += "*.*"; str += (TCHAR)NULL;
	str += "����������� (*.bmp)"; str += (TCHAR)NULL;
	str += "*.bmp"; str += (TCHAR)NULL;
	
	fileDlg.m_ofn.lpstrFilter = str;
	
	// ��� �������� ����� ������� ������������ ������
	// ������, ������� ���������� .bmp
	fileDlg.m_ofn.nFilterIndex = 2;
	// ������� ����� ��� ������� ����� ���������� �����
	TCHAR strName[_MAX_PATH];
	// ���� ����� ������ ���� ����
	strName[0] = (TCHAR)NULL;
	fileDlg.m_ofn.lpstrFile = strName;
	// �������� ��������� ����� �������
	fileDlg.m_ofn.lpstrTitle = title;
	// � ������ ��������������� �� ��������� ��������� �����������
	// ������ ���������� ������ ������������
	//fileDlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
	// ������� ���� ������� �� �����
	if (fileDlg.DoModal() == IDOK)
	{
		m_FileName=fileDlg.m_ofn.lpstrFile;
		m_Flag=1;
	}
	else
	{
		m_FileName="";
		m_Flag=0;
	}


}

BOOL CDib::TypeBmpFile(FILE *In)
{
	BOOL BmpTrue = FALSE;        // ���� ����������
	
	
	fseek(In, 0, SEEK_SET);
	if ( fread( &BmpHeader, sizeof(BITMAPFILEHEADER),1,In)==1)
	{
		if (BmpHeader.bfType == 0x4d42) 
		{
			fseek(In, sizeof(BITMAPFILEHEADER), SEEK_SET);
			
			if (fread( &InfoHeader, sizeof(BITMAPINFOHEADER),1,In) == 1)
			{
				if ((InfoHeader.biPlanes == 1) &&
					(InfoHeader.biBitCount == 8) &&
					(InfoHeader.biCompression == BI_RGB)) BmpTrue = TRUE;
				else BmpTrue = FALSE;
			}
			else BmpTrue = FALSE;
		}
		else BmpTrue = FALSE;
	}
	else BmpTrue = FALSE;
	
	if (BmpTrue == FALSE)
		MessageBox(NULL, "����������� ������ �����",
		"������ !", MB_OK | MB_ICONSTOP);
	
	return BmpTrue;
}

int CDib::GetFlag()
{
	return m_Flag;
}
