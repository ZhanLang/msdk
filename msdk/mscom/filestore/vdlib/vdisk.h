#include "aes.h"

#ifdef __KERNEL__
# include <ntddk.h>
#else
# include <windows.h>
#endif

#ifndef _FILE_DISK_
#define _FILE_DISK_

#ifndef __T
#ifdef _NTDDK_
#define __T(x)  L ## x
#else
#define __T(x)  x
#endif
#endif

#ifndef _T
#define _T(x)   __T(x)
#endif

#define DEVICE_NAME_PREFIX		_T("\\Device\\RsDisk")
#define DEVICE_NAME_POSTFIX		L"RsDiskCdo"
#define DEVICE_NAME_CDO			L"\\Device\\" DEVICE_NAME_POSTFIX
#define DEVICE_LINK_NAME_CDO		L"\\DosDevices\\" DEVICE_NAME_POSTFIX
#define DEVICE_WIN32_NAME		L"\\\\.\\" DEVICE_NAME_POSTFIX

#define FILE_DEVICE_VDISK		0x8000
//#define IOCTL_VDISK_OPEN_FILE		CTL_CODE(FILE_DEVICE_VDISK, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_VDISK_CLOSE_FILE	CTL_CODE(FILE_DEVICE_VDISK, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VDISK_SET_USER_DATA	CTL_CODE(FILE_DEVICE_VDISK, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VDISK_GET_USER_DATA	CTL_CODE(FILE_DEVICE_VDISK, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VDISK_QUERY_FILE		CTL_CODE(FILE_DEVICE_VDISK, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_VDISK_CREATE_MOUNT_POINT  CTL_CODE(FILE_DEVICE_VDISK, 0x803, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VDISK_DELETE_MOUNT_POINT  CTL_CODE(FILE_DEVICE_VDISK, 0x804, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VDISK_QUERY_INFO		CTL_CODE(FILE_DEVICE_VDISK, 0x805, METHOD_BUFFERED, FILE_READ_ACCESS)

struct vdisk_user_data {
	uint32_t userid;
	ULONG data_len;
	UCHAR data[1];
};

struct vdisk_info {
	uint32_t userid;
	uint8_t	letter; /* 驱动器索引 */
	LARGE_INTEGER filesize;
	BOOLEAN readonly;
	AES_KEY encrypt_key;
	AES_KEY decrypt_key;
	USHORT filename_len;
	UCHAR filename[1];
};

struct vdisk_info_ex {
	uint32_t userid;
	uint32_t drvmap; /* 驱动器索引位图 */
	uint64_t local_time;
	LARGE_INTEGER filesize;
	BOOLEAN readonly;
	AES_KEY encrypt_key;
	AES_KEY decrypt_key;
	UCHAR filename[0x200];
};



// must be 16-byte alignment
struct dyndisk_header {
	uint64_t size;		/* in bytes */
	uint8_t cluster_bits;
	uint8_t l2_bits;
#define RSDISK_CRYPT_NONE 0
#define RSDISK_CRYPT_AES  1
	uint64_t l1_table_offset;
};


//直接从U盘隐藏分区加载
struct hudisk_header
{
	int		 disknum;
	int		 partnum;
	uint64_t start;	//起始位置
	uint64_t size;	//结束位置
};

// must be 16-byte alignment
struct fixdisk_header {
	uint64_t reserved[2];
};

struct fixdisk_state {
	void *fd;
	uint32_t crypt_method;	/* current crypt method, 0 if no key yet */
	uint32_t crypt_method_header;
	AES_KEY aes_encrypt_key;
	AES_KEY aes_decrypt_key;
};

//must be 16byte alignment
struct rsdisk_header {
	//non crypt info
	uint64_t size;	//sizeof(rsdisk_header)
	uint64_t magic;
	uint64_t version;
#define RSDISK_MAGIC_HI ((uint64_t)('R' << 24) | (uint64_t)('S' << 16) | (uint64_t)('V' << 8) | (uint64_t)('D'))
#define RSDISK_MAGIC_LO ((uint64_t)('S' << 24) | (uint64_t)('I' << 16) | (uint64_t)('G' << 8) | (uint64_t)(0xfe))
#define RSDISK_MAGIC ((RSDISK_MAGIC_HI << 32) | RSDISK_MAGIC_LO)
#define RSDISK_VERSION 1

	//crypt info
	uint64_t cryptmagic;
	uint32_t disk_type;
#define RSDISK_TYPE_FIXED 0
#define RSDISK_TYPE_DYNAMIC 1
#define RSDISK_TYPE_DISKPART 2
	uint32_t crypt_method;
	union {
		struct fixdisk_header f_hdr;
		struct dyndisk_header d_hdr;
		struct hudisk_header  p_hdr;
	};
};

#endif
