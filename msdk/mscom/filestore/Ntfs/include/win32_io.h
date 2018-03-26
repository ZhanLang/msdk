#pragma once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"

//////////////////////////////////////////////////////////////////////////


typedef struct {
	HANDLE handle;
	s64 pos;		/* Logical current position on the volume. */
	s64 part_start;
	s64 part_length;
	int part_hidden_sectors;
	s64 geo_size, geo_cylinders;
	DWORD geo_sectors, geo_heads;
	HANDLE vol_handle;
} win32_fd;



//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif


/*extern*/ int ntfs_device_win32_open(struct ntfs_device *dev, int flags);

/*extern*/ int ntfs_device_win32_close(struct ntfs_device *dev);

/*extern*/ s64 ntfs_device_win32_seek(struct ntfs_device *dev, s64 offset, int whence);

/*extern*/ s64 ntfs_device_win32_read(struct ntfs_device *dev, void *b, s64 count);

/*extern*/ s64 ntfs_device_win32_write(struct ntfs_device *dev, const void *b, s64 count);
			
			int ntfs_device_win32_stat(struct ntfs_device *dev, struct x_stat *buf);

/*extern*/ s64 ntfs_device_win32_pread(struct ntfs_device *dev, void *b, s64 count, s64 offset);

/*extern*/ s64 ntfs_device_win32_pwrite(struct ntfs_device *dev, const void *b, s64 count, s64 offset);

/*extern*/ int ntfs_device_win32_sync(struct ntfs_device *dev);

/*extern*/ int ntfs_device_win32_ioctl(struct ntfs_device *dev, int request, void *argp);

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////

