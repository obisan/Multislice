/*	***  sumpix.c ***

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

  read in two or more images and average
  optionally take the FFT and sum square magnitudes
  (for frozen phonon averaging of CBED patterns)

  started 7-mar-1997 E. Kirkland
  added log option 8-mar-1997 ejk
  added real space averaging and changed name from sumCBED to sumpix
		6-jan-1998 ejk
  in working form 16-jan-1998 ejk
  fixed small problem with complex images in real space 19-jan-1998 ejk
  changed greyscale scaling on log() option 25-feb-1998 ejk
*/

#include <stdio.h>	/* ANSI C libraries */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "fft2dc.h"	/* FFT routines */
#include "memory.h"	/* memory allocation routines */
#include "slicelib.h"	/* misc. routines for multislice */
#include "tiffsubs.h"	/* file I/O routines in TIFF format */

#define NCMAX	132	/* max number of characters per line */
#define NPARAM	64	/* number of parameters */

#define integerPIX	0	/* pix type flags */
#define floatPIX	1

void main()
{
	char **filein, fileout[NCMAX];
	char datetime[20];

	int i, ipix, ix, iy, nx, ny, nxold, nyold, ixmid, iymid, npix, npixold,
		ninput, nsum, nbits[3], samples, nh, logpix, 
		PowerSpectra, pixtype;
	long nxl, nyl, *nhist, **lpix, **lpixout, psub, lp, lmin, lmax;

	float scale, pixc, rmin,rmax, aimin,aimax,tr, ti;
	float *param;
        float  **pixr, **pixi, **pixout;
        double sum, *hist, ax, by, rx, ry2, dscale;

	FILE *fp;

/* get input file names etc. */

	printf( "sumpix.c version dated 25-feb-1998 ejk\n");

	printf( "Sum multiple image or wave function files,\n"
		"complex images will be converted to squared "
		"magnitude before summing.\n");
	printf( "All input images must be the same type and size.\n\n" );
	printf( "Type number of input image files\n");
	scanf( "%d", &ninput );
	filein = char2D( ninput, NCMAX, "filein" );
	for( ipix=0; ipix<ninput; ipix++) {
	    printf("input %d : ", ipix );
	    scanf("%s", filein[ipix] );
	}
	printf("\n");

	printf("Type name of output file:\n");
	scanf( "%s", fileout );

	logpix = askYN( "Do you want to display on log scale");

	PowerSpectra = askYN( "Do you want to convert to a power spectra");

/* get image size and type from the first input pix
	all succesive images have to be the same type and size !!! 
  remember that floating point images have an 8 bit integer image first
  	so the test must be done in this order.
*/
	if( tFloatTest( filein[0] ) == 1 ) {

	   	if( topenFloat( filein[0] ) != 1 ) {
			printf("Cannot open floating pt. pix %s\n", filein[0] );
			exit( 0 );
	   	}
	   	tsize( &nxl, &nyl, nbits, &samples );
	   	tclose();
	   	nx = nxold = (int) nxl;
	   	ny = nyold = (int) nyl;
	   	pixr = float2D( 2*nx, ny, "pixr-1" );
	   	pixout = float2D( nx, ny, "pixout-1" );
		for( ix=0; ix<nx; ix++) 
		    for( iy=0; iy<ny; iy++) pixout[ix][iy] = 0.0F;

	   	pixtype = floatPIX;
     	
  	} else if( tifftest( filein[0] ) == 1 ) {
 	   	if( topen( filein[0] ) != 1 ) {
			printf("Cannot open file %s\n", filein[0] );
			exit( 0 );
	   	}
	   	tsize( &nxl, &nyl, nbits, &samples );
	   	tclose();
	   	nx = nxold = (int) nxl;
	  	ny = nyold = (int) nyl;
	   	lpix = long2D( ny, nx, "lpix-1" );
	   	lpixout = long2D( ny, nx, "lpixout-1" );
		for( iy=0; iy<ny; iy++)
			for( ix=0; ix<nx; ix++) 
				lpixout[iy][ix] = 0;
	   	pixtype = integerPIX;

	} else{
		printf( "%s is not a valid TIFF file.\n", filein[0] );
		exit( 0 );
	}

	printf( "Image size : Nx= %d, Ny= %d\n", nx, ny );

/* -------- read floating point images and average --------

   remember that complex images are stacked side by side
	with npix=2 and nx twice its real value 
	(real images have npix=1 and nx its normal value)
*/
	
	if( pixtype == floatPIX ) {
		param = float1D( NPARAM, "param" );
		for( ipix=0; ipix<ninput; ipix++) {
	    		for( ix=0; ix<NPARAM; ix++) param[ix] = 0.0F;
	   		if( topenFloat( filein[ipix] ) != 1 ) {
				printf("Cannot open file %s\n", filein[ipix] );
				exit( 0 );
	   			}
	   		tsize( &nxl, &nyl, nbits, &samples );
	   		nx = (int) nxl;
	   		ny = (int) nyl;
	   		if( (nx != nxold) || (ny != nyold) ) {
	   			printf( "different size in file %s, "
	   			  " nx= %d, ny= %d\n", filein[ipix], nx, ny );
	   			exit( 0 );
	   		}
	   		if(treadFloatPix( pixr, nxl,nyl, &npix, datetime,
	   			param) != 1){    
	   			   printf("Cannot read input file %s.\n",
	   			   		filein[ipix] );
				   exit(0);
	   		}
	   		tclose();
	   		if( ipix == 0 ) npixold = npix;
	   		if( npix != npixold ) {
	   			printf( "Can't mix real and complex images"
	   				" in file: %s\n", filein[ipix] );
	   			exit( 0 );
	   		}
	   		if( (npix<1) || (npix>2) ) {
				printf( "bad npix = %d in TIFF file %s\n",
					npix, filein[ipix] );
				exit( 0 );
	   		}

			nx = nx /npix;
           		ax = param[pDX] * ((float)nx);
           		by = param[pDY] * ((float)ny);
	  		rmin = param[pRMIN];
	   		rmax = param[pRMAX];
	   		aimin = param[pIMIN];
	   		aimax = param[pIMAX];
			if( npix == 2 ) {
	   		    printf( "pix %d created %s, range: %g to %g (real),"
	   				"\n         and %g to %g (imag)\n",
	   				ipix, datetime, rmin, rmax, aimin, aimax);
			} else if( npix == 1 ) {
	   		    printf( "pix %d created %s, range: %g to %g (real)\n",
	   				ipix, datetime, rmin, rmax );
			}
			if( PowerSpectra == 1 ) {
				if( npix == 1 ) {
					if( ipix == 0 )
						pixi = float2D( nx, ny, "pixi-2" );
          				for( ix=0; ix<nx; ix++) 
              				for( iy=0; iy<ny; iy++) 
              					pixi[ix][iy] = 0.0F;
              		   	} else if( (npix==2) && (ipix==0) )
					pixi = pixr + nx;
				npix = 2;
	    			fft2d ( pixr, pixi, nx, ny, +1);
			}

	   		if( npix == 1 ) {		/* real pix */
          			for( ix=0; ix<nx; ix++) 
              			for( iy=0; iy<ny; iy++) 
              				pixout[ix][iy] += pixr[ix][iy];
	   		} else if( npix == 2 ) {	/* complex pix */
				if( ipix == 0 ) pixi = pixr + nx;
          			for( ix=0; ix<nx; ix++) 
              			for( iy=0; iy<ny; iy++) {
              				tr = pixr[ix][iy];
              				ti = pixi[ix][iy];
              				pixout[ix][iy] += ( tr*tr + ti*ti);
              			}
    			}
    		}  /* end for(ipix=... ) */
    
/* ------ read integer valued images and average --------- */

	} else if( pixtype == integerPIX ) {

		if( PowerSpectra == 1 ) {
			pixr = float2D( 2*nx, ny, "pixr-3" );
			pixi = pixr + nx;
			pixout = float2D( nx, ny, "pixout-3" );
			for( ix=0; ix<nx; ix++) 
			for( iy=0; iy<ny; iy++)
				pixout[ix][iy] = 0.0F;
			param = float1D( NPARAM, "param" );
		}
		for( ipix=0; ipix<ninput; ipix++) {
		
			printf( "read file %s\n", filein[ipix] );
			if( topen( filein[ipix] ) != 1 ) {
				printf("Cannot open file %s\n", filein[ipix] );
				exit( 0 );
	   		}
	   		tsize( &nxl, &nyl, nbits, &samples );
	   		nx = (int) nxl;
	  		ny = (int) nyl;
	   		if( (nx != nxold) || (ny != nyold) ) {
	   			printf( "different size in file %s, "
	   			  " nx= %d, ny= %d\n", filein[ipix], nx, ny );
	   			exit( 0 );
	   		}
	   		if( treadPix( lpix ) != 1 ) {
				printf("Cannot read input file %s.\n", filein[ipix] );
				exit(0);
	   		}
	   		tclose();
			if( PowerSpectra == 1 ) {
            			for( ix=0; ix<nx; ix++) 
              			for( iy=0; iy<ny; iy++) {
					pixr[ix][iy] = (float) lpix[iy][ix];
					pixi[ix][iy] = 0.0F;
				}
	    			fft2d ( pixr, pixi, nx, ny, +1);
          			for( ix=0; ix<nx; ix++) 
              			for( iy=0; iy<ny; iy++) {
              				tr = pixr[ix][iy];
              				ti = pixi[ix][iy];
              				pixout[ix][iy] += ( tr*tr + ti*ti);
              			}
			} else {
            			for( ix=0; ix<nx; ix++) 
              			for( iy=0; iy<ny; iy++)
					lpixout[iy][ix] += lpix[iy][ix];
	    		}
	
	   		npix = 1;
			if( PowerSpectra == 1 ) {
				pixtype = floatPIX;
				npix = 2;
           			ax = (float) nx;
           			by = (float) ny;
				param[pDX] = 1.0F;
				param[pDY] = 1.0F;
			}

		} /*  end for(ipix=... ) */

	}  /* end if( pixtype==integerPIX ) */

/*  Output results and find min and max to echo
     NOTE the logarithmic scaling of diffraction pattern
        is taken from Gonzalez and Wintz pg 48
    added scaling trick from showpix.f  9-aug-1995 ejk
*/
	printf("Output pix size : Nx= %d, Ny= %d\n", nx, ny );

	if( (PowerSpectra == 1) && ( pixtype == floatPIX ) ) {

		/* put (0,0) in the center */
		invert2D( pixout, nx, ny);

		/* histogram the azimutal average */
		hist = double1D( (nx+ny), "hist" );
		nhist = (long*) malloc( (nx+ny) * sizeof(long) );
		if( nhist == NULL ) {
			printf( "cannot allocate nhist\n");
			exit( 0 );
		}
		for( ix=0; ix<(nx+ny); ix++) {
			hist[ix] = 0.0;
			nhist[ix] = 0;
		}

		scale = 1.0F / ( ((float)nx) * ((float)ny) );

		sum = 0.0;
		nsum = 0;
		nh = 0;
		ixmid = nx/2;
		iymid = ny/2;

		for( iy=0; iy<ny; iy++) {
		    ry2 = (double) ( iy-iymid);
		    ry2 = ry2*(ax/by);
		    ry2 = ry2*ry2;
		    for( ix=0; ix<nx; ix++) {
			pixc = pixout[ix][iy];
			rx = (double) (ix-ixmid);
			i = (int) ( sqrt( rx*rx + ry2 ) + 0.5);
			hist[i] += pixc;
			nhist[i]++;
			if( i > nh ) nh = i;
			if( logpix == 1 ) {
				if( pixc > 1.e-10F)  pixc = 
					(float) log( (double) fabs(pixc) );
				else pixc = -23.0F;
				pixout[ix][iy] = pixc;
			}
			if( (ix == 0) && (iy == 0) ) {
				rmin = pixc;
				rmax = rmin;
			} else if( (ix != ixmid) && (iy != iymid) ) {
				if( pixc < rmin ) rmin = pixc;
				if( pixc > rmax ) rmax = pixc;
			}
			if( (ix>(3*nx)/8) && (ix<(5*nx)/8) &&
				(iy>(3*ny)/8) && (iy<(5*ny)/8) ) {
				sum = sum + pixc;
				nsum += 1;
			}

		    }  /* end for ix... */
		} /* end for iy... */

		printf( "write azimuthal averaged intensity vs. \n"
			"  spatial frequency k, into file azimuth.dat\n");
		fp = fopen( "azimuth.dat", "w+" );
		if( fp == NULL ) {
			printf("cannot open file azimuthal.dat\n");
			exit( 0 );
		}
		for( i=0; i<=nh; i++) {
			hist[i] = hist[i] / nhist[i];
			fprintf( fp, "%16.8g  %16.8g\n", ((double)i)/ax, hist[i] );
		}
		fclose( fp );

		param[pRMAX] = rmax;
		param[pIMAX] = 0.0F;
		param[pIMIN] = 0.0F;
		param[pRMIN] = (float) (0.05*rmin + 0.95*sum/nsum);
		param[pDX] = 1.0F / (nx*param[pDX]);
		param[pDY] = 1.0F / (ny*param[pDY]);
		printf("output image size: %f to %f /Angstroms\n",
			nx*param[pDX], ny*param[pDY] );
		printf("Power Spectra range %f to %f\n",  rmin, rmax );

		tcreateFloatPixFile( fileout, pixout, (long) nx,
			(long) ny, 1, param );

	} else if(  (pixtype == floatPIX) && (PowerSpectra == 0) ) {

		for( iy=0; iy<ny; iy++) {
		    for( ix=0; ix<nx; ix++) {
			pixc = pixout[ix][iy];
			if( logpix == 1 ) {
				if( pixc > 1.e-30F)  pixc = 
					(float) log( (double) fabs(pixc) );
				else pixc = -100.0F;
				pixout[ix][iy] = pixc;
			}
			if( (ix == 0) && (iy == 0) ) {
				rmin = pixc;
				rmax = rmin;
			} else {
				if( pixc < rmin ) rmin = pixc;
				if( pixc > rmax ) rmax = pixc;
			}

		    }  /* end for ix... */
		} /* end for iy... */

		param[pRMAX] = rmax;
		param[pIMAX] = 0.0F;
		param[pIMIN] = 0.0F;
		param[pRMIN] = rmin;
		printf("Summed pix range %f to %f\n",  rmin, rmax );

		tcreateFloatPixFile( fileout, pixout, (long) nx,
			(long) ny, 1, param );

	} else if( pixtype == integerPIX ) {

		for( iy=0; iy<ny; iy++) {
		    for( ix=0; ix<nx; ix++) {
			lp = lpixout[iy][ix];
			if( logpix == 1 ) {
				pixc = (float) lp;
				if( pixc > 1.e-10F)  pixc = 
					(float) log( (double) fabs(pixc) );
				else pixc = -23.0F;
				lpixout[iy][ix] = lp = (long) pixc;
			}
			if( (ix == 0) && (iy == 0) ) {
				lmin = lp;
				lmax = lmin;
			} else {
				if( lp < lmin ) lmin = lp;
				if( lp > lmax ) lmax = lp;
			}

		    }  /* end for ix... */
		} /* end for iy... */

		printf("Summed pix range %d to %d\n",  lmin, lmax );

		if( (lmax>255) || (lmin<0) ) {	/* minimize the scaling */
			psub = lmin;
			if( (lmax-lmin) > 255 )
				dscale = 255.0/(lmax-lmin);
			else
				dscale = 1.0;
			printf("rescaled by %f, and offset by %d\n",
				dscale, psub);
		} else {
			psub = 0;
			dscale = 1.0;
		}

		tcreatePixFile( fileout, lpixout, (long) nx, (long) ny,
			0, 0, 8, 0, psub, dscale, 1.0 );

	}

} /* end main() */