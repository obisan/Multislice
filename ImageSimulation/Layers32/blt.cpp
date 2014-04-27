
#include "stdafx.h"
#include "blt.h"


int SumBlt(CMyImage<double> *DestIm,int nXDest,int nYDest,CMyImage<double> *SrcIm)
{
	ASSERT(DestIm);
	ASSERT(SrcIm);
	if (DestIm==NULL || SrcIm==NULL)
		return 0;
	int nXSrc=0,nYSrc=0;
	int DestWidth=DestIm->Width(),  SrcWidth=SrcIm->Width();
	int DestHeight=DestIm->Height(),SrcHeight=SrcIm->Height();
	if (nXDest<0)
	{
		nXSrc=abs(nXDest);
		if (nXSrc>=SrcWidth)
			return 0;
		nXDest=0;
	}
	if (nYDest<0)
	{
		nYSrc=abs(nYDest);
		if (nYSrc>=SrcHeight)
			return 0;
		nYDest=0;
	}
	int nSizeX=SrcWidth-nXSrc;
	int nSizeY=SrcHeight-nYSrc;

	if ((nXDest+nSizeX)>DestWidth)
	{
		nSizeX=DestWidth-nXDest;
	}
	if ((nYDest+nSizeY)>DestHeight)
	{
		nSizeY=DestHeight-nYDest;
	}
	if (nSizeX<=0 || nSizeY<=0)
	{
		return 0;
	}
	
	double *DestImage,*SrcImage;
	for(int i=nSizeY-1;i>=0;i--)
	{
		DestImage=&(DestIm->m_lpImage[(i+nYDest)*DestWidth+nXDest]);
		SrcImage=&(SrcIm->m_lpImage[(i+nYSrc)*SrcWidth+nXSrc]);
		for(int j=nSizeX-1;j>=0;j--)
		{
			*DestImage+=*SrcImage;
			DestImage++;
			SrcImage++;
		}
	}
	
	return nSizeY;
}


