// ObjectFrame.h: interface for the CObjectFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTFRAME_H__AB29C8CB_9533_11D4_A6D4_00A0C99E2B24__INCLUDED_)
#define AFX_OBJECTFRAME_H__AB29C8CB_9533_11D4_A6D4_00A0C99E2B24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SpectrumFrame.h"



/*#define cmplx(x,y,z) {(z).r=(x); (z).i=(y);}
#define cadd(x,y,z) (z).r=(x).r+(y).r; (z).i=(x).i+(y).i
#define csub(x,y,z) (z).r=(x).r-(y).r; (z).i=(x).i-(y).i
#define cmul(x,y,z) (z).r=(x).r*(y).r-(x).i*(y).i; (z).i=(x).i*(y).r+(x).r*(y).i
#define cdiv(x,y,z) (z).i=(y).r*(y).r+(y).i*(y).i; (z).r=((x).r*(y).r+(x).i*(y).i)/(z).i; (z).i=((x).i*(y).r-(x).r*(y).i)/(z).i
#define cconj(x,y) (y).r=(x).r; (y).i=-(x).i
#define cexp(fi,z) (z).r=cos((float)(fi)); (z).i=sin((float)(fi))
#define cmod(z,d) (d)=hypot((z).r,(z).i)
#define cpha(z,f) (f)=atan2((z).i,(z).r)
#define MIN(x,y)        ((x)<(y) ? (x):(y))
#define MAX(x,y)        ((x)<(y) ? (y):(x))
#define M_PI   3.1415926535898
#define M_2_PI 6.2831853071796
#define M_PI_2 1.5707963267949*/



class CObjectFrame : public CSpectrumFrame  
{
public:
	CObjectFrame();
	CObjectFrame(fi_complex *arroy, int Dem);

	
	virtual ~CObjectFrame();

protected:
	double fi;
	struct fi_complex z1,z2;
	//применение разных дефокусировок//

	virtual int Calc();                      //
	///////////////////////////////////
	struct fi_complex *zz;
	double fi0;
	double fi01;
	double fi02;
	double fi03;
	double fi04;
	double fi05;
	void GetSpectrum();
	void Fft();
};

#endif // !defined(AFX_OBJECTFRAME_H__AB29C8CB_9533_11D4_A6D4_00A0C99E2B24__INCLUDED_)
