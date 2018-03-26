#ifndef __ntfs_lib_h__
#define __ntfs_lib_h__


#include <windows.h>
#include "include\types.h"

/* File access control and file status flags  */
#define	NTFS_OPEN_EXISTING	0x01	/* Opens the file. The function fails if the file is not existing. */
#define	NTFS_CREATE_NEW		0x02	/* Creates a new file. The function fails if the file is already existing. */

struct ntfs_device_ops
{
	void * priv_data;
	int (*disk_initialize) (struct fat_device_operations * io_ops,
		void *dev /* Physical drive number */);
	int (*disk_status) (struct fat_device_operations * io_ops,
		void *dev /* Physical drive number */);
	int (*disk_read) (struct fat_device_operations * io_ops,
		void *dev, /* Physical drive number */
		unsigned char* Buffer, /* Pointer to the read data buffer */
		s64 offset, /* Start sector number */
		s64 length /* Number of sectros to read */);
	int (*disk_write) (struct fat_device_operations * io_ops,
		void *dev, /* Physical drive number */
		const unsigned char* Buffer,  /* Pointer to the write data (may be non aligned) */
		s64 offset, /* Sector number to write */
		s64 length /* Number of sectors to write */);
	int (*disk_ioctl) (struct fat_device_operations * io_ops,
		void *dev, /* Physical drive number */
		unsigned char Command, /* Control command code */
		void* Buffer /* Data transfer buffer */);
};

void ntfs_register_io_ops( struct ntfs_device_ops *ops );


int ntfs_format( void *dev );

BOOL ntfs_rec( void *dev );

void *ntfs_fopen( void *dev, char *filename, unsigned char mode );

int ntfs_fread( void *file, void *buffer, s32 bufsize );

int ntfs_fwrite( void *file, void *buffer, s32 bufsize );

int ntfs_flseek( void *file, int offset, int whence);

void ntfs_fclose( void *file );



#endif