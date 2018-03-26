#include "crc32.h"
#include <windows.h>
#include "vdisk_dll.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <conio.h>



int __cdecl main(int argc, char *argv[])
{
	/*
	vdisk_error	error;
	void *dev = NULL;
	void *fd = NULL;
	int ret = 0;
	char buf[255] = "\0";

	error = vdisk_create( "d:\\1.rvd", 1024 * 1024 * 1024, FALSE, "fuck", TRUE );

	
	error = vdisk_open( "d:\\1.rvd", "fuck", &dev );

	error = vdisk_format( dev, fmt_type_ntfs );
	//return 0;
	fd = vdisk_fopen( dev, "\\rsvdico.ico", VDISK_CREATE_NEW );

	ret = vdisk_fwrite( fd, "fuck", sizeof("fuck") );
	
	ret = vdisk_flseek( fd, 0, 0 );

	ret = vdisk_fread( fd, buf, 255 );
	printf( "%s\n", buf );

	vdisk_fclose( fd );

	vdisk_close( dev );

	system( "pause" );
	*/
	return 0;
}