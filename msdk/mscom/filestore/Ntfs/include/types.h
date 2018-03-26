/*
 * types.h - Misc type definitions not related to on-disk structure.  
 *           Originated from the Linux-NTFS project.
 */

#ifndef _NTFS_TYPES_H
#define _NTFS_TYPES_H

#include <windows.h>


typedef unsigned char  u8;			/* Unsigned types of an exact size */
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned __int64 u64;

typedef signed char  s8;			/* Signed types of an exact size */
typedef signed short s16;
typedef signed int s32;
typedef signed __int64 s64;

typedef u16 le16;
typedef u32 le32;
typedef u64 le64;

/*
 * Declare sle{16,32,64} to be unsigned because we do not want sign extension
 * on BE architectures.
 */
typedef u16 sle16;
typedef u32 sle32;
typedef u64 sle64;

typedef u16 ntfschar;			/* 2-byte Unicode character type. */
typedef s32 ssize_t;
#define UCHAR_T_SIZE_BITS 1

/*
 * Clusters are signed 64-bit values on NTFS volumes.  We define two types, LCN
 * and VCN, to allow for type checking and better code readability.
 */
typedef s64 VCN;
typedef sle64 leVCN;
typedef s64 LCN;
typedef sle64 leLCN;

/*
 * The NTFS journal $LogFile uses log sequence numbers which are signed 64-bit
 * values.  We define our own type LSN, to allow for type checking and better
 * code readability.
 */
typedef s64 LSN;
typedef sle64 leLSN;

/*
 * Cygwin has a collision between our BOOL and <windef.h>'s
 * As long as this file will be included after <windows.h> were fine.
 */

struct mkntfs_options {
	char *dev_name;			/* Name of the device, or file, to use */
	void *priv_data;
	BOOL enable_compression;	/* -C, enables compression of all files on the volume by default. */
	BOOL quick_format;		/* -f or -Q, fast format, don't zero the volume first. */
	BOOL force;			/* -F, force fs creation. */
	long heads;			/* -H, number of heads on device */
	BOOL disable_indexing;		/* -I, disables indexing of file contents on the volume by default. */
	BOOL no_action;			/* -n, do not write to device, only display what would be done. */
	long long part_start_sect;	/* -p, start sector of partition on parent device */
	long sector_size;		/* -s, in bytes, power of 2, default is 512 bytes. */
	long sectors_per_track;		/* -S, number of sectors per track on device */
	BOOL use_epoch_time;		/* -T, fake the time to be 00:00:00 UTC, Jan 1, 1970. */
	long mft_zone_multiplier;	/* -z, value from 1 to 4. Default is 1. */
	long long num_sectors;		/* size of device in sectors */
	long cluster_size;		/* -c, format with this cluster-size */
	char *label;			/* -L, volume label */
};


/**
 * enum IGNORE_CASE_BOOL -
 */
typedef enum {
	CASE_SENSITIVE = 0,
	IGNORE_CASE = 1,
} IGNORE_CASE_BOOL;

#define STATUS_OK				(0)
#define STATUS_ERROR				(-1)
#define STATUS_RESIDENT_ATTRIBUTE_FILLED_MFT	(-2)
#define STATUS_KEEP_SEARCHING			(-3)
#define STATUS_NOT_FOUND			(-4)

#define strtoll			_strtoui64
#define snprintf		_snprintf

typedef struct {
	BOOL bUse_SCSI ;
	BOOL bDriverLoaded ;
} SCSI_DRIVER, *PSCSI_DRIVER ;

SCSI_DRIVER g_scsi_driver ;

#endif /* defined _NTFS_TYPES_H */

