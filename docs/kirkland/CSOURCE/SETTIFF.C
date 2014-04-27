/*		*** settiff.c  ***

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

   quick Mac program to set the file type to "TIFF"
   
   This program only works under Symantec Think C/C++ 7.0 (68K mode)
   and requires the library MacTraps
   
   The file type can also be changed using Apple ResEdit or MPW
   
   started 15-jan-1998 E. Kirkland
   
*/

#include <stdio.h>
#include <string.h>

#include "files.h"

void main()
{
	FInfo fi;
	short returncode;
	unsigned char filename[256];

	printf("Set file type to TIFF, type file name:\n");
	scanf( "%s", &filename[1] );
	
	/* make a Mac Pascal str255 variable */
	filename[0] = strlen( (char*) &filename[1] );
	
	returncode = GetFInfo( filename, 0, &fi );	
	if( returncode != 0 ) {
		printf( "Bad GetFInfo() return code = %d\n", returncode );
		exit( 0 );
	}
		
	printf( "current file type = %.4s, file creator = %.4s\n", 
		(char*) &fi.fdType, (char*) &fi.fdCreator );
	
	/*  set file type to TIFF and leave other file info the unchanged */
	fi.fdType = 'TIFF';
	SetFInfo( filename, 0, &fi );
	printf( "The type of this file has been set to TIFF\n");
}