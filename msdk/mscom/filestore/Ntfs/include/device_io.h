/*
 * device_io.h - Exports for default device io. Originated from the Linux-NTFS project.
 */

#ifndef _NTFS_DEVICE_IO_H
#define _NTFS_DEVICE_IO_H


#ifndef HDIO_GETGEO
#	define HDIO_GETGEO	0x301
/**
 * struct hd_geometry -
 */
struct hd_geometry {
	unsigned char heads;
	unsigned char sectors;
	unsigned short cylinders;
	unsigned long start;
};
#endif
#ifndef BLKGETSIZE
#	define BLKGETSIZE	0x1260
#endif
#ifndef BLKSSZGET
#	define BLKSSZGET	0x1268
#endif
#ifndef BLKGETSIZE64
#	define BLKGETSIZE64	0x80041272
#endif
#ifndef BLKBSZSET
#	define BLKBSZSET	0x40041271
#endif

/* On Cygwin; use Win32 low level device operations. */
#define ntfs_device_default_io_ops g_ntfs_device_win32_io_ops


/* Forward declaration. */
struct ntfs_device_operations;

extern struct ntfs_device_operations ntfs_device_default_io_ops;

#endif /* defined _NTFS_DEVICE_IO_H */