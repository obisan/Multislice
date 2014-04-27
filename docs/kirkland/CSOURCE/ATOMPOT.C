/*
          *** atompot.c ***

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

  ANSI-C and TIFF version

  Calculate the atomic potentials using tabulated electron
  scattering factors

  The units of the output are such that when multiplied by
  Lambda*(M/M0) the result is the phase shift of the transmitted
  electron wave (i.e. a simple sum of the unadulterated Born
  approx. scattering amplitudes).

  The calculation is performed in a 2D planar slice as a
  prelude to the multislice calculation of electron micrographs.

  Gaussian random displacements may be added to atom coordinate
  to simulate thermal vibrations (i.e. one snap shoot).

    rewritten in C  13-June-1995 ejk
    C version working 28-jun-1995 ejk
    fix bug in natom calculation 7-july-1995
    minor rearrangments 13-July-1995 ejk
    fix small bug in seval on end of range 16-july-1995 ejk
    converted to TIFF file format 8-apr-1996 ejk
    put fetab, featom() in slicelib.c 26-july-1996 ejk
    switch to new fe params 16-jan-1997 ejk
    switched to rangauss() random number generator 22-may-1997 ejk
    removed comma delimiters on input 5-july-1997 ejk
    remove option to set Fourier resolution and fix at max
    		25-sep-1997 ejk
    switch to temperature input on thermal vibrations
		3-oct-1997 ejk
	cosmetic changed to compile on MAC/codeWarrior 5-mar-1998 ejk

  This program is ANSI standard C and should be transportable

  This source code is formatted for a tab size of 8.

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

#define NAMAX	2000	/* max number of atoms of one type */
#define NSMAX	20	/* max number of symmetry operations */
#define NZMAX	103	/* max atomic number */
#define	NCMAX	132	/* characters per line to read */

int natom, nsx, nsy;  /* global vars for scamp() */
float *x, *y, *occ;

/* define subroutines defined at end of file */

void scamp( float, float, double*, double* );

void main() 
{
	int ix,iy, nx, ny, ixmid, iymid,  nx2, ny2,
		i, j, nsym, jj, iz, is, lwobble;
	int ncellx, ncelly, ncellz;
	long ncoeff, ltime;
	unsigned long iseed;

	char filein[NCMAX], fileot[NCMAX], cline[NCMAX];

	float **pix, *param, *kx, *ky, *wobble;
	float rmin, rmax;
	float *symx1, *symx2, *symy1, *symy2;
	double scampr, scampi, sum, runtime, fe, ky2, k2, k2max,
		scale, ax,by,cz,total1, total2, rx2, ry2,
		temperature, scalet;

	FILE *fp;

/*  the following are the chemical symbols for the periodic table */

        const char symbol[] = {
		" HHeLiBe B C N O FNeNaMgAlSi P SCl"
                "Ar KCaScTi VCrMnFeCoNiCuZnGaGeAsSeBr"
                "KrRbSr YZrNbMoTcRuRhPdAgCdInSnSbTe"
                " IXeCsBaLaCePrNdPmSmEuGdTbDyHoErTm"
                "YbLuHfTa WReOsIrPtAuHgTlPbBiPoAtRn"
                "FrRaAcThPa UNpPuAmCmBkCfEsFmMdNoLr"
	};

	/*  Echo version date */
        printf("atompot C-version dated 3-Oct-1997 EJK\n\n");

	/*  Get input file name etc. */
        printf("Name of file with input crystal data :\n");
        scanf("%s", filein);

        printf("Name of file to get binary output"
              " of atomic potential :\n");
        scanf("%s", fileot);

	do{
           printf("Real space dimensions in pixels Nx, Ny :\n");
           scanf("%d %d", &nx, &ny);

	   nx2 = (int) powerof2( (long) nx );
	   ny2 = (int) powerof2( (long) ny );
           if( (nx != nx2) || (ny != ny2) )
		printf(" Nx=%d, Ny=%d must be a power of 2, try again.\n",
		       nx, ny );
	} while(  (nx != nx2) || (ny != ny2) );

        printf("Replicate unit cell by NCELLX,NCELLY,NCELLZ :\n");
	scanf("%d %d %d", &ncellx, &ncelly, &ncellz);
	if( ncellx < 1 ) ncellx = 1;
	if( ncelly < 1 ) ncelly = 1;
	if( ncellz < 1 ) ncellz = 1;

/*  read in parameters for Monte Carlo displacements
      to simulate thermal motion 
    get the initial seed from the time counter if it exists
	otherwise ask user for a seed 
   - this should get a different random number sequence each time the
          program is run 
*/
	lwobble = askYN( "Do you want to add thermal displacements to atomic coord.?" );
	if( lwobble == 1 ) {
           printf("Temperature in degrees K:\n");
           scanf("%lg", &temperature);
           ltime = (long) time( NULL );
           iseed = (unsigned) ltime;
            if( ltime == -1 ) {
              printf("Type initial seed for random number generator:\n");
              scanf("%ld", &iseed);
           } else
              printf( "Random number seed initialized to %ld\n", iseed );
        }


/*  start timer */

	runtime = cputim();

/*  Start to read in specimen parameters
        remember to use fgets() in ReadLine() and NOT fscanf() 
        because fscanf() ignores newlines and 
        	we need to sync on whole lines
*/

        fp = fopen( filein,"r" );
	if( fp == NULL ) {
		perror(" can't open crystal data input file");
		exit(0 );
	}
	ReadLine( fp, cline, NCMAX, filein );
        sscanf( cline, "%lf %lf %lf", &ax, &by, &cz);
	printf("2D lattice constants= %f x %f Angstroms\n"
          " and propagation constant= %f Angstroms\n",
	       ax, by, cz );
        if( (ncellx > 1) || (ncelly > 1) || (ncellz > 1 ) ) {
          ax = ax * ncellx;
          by = by * ncelly;
          cz = cz * ncellz;
	  printf("Unit cell replicated to a= %g, b= %g, c=%g  Angstroms\n",
		 ax, by, cz );
	}

	/* read in symmetry operations */

	ReadLine( fp, cline, NCMAX, filein );
        sscanf( cline, "%d", &nsym);
	if( nsym > 0 ) {
	   symx1 = float1D( nsym, "symx1" );
	   symx2 = float1D( nsym, "symx2" );
	   symy1 = float1D( nsym, "symy1" );
	   symy2 = float1D( nsym, "symy2" );
	   for( i=0; i<nsym; i++)  {
		ReadLine( fp, cline, NCMAX, filein );
		sscanf( cline, "%f %f %f %f",
		  &symx1[i], &symx2[i], &symy1[i], &symy2[i]);
	   }
	}

/*  Calculate misc constants (Nx*Ny added because FFT2D
    performs scaling) also adjust k2max for circular symmetry
    remember that k2max is still in units of Angstroms
*/
	k2max = 2.0*ax/nx;
	scale = 2.0*by/ny;
        if( scale > k2max ) k2max =scale;

        printf("Maximum symmetrical resolution set to %g Angstroms\n",
	       k2max );
        k2max = 1.0 / (k2max * k2max);

        rx2 = (1.0/ax);  rx2 = rx2*rx2;
        ry2 = (1.0/by);  ry2 = ry2*ry2;
        scale = ( ((double)nx) * ((double)ny) ) /(ax*by);
        ixmid = nx/2;
        iymid = ny/2;

	ky = float1D( ny , "ky" );
	kx = float1D( nx , "kx" );

	for( iy=0; iy<ny; iy++) {
	   if( iy < iymid )  ky[iy] = (float) iy;
            else ky[iy] = (float)(iy-ny);
	}

	for( ix=0; ix<=ixmid; ix++) kx[ix] = (float) ix;

	pix = float2D( nx+2, ny, "pix" );
	for( ix=0; ix<(nx+2); ix++) 
		for( iy=0; iy<ny;iy++)  pix[ix][iy] = 0.0F;

	/* alloc misc arrays */

	x      = float1D( NAMAX, "x" );
	y      = float1D( NAMAX, "y" );
	occ    = float1D( NAMAX, "occ" );
	wobble = float1D( NAMAX, "wobble" );

	/*  read in actual coordinates */

	total2 = 0.0;
	printf("\n");
L130:
       ReadLine( fp, cline, NCMAX, filein );
       sscanf( cline, "%d", &iz);
       if( (strlen(cline) > 1) && (iz >= 1) && (iz <= NZMAX) ) {
          j = 0;
          total1 = 0.0;
	  while( ReadLine( fp, cline, NCMAX, filein ) > 2 ) {
	  	sscanf( cline, "%f %f %f %f", &occ[j], &x[j], &y[j], &wobble[j] );
          	total1 = total1 +  occ[j] * (nsym+1) * ncellx * ncelly;
          	if( nsym > 0) {
             		if( (j+nsym+1) > NAMAX) {
                   		printf("Too many atoms\n");
                   		printf("  Maximum allowed = %d\n",NAMAX);
                   		fclose( fp );
                   		exit(0);

	     		}
             		for( jj=0; jj<nsym; jj++) {
                   		occ[j+jj+1] = occ[j];
                    		wobble[j+jj+1] = wobble[j];
                    		x[j+jj+1] = (symx1[jj]*x[j] +symx2[jj] )/ncellx;
                     		y[j+jj+1] = (symy1[jj]*y[j] +symy2[jj] )/ncelly;
	     		}
	  	}
          	x[j] = x[j] /ncellx;
         	y[j] = y[j] /ncelly;
          	j = j + nsym + 1;
          	if( j > NAMAX ) {
              		printf("Too many atoms\n");
               		printf("  Maximum allowed = %d\n",NAMAX);
	       		fclose( fp );
               		exit( 0 );
	  	}
	   }  /* end while( ReadLine()>2 */

           natom = j ;

          is = 2*(iz-1);
          printf("%8.2f atoms with Z= %3d (%c%c)\n",
		  total1, iz, symbol[is], symbol[is+1]);

	/*   if thermal vibrations are requested then we must expand
		ncellx,y here otherwise factor it inside scamp() 
		(its faster)
	     add random displacements to all positions
	*/
          if( lwobble == 1 ) {
              if( (j-1)*ncellx*ncelly > NAMAX) {
                 printf("Too many atoms\n");
                 printf("  maximum allowed = %d\n",NAMAX);
	         exit( 0 );
	      }

              if( ncellx > 1) {
		 for( ix=1; ix<ncellx; ix++)
		    for( i=0; i<natom; i++) {
                       x[j] = x[i] + ((float)ix)/((float)ncellx);
                       y[j] = y[i];
		       wobble[j] = wobble[i];
                       occ[j++] = occ[i];
		  }
                 natom = j - 1;
	       }

              if( ncelly > 1) {
		 for( iy=1; iy<ncelly; iy++)
		    for( i=0; i<natom; i++) {
                       x[j] = x[i];
                       y[j] = y[i] + ((float)iy)/((float)ncelly);
		       wobble[j] = wobble[i];
                       occ[j++] = occ[i];
		    }
                 natom = j - 1;
	      }

	      /* scale thermal displacements to 300 deg. K 
		and normalize to 3D - integrating over the z direction */
	      scalet = sqrt( temperature / 300.0 );
	      for( i=0; i<natom; i++) {
                 x[i] = x[i] + 
			(float) (wobble[i] * scalet * rangauss( &iseed ) /ax);
                 y[i] = y[i] + 
			(float) (wobble[i] * scalet * rangauss( &iseed ) /by);
	      }

              nsx = 1;
              nsy = 1;

           } else {
              nsx = ncellx;
              nsy = ncelly;

	   }  /* end if( lwobble == TRUE ) */

/*  calculate scattering amplitude in left half plane
   NOTE zero freg is in the bottom left corner and
     expandes into all other corners - not in the center
     this is required for FFT, high freq is in the center

*/
            ncoeff = 0;
	    for( iy=0; iy<ny; iy++) {
                ky2 = ky[iy] * ky[iy] * ry2;
		for( ix=0; ix<=ixmid; ix++) {
                   k2 = kx[ix] * kx[ix] * rx2 + ky2;
                   if( k2 <= k2max) {
		      fe = featom( iz, k2 );
		      scamp( kx[ix], ky[iy], &scampr, &scampi ) ;
                      pix[2*ix  ][iy] += (float) (scampr * scale * fe);
                      pix[2*ix+1][iy] += (float) (scampi * scale * fe);
                      ncoeff++;
		   }

	       }  /* end for(ix */
	   } /* end for(iy */

           total2 = total2 + total1;
           goto L130;

        }  /* end top if( iz>= 1) */

	/*  end loop over types */

        printf("\n   for a grand total of %10.2f atoms\n", total2);
	fclose( fp );

	rfft2d( pix, nx, ny, -1);	/*  inverse fft */

	/*  output results and find min and max to echo */

	if( nsym > 0 ) {
		free( symx1 );
		free( symx2 );
		free( symy1 );
		free( symy2 );
	}
	param = float1D( 64, "param");
 
        rmin = rmax = pix[0][0];
        sum = 0.0;
	for( iy=0; iy<ny; iy++) {
	  for( ix=0; ix<nx; ix++) {
            sum += pix[ix][iy];
	    if( pix[ix][iy] < rmin ) rmin = pix[ix][iy];
	    if( pix[ix][iy] > rmax ) rmax = pix[ix][iy];
	  }
	}

	for( ix=0;ix<64; ix++) param[ix] = 0.0F;
        param[pRMAX]= rmax;
        param[pRMIN]= rmin;
        param[pC]= (float) cz;
        param[pRES]= (float) (1.0/sqrt(k2max));
        param[pDX]= (float) ax / ((float)nx);
        param[pDY]= (float) by / ((float)ny);

	if( tcreateFloatPixFile( fileot, pix, (long) nx, (long) ny,
			 1, param ) != 1 )
		printf( "atompot cannot write an output file.\n");

	printf(" pix range %g to %g\n", rmin, rmax);
	printf("%ld fourier coeff. calculated in right half plane\n",
	       ncoeff);
	printf("The average real space value was %g\n",
	       sum /(((float)nx)*((float)ny)));
	       
	printf("CPU time (excluding set-up) = %f sec.\n", cputim()-runtime );

}  /* end main() */

/*--------------------- scamp() -----------------------*/
/*
  Complex specimen scattering amplitude

  kx,ky = real scattering vectors
  scampr, scampi = real and imag parts of scatttering amplitude
                   returned by this routine

  global variables used:
  x[natom], y[natom] = real array with atomic coord.
  occ[natom] = real array with occupations at each x,y
  natom      = integer number of atoms (all with same Z)
  nsx, nsy = integer number of cells to replicate in x,y

*/
void scamp( float kx, float ky, double *scampr, double *scampi )
{
        int i, ixc, iyc;
        double pi2, w;
        float  scalex, scaley;

        double phasexr, phasexi, phaseyr, phaseyi;

	pi2 = 8.0 * atan( 1.0 );

        scalex = 1.0F/ ((float)nsx);
        scaley = 1.0F/ ((float)nsy);

        *scampr = *scampi = 0.0;

/*  use a trick to sum nsx + nsy terms
    instead of nsx * nsy
*/
	for( i=0; i<natom; i++) {

            phasexr = phasexi = 0.0;
	    for( ixc=0; ixc<nsx; ixc++) {
              w = pi2 * ( kx * ( x[i] + ixc*scalex ) );
              phasexr += cos(w);
              phasexi +=  sin(w);
	    }

            phaseyr = phaseyi = 0.0;
	    for( iyc=0; iyc<nsy; iyc++) {
              w = pi2 * ( ky * ( y[i] + iyc*scaley ) );
              phaseyr += cos(w);
              phaseyi += sin(w);
	    }

	    *scampr += ( phasexr*phaseyr - phasexi*phaseyi ) * occ[i];
	    *scampi += ( phasexr*phaseyi + phasexi*phaseyr ) * occ[i];

	} /* end for(i=0... */

        return;

 }  /* end scamp() */
