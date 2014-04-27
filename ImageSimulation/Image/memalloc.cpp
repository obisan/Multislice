#include "stdafx.h"
#include "memalloc.h"

namespace ImageSpace {

	/*---------------------------- malloc1D() -------------------------------*/
	/*
		1D array allocator for arbitrary type
		make space for m[0...(n-1)]
		printf error message and exit if not successful
    
		this save checking for a NULL return etc. every time 
    
		n = number of elements
		size = size of each element as returned by
			sizeof(double), sizeof(int), etc.

	*/
	void* malloc1D( int n, size_t size, const char *message )
	{
		void *m;
    
		m = (void*) malloc( n * size );
		if( m == NULL ) {
			printf("out of memory in malloc1D(), size= %d: %s\n",
				   n, message);
			exit( 0 );
		}
		return( m );

	}  /* end malloc1D() */

	/*---------------------------- malloc2D() -------------------------------*/
	/*
		2D array allocator for type float
		make space for m[0...(nx-1)][0..(ny-1)]

		nx,ny = number of elements
		size = size of each element as returned by
			sizeof(double), sizeof(int), etc.

	*/
	void** malloc2D( int nx, int ny, size_t size, const char *message )
	{   void **m;
		int i;

		m = (void**) malloc( nx * sizeof( void* ) ); 
		if( m == NULL ) {
			printf("out of memory in malloc2D(), size=%d, %d: %s\n",
				   nx, ny, message );
			exit(0);
		}

		for (i=0; i<nx; i++){
			m[i] = (void*) malloc( ny * size );
			if( m[i] == NULL ){
				printf("out of memory in malloc2D(),"
					" size=%d, %d: %s\n", nx, ny, message );
				exit(0);
			}
		}

		return m;

	}  /* end malloc2D() */

	/*---------------------------- malloc3D() -------------------------------*/
	/*
		3D array allocator for type float
		make space for m[0...(nx-1)][0..(ny-1)][0..(nz-1)]

		nx,ny,nz = number of elements
		size = size of each element as returned by
			sizeof(double), sizeof(int), etc.

	*/
	void*** malloc3D( int nx, int ny, int nz, size_t size, const char *message )
	{   void ***m;
		int i, j;

		m = (void***) malloc( nx * sizeof( void** ) ); 
		if( m == NULL ) {
			printf("out of memory in malloc3D(), size=%d, %d, %d: %s\n",
				   nx, ny, nz, message );
			exit(0);
		}

		for (i=0; i<nx; i++){
			m[i] = (void**) malloc( ny * sizeof( void* ) );
			if( m[i] == NULL ){
				printf("out of memory in malloc3D(),"
					" size=%d, %d, %d: %s\n",
					   nx, ny, nz, message );
				exit(0);
			}
			for (j=0; j<ny; j++){
				m[i][j] = (void*) malloc( nz * size );
				if( m[i][j] == NULL ){
					printf("out of memory in malloc3D(),"
						   " size=%d, %d, %d: %s\n",
						  nx, ny, nz, message );
					exit(0);
				}
			}
		}

		return m;

	}  /* end malloc3D() */

}