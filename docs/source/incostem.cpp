/*      incostem.cpp

------------------------------------------------------------------------
Copyright 1998-2013 Earl J. Kirkland

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

---------------------- NO WARRANTY ------------------
THIS PROGRAM IS PROVIDED AS-IS WITH ABSOLUTELY NO WARRANTY
OR GUARANTEE OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL THE AUTHOR BE LIABLE
FOR DAMAGES RESULTING FROM THE USE OR INABILITY TO USE THIS
PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA
BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR
THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH
ANY OTHER PROGRAM). 

------------------------------------------------------------------------

  calculate images in the incoherent STEM approximation

  put the partial cross section (integrated over the ADF detector
  angles) at a single pixels (position of the corresponding atom)
  and convolve with the point spread function (focused probe intensity)

  reference:

  [1] E. Kirkland, "Advanced Computing in Electron Microscopy",
        Plenum 1998, 2nd edit. Springer 2010
  
  this file is formatted for a tab size of 4 char

  started 16-mar-1998 E. Kirkland
  change float1D() etc to malloc1D() etc. and get rid of
     memory.h 18-jun-2007 ejk
  add cputim() echo 27-jun-2007 ejk
  put in adaptive quadrature for r and k integration 
        - faster and more accurate 28-jun-07 ejk
  add source size 13-jul-2008 ejk
  start adding defocus spread (Cc effects) 1-mar-2009 ejk
  add Cs5 on 7-mar-2009 ejk
  add Gauss-Hermite integration of defocus spread 15-mar-2009 ejk
  change wording on initial dialog 14-nov-2009 ejk
  fix bug in FWHM conversion (factor of two error) 13-nov-2010 ejk
  start convert to FFTW  8-feb-2011 ejk
  adding many aberration mode (C32a etc)  13-feb-2011
  get return value of scanf() to remove warnings from gcc 4.4
      13-feb-2011 ejk
  convert many aberration mode to use chi() from slicelib
      and fix a few small typos 21-may-2011 ejk
  convert &lf to &lg reading multipole aberration and fix
      small typo in comments 20-jun-2011 ejk
  fix small typo's in comments and formatting 27-jul-2011 ejk
  convert to C++ and floatTIFF.cpp  14-aug-2012 ejk
  convert to cfpix/fftw class from raw fftw 26-feb-2013 to 30-oct-2012 ejk
  fix typo in final param[] saving loop  17-mar-2013 ejk 
  fix typo in final param[] updates - keep existing aberr.
     and get rid of redundant variable wavelen (use wavl) 30-mar-2013 ejk
*/

#include <stdio.h>  /* standard ANSI libraries */
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cfpix.hpp"       /* complex image handler with FFT */
#include "slicelib.hpp"    /* misc. routines for multislice */
#include "floatTIFF.hpp"   /* file I/O routines in TIFF format */

#define MANY_ABERR      /*  define to include many aberrations */

#define NZMAX 103   /* max number of atomic numbers Z */
#define NCMAX 512   /* max number of characters in read buffer */

#define NZMAX   103 /* max atomic number Z */

/*  some global constants */
double twopi, sigmae, wavl;
float *kx, *ky, *xpos, *ypos, *kx2, *ky2;

/*  define subroutines at bottom of file */
int makeProbe( cfpix &cpix, int nx, int ny,
          double xp, double yp, float p[], 
          double wavlen, double k2max, double pixel, int multiMode,
          int ismoth );
double prbSize( float** pixsq, int nx, int ny,
        double xp, double yp, double ax, double by );
void invert2D( float** pix, long nx, long ny );

/* define routines at end of file */
double atomf( double t, double p[] );
double atomsignal( int zatom, double keV, double thetamin, double thetamax );
double BJ0( double x );
void feMoliere( double k, int zatom, double *rfe, double *ife );
double femi( double r, double p[] );
double femr( double r, double p[] );
double integrate45( double (*fint)(double x, double p[]), double p[],
    double xmin, double xmax, double maxerror, int maxsteps );

/*  absiccas and weights for Gauss-Hermite Quadrature 
   with exp(-x*x) weighting of integrand 
   from Abramowitz and Stegun, and Numerical Recipes */
const int NGH=9;   /* number of Gauss-Hermete coeff. to use  */
double xGH[]={ 3.190993201781528, 2.266580584531843, 1.468553289216668,
    0.723551018752838, 0.000000000000000, -0.723551018752838,
    -1.468553289216668,-2.266580584531843,-3.190993201781528};
double wGH[]={3.960697726326e-005, 4.943624275537e-003 ,8.847452739438e-002,
    4.326515590026e-001, 7.202352156061e-001, 4.326515590026e-001,
    8.847452739438e-002, 4.943624275537e-003, 3.960697726326e-005};


int main()
{
    char filein[NCMAX], fileout[NCMAX], description[NCMAX],
        cline[NCMAX];

    int ix, iy, nx, ny, ixmid, iymid, i, ncellx, ncelly,
        ncellz, ndf, idf, done, natom, *Znum, ns, ismoth,
        npixels, multiMode=0, status, NPARAM;
    long nxl, nyl;

    float *x, *y, *z, *occ, *wobble, *atoms;
   
    float wmin, wmax, xmin,xmax, ymin, ymax, zmin, zmax;

    float **pixsq, *param, **ctf;

    float tr, ti, wr, wi, k2, scale, rx, ry, fdx, fdy,
        ax, by, cz, rmin, rmax, aimin, aimax, weight;
    
    double  rx2, ry2, sum, pixel, xp, yp, vz,
        Cs3, Cs5, df, df0, ddf, ddf2, aobj, k2max, dx, dy;

    double  keV, thetamin, thetamax, timer, dsource, ds, ds2,
        pi;

    cfpix trans, pix;  //  complex floating point images

    floatTIFF myFile;  // file handler

/* ------ echo version date and get input file name ---------- */

    printf( "incostem version dated 30-mar-2013 ejk\n"
        "calculate ADF-STEM images for very thin specimens\n"
        "using the incoherent image model\n" );
    printf("Copyright (C) 1998-2013 Earl J. Kirkland\n" );
    printf( "This program is provided AS-IS with ABSOLUTELY NO WARRANTY\n "
        " under the GNU general public license\n\n" );

    NPARAM = myFile.maxParam();
    pi = (float) (4.0 * atan( 1.0 ));

    param = (float*) malloc1D( NPARAM, sizeof(float), "param" );
    for( i=0; i<NPARAM; i++) param[i] = 0.0f;

    printf("Name of file with input atomic "
           "potential in x,y,z format:\n");
    ns = scanf("%s", filein );

/* ------ get simulation options ------ */

    printf("Replicate unit cell by NCELLX,NCELLY,NCELLZ :\n");
    scanf("%d %d %d", &ncellx, &ncelly, &ncellz);
    if( ncellx < 1 ) ncellx = 1;
    if( ncelly < 1 ) ncelly = 1;
    if( ncellz < 1 ) ncellz = 1;

    printf("Name of file to get binary output of adf-stem result:\n");
    ns = scanf("%s", fileout );

    /*  any size works with FFTW so don't need to force power of 2 */ 
    printf("Image size, Nx,Ny in pixels : \n");
    ns = scanf("%d %d", &nx, &ny);

    printf("STEM probe parameters, V0(kv), Cs3(mm), Cs5(mm),"
           " df(Angstroms), apert(mrad):\n");
    ns = scanf("%lg %lg %lg %lg %lg", &keV, &Cs3, &Cs5, &df0, &aobj);
    aobj *= 0.001;
    param[pDEFOCUS] = (float) df0;
    param[pCS]  = (float) ( Cs3*1.0e7 );
    param[pCS5] = (float) ( Cs5*1.0e7 );

    printf("ADF detector angles thetamin, thetamax (in mrad) =\n");
    ns = scanf("%lf %lf", &thetamin, &thetamax);
    thetamin *= 0.001;
    thetamax *= 0.001;

    /*   get higher order aberrations if necessary */
    done = multiMode = 0;
#ifdef MANY_ABERR
    /*   get higher order aberrations if necessary */
    printf("type higher order aber. name (as C32a, etc.) followed\n"
        " by a value in mm. (END to end)\n");
    do{
        ns = scanf( "%20s", cline );
        if( strstr( cline, "END" ) != NULL ) {
            done = 1;
        } else {
            ns = scanf( "%lg", &vz );
            status = readCnm( cline, param, vz );        
            if( status < 0 ) {
                printf( "unrecognized aberration, exit...\n");
                exit( EXIT_SUCCESS );
            } else multiMode = 1;
        }
    } while( !done );
#endif

    printf( "Source size at specimen (FWHM in Ang.):\n");
    ns = scanf( "%lf", &dsource);

    printf( "Defocus spread (FWHM in Ang.):\n");
    ns = scanf( "%lf", &ddf);
    
    /* start timing the actual computation just for fun */
    timer = cputim();

/*-------- global constants ----------- */
/* ------ calculate relativistic electron wavelength ------ */

    twopi = 2.0 * 4.0 * atan( 1.0 );
    wavl = wavelength( keV );
    sigmae = sigma( keV )/ 1000.0;

    printf("electron wavelength = %g Angstroms\n", wavl );

/* ------ read in specimen coordinates and scattering factors ------ */

    natom = ReadXYZcoord( filein, ncellx, ncelly, ncellz,
        &ax, &by, &cz, &Znum, &x, &y, &z, &occ, &wobble,
        description, NCMAX );

    printf("%d atomic coordinates read in\n", natom );
    printf("%s", description );

    printf("Lattice constant a= %12.4f, b= %12.4f\n", ax, by);

    /*  calculate the total specimen volume and echo */
    xmin = xmax = x[0];
    ymin = ymax = y[0];
    zmin = zmax = z[0];
    wmin = wmax = wobble[0];

    for( i=0; i<natom; i++) {
        if( x[i] < xmin ) xmin = x[i];
        if( x[i] > xmax ) xmax = x[i];
        if( y[i] < ymin ) ymin = y[i];
        if( y[i] > ymax ) ymax = y[i];
        if( z[i] < zmin ) zmin = z[i];
        if( z[i] > zmax ) zmax = z[i];
        if( wobble[i] < wmin ) wmin = wobble[i];
        if( wobble[i] > wmax ) wmax = wobble[i];
    }
    printf("Total specimen range is\n %g to %g in x\n"
           " %g to %g in y\n %g to %g in z\n", xmin, xmax,
           ymin, ymax, zmin, zmax );

/* ------  calculate spatial frequencies and positions for future use ------ */

    rx = 1.0F/ax;
    rx2= rx*rx;
    ry = 1.0F/by;
    ry2= ry*ry;
    ixmid = nx/2;
    iymid = ny/2;
    nxl = nx;
    nyl = ny;

    kx   = (float*) malloc1D( nx, sizeof(float), "kx" );
    kx2  = (float*) malloc1D( nx, sizeof(float), "kx2" );
    xpos = (float*) malloc1D( nx, sizeof(float), "xpos" );
    freqn( kx, kx2, xpos, nx, ax );

    ky   = (float*) malloc1D( ny, sizeof(float), "ky" );
    ky2  = (float*) malloc1D( ny, sizeof(float), "ky2" );
    ypos = (float*) malloc1D( ny, sizeof(float), "ypos" );
    freqn( ky, ky2, ypos, ny, by );

    /*------  make FFTW arrays and plans ------- */
    trans.resize( nx, ny );
    trans.init( 1 );

/* ------  Calculate the transfer function ------  */

    printf("calculate the transfer function...\n");
    ctf = (float**) malloc2D( nx, ny, sizeof(float), "ctf" );
    pixsq = (float**) malloc2D( nx, ny, sizeof(float), "pixsq" );

    k2max = aobj / wavl;
    k2max = k2max * k2max;
    dx = ( ax/((float)nx) );
    dy = ( by/((float)ny) );

    if( ddf > 1.0 ) {       /*  df integration parameters */
        ndf = NGH;
        /* ddf2 = sqrt(log(2.0)/(ddf*ddf));   convert from HWHM */
        ddf2 = sqrt(log(2.0)/(ddf*ddf/4.0));  /* convert from FWHM */
    } else {
        ndf = 1;
        ddf2 = 0.0;
    }

    for( ix=0; ix<nx; ix++)
    for( iy=0; iy<ny; iy++) ctf[ix][iy] = 0.0;

    xp = 0.0;  /* probe position */
    yp = 0.0;
    pixel = 0.0;  /* smoothing size = not used */
    ismoth = 0;

    /*---- integrate over defocus spread if ddf is large enough ----*/
    /* use Gauss-Hermite quadrature and convert exp(-a^2df^2) to exp(-u^2)  */
    for( idf=0; idf<ndf; idf++) {

        if( ndf > 1 ){
                df = df0 + xGH[idf]/ddf2;
                weight = (float) wGH[idf];
        }  else {
                df = df0;
                weight = 1.0;
        }
        /*  diagnostic */
        printf("df step %d, df= %g, weight= %g\n", idf, df, weight );
            
        param[pDEFOCUS] = (float) df;
            
        /* --------- calculate probe wavefunction -------- */
        npixels = makeProbe( trans, nx, ny, xp, yp, 
            param, wavl, k2max, pixel, multiMode, ismoth );

        /* -----  normalize and save probe intensity ----- */
        sum = 0.0;
        for( ix=0; ix<nx; ix++)
        for( iy=0; iy<ny; iy++) {
            tr = trans.re(ix,iy); 
            ti = trans.im(ix,iy);
            pixsq[ix][iy] = tr*tr + ti*ti;
            sum +=  pixsq[ix][iy];
        }
        
        /* ----- Normalize probe intensity to unity and add weight ------------ */
        scale = weight * ((float)sqrt( 1.0 / sum ) ); 
        
        for( ix=0; ix<nx; ix++) 
           for( iy=0; iy<ny; iy++) {
            ctf[ix][iy] +=  scale * pixsq[ix][iy];
        }

    }  /*  end for(idf...)  */

    sum = 0.0;
    for( ix=0; ix<nx; ix++)     /* find integrated intensity */
    for( iy=0; iy<ny; iy++)  sum += ctf[ix][iy];

    scale = (float) (1.0 / (dx*dy*sum) );
    for( ix=0; ix<nx; ix++)     /* normalize integrated intensity */
    for( iy=0; iy<ny; iy++) {
        trans.re(ix,iy) = scale * ctf[ix][iy];
        trans.im(ix,iy) = 0.0F;
    }

    /*  find probe size just for fun: 
      remember that prbSize() doesn't handle wrap-around properly so
      move probe to the center first */
    invert2D( ctf, nx, ny );
    xp = 0.5*ax;
    yp = 0.5*by;
    sum = prbSize( ctf, nx, ny, xp, yp, ax, by );  /* about orig center */
    printf("probe size (FWHM-II) = %g Ang.\n", sum);

    /* -----  make final transfer function ----- */
    trans.fft();

/*------  allocate scratch arrays and cross section look-up-table array */

    atoms = (float*) malloc1D( NZMAX+1, sizeof(float), "atoms" );   /* cross-section LUT */
    for( i=0; i<=NZMAX; i++) atoms[i] = -1.0F;

/* ------  Calculate 2D distribution of partial cross sections in specimen 
    use look-up-table for cross sections because it takes
    a lot of CPU cycles to calculate ------ */

    printf("calculate the 2D specimen function...\n");
    pix.resize( nx, ny );
    pix.copyInit( trans );

    for( ix=0; ix<nx; ix++)
    for( iy=0; iy<ny; iy++)
        pix.re(ix,iy) = pix.im(ix,iy) = 0.0F;

    for( i=0; i<natom; i++) {
        ix = (int) ( nx*x[i]/ax );      /* make sure coord. is inside image */
        while( ix < 0 ) ix = ix + nx;
        ix = ix % nx;

        iy = (int) ( ny*y[i]/by );
        while( iy < 0 ) iy = iy + ny;
        iy = iy % ny;

        if( atoms[Znum[i]] < 0.0F ) {
            atoms[Znum[i]] = (float)
            atomsignal( Znum[i], keV, thetamin, thetamax);
            printf( "the partial cross section for Z = %3d,"
                " is %g sq-Ang\n", Znum[i], atoms[Znum[i]] );
        }

        pix.re(ix,iy) += atoms[Znum[i]];
    }

/*------   Convolve specimen function with transfer function ------ */
/*  NOTE: could do this faster because both the psf and the image are real
      - use real to complex FFT 28-jun-2007 ejk */

    /* fft2d( pixr, pixi, nx, ny, +1); */
    pix.fft();

    dsource = 0.5* dsource;   /* convert diameter to radius */
    ds = pi*pi * dsource*dsource/log(2.0);  /* source size factor- convert to FWHM*/
    for( ix=0; ix<nx; ix++)
    for( iy=0; iy<ny; iy++) {
        k2 = kx2[ix] + ky2[iy];
        if( k2 <= 4.0*k2max ) {
            ds2 = exp( -ds*k2 );
            tr = trans.re(ix,iy);
            ti = trans.im(ix,iy);
            wr = pix.re(ix,iy);
            wi = pix.im(ix,iy);
            pix.re(ix,iy) = (float) ( (tr*wr - ti*wi) * ds2 );
            pix.im(ix,iy) = (float) ( (ti*wr + tr*wi) * ds2 );
        } else {
            pix.re(ix,iy) = 0;
            pix.im(ix,iy) = 0;
        }
    }
    /* fft2d( pixr, pixi, nx, ny, -1);  */
    pix.ifft();

    /*   find pix range and move to old style real array for output */
    pix.findRange( rmin, rmax, aimin, aimax );
    for( ix=0; ix<nx; ix++)
    for( iy=0; iy<ny; iy++) {
        ctf[ix][iy] = pix.re(ix,iy);         // save real part for output
    }

    timer = cputim() - timer;
    printf("CPU time = %g sec.\n", timer);

    printf( "final pix range: %g to %g (real)\n"
        "          %g to %g (imag)\n",
        rmin, rmax, aimin, aimax);

    //  update a extra parameters - remember that most aberrations already set
    param[pRMAX]  = rmax;
    param[pIMAX]  = aimax;
    param[pRMIN]  = rmin;
    param[pIMIN]  = aimin;
    param[pDEFOCUS] = (float) df0;
    param[pDDF] = (float) ddf;
    param[pWAVEL] = (float) wavl;
    param[pOAPERT] = (float) aobj;
    param[pENERGY] = (float) keV;
    param[pDX] = fdx = (float) ( ax/((float)nx) );
    param[pDY] = fdy = (float) ( by/((float)ny) );
    param[pMINDET] = (float) thetamin;
    param[pMAXDET] = (float) thetamin;

/*------  write the results into a file ------ */

    for( i=0; i<NPARAM; i++ ) myFile.setParam( i, param[i]);

    printf( "output only the real part\n");

    myFile.resize( nx, ny );
    myFile.setnpix( 1 );
    for( ix=0; ix<nx; ix++) for( iy=0; iy<ny; iy++)
        myFile(ix,iy) = ctf[ix][iy];
    i = myFile.write( fileout, rmin, rmax, aimin, aimax, fdx, fdy );

    if( i != 1 ) printf( "incostem cannot write TIF file %s\n",
            fileout );


#ifdef footest

    double  rfe, ife, k, k2max;
    
    FILE *fp;

/* ------  quick test ------ */

    k = 0.0;
    za = 6;
    keV = 200.0;
    feMoliere( k, za, keV, &rfe, &ife );
    printf( "fe = %g, %g\n", rfe, ife );

/*------  output single atom cross section vs atomic number Z  ------ */

    printf( "write atomcs.dat...\n" );
    fp = fopen( "atomcs.dat", "w+" );

    keV = 200.0;
    wavl = wavelength( keV );
    thetamin = 0.050;
    thetamax = 0.200;
    for( za=1; za<=NZMAX; za++) {
        signal = atomsignal( za, keV, thetamin, thetamax );
        printf( "%5d    %g\n", za, signal );
        fprintf( fp, "%5d    %g\n", za, signal );
    }
    fclose( fp );
#endif

    printf( "all done\n" );

}  /* end main() */


/*-------------------- atomsignal() ------------------------------
/*
    calculate single atom partial cross section 
    integrated from kmin to kmax (i.e. the ADF detector)
    (does NOT include objective aberration function)

    zatom = atomic number Z
    keV   = beam energy in keV
    thetamin  = minimum scattering angle in radians
    thetamax  = maximum scattering angle in radians

    the returned value is the scattering cross section
    in square-Angstroms integrated between thetamin and thetamax

    assumed global: temp2[]
*/
double atomsignal( int zatom, double keV, double thetamin, double thetamax )
{   

    double tol, t2, p[3];
    int maxsteps;
    
    tol = 1.0e-5;       /* only need approx. answer here */
    maxsteps = 10000;
    p[0] = zatom;

    /*  this requires that integrate45() be safe to call recursively */

    t2 = twopi * integrate45( atomf, p, thetamin, thetamax, tol, maxsteps );

    return( t2 );

} /* end atomsignal() */

/*-------------------- atomf() ------------------------------
/*
    dummy function for atomsignal() to integrate

    t = scatt. angl. (in rad.)

    p[0] = zatom = atomic number Z

    assumed global constants: twopi, wavl
*/
double atomf( double t, double p[] )
{   
    int zatom;
    double k, rfe, ife, t1;

    zatom = (int)( p[0] + 0.1);
    k = t / wavl;

    feMoliere( k, zatom, &rfe, &ife );
    t1 = t * (rfe*rfe + ife*ife);  /* theta*fe^2 */

    return( t1 );

} /* end atomf() */


/*---------------------------- BJ0 ----------------------------*/
/*
  real function to return zeroth order Bessel function of the argument
   using the polynomial expression given on page 369 of
       Abromowitz and Stegum
*/
double BJ0( double x )
{
    int i;
    double x3, p0, f0, y0, xa;
    
    static double p[] = {1.0, -2.2499997,  1.2656208, -0.3163866,
                   0.0444479, -0.0039444, 0.0002100 };
    static double f[] = { 0.79788456, -0.00000077, -0.00552740,
        -0.00009512,  0.00137237, -0.00072805, 0.00014476 };
    static double y[]  = {  -0.78539816,  -0.04166397, -0.00003954,
         0.00262573,  -0.00054125, -0.00029333, 0.00013558 };

    xa = fabs( x );
    if( xa < 3.0 ) {    
        x3 = xa/ 3.0;   x3 = x3 * x3;   p0 = p[6];
        for( i=5; i>=0; i=i-1)  p0 = p0 * x3 + p[i];
        return( p0 );
    } else {
        x3 = 3.0/xa;    f0 = f[6];  y0 = y[6];
        for(i=5; i>=0; i=i-1) {
            y0 = y0 * x3 + y[i];
            f0 = f0 * x3 + f[i];
        }
        y0 = xa + y0;
        return( f0*cos(y0)/sqrt( xa ) );
    } 

}  /* end BJ0 */

/*-------------------- feMoliere() ------------------------------
/*
    calculate complex electron scattering factor in Moliere approx
    from the projected atomic potential Vz(r)

    as in eq. 5.18 of [1]

    k = wave vector magnitude in 1/Ang
    zatom = atomic number Z
    keV = beam energy in keV

    rfe,ife = (real,image) scattering factor in Angstroms

    assumed global constants: twopi, sigmae, wavl
*/
void feMoliere( double k, int zatom, double *rfe, double *ife )
{   
    int maxsteps;
    double x, rmin=1.0e-5, rmax=6.0, tol, p[3];

    x = twopi / wavl;

    tol = 1.0e-5;       /* only need approx. answer here */
    maxsteps = 10000;
    p[0] = k;
    p[1] = zatom;

    *rfe = x * integrate45( femr, p, rmin, rmax, tol, maxsteps );
    *ife = x * integrate45( femi, p, rmin, rmax, tol, maxsteps );

    return;

} /* end feMoliere() */

/*-------------------- femi() ------------------------------
/*
    dummy function for feMoliere() to integrate
    imag. part of integrand

    r = radial coord. (in Ang.)

    p[0] = k = wave vector magnitude in 1/Ang
    p[1] = zatom = atomic number Z

    assumed global constants: twopi, sigmae
*/
double femi( double r, double p[] )
{   
    int zatom;
    double k, t1, t2;

    k = p[0];
    zatom = (int)( p[1] + 0.1);

    t1 = r * BJ0( twopi * k * r );
    t2 = sigmae * vzatom( zatom, r );

    return( t1 * ( 1.0 - cos( t2 ) ) );

} /* end femi() */


/*-------------------- femr() ------------------------------
/*
    dummy function for feMoliere() to integrate
    real part of integrand

    r = radial coord. (in Ang.)

    p[0] = k = wave vector magnitude in 1/Ang
    p[1] = zatom = atomic number Z

    assumed global constants: twopi, sigmae
*/
double femr( double r, double p[] )
{   
    int zatom;
    double k, t1, t2;

    k = p[0];
    zatom = (int)( p[1] + 0.1);

    t1 = r * BJ0( twopi * k * r );
    t2 = sigmae * vzatom( zatom, r );

    return( t1 * sin( t2 ) );

} /* end femr() */



/*---------------------------- integrate45() ----------------------------*/
/*
    adaptive quadrature using a simplified auto step size
    5th order Runge-Kutta (i.e. the function only depends on x and not y)

    integrate f(x) from x=xmin to x=xmax by solving the ODE
    dy/dx = f(x) with initial condition y(xmin)=0

   NOTE: The value of the constant SMALL assumes that f(x) is of order
     unity (1). If this is not true then you should change the value
     of SMALL (must be positive).

    fint(x,p[]) = function to integrate, x=independent variable and
            p[] is a parameter array to pass to fint()
    p[]     = parameter array to pass to fint()
    xmin,xmax   = range of integration
    maxerror    = maximum allowable error at each step
    maxsteps    = maximum number of steps
    
    Auto Step Size 5h order Runge-Kutta function with embedded
    4th order Runge-Kutta using the 
    Cash-Karp coefficients as described in Section 16.2 of
    Numerical Recipes 2nd edit. by Press et al
    
    started 22-jul-2002 E. Kirkland
    fixed various special case failures 23-jul-2002 ejk 
*/
double integrate45( double (*fint)(double x, double p[]), double p[],
    double xmin, double xmax, double maxerror, int maxsteps )
{
    int iter;
    double k1, k2, k3, k4, k5, k6, y2;
    double  h, x, dx, delta, yfinish, scale;
    const double SMALL=1.0e-40;

    //--- the Cash-Karp Runge-Kutta coefficients ----
    static const double a2=1.0/5.0, a3=3.0/10.0, a4=3.0/5.0, a5=1.0, a6=7.0/8.0;
    static const double c1=37.0/378.0, c3=250.0/621.0, c4=125.0/594.0, c6=512.0/1771.0;
    static const double cs1=2825.0/27648.0, cs3=18575.0/48384.0,
            cs4=13525.0/55296.0, cs5=277.0/14336.0, cs6=1.0/4.0;

    iter = 0;

    h = (xmax - xmin)/5.0;      //  for lack of a better start step size
    x = xmin;

    yfinish = 0.0;

    while( (x < xmax) && (iter < maxsteps) ) {
        k1 = fint( x, p );
        scale = fabs(yfinish) + fabs(h*k1);
        if( scale < SMALL ) scale = SMALL; // in case of zero starting point

        do {
            dx = h;
            iter += 1;
            if( iter >= maxsteps) printf( 
                "Warning maxiter exceeded in integrate45()!\n");

            k2 = fint( x+a2*h, p );
            k3 = fint( x+a3*h, p );
            k4 = fint( x+a4*h, p );
            k5 = fint( x+   h, p );
            k6 = fint( x+a6*h, p );

            /* the next point */
            y2 = h * ( c1*k1 + c3*k3 + c4*k4 + c6*k6 );

            /* the error term */
            delta = y2 - h * ( cs1*k1 + cs3*k3 + cs4*k4 + cs5*k5 + cs6*k6 );

            delta = fabs( delta/scale );
            
            if( (delta < 0.5*maxerror) && ( delta> SMALL)  ) 
                h = h * pow( fabs(maxerror/delta), 0.2 );
            else if( delta > maxerror ) h = h/2.0;

        } while ( delta > maxerror );
        
        if( fabs(h) < SMALL*fabs(x) )    // guard against h->0
            h = SMALL * fabs(x) * h/fabs(h);

        x = x + dx; // large round off error here but there is no other way
        if( (x+h) > xmax ) h = xmax - x;  // don't go past the end
        yfinish = y2 + yfinish;
    }

    return( yfinish );
    
}  /* end integrate45() */

/*--------- below are subroutines from probe.c Jan 2011 ejk----------------- */

/*   ------------------ makeProbe() ----------------------
    calculate aberration limited probe wavefunction

    NOTE zero freq. is in the bottom left corner and
    expands into all other corners - not in the center
    this is required for FFT

  input:
    cpix[] = fftw array to get wave function
    nx, ny = size of wavefunciton in pixels
    xp, yp = probe position in Ang.
    p[]    = aberration values
    k2max  = max k^2
    pixel  = smoothing range
    multiMode = if not 0 then include multipole aberrations
    ismoth = flag to control smoothing at the edge

  returned value = number of pixels in the aperture

  assumed globals:
      planeTi
      kx[], kx2[], xpos[], ky[], ky2[], ypos[]

*/

int makeProbe( cfpix &cpix, int nx, int ny,
          double xp, double yp, float p[], 
          double wavlen, double k2max, double pixel, int multiMode,
          int ismoth )
{ 
    int ix, iy, npixels;
    double alx, aly, k2, chi0, pi, dx2p, dy2p;

    /*    PIXEL = diagonal width of pixel squared
        if a pixel is on the aperture boundary give it a weight
        of 1/2 otherwise 1 or 0
    */
    npixels = 0;
    pi = 4.0 * atan( 1.0 );

    dx2p = 2.0*pi*xp;
    dy2p = 2.0*pi*yp;

    for( iy=0; iy<ny; iy++) {
        aly = wavlen * ky[iy];  /* y component of angle alpha */
        for( ix=0; ix<nx; ix++) {
            k2 = kx2[ix] + ky2[iy];
            alx = wavlen * kx[ix];  /* x component of angle alpha */
            if ( ( ismoth != 0) && 
                ( fabs(k2-k2max) <= pixel) ) {
                   chi0 = (2.0*pi/wavlen) * chi( p, alx, aly, multiMode )
                           - ( (dx2p*kx[ix]) + (dy2p*ky[iy]) );
                cpix.re(ix,iy) = (float) ( 0.5 * cos(chi0));  /* real */
                cpix.im(ix,iy) = (float) (-0.5 * sin(chi0));  /* imag */
                /* printf("smooth by 0.5 at ix=%d, iy=%d\n", ix, iy ); */
            } else if ( k2 <= k2max ) {
                   chi0 = (2.0*pi/wavlen) * chi( p, alx, aly, multiMode )
                           - ( (dx2p*kx[ix]) + (dy2p*ky[iy]) );
                cpix.re(ix,iy) = (float)  cos(chi0);  /* real */
                cpix.im(ix,iy) = (float) -sin(chi0);  /* imag */
                npixels++;
            } else {
                cpix.re(ix,iy) = cpix.im(ix,iy) = 0.0F;
            }
        }  /* end for( ix=0... )  */
    }  /* end for( iy=0... )  */


/* ------- inverse transform back to real space ------------ */

    cpix.ifft();

    return( npixels );

}  /* end makeProbe()  */

/* -------------------  prbsSize() -------------------
   calculate probe size 

  input:
     pixsq[][] = intensity in probe
         nx, ny = size of probe in pixels
         xp, yp = original probe position (in Ang.)
         ax, by = size of cell in Ang.

  return value:  size of probe in Ang.

*/
double prbSize( float** pixsq, int nx, int ny,
        double xp, double yp, double ax, double by )
{
        int ix, iy, ir, nr;
        double *ivsr, dr, scale, rx, ry, x, y, y2;

    dr = ax/nx;
    scale = by/ny;
    if( scale < dr ) dr = scale;  /*  smallest radial spacing */
    dr = 0.5*dr;                  /*  use sub pixel sampling */
    if( ny > nx ) nr = ny;
    else nr = nx;
    nr = 2*nr;
    ivsr = (double*) malloc1D( nr, sizeof(double), "ivsr");
    for( ir=0; ir<nr; ir++) ivsr[ir] = 0.0;

    /*  find curent vs. radius = azimuthal average */
    /*    orig. probe position = (dx,dy)  */
    rx = ax/nx;
    ry = by/ny;
    for( iy=0; iy<ny; iy++) {
        y = (iy*ry) - yp;
                y2 = y*y;
        for( ix=0; ix<nx; ix++) {
            scale = pixsq[ix][iy];
            x = (ix*rx) - xp;
            ir = (int)( sqrt( x*x + y2 )/dr + 0.5);
            if( ir < 0 ) ir = 0;
            if( ir >= nr ) ir = nr-1;
            ivsr[ir] += scale;
       }
    }

    /*  integrate current */
    for( ir=1; ir<nr; ir++) ivsr[ir] += ivsr[ir-1];

    /*  find half current radius */
    scale = 0.5 * ivsr[nr-1];
    for( ir=0; ir<nr; ir++) {
        ix = ir;
        if( ivsr[ir] > scale ) break;
    }

    y = ivsr[ix] - ivsr[ix-1];   /* interpolate */
    if( fabs(y) > 0.0 ) y = (scale-ivsr[ix-1])/y;
    else y = 0.0;
    x = 2.0 * ( (ix-1)*dr + y*dr );

    free( ivsr );

    return( x );

}  /*  end prbSize() */

/*----- below is from fft2dc.c ----------------------------- */
/*------------------------- invert2D() ----------------------*/
/*
        rearrange pix with corners moved to center (a la FFT's)

         pix[ix][iy] = real array with image
         nx,ny = range of image 0<ix<(nx-1) and 0<iy<(ny-1)

*/
void invert2D( float** pix, long nx, long ny )
{
#define SWAP(a,b)       {t=a; a=b; b=t;}

        long ix, iy, ixmid, iymid;
        float t;

        ixmid = nx/2;
        iymid = ny/2;

        for( ix=0; ix<nx; ix++) 
        for( iy=0; iy<iymid; iy++)
                SWAP( pix[ix][iy], pix[ix][iy+iymid] );

        for( ix=0; ix<ixmid; ix++) 
        for( iy=0; iy<ny; iy++)
                SWAP( pix[ix][iy], pix[ix+ixmid][iy] );

#undef SWAP
}

