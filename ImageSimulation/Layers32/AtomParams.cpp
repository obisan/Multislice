// AtomParams.cpp: implementation of the CAtomParams class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AtomParams.h"
#include "FParams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*-------------------- bessi0() ---------------*/
/*
    modified Bessel function I0(x)
    see Abramowitz and Stegun page 379

    x = (double) real arguments

    12-feb-1997 E. Kirkland
 */
 double bessi0( double x )
 {
 	int i;
 	double ax, sum, t;
 	
 	double i0a[] = { 1.0, 3.5156229, 3.0899424, 1.2067492,
		0.2659732, 0.0360768, 0.0045813 };

 	double i0b[] = { 0.39894228, 0.01328592, 0.00225319,
 		-0.00157565, 0.00916281, -0.02057706, 0.02635537,
 		-0.01647633, 0.00392377};

	ax = fabs( x );
	if( ax <= 3.75 ) {
		t = x / 3.75;
		t = t * t;
		sum = i0a[6];
		for( i=5; i>=0; i--) sum = sum*t + i0a[i]; 
	} else {
		t = 3.75 / ax;
		sum = i0b[8];
		for( i=7; i>=0; i--) sum = sum*t + i0b[i];
		sum = exp( ax ) * sum / sqrt( ax );
	}
	return( sum );

}  /* end bessi0() */

/*-------------------- bessk0() ---------------*/
/*
    modified Bessel function K0(x)
    see Abramowitz and Stegun page 380
    
    Note: K0(0) is not define and this function
        returns 1E20
 
    x = (double) real arguments
    
    this routine calls bessi0() = Bessel function I0(x)
    
    12-feb-1997 E. Kirkland
 */
double bessk0( double x )
{
	double bessi0(double);
	
	int i;
	double ax, x2, sum;
	double k0a[] = { -0.57721566, 0.42278420, 0.23069756,
		0.03488590, 0.00262698, 0.00010750, 0.00000740};
	
	double k0b[] = { 1.25331414, -0.07832358, 0.02189568,
		-0.01062446, 0.00587872, -0.00251540, 0.00053208};
	
	ax = fabs( x );
	if( (ax > 0.0)  && ( ax <=  2.0 ) ) {
		x2 = ax / 2.0;
		x2 = x2 * x2;
		sum = k0a[6];
		for( i=5; i>=0; i--) sum = sum*x2 + k0a[i];
		sum = -log(ax/2.0) * bessi0(x) + sum;
	} else if( ax > 2.0 ) {
		x2 = 2.0/ax;
		sum = k0b[6];
		for( i=5; i>=0; i--) sum = sum*x2 + k0b[i];
		sum = exp( -ax ) * sum / sqrt( ax );
	} else sum = 1.0e20;
	return ( sum );
	
}  /* end bessk0() */


double	CAtomParams::m_dDimension=0;
//CMap<int,int,CMyImage<double>*,CMyImage<double>*> CAtomParams::m_MapOfImages;

CAtomParams::CAtomParams()
{
	m_xPos = 0;
	m_yPos = 0;
	m_Image = new CMyImage<double>();
	m_Z = 12;
	m_bNeedUpdate = TRUE;
	m_bitTweenX = 0;
	m_bitTweenY = 0;
}

CAtomParams::~CAtomParams()
{
	delete m_Image;
}

BOOL CAtomParams::Render()
{
	//double PI=3.1415926535897932384626433832795;
	//double a0=0.529;
	//double e=14.4;

	ASSERT(!(m_Z < 0));
	ASSERT(!(m_Z > 102));
	if (m_dDimension <= 0)
		return FALSE;
	size_t nWidth = (int)(m_dDimension * ATOM_IMAGE_SIZE);
	size_t nHeight = (int)(m_dDimension * ATOM_IMAGE_SIZE);
	if (nWidth < 1)
		nWidth = 1;
	if (nHeight < 1)
		nHeight = 1;
	if (m_Image->Width() == 0 && m_Image->Height() == 0) {
		m_Image->Create(nWidth, nHeight);
		m_bNeedUpdate = TRUE;
	} else {
		if (nWidth != m_Image->Width() || nHeight != m_Image->Height()) {
			m_Image->Destroy();
			m_Image->Create(nWidth, nHeight);
			m_bNeedUpdate = TRUE;
		}
	}
	

	if (m_bNeedUpdate) {
		double dk = ATOM_IMAGE_SIZE / (double) nWidth;
		for(size_t i = 0; i < nHeight; i++) {
			double dY = (double) ((double) i - (double) nHeight / 2.0) * dk;
			for(size_t j = 0; j < nWidth; j++) {
				double dX = (double) ((double) j - (double) nWidth / 2.0) * dk;
				double dR = sqrt(dX * dX + dY * dY);
				if( dR < 1.0e-100 ) 
					dR = 1.0e-100;
				double sumf = 0, sums = 0;
				double dR1 = 6.2831853071796 * dR;
				for(int k = 0; k < 3; k++) {
					int Offs = (m_Z) * 12 + k * 2;
					sumf += FParams[Offs + 0] * bessk0(dR1 * sqrt(FParams[Offs + 1]));
				}
				
				sumf *= 300.73079394295; // 4 * PI * PI *a0 * e
				for(int k = 0; k < 3; k++) {
					int Offs = (m_Z) * 12 + k * 2;
					sums += (FParams[Offs + 6] / FParams[Offs + 7]) * exp(-(6.2831853071796 * dR * dR) / FParams[Offs + 7]);
				}
				
				sums *= 150.36539697148; // 2 * PI * PI * a0 * e
				m_Image->m_lpImage[i * nWidth + j] = (sumf + sums);
			}
		}
		m_bNeedUpdate = FALSE;
	}
	return TRUE;
}

BOOL CAtomParams::SetZ(int Z) {
	if (Z < 0 || Z > 102)
		return FALSE;
	m_Z = Z;
	m_bNeedUpdate = TRUE;
	return TRUE;
}

int CAtomParams::GetZ() {
	return m_Z;
}

double CAtomParams::GetXPos() {
	return m_xPos;
}

void CAtomParams::SetXPos(double xPos) {
	m_xPos = xPos;
	m_bNeedUpdate = TRUE;
}

double CAtomParams::GetYPos() {
	return m_yPos;
}

void CAtomParams::SetYPos(double yPos) {
	m_yPos = yPos;
	m_bNeedUpdate = TRUE;
}