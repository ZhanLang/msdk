
#ifndef _FATLIB
#define _FATLIB

#include <windows.h>

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

/* Command code for disk_ioctrl() */
#define CTRL_SYNC			0	/* Mandatory for write functions */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2	/* Mandatory for multiple sector size cfg */
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */

/* File access control and file status flags  */
#define	FAT_READ			0x01	/* Specifies read access to the object. Data can be read from the file. */
#define	FAT_WRITE			0x02	/* Specifies write access to the object. Data can be written to the file. */
#define	FAT_OPEN_EXISTING	0x00	/* Opens the file. The function fails if the file is not existing. */
#define	FAT_OPEN_ALWAYS		0x10	/* Opens the file if it is existing. If not, a new file is created. */
#define	FAT_CREATE_NEW		0x04	/* Creates a new file. The function fails if the file is already existing. */
#define	FAT_CREATE_ALWAYS	0x08	/* Creates a new file. If the file is existing, it is truncated and overwritten. */

/* Partitioning rule for fat_format() */
#define PR_FDISK		0	 /* A partition table is created into the master boot record
								and a primary DOS partition is created
								and then an FAT volume is created on the partition.
								Used for harddisk and memory cards. */
#define PR_SFD			1	 /* The FAT volume starts from the first sector on the drive without partition table.
								Used for floppy disk and most optical disk. */

/* File pointer position for fat_flseek() */
#define SEEK_SET	0		/* Start of file */
#define SEEK_CUR	1		/* Current position of file */
#define SEEK_END	2		/* End of file */


typedef void *	FAT_FILE;

/* File function return code (FRESULT) */
typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Acces denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Acces denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume on the physical drive */
	FR_MKFS_ABORTED,		/* (14) The fat_format() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files is too many */
	FR_INVALID_IO_OPS		/* (19) Disk IO operations is invalid */
} FRESULT;

/* Status of Disk Functions */
typedef unsigned char	DSTATUS;

/* Results of Disk Functions */
typedef enum {
		RES_OK = 0,		/* 0: Successful */
		RES_ERROR,		/* 1: R/W Error */
		RES_WRPRT,		/* 2: Write Protected */
		RES_NOTRDY,		/* 3: Not Ready */
		RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

struct fat_device_operations
{
 	void * priv_data;
	DSTATUS (*disk_initialize) (struct fat_device_operations * io_ops,
 					void *dev /* Physical drive number */);
	DSTATUS (*disk_status) (struct fat_device_operations * io_ops,
 					void *dev /* Physical drive number */);
	DRESULT (*disk_read) (struct fat_device_operations * io_ops,
 					void *dev, /* Physical drive number */
					unsigned char* Buffer, /* Pointer to the read data buffer */
					unsigned long SectorNumber, /* Start sector number */
					unsigned char SectorCount /* Number of sectros to read */);
	DRESULT (*disk_write) (struct fat_device_operations * io_ops,
 					void *dev, /* Physical drive number */
					const unsigned char* Buffer,  /* Pointer to the write data (may be non aligned) */
					unsigned long SectorNumber, /* Sector number to write */
					unsigned char SectorCount /* Number of sectors to write */);
	DRESULT (*disk_ioctl) (struct fat_device_operations * io_ops,
 					void *dev, /* Physical drive number */
					unsigned char Command, /* Control command code */
					void* Buffer /* Data transfer buffer */);
};

BOOL fat_rec( void *dev );
FAT_FILE fat_fopen(void *dev, char * filename, unsigned char mode);
FRESULT fat_fclose(FAT_FILE file);
int fat_flseek(FAT_FILE file, int offset, int whence);
int fat_fread(FAT_FILE file, void * buffer, unsigned int bufsize);
int fat_fwrite(FAT_FILE file, void * buffer, unsigned int bufsize);
FRESULT fat_mkdir(void *dev, char * path);
FRESULT fat_chdir(void *dev, char * path);
FRESULT fat_remove(void *dev, char * name);
FRESULT fat_rename(void *dev, char * oldname, char * newname);
/*	This function does not support multiple partition,
	so that existing partitions on the physical dirve will be deleted and re-created a new partition occupies entire disk space! */
FRESULT fat_format(void *dev, unsigned char partitioning_rule, unsigned int cluster_size);
FRESULT fat_register_io_ops(struct fat_device_operations * io_ops);
void fat_unregister_io_ops();

/* RTC function */
unsigned long get_fattime (void);


#endif
