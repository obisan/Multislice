#pragma once

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
	void* malloc1D( int n, size_t size, const char *message );

	/*---------------------------- malloc2D() -------------------------------*/
	/*
		2D array allocator for type float
		make space for m[0...(nx-1)][0..(ny-1)]

		nx,ny = number of elements
		size = size of each element as returned by
			sizeof(double), sizeof(int), etc.

	*/
	void **malloc2D( int nx, int ny, size_t size, const char *message );

	/*---------------------------- malloc3D() -------------------------------*/
	/*
		3D array allocator for type float
		make space for m[0...(nx-1)][0..(ny-1)][0..(nz-1)]

		nx,ny,nz = number of elements
		size = size of each element as returned by
			sizeof(double), sizeof(int), etc.

	*/
	void ***malloc3D( int nx, int ny, int nz, size_t size, const char *message );

}