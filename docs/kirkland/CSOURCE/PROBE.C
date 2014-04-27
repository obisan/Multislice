/*          *** probe.c ***

------------------------------------------------------------------------
Copyright 1998 Earl J. Kirkland
The computer code and or data in this file is provided for demonstration
purposes only with no guarantee or warranty of any kind that it is correct
or produces correct results.  By using the code and or data in this file
the user agrees to accept all risks and liabilities associated with the code
and or data. The computer code and or data in this file may be copied (and used)
for non-commercial academic or research purposes only, provided that this
notice is included. This file or any portion of it may not be resold, rented
or distributed without the written permission of the author.
------------------------------------------------------------------------

	ANSI-C version

	Calculate a focused probe wavefunction in real space

	this file is formatted for  a tab size of 8 characters

	rewritten in C 6-dec-1995 ejk
	fixed sign error in aberration function 1-mar-1997 ejk
	removed commas from keyboard input 3-oct-1997 ejk
*/

#include <stdio.h>	/*  ANSI-C libraries */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "fft2dc.h"	/*  FFT's */
#include "memory.h"	/* memory allocation routines */
#include "slicelib.h"	/* define parameter offsets */
#include "tiffsubs.h"	/* file I/O libraries */

#define	NCMAX	132	/* characters per line to read featom.tab */
#define NPARAM	64	/* number of parameters */

void main()
{
	char fileout[NCMAX];
	int ix, iy, nx, ny, ixmid, iymid, i, ismoth;
	float rmin, rmax, aimin, aimax;
	float *param, **pixr, **pixi;
	double  kx, ky, ky2, k2, k2max, v0, wavlen,
		ax, by, rx2, ry2, pi, dx, dy, scale, pixel,
		Cs, df, chi1, chi2, sum, chi, time;

/*  Echo version date  */

	printf( "c-probe version dated 3-oct-1997 ejk\n");

/*  Get desired image size, etc. */

	printf("Name of file to get focussed probe wavefunction:\n");
	scanf("%s", fileout );

	printf("Desired size of output image in pixels Nx,Ny:\n");
	scanf("%d %d", &nx, &ny );

	if( (nx != powerof2(nx)) || (ny != powerof2(ny)) ) {
		printf("Nx=%d, Ny=%d must be a power of 2,\n"
			"try again.\n", nx, ny);
		exit( 0 );
	}

	printf("Size of output image in Angstroms ax,by:\n");
	scanf("%lf %lf", &ax, &by );

	printf("Beam voltage in kilovolts:\n");
	scanf("%lf", &v0 );

	printf("Spherical aberration in mm.:\n");
	scanf("%lf", &Cs );
	Cs = Cs * 1.0e7;

	printf("Defocus in Angstroms:\n");
	scanf("%lf", &df );

	printf("Aperture in mrad:\n");
	scanf("%lf", &k2max );
        k2max = k2max * 0.001;

	printf("Type 1 for smooth aperture:\n");
	scanf("%d", &ismoth );

	printf("Probe position in Angstroms:\n");
	scanf("%lf %lf", &dx, &dy );

/*  Calculate misc constants  */

	time = cputim( );
        pi = 4.0 * atan( 1.0 );
	
        rx2 = 1.0/ax;
        rx2 = rx2 * rx2;
        ry2 = 1.0/by;
        ry2 = ry2 * ry2;
        
        ixmid = nx/2;
        iymid = ny/2;
        
        wavlen = 12.26/ sqrt( v0*1.e3 + v0*v0*0.9788 );
        chi1 = pi*wavlen;
        chi2 = 0.5 * Cs * wavlen*wavlen;
        k2max = k2max/wavlen;
        k2max = k2max * k2max;

	param = float1D( NPARAM, "probe-param" );
	for( i=0; i<NPARAM; i++) param[i] = 0.0F;
	pixr = float2D( 2*nx, ny, "pixr" );
	pixi = pixr + nx;

/*   Calculate MTF 
	NOTE zero freg is in the bottom left corner and
	expandes into all other corners - not in the center
	this is required for FFT

	PIXEL = diagonal width of pixel squared
	if a pixel is on the apertur boundary give it a weight
	of 1/2 otherwise 1 or 0
*/
	pixel = ( rx2 + ry2 );

	for( iy=0; iy<ny; iy++) {
	   ky = (double) iy;
	   if( iy > iymid ) ky = (double) (iy-ny);
	   ky2 = ky*ky*ry2;
	   for( ix=0; ix<nx; ix++) {
		kx = (double) ix;
		if( ix > ixmid ) kx = (double) (ix-nx);
		k2 = kx*kx*rx2 + ky2;
		if ( ( ismoth != 0) && 
			( fabs(k2-k2max) <= pixel) ) {
		   chi= chi1*k2* (chi2*k2-df) -
			2.0F*pi*( (dx*kx/ax) + (dy*ky/by) );
                   pixr[ix][iy]= (float) ( 0.5 * cos(chi));
                   pixi[ix][iy]= (float) (-0.5 * sin(chi));
                   printf("smooth by 0.5 at ix=%d, iy=%d\n", ix, iy );
		} else if ( k2 <= k2max ) {
		   chi= chi1*k2* (chi2*k2-df) -
			2.0F*pi*( (dx*kx/ax) + (dy*ky/by) );
                   pixr[ix][iy]= (float)  cos(chi);
                   pixi[ix][iy]= (float) -sin(chi);
		} else {
                   pixr[ix][iy] = pixi[ix][iy] = 0.0F;
                }
	   }
	}

	fft2d( pixr, pixi, nx, ny, -1);

/*  Normalize probe intensity to unity  */

        sum = 0.0;
        for( ix=0; ix<nx; ix++) for( iy=0; iy<ny; iy++) 
	        sum +=  pixr[ix][iy]*pixr[ix][iy]
	        		+ pixi[ix][iy]*pixi[ix][iy];

	scale = 1.0 / sum;
	scale = scale * ((double)nx) * ((double)ny);
	scale = (double) sqrt( scale );

        for( ix=0; ix<nx; ix++) 
           for( iy=0; iy<ny; iy++) {
		pixr[ix][iy] *= (float) scale;
		pixi[ix][iy] *= (float) scale;
	}

/*  Output results and find min and max to echo
    remember that complex pix are stored in the file in FORTRAN
		order for compatability
*/

        rmin = pixr[0][0];
        rmax = rmin;
        aimin = pixi[0][0];
        aimax = aimin;
        for( iy=0; iy<ny; iy++) {
           for( ix=0; ix<nx; ix++) {
		if( pixr[ix][iy] < rmin ) rmin = pixr[ix][iy];
		if( pixr[ix][iy] > rmax ) rmax = pixr[ix][iy];
		if( pixi[ix][iy] < aimin ) aimin = pixi[ix][iy];
		if( pixi[ix][iy] > aimax ) aimax = pixi[ix][iy];
	   }
	}

	param[pRMAX] = rmax;
	param[pIMAX] = aimax;
	param[pRMIN] = rmin;
	param[pIMIN] = aimin;
	param[pDEFOCUS]= (float) df;
	param[pDX]= (float) (ax / nx);
	param[pDY]= (float) (by / ny);
	param[pENERGY]= (float) v0;
	param[pWAVEL]= (float) ( sqrt(k2max) * wavlen);
	param[pCS]= (float) Cs;
	param[27]= (float) dx;
	param[28]= (float) dy;

	if( tcreateFloatPixFile( fileout, pixr, (long) (2*nx), (long) ny,
			 2, param ) != 1 )
		printf( "probe cannot write an output file.\n");

	printf( "Pix range %15.7g to %15.7g real,\n"
		"      and %15.7g to %15.7g imaginary\n",
		rmin, rmax, aimin, aimax );
	time = cputim() - time;
	printf("\nCPU time = %f sec\n", time );

}  /* end main() */
