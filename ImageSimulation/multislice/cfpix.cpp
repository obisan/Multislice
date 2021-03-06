/*              *** cfpix.cpp ***

 ------------------------------------------------------------------------
Copyright 2012 Earl J. Kirkland

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

   C++ class to manage complex floating point images (in ANSI-C++)

   collect (isolate) the FFT details here so it can be easily changed in future

   NOTE: the complex to real portion is only partially implemented because
         it is not used much - should fix this some time

The source code is formatted for a tab size of 4.

----------------------------------------------------------
The public member functions are:

nx()       : return x size of current image (in pixels)
ny()       : return y size of current image (in pixels)

re(ix,iy)  : return reference to real part of pixel at (ix,iy)
im(ix,iy)  : return reference to imag part of pixel at (ix,iy)

rre(ix,iy) : return reference to pixel at (ix,iy) of the
             real image for complex to real transform

fft()      :  perform FFT
ifft()     : inverse FFT

init()     : perform initialization
copyInit() : copy the initialization

resize()        : resize current in-memory image (current data may be lost)
findRange()     : find range of values in complex image

operator*=() 
operator+=()

----------------------------------------------------------

   started 9-june-2012 E. Kirkland
   move invert2D() to cfpix 21-nov-2012 ejk
   last modified 21-nov-2012 ejk
*/

#include "cfpix.hpp"    // class definition + inline functions here

//------------------ constructor --------------------------------

cfpix::cfpix( int nx, int ny )
{
    nxl = nyl = nrxyl = 0;

    initLevel = -1;   // negative to indicate no initialization

    if( (nx > 0 ) && ( ny > 0 ) ) resize( nx, ny );

}  // end cfpix::cfpix()

//------------------ destructor ---------------------------------
cfpix::~cfpix()
{
    if( (nxl>0) && (nyl>0) ) fftwf_free( data );

    if( 2 == initLevel ) fftwf_free( rpix );

    nxl = nyl = nrxyl = 0;
    initLevel = -1;

}  // end cfpix::~cfpix()

//------------------ copyInit ---------------------------------
void cfpix::copyInit( cfpix &xx)
{
    if( (nxl != xx.nxl) && (nyl != xx.nyl) ) return;

    planTf = xx.planTf;
    planTi = xx.planTi;

    initLevel = xx.initLevel;

    //????   must allocate real array here is needed ????

}  // end cfpix::copyInit()


//------------------ forward transfrom ---------------------------------
void cfpix::fft()
{
    if( (0 == initLevel) || (1 == initLevel) ) {
        fftwf_execute_dft( planTf, data, data );
    } else {
        printf( "error: cfpix::fft() called before init()\n");
    }

}  // end cfpix::fft()

//------------------ inverse transfrom ---------------------------------
void cfpix::ifft()
{
    int ix, iy, j, nx, ny;
    float scale;

    // complex to complex
    if( (0 == initLevel) || (1 == initLevel) ) {
        fftwf_execute_dft( planTi, data, data );

        /*  multiplied by the scale factor */
        scale = 1.0F/( (float)(nxl * nyl) );

        for( ix=0; ix<nxl; ix++) {
            j = ix*nyl;
            for( iy=0; iy<nyl; iy++) {
                data[j  ][0] *= scale;  //  data[iy + ix*ny][0]
                data[j++][1] *= scale;  //  data[iy + ix*ny][1]
            }
        } /* end for(ix..) */

    //  complex to real 
    } else if( 2 == initLevel ) {
        fftwf_execute( planTi );   // ???
        nx = nxl;
        ny = (nyl-1)*2;

        /*  multiplied by the scale factor */
        scale = 1.0F/( (float)(nx * ny) );

        for( ix=0; ix<nx; ix++) {
            j = ix*ny;
            for( iy=0; iy<ny; iy++) {
                rpix[j++] *= scale;  //  rpix[iy + ix*ny]
            }
        } /* end for(ix..) */

    } else {
        printf( "error: cfpix::ifft() called before init()\n");
    }


}  // end cfpix::ifft()

//------------------ initializer ---------------------------------
//
//   mode = 0 for full measure (slow setup and fast execution)
//   mode = 1 for estimate (fast setup and slow execution)
//   mode = 2 for complex to real transform
//
//   nthreads = number of FFTW threads to use
//
// remember: FFTW has inverse sign convention so forward/inverse reversed
//
void cfpix::init( int mode, int nthreads )
{
    int nx, ny, ns;

    if( (nxl>0) && (nyl>0) ) {
        
        //   for many FFTs of the same size  (lots of CPU time to calculate plan)  
        if( 0 == mode ) {
            initLevel = mode;
	    if( nthreads > 1 ) {    //  initialize FFTW multithreading
                ns = fftwf_init_threads();
                fftwf_plan_with_nthreads( nthreads );
	    }
            planTi = fftwf_plan_dft_2d( nxl, nyl, data, data, 
                FFTW_FORWARD, FFTW_MEASURE );   /* inverse in place */
            planTf = fftwf_plan_dft_2d( nxl, nyl, data, data, 
                FFTW_BACKWARD, FFTW_MEASURE );  /* forward in place */

        //  for a few FFT of the same size (calculate plan fast, but execute slower)
        } else if( 1 == mode ) {
            initLevel = mode;
            planTi = fftwf_plan_dft_2d( nxl, nyl, data, data, 
                FFTW_FORWARD, FFTW_ESTIMATE );  /* inverse in place */
            planTf = fftwf_plan_dft_2d( nxl, nyl, data, data, 
                FFTW_BACKWARD, FFTW_ESTIMATE );  /* forward in place */

        //  complex to real FFT - only partially implemented
        } else if( 2 == mode ) {

            initLevel = mode;

            //  nx,ny = actual size of real image;
            //  nxl,nyl= number of complex coefficents 
            nx = nxl;
            ny = (nyl-1)*2;

            // only add a real array for this special type of FFT
            if( nrxyl != nx*ny ) {
                if( nrxyl > 0 ) fftwf_free( rpix );
                nrxyl = nx*ny;
                rpix = (float*) fftwf_malloc( nx*ny * sizeof(float) );
                if( NULL == rpix ) {
                    printf("Cannot allocate real array memory in cfpix\n" );
                    exit( 0 );
                }
            }
            planTi = fftwf_plan_dft_c2r_2d( nx, ny, data, rpix, FFTW_ESTIMATE );

       }   //  end mode 2 
    }

}  // end cfpix::initx()


/*------------------------- invert2D() ----------------------*/
/*
    rearrange pix with corners moved to center (a la FFT's)

    pix = complex image
    nx,ny = range of image 0<ix<(nx-1) and 0<iy<(ny-1)

*/
void cfpix::invert2D( )
{
    int ix, iy, i, j, ixmid, iymid;
    float t;

    ixmid = nxl/2;
    iymid = nyl/2;

    for( ix=0; ix<nxl; ix++) 
    for( iy=0; iy<iymid; iy++) {
        i = iy + ix*nyl;
        j = (iy+iymid) + ix*nxl;
        t = data[i][0];         // swap i and j positions
        data[i][0] = data[j][0]; 
        data[j][0] = t;
        t = data[i][1];
        data[i][1] = data[j][1]; 
        data[j][1] = t;
    }

    for( ix=0; ix<ixmid; ix++)
    for( iy=0; iy<nyl; iy++) {
        i = iy + ix*nyl;
        j = iy + (ix+ixmid)*nxl;
        t = data[i][0];         // swap i and j positions
        data[i][0] = data[j][0]; 
        data[j][0] = t; 
        t = data[i][1];
        data[i][1] = data[j][1]; 
        data[j][1] = t;
    }

}  // end invert2D()

//--------------------- operator+() ----------------------------------
//   real data not implemented yet
cfpix& cfpix::operator+=( const cfpix& m  )
{
    if( (m.nxl != nxl) || (m.nyl != nyl)  ){
        printf( "cfpix += operator invoked with unequal sizes:\n"
		"   %d x %d and %d x %d\n", nxl,  m.nxl, nxl,  m.nxl );
        exit( EXIT_FAILURE );
    } else {
        int i, nn=nxl*nyl;
        for( i=0; i<nn; i++) {
            data[i][0] += m.data[i][0];
            data[i][1] += m.data[i][1];
        }
        return *this;
    }
}

//--------------------- operator*() ----------------------------------
//  element by element multiply
//   real data not implemented yet
cfpix& cfpix::operator*=( const cfpix& m  )
{
    int nxyt= nxl*nyl;

    if( (m.nxl != nxl) || (m.nyl != nyl)  ){
	printf( "cfpix operator*= invoked with unequal sizes:\n"
		"   %d x %d and %d x %d\n", nxl,  m.nxl, nxl,  m.nxl );
	exit( EXIT_FAILURE );
    } else {
        int i;
        float wr, wi, tr, ti;
        for( i=0; i<nxyt; i++) {
           wr = data[i][0];   //  real part 
           wi = data[i][1];   //  imag part 
           tr = m.data[i][0];
           ti = m.data[i][1];
           data[i][0] = wr*tr - wi*ti;
           data[i][1] = wr*ti + wi*tr;
        }  /* end for(i...) */

        return *this;
    }
}

//--------------------- operator*() ----------------------------------
//  multiply all elements by a constant
//   real data not implemented yet
cfpix& cfpix::operator*=( const float xf  )
{
    int i, nxyt= nxl*nyl;

    if( nxyt > 0 )
        for( i=0; i<nxyt; i++) {
           data[i][0] *= xf;
           data[i][1] *= xf;
        }  // end for(i...)

    return *this;
}

//--------------------- resize() ----------------------------------
//  resize data buffer 
//  note: existing data (if any) may be destroyed
//
int cfpix::resize( const int nx, const int ny )
{
    if( (nx != nxl) || (ny != nyl) ){
        if( (nxl != 0) && (nyl != 0) ) fftwf_free( data );
        nxl = nx;
        nyl = ny;
        data = (fftwf_complex*) fftwf_malloc( nx*ny * sizeof(fftwf_complex) );
        if( NULL == data ) {
            printf( "Cannot allocate image storage in cfpix::resize()\n");
            return( -1 );
        }
    }

    return( +1 );

}  // end cfpix::resize()

//--------------------- findRange() ----------------------------------
//  find range of complex pix
//
void cfpix::findRange( float &rmin, float &rmax, float &aimin, float &aimax )
{
    int nxyt= nxl*nyl;
    float x;

    if( (nxl > 0) && (nyl > 0)  ){
        int i;
	rmin = rmax = data[0][0];
	aimin = aimax = data[0][1];
        for( i=0; i<nxyt; i++) {
           x = data[i][0];
	   if( x < rmin ) rmin = x;
	   if( x > rmax ) rmax = x;
           x = data[i][1];
	   if( x < aimin ) aimin = x;
	   if( x > aimax ) aimax = x;
        }  /* end for(i...) */

        return;
    }

}  // end cfpix::findRange()

