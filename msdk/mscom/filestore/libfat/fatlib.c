
#include <stdlib.h>		/* ANSI memory controls */

#include "ff.h"
#include "fatlib.h"

FATFS FatFs[_DRIVES];	/* File system object for logical drive */
struct fat_device_operations * disk_io_ops = 0;


FRESULT chk_mounted (	/* FR_OK(0): successful, !=0: any error occurred */
					 void *dev,
					 const TCHAR **path,	/* Pointer to pointer to the path name (drive number) */
					 FATFS **rfs,		/* Pointer to pointer to the found file system object */
					 BYTE chk_wp			/* !=0: Check media write protection for write access */
					 );

BOOL fat_rec( void *dev )
{
	FRESULT res;
	DIR dj;

	res = chk_mounted( dev, "", &dj.fs, 0, FALSE );
	return (res != FR_NO_FILESYSTEM);
}

FAT_FILE fat_fopen(void *dev, char * filename, unsigned char mode)
{
	FIL *file;
	FRESULT res;

	if(!disk_io_ops)
		return 0;

	file = malloc(sizeof(FIL));
	if(!file)
		return 0;
	res = f_open(dev, file, (TCHAR *)filename, (BYTE)mode);
	if(res != FR_OK)
	{
		free(file);
		return 0;
	}
	return (FAT_FILE)file;
}

FRESULT fat_fclose(FAT_FILE file)
{
	FRESULT res;

	if(!disk_io_ops)
		return FR_INVALID_IO_OPS;

	if(file == 0)
		return FR_INVALID_OBJECT;
	res = f_close((FIL *)file);
	if(res == FR_OK)
	{
		free((void *)file);
	}
	return res;
}

int fat_flseek(FAT_FILE file, int offset, int whence)
{
	FRESULT res;
	int new_pos;
	DWORD new_off_set;

	if(!disk_io_ops)
		return -1;

	if(file == 0)
		return -1;

	if(whence == SEEK_SET)
		new_pos = offset;
	else if(whence == SEEK_CUR)
		new_pos = ((FIL *)file)->fptr + offset;
	else if(whence == SEEK_END)
		new_pos = ((FIL *)file)->fsize + offset;
	else
		return -1;
    
	if(new_pos < 0)
		new_off_set = 0;
	else
		new_off_set = (DWORD)new_pos;

	if( new_off_set > ((FIL *)file)->fsize )
		new_off_set = ((FIL *)file)->fsize;

	res = f_lseek((FIL *)file, new_off_set);
	if(res != FR_OK)
	{
		return -1;
	}
	return (int)new_off_set;
}

int fat_fread(FAT_FILE file, void * buffer, unsigned int bufsize)
{
	FRESULT res;
	int readbytes;

	if(!disk_io_ops)
		return -1;

	if(file == 0)
		return -1;
	res = f_read((FIL *)file, buffer, (UINT)bufsize, (UINT *)&readbytes);
	if(res != FR_OK)
	{
		return -1;
	}
	return readbytes;
}

int fat_fwrite(FAT_FILE file, void * buffer, unsigned int bufsize)
{
	FRESULT res;
	int writtenbytes;

	if(!disk_io_ops)
		return -1;

	if(file == 0)
		return -1;
	res = f_write((FIL *)file, buffer, (UINT)bufsize, (UINT *)&writtenbytes);
	if(res != FR_OK)
	{
		return -1;
	}
	return writtenbytes;
}

FRESULT fat_mkdir(void *dev, char * path)
{
	FRESULT res;

	if(!disk_io_ops)
		return FR_INVALID_IO_OPS;

	res = f_mkdir(dev, (TCHAR *)path);
	return res;
}

FRESULT fat_chdir(void *dev, char * path)
{
	FRESULT res;

	if(!disk_io_ops)
		return FR_INVALID_IO_OPS;

	res = f_chdir(dev, (TCHAR *)path);
	return res;
}

FRESULT fat_remove(void *dev, char * name)
{
	FRESULT res;

	if(!disk_io_ops)
		return FR_INVALID_IO_OPS;

	res = f_unlink(dev, (TCHAR *)name);
	return res;
}

FRESULT fat_rename(void * dev, char * oldname, char * newname)
{
	FRESULT res;

	if(!disk_io_ops)
		return FR_INVALID_IO_OPS;

	res = f_rename(dev, (TCHAR *)oldname, (TCHAR *)newname);
	return res;
}

FRESULT fat_format(void *dev, unsigned char partitioning_rule, unsigned int cluster_size)
{
	FRESULT res;

	if(!disk_io_ops)
		return FR_INVALID_IO_OPS;

	res = f_mkfs(dev, 0, (BYTE)partitioning_rule, (UINT)cluster_size);
	return res;
}

FRESULT fat_register_io_ops(struct fat_device_operations * io_ops)
{
	FRESULT res;

	if(disk_io_ops || !io_ops)
		return FR_INVALID_IO_OPS;
	if( !(io_ops->disk_initialize) || !(io_ops->disk_status) ||
		!(io_ops->disk_read) || !(io_ops->disk_write) || !(io_ops->disk_ioctl) )
		return FR_INVALID_IO_OPS;

	res = f_mount(0, &FatFs[0]);
	if(res == FR_OK)
		disk_io_ops = io_ops;
	return res;
}

void fat_unregister_io_ops()
{
	disk_io_ops = 0;
	f_mount(0, 0);
}
