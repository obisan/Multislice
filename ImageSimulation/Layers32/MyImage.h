#ifndef __IMAGE_H__
#define __IMAGE_H__

template<class TYPE> class CMyImage
{
protected:
	int m_nWidth;
	int m_nHeight;
	BOOL m_bSavePtr;
public:
	int Width(){return m_nWidth;}
	int Height(){return m_nHeight;}
	TYPE *m_lpImage;
	
	CMyImage(int nWidth,int nHeight,TYPE *lpImage=NULL)
	{
		m_nWidth=0;
		m_nHeight=0;
		m_lpImage=NULL;
		m_bSavePtr=FALSE;
		if (!Create(nWidth,nHeight,lpImage))
		{
			ASSERT(0);
			throw;
		}
	}
	CMyImage()
	{
		m_nWidth=0;
		m_nHeight=0;
		m_lpImage=NULL;
		m_bSavePtr=FALSE;
	}
	BOOL Create(int nWidth,int nHeight,TYPE *lpImage=NULL)
	{
		if (nWidth<=0 || nHeight<=0)
		{
			ASSERT(0);
			return FALSE;
		}
		if (m_nWidth!=0 || m_nHeight!=0)
		{
			ASSERT(0);
			return FALSE;
		}
		if (m_lpImage)// && (m_bSavePtr==FALSE))
		{
			ASSERT(0);
			return FALSE;
		}
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		if (lpImage==NULL)
		{
			m_bSavePtr=FALSE;
			m_lpImage=new TYPE[nWidth*nHeight];
		}else
		{
			m_bSavePtr=TRUE;
			m_lpImage=lpImage;
		}
		return TRUE;
	}
	void Destroy()
	{
		if (m_lpImage && (m_bSavePtr==FALSE))
			delete[] m_lpImage;
		m_nWidth=0;
		m_nHeight=0;
		m_lpImage=NULL;
		m_bSavePtr=FALSE;
	}
	
	/*
	CMyImage(UCHAR *_Im,int _W,int _H,BOOL bSavePtr=FALSE):m_bSavePtr(bSavePtr)
	{
		int _WH=_W*_H;
		m_Width=_W;m_Height=_H;
		
		if (bSavePtr)
		{
			m_Image=_Im;
		}else
		{
			m_Image=new UCHAR[_WH];
		}
		UCHAR *BMP=m_Image;
		memset(m_nSlicesV,0,256*sizeof(char));
		if (bSavePtr)
		{
			for(int i=0;i<_WH;i++)
			{
				m_nSlicesV[BMP[i]]=1;
			}
		}else
		{
			UCHAR Pixel;
			for(int i=0;i<_WH;i++)
			{
				Pixel=_Im[i];
				BMP[i]=Pixel;
				m_nSlicesV[Pixel]=1;
			}
		}
	};

	CMyImage(CMyImage *CIm)
	{
		m_Width=CIm->Width();m_Height=CIm->Height();
		m_Image=new UCHAR[m_Width*m_Height];
		memcpy(m_Image,CIm->m_Image,sizeof(UCHAR)*m_Width*m_Height);
		memcpy(m_nSlicesV,CIm->m_nSlicesV,sizeof(char)*256);
	};
	*/
	
	~CMyImage() 
	{
		Destroy();
	}
};

#endif