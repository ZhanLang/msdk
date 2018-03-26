// vdisk.cpp : Defines the exported functions for the DLL application.
//

#include <windows.h>
#include <errno.h>
#include "vdisk.h"
#include "aes.h"
#include "dyndisk.h"
#include "fops.h"
#include "vdisk_dll.h"
#include <WinIoCtl.h>
#include <stdio.h>
#include "../libfat/fatlib.h"
#include "../ntfs/ntfslib.h"




/* 设备指针 */
struct vdisk_dev {
	uint32_t disk_type;

	union {
		struct dyndisk_state dd_stat;
		struct fixdisk_state sd_stat;
	};

	LARGE_INTEGER filesize;
};

struct _PART_LAYOUT_INFO
{
	DRIVE_LAYOUT_INFORMATION_EX layout[128];
};

INT b_g_init_cont = 0;

DSTATUS disk_initialize(struct fat_device_operations * io_ops,
							void *dev /* Physical drive number */)
{
	return RES_OK;
}

DSTATUS disk_status(struct fat_device_operations * io_ops,
						void *dev /* Physical drive number */)
{
	return RES_OK;
}

DRESULT disk_read_fat(struct fat_device_operations * io_ops,
				  void *dev, /* Physical drive number */
				  unsigned char* Buffer, /* Pointer to the read data buffer */
				  unsigned long SectorNumber, /* Start sector number */
				  unsigned char SectorCount /* Number of sectros to read */)
{
	long ret = 0;
	uint64_t offset = 0;
	uint32_t length = 0;
	struct vdisk_dev *vd = (struct vdisk_dev *)dev;

	length = SectorCount * VDISK_SECTOR_SIZE;
	offset = SectorNumber * VDISK_SECTOR_SIZE;

	switch (vd->disk_type) {

case RSDISK_TYPE_DYNAMIC:
	{
		if (dyndisk_read( &vd->dd_stat, offset / VDISK_SECTOR_SIZE, Buffer,
			length / VDISK_SECTOR_SIZE) < 0) {
				return RES_ERROR;
		}
	}
	break;
case RSDISK_TYPE_FIXED:
	{
		offset += VDISK_SECTOR_SIZE;

		vdisk_file_lseek( vd->sd_stat.fd, offset, 0 );
		ret = vdisk_file_read( vd->sd_stat.fd, Buffer, length );
		if (!ret || ret != length) {
			return RES_ERROR;
		}

		if (vd->sd_stat.crypt_method == RSDISK_CRYPT_AES) {
			encrypt_sectors( offset / VDISK_SECTOR_SIZE - 1, Buffer,
				Buffer, length / VDISK_SECTOR_SIZE, 0,
				&vd->sd_stat.aes_decrypt_key );
		}
	}

	break;
default:
	break;
	}

	return RES_OK;
}

DRESULT disk_write_fat(struct fat_device_operations * io_ops,
				   void *dev, /* Physical drive number */
				   const unsigned char* Buffer,  /* Pointer to the write data (may be non aligned) */
				   unsigned long SectorNumber, /* Sector number to write */
				   unsigned char SectorCount /* Number of sectors to write */)
{
	long ret = 0;
	uint64_t offset = 0;
	uint32_t length = 0;
	struct vdisk_dev *vd = (struct vdisk_dev *)dev;

	length = SectorCount * VDISK_SECTOR_SIZE;
	offset = SectorNumber * VDISK_SECTOR_SIZE;

	switch (vd->disk_type) {

case RSDISK_TYPE_DYNAMIC:
	{
		if (dyndisk_write( &vd->dd_stat, offset / VDISK_SECTOR_SIZE, Buffer,
			length / VDISK_SECTOR_SIZE ) < 0)
			return RES_ERROR;
	}
	break;
case RSDISK_TYPE_FIXED:
	{
		offset += VDISK_SECTOR_SIZE;
		vdisk_file_lseek( vd->sd_stat.fd, offset, 0 );
		if ( vd->sd_stat.crypt_method == RSDISK_CRYPT_AES ) {
			uint8_t *encryptbuf = malloc( length );
			if (!encryptbuf) {
				return RES_ERROR;
			}
			encrypt_sectors( offset / VDISK_SECTOR_SIZE - 1, encryptbuf,
				Buffer, length / VDISK_SECTOR_SIZE, 1,
				&vd->sd_stat.aes_encrypt_key );
			ret = vdisk_file_write( vd->sd_stat.fd, encryptbuf, length );
			free( encryptbuf );
		} else {
			ret = vdisk_file_write( vd->sd_stat.fd, Buffer, length );
		}
		if (!ret || ret != length) {
			return RES_ERROR;
		}
	}

	break;
default:
	break;
	}

	return RES_OK;
}

DRESULT disk_ioctl_fat(struct fat_device_operations * io_ops,
				   void *dev, /* Physical drive number */
				   unsigned char Command, /* Control command code */
				   void* Buffer /* Data transfer buffer */)
{
	struct vdisk_dev *vd = (struct vdisk_dev *)dev;

	switch( Command ) {

	case GET_SECTOR_COUNT:
		{
			if (vd->disk_type == RSDISK_TYPE_DYNAMIC) {
				*(DWORD *)Buffer = (DWORD)vd->dd_stat.total_sectors;
			} else {
				*(DWORD *)Buffer = (DWORD)vd->filesize.QuadPart / VDISK_SECTOR_SIZE;
			}
		}
		break;

	case GET_BLOCK_SIZE:
		{
			*(DWORD *)Buffer = VDISK_SECTOR_SIZE;
		}
		break;

	case CTRL_SYNC:
		{

		}
		break;

	default:
		{

		}
		break;
	}

	return RES_OK;
}

DRESULT disk_read(struct fat_device_operations * io_ops,
					  void *dev, /* Physical drive number */
					  unsigned char* Buffer, /* Pointer to the read data buffer */
					  s64 offset, /* Start sector number */
					  s32 length /* Number of sectros to read */)
{
	long ret = 0;
	struct vdisk_dev *vd = (struct vdisk_dev *)dev;
	s32 new_length = (offset % VDISK_SECTOR_SIZE) + length;
	s32 secnr = (new_length / VDISK_SECTOR_SIZE) + (((s32)(new_length % VDISK_SECTOR_SIZE) != 0) ? 1 : 0);
	uint8_t *new_buf = Buffer;
	s64 new_offset = offset - (offset % VDISK_SECTOR_SIZE);
	new_length = secnr * VDISK_SECTOR_SIZE;

	switch (vd->disk_type) {

		case RSDISK_TYPE_DYNAMIC:
			{
				if (new_length != length) {

					new_buf = (uint8_t *)malloc( secnr * VDISK_SECTOR_SIZE );
					if (new_buf == NULL) {
						return RES_ERROR;
					}
				}

				if (dyndisk_read( &vd->dd_stat, new_offset / VDISK_SECTOR_SIZE, new_buf, secnr ) < 0) {
					if (new_buf != Buffer) { free( new_buf ); }
					return RES_ERROR;
				}
				if (new_length != length) {
					memcpy( Buffer, &new_buf[offset - new_offset], length );
					free( new_buf );
				}
			}
			break;
		case RSDISK_TYPE_FIXED:
			{
				offset += VDISK_SECTOR_SIZE;
				new_offset += VDISK_SECTOR_SIZE;

				if (new_length != length) {

					new_buf = (uint8_t *)malloc( secnr * VDISK_SECTOR_SIZE );
					if (new_buf == NULL) {
						return RES_ERROR;
					}
				}

				vdisk_file_lseek( vd->sd_stat.fd, new_offset, 0 );
				ret = vdisk_file_read( vd->sd_stat.fd, new_buf, new_length );
				if (!ret || ret != new_length) {
					if (new_length != length) { free( new_buf ); }
					return RES_ERROR;
				}

				if (vd->sd_stat.crypt_method == RSDISK_CRYPT_AES) {
					encrypt_sectors( new_offset / VDISK_SECTOR_SIZE - 1, new_buf,
						new_buf, new_length / VDISK_SECTOR_SIZE, 0,
						&vd->sd_stat.aes_decrypt_key );
				}

				if (new_length != length) {
					memcpy( Buffer, &new_buf[offset - new_offset], length );
					free( new_buf );
				}
			}

		case RSDISK_TYPE_DISKPART:
			{
				offset += VDISK_SECTOR_SIZE;
				new_offset += VDISK_SECTOR_SIZE;

				if (new_length != length) {

					new_buf = (uint8_t *)malloc( secnr * VDISK_SECTOR_SIZE );
					if (new_buf == NULL) {
						return RES_ERROR;
					}
				}

				vdisk_file_lseek( vd->sd_stat.fd, new_offset, 0 );
				ret = vdisk_file_read( vd->sd_stat.fd, new_buf, new_length );
				if (!ret || ret != new_length) {
					if (new_length != length) { free( new_buf ); }
					return RES_ERROR;
				}

				if (vd->sd_stat.crypt_method == RSDISK_CRYPT_AES) {
					encrypt_sectors( new_offset / VDISK_SECTOR_SIZE - 1, new_buf,
						new_buf, new_length / VDISK_SECTOR_SIZE, 0,
						&vd->sd_stat.aes_decrypt_key );
				}

				if (new_length != length) {
					memcpy( Buffer, &new_buf[offset - new_offset], length );
					free( new_buf );
				}
			}
			break;
		default:
			break;
	}

	return RES_OK;
}
/* fat 32 要改 */
DRESULT disk_write(struct fat_device_operations * io_ops,
					   void *dev, /* Physical drive number */
					   const unsigned char* Buffer,  /* Pointer to the write data (may be non aligned) */
					   s64 offset, /* Sector number to write */
					   s32 length /* Number of sectors to write */)
{
	long ret = 0;
	struct vdisk_dev *vd = (struct vdisk_dev *)dev;
	s32 new_length = (offset % VDISK_SECTOR_SIZE) + length;
	s32 secnr = (new_length / VDISK_SECTOR_SIZE) + (((s32)(new_length % VDISK_SECTOR_SIZE) != 0) ? 1 : 0);
	uint8_t *new_buf = Buffer;
	s64 new_offset = offset - (offset % VDISK_SECTOR_SIZE);
	new_length = secnr * VDISK_SECTOR_SIZE;

	switch (vd->disk_type) {

	case RSDISK_TYPE_DYNAMIC:
		{
			if (new_length != length) {

				new_buf = (uint8_t *)malloc( secnr * VDISK_SECTOR_SIZE );
				if (new_buf == NULL) {
					return RES_ERROR;
				}
				if (dyndisk_read( &vd->dd_stat, new_offset / VDISK_SECTOR_SIZE, new_buf, secnr ) < 0) {
					free( new_buf );
					return RES_ERROR;
				}

				memcpy( &new_buf[offset - new_offset], Buffer, length );
			}

			if (dyndisk_write( &vd->dd_stat, new_offset / VDISK_SECTOR_SIZE, new_buf, secnr ) < 0) {
				if (new_buf != Buffer) { free( new_buf ); }
				return RES_ERROR;
			}
		}
		break;
	case RSDISK_TYPE_FIXED:
		{
			new_offset += VDISK_SECTOR_SIZE;
			offset += VDISK_SECTOR_SIZE;
			
			if ( vd->sd_stat.crypt_method == RSDISK_CRYPT_AES ) {

				uint8_t *encryptbuf = malloc( secnr * VDISK_SECTOR_SIZE );
				if (!encryptbuf) {
					return RES_ERROR;
				}

				vdisk_file_lseek( vd->sd_stat.fd, new_offset, 0 );

				if (new_length != length) {
					new_buf = malloc( secnr * VDISK_SECTOR_SIZE );
					if (!new_buf) {
						free( encryptbuf );
						return RES_ERROR;
					}

					if (vdisk_file_read( vd->sd_stat.fd, new_buf, new_length ) < 0) {
						free( encryptbuf );
						free( new_buf );
						return RES_ERROR;
					}

					encrypt_sectors( new_offset / VDISK_SECTOR_SIZE - 1, new_buf,
						new_buf, new_length / VDISK_SECTOR_SIZE, 0,
						&vd->sd_stat.aes_decrypt_key );

					memcpy( &new_buf[offset - new_offset], Buffer, length );
				}

				encrypt_sectors( new_offset / VDISK_SECTOR_SIZE - 1, encryptbuf, new_buf, secnr, 1, &vd->sd_stat.aes_encrypt_key );
				vdisk_file_lseek( vd->sd_stat.fd, new_offset, 0 );
				ret = vdisk_file_write( vd->sd_stat.fd, encryptbuf, new_length );
				free( encryptbuf );

				if (!ret || ret != new_length) {
					return RES_ERROR;
				}

			} else {

				vdisk_file_lseek( vd->sd_stat.fd, offset, 0 );
				ret = vdisk_file_write( vd->sd_stat.fd, Buffer, length );

				if (!ret || ret != length) {
					return RES_ERROR;
				}
			}
			
		}

		break;
	default:
		break;
	}

	if (new_buf != Buffer) { free( new_buf ); }

	return RES_OK;
}

DRESULT disk_ioctl(struct fat_device_operations * io_ops,
					   void *dev, /* Physical drive number */
					   unsigned char Command, /* Control command code */
					   void* Buffer /* Data transfer buffer */)
{
	struct vdisk_dev *vd = (struct vdisk_dev *)dev;

	switch( Command ) {

	case GET_SECTOR_COUNT:
		{
			if (vd->disk_type == RSDISK_TYPE_DYNAMIC) {
				*(uint64_t *)Buffer = (uint64_t)vd->dd_stat.total_sectors;
			} else {
				*(uint64_t *)Buffer = (uint64_t)vd->filesize.QuadPart / VDISK_SECTOR_SIZE;
			}
		}
		break;

	case GET_BLOCK_SIZE:
		{
			*(DWORD *)Buffer = VDISK_SECTOR_SIZE;
		}
		break;

	case CTRL_SYNC:
		{

		}
		break;

	default:
		{

		}
		break;
	}

	return RES_OK;
}




struct fat_device_operations dev_operations = {
	NULL,
	disk_initialize,
	disk_status,
	disk_read_fat,
	disk_write_fat,
	disk_ioctl_fat,
};

struct ntfs_device_ops ntfs_dev_operations = {
	NULL,
	disk_initialize,
	disk_status,
	disk_read,
	disk_write,
	disk_ioctl,
};


int diskpart_create(void *fd, struct rsdisk_header *hdr);
uint64_t get_diskpart_startpos(void* fd, int part);

/*
BOOL APIENTRY DllMain( HMODULE hModule,
		      DWORD  ul_reason_for_call,
		      LPVOID lpReserved
		      )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			vdisk_init();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			vdisk_uninit();
		}
		break;
	}
	return TRUE;
}

*/
vdisk_error vdisk_init()
{
	if ( b_g_init_cont == 0)
	{
		FRESULT res;

		res = fat_register_io_ops( &dev_operations );
		if(res != FR_OK) {
			return VDISK_ERROR_UNSUCCESSFUL;
		}

		ntfs_register_io_ops( &ntfs_dev_operations );

	
	}
	
	b_g_init_cont++;
	return VDISK_ERROR_SUCCESS;
}

vdisk_error vdisk_uninit()
{
	if ( b_g_init_cont )
	{
		b_g_init_cont--;
		if ( b_g_init_cont == 0 )
		{
			fat_unregister_io_ops();
		}
	}
	

	return VDISK_ERROR_SUCCESS;
}

vdisk_error vdisk_mount( char *diskfile, bool_t readonly, char *key, uint32_t userid, char drv_letter )
{
	vdisk_error rc = VDISK_ERROR_SUCCESS;
	struct vdisk_info *vdi = NULL;
	struct rsdisk_header *hdr = NULL;
	uint8_t *tmpbuf = NULL;
	void *fd = NULL;
	BOOLEAN brwtype = FALSE;
	char drvletter[2] = "\0";
	char fullpath[MAX_PATH] = "\0";
	unsigned long aligned_hdrsize = 0;
	HANDLE	cdo = INVALID_HANDLE_VALUE;
	size_t	retlen = 0;
	char devname[255] = "\0";
	char volname[] = " :";
	char userkey[128] = "\0";
	char filepath[MAX_PATH] = { 0 };

	//add by magj
	int nDiskNum = 0;
	int nPartNum = 0;
	int disktype = RSDISK_TYPE_DISKPART;
	//end add

	if (diskfile == NULL || key == NULL) 
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	strncpy( userkey, key, 128 );

	drvletter[0] = drv_letter;
	_strupr( drvletter );

	if (drvletter[0] > 'Z' || drvletter[0] < 'A' )
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	//add by maj
	strcpy(filepath, diskfile);
	if ( diskfile[2] != ':')
	{
		char szDiskPath[MAX_PATH] = { 0 };
		int nNum = 0;
		strcpy_s(szDiskPath, MAX_PATH, filepath);
		strlwr(szDiskPath);
		nNum = sscanf_s(szDiskPath, "\\\\.\\Harddisk%d\\Partition%d", &nDiskNum, &nPartNum);
		if ( nNum != 2 )
		{
			return VDISK_ERROR_INVALID_PARAMETER;
		}

		
		sprintf_s(fullpath, "\\\\.\\PhysicalDrive%d", nDiskNum);
	}
	else
	{
		if (0 == SearchPath( NULL, filepath, NULL, MAX_PATH, fullpath, NULL )) 
		{
			return VDISK_ERROR_FILE_NOT_FOUND;
		}

		disktype = RSDISK_TYPE_FIXED;
	}

	//end add

	
	if ( disktype != RSDISK_TYPE_DISKPART)
	{
		aligned_hdrsize = sizeof(struct rsdisk_header);
		aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);
	}
	else
	{
		aligned_hdrsize = VDISK_SECTOR_SIZE;
	}
	

	do {
		/* 打开磁盘文件 */
		fd = vdisk_file_open( fullpath, O_RDWR );
		if (fd == NULL) {
			fd = vdisk_file_open( fullpath, O_RDONLY );
		}

		if (fd == NULL) 
		{
			rc = VDISK_ERROR_FILE_NOT_FOUND; break;
		}

		/* 分配并 初始化 vdi 结构 */
		vdi = malloc( sizeof(struct vdisk_info) + strlen( fullpath ) + 20 );
		if (!vdi) {
			rc = VDISK_ERROR_INSUFFICIENT_RESOURCES;
			break;
		}
		memset( vdi, 0, sizeof(struct vdisk_info) + strlen( fullpath ) + 20 );

		strcpy(vdi->filename, diskfile);
		vdi->filename_len = (USHORT)strlen( vdi->filename );
		vdi->readonly = readonly;
		vdi->userid = userid;
		vdi->letter = drvletter[0] - 'A';

		tmpbuf = malloc( aligned_hdrsize );
		if (tmpbuf == NULL) 
		{
			rc = VDISK_ERROR_INSUFFICIENT_RESOURCES;
			break;
		}

		memset( tmpbuf, 0, aligned_hdrsize );
		hdr = (struct rsdisk_header *)tmpbuf;

		if ( disktype != RSDISK_TYPE_DISKPART)
		{
			vdisk_file_lseek( fd, 0, SEEK_SET );
		}
		else
		{
			struct rsdisk_header h = {0 };
			if ( get_diskpart_info(fd, &h) != 0 )
			{
				return VDISK_ERROR_INVALID_PARAMETER;
			}
			
			vdisk_file_lseek( fd, h.p_hdr.start, SEEK_SET );
		}
		
		vdisk_file_read( fd, tmpbuf, aligned_hdrsize );
		vdisk_file_close( fd ); fd = NULL;

		/* 验证密码 */
		if (hdr->magic != RSDISK_MAGIC || hdr->version != RSDISK_VERSION) 
		{
			rc = VDISK_ERROR_UNSUCCESSFUL; break;
		}

		if (hdr->cryptmagic != hdr->magic) 
		{

			if (AES_set_encrypt_key( userkey, 128, &vdi->encrypt_key )) {
				rc = VDISK_ERROR_UNSUCCESSFUL; break;
			}

			if (AES_set_decrypt_key( userkey, 128, &vdi->decrypt_key )) {
				rc = VDISK_ERROR_UNSUCCESSFUL; break;
			}

			encrypt_aes_blocks( tmpbuf + 16, tmpbuf + 16, aligned_hdrsize - 16, 0, &vdi->decrypt_key );

			if (hdr->cryptmagic != hdr->magic) {
				rc = VDISK_ERROR_INVALID_KEY; break;
			}
		}

		/* 向驱动提交挂载请求 */
		cdo = CreateFileW( DEVICE_WIN32_NAME,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (cdo == INVALID_HANDLE_VALUE)
		{
			return VDISK_ERROR_INVALID_DEVICE;
		}

		if (!DeviceIoControl( cdo, IOCTL_VDISK_CREATE_MOUNT_POINT, vdi, sizeof(struct vdisk_info) + vdi->filename_len - 1,  devname, 255, &retlen, NULL )) 
		{
			rc = VDISK_ERROR_UNSUCCESSFUL; 
			break;
		}

		CloseHandle( cdo ); cdo = NULL;

		volname[0] = vdi->letter + 'A';
		if (!DefineDosDevice( DDD_RAW_TARGET_PATH, volname, devname )) 
		{
			return VDISK_ERROR_UNSUCCESSFUL;
		}

	} while (FALSE);

	if (vdi != NULL)
		free( vdi );

	if (tmpbuf != NULL)
		free( tmpbuf );

	if (cdo != INVALID_HANDLE_VALUE)
		CloseHandle( cdo );

	if (fd != NULL)
		vdisk_file_close( fd );

	return rc;
}

vdisk_error vdisk_umount( uint32_t userid, char drv_letter )
{
	vdisk_error rc = VDISK_ERROR_SUCCESS;
	char volname[] = "\\\\.\\ :";
	HANDLE vdo;
	DWORD BytesReturned;
	struct vdisk_info vdi;


	volname[4] = drv_letter;
	_strupr( volname );

	if (volname[4] > 'Z' || volname[4] < 'A' ) {
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	vdo = CreateFile( volname,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL );

	if (vdo == INVALID_HANDLE_VALUE) {
		return VDISK_ERROR_INVALID_DEVICE;
	}

	if (!DeviceIoControl(vdo,
		FSCTL_LOCK_VOLUME,
		NULL, 0, NULL, 0, &BytesReturned, NULL)) {
			CloseHandle(vdo);
			return VDISK_ERROR_UNSUCCESSFUL;
	}

	if (!DeviceIoControl(vdo,
		FSCTL_DISMOUNT_VOLUME,
		NULL, 0, NULL, 0, &BytesReturned, NULL)) {
			CloseHandle(vdo);
			return VDISK_ERROR_UNSUCCESSFUL;
	}
	if (!DeviceIoControl(vdo,
		FSCTL_UNLOCK_VOLUME,
		NULL, 0, NULL, 0, &BytesReturned, NULL)) {
			CloseHandle(vdo);
			return VDISK_ERROR_UNSUCCESSFUL;
	}
	CloseHandle(vdo);

	if (!DefineDosDevice(DDD_REMOVE_DEFINITION, &volname[4], NULL)) {
		return VDISK_ERROR_UNSUCCESSFUL;
	}

	/* 卸载 */
	vdo = CreateFileW(
		DEVICE_WIN32_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (vdo == INVALID_HANDLE_VALUE) {		
		return VDISK_ERROR_INVALID_DEVICE;
	}

	vdi.letter = volname[4] - 'A';
	vdi.userid = userid;

	if (!DeviceIoControl(vdo, IOCTL_VDISK_DELETE_MOUNT_POINT, &vdi, sizeof(struct vdisk_info), NULL, 0, &BytesReturned, NULL)) {

		CloseHandle(vdo);			
		return VDISK_ERROR_UNSUCCESSFUL;
	}
	CloseHandle( vdo );

	return rc;
}

// 分区类型 \\\\.\\Harddisk2\Partition1
vdisk_error vdisk_create( char *diskfile, uint64_t disksize, bool_t is_encryt, char *key, bool_t is_dyndisk )
{
	int disktypeid = RSDISK_TYPE_FIXED;
	int encryptid = RSDISK_CRYPT_NONE;
	struct rsdisk_header hdr;
	AES_KEY encrypt_key;

	int ret = -EINVAL;
	void *fd;
	uint8_t *inbuf;
	uint8_t *outbuf;
	unsigned long aligned_hdrsize;
	char userkey[128] = "\0";
	

	//add by magj
	char filepath[MAX_PATH] = { 0 };
	int nDiskNum = 0;
	int nPartNum = 0;

	
	if (diskfile == NULL || disksize == 0) {
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	strcpy_s(filepath, MAX_PATH, diskfile);
	if ( diskfile[1] != ':')
	{
		char szDiskPath[MAX_PATH] = { 0 };
		int nNum = 0;
		strcpy_s(szDiskPath, MAX_PATH, diskfile);
		strlwr(szDiskPath);
		nNum = sscanf_s(szDiskPath, "\\\\.\\Harddisk%d\Partition%d", &nDiskNum, &nPartNum);
		if ( nNum != 2 )
		{
			return VDISK_ERROR_INVALID_PARAMETER;
		}

		sprintf_s(filepath, "\\\\.\\PhysicalDrive%d", nDiskNum);
		disktypeid = RSDISK_TYPE_DISKPART;
		is_dyndisk = 0;
	}
	
	if ( key )
	{
		strncpy( userkey, key, 128 );
	}
	

	if ( disktypeid == RSDISK_TYPE_DISKPART)
	{
		aligned_hdrsize = VDISK_SECTOR_SIZE;
	}
	else
	{
		aligned_hdrsize = sizeof(struct rsdisk_header);
		aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);
		disktypeid = is_dyndisk ? RSDISK_TYPE_DYNAMIC : RSDISK_TYPE_FIXED;
	}

	
	encryptid = is_encryt ? RSDISK_CRYPT_AES : RSDISK_CRYPT_NONE;

	memset( &encrypt_key, 0, sizeof(AES_KEY) );
	// fill rs disk herader
	memset(&hdr, 0, sizeof(struct rsdisk_header));
	hdr.magic = hdr.cryptmagic = RSDISK_MAGIC;
	hdr.version = RSDISK_VERSION;
	hdr.crypt_method = encryptid;

	if (hdr.crypt_method < RSDISK_CRYPT_NONE || hdr.crypt_method > RSDISK_CRYPT_AES) 
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	hdr.disk_type = disktypeid;
	if (hdr.disk_type < RSDISK_TYPE_FIXED || hdr.disk_type > RSDISK_TYPE_DISKPART) 
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	// allocate file object
	fd = vdisk_file_open( filepath, O_WRONLY | O_CREAT | O_TRUNC );
	if (!fd) 
	{
		return VDISK_ERROR_FILE_NOT_FOUND;
	}

	inbuf = malloc(aligned_hdrsize);
	if (!inbuf) 
	{
		return VDISK_ERROR_INSUFFICIENT_RESOURCES;
	}

	outbuf = malloc(aligned_hdrsize);
	if (!outbuf) 
	{
		return VDISK_ERROR_INSUFFICIENT_RESOURCES;
	}

	memset(inbuf, 0, aligned_hdrsize);
	memset(outbuf, 0, aligned_hdrsize);

	if (hdr.disk_type == RSDISK_TYPE_FIXED) 
	{
		ret = vdisk_file_truncate(fd, (disksize + 1) * VDISK_SECTOR_SIZE);	//first sector for ourself
	} 
	else if (hdr.disk_type == RSDISK_TYPE_DYNAMIC) 
	{
		hdr.d_hdr.size = disksize * VDISK_SECTOR_SIZE;
		ret = dyndisk_create(fd, &hdr);
	}
	else if ( hdr.disk_type == RSDISK_TYPE_DISKPART )
	{
		hdr.p_hdr.disknum = nDiskNum;
		hdr.p_hdr.partnum = nPartNum;
		ret = diskpart_create(fd, &hdr);
	}

	if (ret == 0) 
	{
		memcpy(inbuf, &hdr, sizeof(struct rsdisk_header));
		memcpy(outbuf, &hdr, sizeof(struct rsdisk_header));
		if (hdr.crypt_method == RSDISK_CRYPT_AES) 
		{
			if (AES_set_encrypt_key(userkey, 128, &encrypt_key)) 
			{
				vdisk_file_close(fd);
				free(inbuf);
				free(outbuf);
				return VDISK_ERROR_UNSUCCESSFUL;
			}
			encrypt_aes_blocks(outbuf + 16, inbuf + 16,aligned_hdrsize - 16, 1, &encrypt_key);
		}

		if ( hdr.disk_type == RSDISK_TYPE_DISKPART)
			vdisk_file_lseek(fd, hdr.p_hdr.start, SEEK_SET);
		else
			vdisk_file_lseek(fd, 0, SEEK_SET);

		vdisk_file_write(fd, outbuf, aligned_hdrsize);
	}

	vdisk_file_close(fd);
	free(inbuf);
	free(outbuf);
	return (ret == 0) ? VDISK_ERROR_SUCCESS : VDISK_ERROR_UNSUCCESSFUL;

}

vdisk_error vdisk_mout_diskpart(int nDiskNum, int part, bool_t is_encryt, char *key)
{
	int disktypeid = RSDISK_TYPE_FIXED;
	int encryptid = RSDISK_CRYPT_NONE;
	struct rsdisk_header hdr;
	AES_KEY encrypt_key;
	HANDLE hDiskHandle = INVALID_HANDLE_VALUE;
	int ret = -EINVAL;
	void *fd;
	uint8_t *inbuf;
	uint8_t *outbuf;
	unsigned long aligned_hdrsize;
	char userkey[128] = "\0";
	struct _PART_LAYOUT_INFO layoutInfo = { 0 };
	DWORD    nDiskBytesRead = 0;
	BOOL  bRet = FALSE;

	uint64_t start = 0;
	uint64_t disksize = 0;
	unsigned long dwLoop = 0;
	char szDrivePath[MAX_PATH] = {0};
	sprintf_s( szDrivePath, MAX_PATH - 1, "\\\\.\\PhysicalDrive%d", nDiskNum );
	hDiskHandle = CreateFileA(szDrivePath, GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);

	if (hDiskHandle == INVALID_HANDLE_VALUE) {
		return VDISK_ERROR_INVALID_DEVICE;
	}


	
	bRet = DeviceIoControl(szDrivePath,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID) &layoutInfo,(DWORD) sizeof(layoutInfo),(LPDWORD) &nDiskBytesRead,NULL);
	if ( !bRet )
	{
		CloseHandle(hDiskHandle);
		return VDISK_ERROR_INVALID_DEVICE;
	}


	bRet = FALSE;
	for ( dwLoop = 0 ; dwLoop < layoutInfo.layout->PartitionCount ; dwLoop++)
	{
		PPARTITION_INFORMATION_EX tPartInfo = &layoutInfo.layout->PartitionEntry[dwLoop];
		if ( tPartInfo->PartitionNumber == part)
		{
			start = tPartInfo->StartingOffset.QuadPart;
			disksize = tPartInfo->PartitionLength.QuadPart;
			bRet = TRUE;
			break;
		}
	}


	if ( !bRet )
	{
		CloseHandle(hDiskHandle);
		return VDISK_ERROR_INVALID_DEVICE;
	}

	CloseHandle(hDiskHandle);

	



	strncpy( userkey, key, 128 );

	aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	memset( &encrypt_key, 0, sizeof(AES_KEY) );

	disktypeid = RSDISK_TYPE_DISKPART;

	encryptid = is_encryt ? RSDISK_CRYPT_AES : RSDISK_CRYPT_NONE;

	// fill rs disk herader
	memset(&hdr, 0, sizeof(struct rsdisk_header));
	hdr.magic = hdr.cryptmagic = RSDISK_MAGIC;
	hdr.version = RSDISK_VERSION;
	hdr.crypt_method = encryptid;
	if (hdr.crypt_method < RSDISK_CRYPT_NONE || hdr.crypt_method > RSDISK_CRYPT_AES) 
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	hdr.disk_type = disktypeid;

	if (hdr.disk_type < RSDISK_TYPE_FIXED || hdr.disk_type > RSDISK_TYPE_DYNAMIC) 
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	// allocate file object
	fd = vdisk_file_open( szDrivePath, O_WRONLY | O_CREAT | O_TRUNC );
	if (!fd) {
		return VDISK_ERROR_FILE_NOT_FOUND;
	}

	inbuf = malloc(aligned_hdrsize);
	if (!inbuf) {
		return VDISK_ERROR_INSUFFICIENT_RESOURCES;
	}
	outbuf = malloc(aligned_hdrsize);
	if (!outbuf) {
		return VDISK_ERROR_INSUFFICIENT_RESOURCES;
	}
	memset(inbuf, 0, aligned_hdrsize);
	memset(outbuf, 0, aligned_hdrsize);


	if (ret == 0) {
		memcpy(inbuf, &hdr, sizeof(struct rsdisk_header));
		memcpy(outbuf, &hdr, sizeof(struct rsdisk_header));
		if (hdr.crypt_method == RSDISK_CRYPT_AES) {

			if (AES_set_encrypt_key(userkey, 128, &encrypt_key)) {
				vdisk_file_close(fd);
				free(inbuf);
				free(outbuf);
				return VDISK_ERROR_UNSUCCESSFUL;
			}
			encrypt_aes_blocks(outbuf + 16, inbuf + 16,
				aligned_hdrsize - 16, 1, &encrypt_key);
		}
		vdisk_file_lseek(fd, start, SEEK_SET);
		vdisk_file_write(fd, outbuf, aligned_hdrsize);
	}

	vdisk_file_close(fd);
	free(inbuf);
	free(outbuf);

	
	return (ret == 0) ? VDISK_ERROR_SUCCESS : VDISK_ERROR_UNSUCCESSFUL;
}



vdisk_error vdisk_set_user_data( uint32_t userid, uint8_t *user_data, size_t user_data_len )
{
	vdisk_error retval = VDISK_ERROR_SUCCESS;
	char VolumeName[] = " :";
	char DeviceName[255] = "\0";
	HANDLE cdo = INVALID_HANDLE_VALUE;
	DWORD BytesReturned;
	char drvletter[2] = "\0";
	size_t	vud_len = 0;
	struct vdisk_user_data	*vud = NULL;

	if (user_data == NULL || user_data_len == 0) {
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	do {
		cdo = CreateFileW( 
			DEVICE_WIN32_NAME, 
			GENERIC_READ | GENERIC_WRITE, 
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (cdo == INVALID_HANDLE_VALUE) {
			retval = VDISK_ERROR_INVALID_DEVICE;
			break;
		}

		vud_len = sizeof(struct vdisk_user_data) + user_data_len - 1;
		vud = malloc( vud_len );
		if (vud == NULL) {
			retval = VDISK_ERROR_INSUFFICIENT_RESOURCES;
			break;
		}

		vud->data_len = user_data_len;
		vud->userid = userid;
		memcpy( &vud->data, user_data, user_data_len );

		if (!DeviceIoControl( cdo, IOCTL_VDISK_SET_USER_DATA, vud, vud_len, NULL, 0, &BytesReturned, NULL )) {
			retval = VDISK_ERROR_UNSUCCESSFUL;
			break;
		}

	} while (FALSE);

	if (cdo != INVALID_HANDLE_VALUE)
		CloseHandle( cdo );

	if (vud != NULL)
		free( vud );

	return retval;
}

vdisk_error vdisk_get_user_data( uint32_t userid, uint8_t *user_data, size_t user_data_len )
{
	vdisk_error retval = VDISK_ERROR_SUCCESS;
	char VolumeName[] = " :";
	char DeviceName[255] = "\0";
	HANDLE cdo = INVALID_HANDLE_VALUE;
	DWORD BytesReturned;
	char drvletter[2] = "\0";
	size_t	vud_len = 0;
	struct vdisk_user_data	*vud = NULL;

	if (user_data == NULL || user_data_len == 0) {
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	do {
		cdo = CreateFileW(
			DEVICE_WIN32_NAME,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (cdo == INVALID_HANDLE_VALUE) {
			retval = VDISK_ERROR_INVALID_DEVICE;
			break;
		}

		vud_len = sizeof(struct vdisk_user_data) + user_data_len - 1;
		vud = malloc( vud_len );
		if (vud == NULL) {
			retval = VDISK_ERROR_INSUFFICIENT_RESOURCES;
			break;
		}

		memset( vud, 0, vud_len );
		vud->userid = userid;

		if (!DeviceIoControl( cdo, IOCTL_VDISK_GET_USER_DATA, vud, vud_len, user_data, user_data_len, &BytesReturned, NULL )) {
			retval = VDISK_ERROR_UNSUCCESSFUL;
			break;
		}

	} while (FALSE);



	if (cdo != NULL)
		CloseHandle( cdo );

	if (vud != NULL)
		free( vud );

	return retval;
}

vdisk_error vdisk_get_info( struct vdisk_info_entry *info, size_t innr, size_t *outnr )
{
	int retval = VDISK_ERROR_SUCCESS;
	char VolumeName[] = " :";
	char DeviceName[255] = "\0";
	HANDLE cdo = INVALID_HANDLE_VALUE;
	DWORD BytesReturned;
	char drvletter[2] = "\0";
	void *buf = NULL;
	struct vdisk_info_ex *vi = NULL;
	size_t nr, info_nr = 0;
	size_t max_nr = 0;

	if (outnr == NULL || info == NULL) {
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	*outnr = 0;

	do {
		cdo = CreateFileW(
			DEVICE_WIN32_NAME,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (cdo == INVALID_HANDLE_VALUE) {
			retval = VDISK_ERROR_INVALID_DEVICE;
			break;
		}

		max_nr = innr;

		do {

			nr = max_nr;

			if (buf != NULL) {
				free( buf ); buf = NULL;
			}

			buf = malloc( sizeof(size_t) + (sizeof(struct vdisk_info_ex) * nr) );
			if (buf == NULL) {
				retval = VDISK_ERROR_INSUFFICIENT_RESOURCES;
				break;
			}

			if (!DeviceIoControl( cdo, IOCTL_VDISK_QUERY_INFO, NULL, 0, buf, sizeof(size_t) + (sizeof(struct vdisk_info_ex) * nr), &BytesReturned, NULL )) {
				retval = VDISK_ERROR_UNSUCCESSFUL;
				break;
			}

			max_nr = *(size_t *)buf;

		} while (max_nr > nr);

		vi = (struct vdisk_info_ex *)((uint8_t *)buf + sizeof(size_t));

		for (nr = 0, info_nr = 0; nr < max_nr; nr++) {

			uint32_t i;
			for (i = 0; i < 26; i++) {
				if ((vi[nr].drvmap & (1 << i)) != 0) {

					if (info_nr >= innr) {
						(*outnr)++;
						continue;
					}
					
					if (strncmp( vi[nr].filename, "\\??\\", 4 ) == 0) {
						strcpy( info[info_nr].diskfile, &vi[nr].filename[4] );
					} else {
						strcpy( info[info_nr].diskfile, vi[nr].filename );
					}
					
					info[info_nr].local_time = vi[nr].local_time;
					info[info_nr].userid = vi[nr].userid;
					info[info_nr].drv_letter = i + 'A';
					info_nr++;
					(*outnr)++;
				}
			}
		}

	} while (FALSE);

	if (buf != NULL)
		free( buf );


	if (cdo != NULL)
		CloseHandle( cdo );

	if (info_nr < *outnr) {
		retval = VDISK_ERROR_BUFFER_TOO_SMALL;
	}

	return retval;
}

/************************************************************************/
/*                               文件操作                               */
/************************************************************************/

DWORD get_fattime (void)
{
	SYSTEMTIME tm;

	/* Get local time */
	GetLocalTime(&tm);

	/* Pack date and time into a DWORD variable */
	return 	  ((DWORD)(tm.wYear - 1980) << 25)
		| ((DWORD)tm.wMonth << 21)
		| ((DWORD)tm.wDay << 16)
		| (WORD)(tm.wHour << 11)
		| (WORD)(tm.wMinute << 5)
		| (WORD)(tm.wSecond >> 1);
}


vdisk_error vdisk_open( char *diskfile, char *key, void **dev )
{
	vdisk_error rc = VDISK_ERROR_SUCCESS;
	struct vdisk_dev *vdi = NULL;
	struct rsdisk_header *hdr = NULL;
	uint8_t *tmpbuf = NULL;
	void *fd = NULL;
	char fullpath[MAX_PATH] = "\0";
	unsigned long aligned_hdrsize = 0;
	char userkey[128] = "\0";
	AES_KEY encrypt_key = {0};
	AES_KEY decrypt_key = {0};

	if (diskfile == NULL  || dev == NULL) {
		return VDISK_ERROR_INVALID_PARAMETER;
	}

	if ( key )
	{
		strncpy( userkey, key, 128 );
	}
	

	if (0 == SearchPath( NULL, diskfile, NULL, MAX_PATH, fullpath, NULL )) {
		return VDISK_ERROR_FILE_NOT_FOUND;
	}

	aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	do {

		/* 打开磁盘文件 */
		fd = vdisk_file_open( fullpath, O_RDWR );
		if (fd == NULL) {
			fd = vdisk_file_open( fullpath, O_RDONLY );
		}

		if (fd == NULL) {
			rc = VDISK_ERROR_FILE_NOT_FOUND; break;
		}

		/* 分配并 初始化 vdi 结构 */
		vdi = malloc( sizeof(struct vdisk_dev) );
		if (!vdi) {
			rc = VDISK_ERROR_INSUFFICIENT_RESOURCES;
			break;
		}
		memset( vdi, 0, sizeof(struct vdisk_dev) );

		tmpbuf = malloc( aligned_hdrsize );
		if (tmpbuf == NULL) {
			rc = VDISK_ERROR_INSUFFICIENT_RESOURCES;
			break;
		}
		memset( tmpbuf, 0, aligned_hdrsize );
		hdr = (struct rsdisk_header *)tmpbuf;

		vdisk_file_lseek( fd, 0, SEEK_SET );
		vdisk_file_read( fd, tmpbuf, aligned_hdrsize );

		/* 验证密码 */
		if (hdr->magic != RSDISK_MAGIC || hdr->version != RSDISK_VERSION) {
			rc = VDISK_ERROR_UNSUCCESSFUL; break;
		}

		if (hdr->cryptmagic != hdr->magic) {

			if (AES_set_encrypt_key( userkey, 128, &encrypt_key )) {
				rc = VDISK_ERROR_UNSUCCESSFUL; break;
			}

			if (AES_set_decrypt_key( userkey, 128, &decrypt_key )) {
				rc = VDISK_ERROR_UNSUCCESSFUL; break;
			}

			encrypt_aes_blocks( tmpbuf + 16, tmpbuf + 16, aligned_hdrsize - 16, 0, &decrypt_key );

			if (hdr->cryptmagic != hdr->magic) {
				rc = VDISK_ERROR_INVALID_KEY; break;
			}
		}

		vdi->disk_type = hdr->disk_type;

		switch (vdi->disk_type) {

		case RSDISK_TYPE_DYNAMIC:
			{
				vdi->dd_stat.fd = fd;
				memcpy(&vdi->dd_stat.aes_encrypt_key, &encrypt_key, sizeof(AES_KEY));
				memcpy(&vdi->dd_stat.aes_decrypt_key, &decrypt_key, sizeof(AES_KEY));
				rc = dyndisk_open( &vdi->dd_stat, hdr );
				if (rc < 0) {
					rc = VDISK_ERROR_UNSUCCESSFUL; break;
				}
			}
			break;
		case RSDISK_TYPE_FIXED:
			{
				vdi->sd_stat.fd = fd;
				vdi->sd_stat.crypt_method = hdr->crypt_method;
				vdi->sd_stat.crypt_method_header = vdi->sd_stat.crypt_method;
				memcpy(&vdi->sd_stat.aes_encrypt_key, &encrypt_key, sizeof(AES_KEY));
				memcpy(&vdi->sd_stat.aes_decrypt_key, &decrypt_key, sizeof(AES_KEY));
				vdi->filesize.QuadPart = vdisk_file_lseek(fd, 0, 2);
			}
			break;
		default:
			break;
		}

		*(struct vdisk_dev **)dev = vdi;

	} while (FALSE);

	if (tmpbuf != NULL)
		free( tmpbuf );

	if (rc != VDISK_ERROR_SUCCESS) {

		if (vdi != NULL)
			free( vdi );

		if (fd != NULL)
			vdisk_file_close( fd );
	}

	return rc;
}

void vdisk_close( void *dev )
{
	struct vdisk_dev *vd = dev;

	if (dev == NULL)
		return;

	switch (vd->disk_type) {

	case RSDISK_TYPE_DYNAMIC:
		{
			vdisk_file_close( vd->dd_stat.fd );
		}
		break;
	case RSDISK_TYPE_FIXED:
		{
			vdisk_file_close( vd->sd_stat.fd );
		}
		break;
	default:
		break;
	}

	free( vd );
}

struct vdisk_fd {
	void *fd;
	fmt_type type;
};

void *vdisk_fopen( void *dev, char *filename, unsigned char mode )
{
	void *res = NULL;
	struct vdisk_fd *fd = NULL;
	unsigned char new_mode = 0;

	if (dev == NULL || filename == NULL)
		return NULL;

	fd = malloc( sizeof(struct vdisk_fd) );
	if (fd == NULL) {
		return NULL;
	}

	if (ntfs_rec( dev )) {

		if (mode == VDISK_CREATE_NEW) {
			new_mode = NTFS_CREATE_NEW;
		} else if (mode == VDISK_OPEN_EXISTING) {
			new_mode = NTFS_OPEN_EXISTING;
		}

		res = ntfs_fopen( dev, filename, new_mode );
		fd->type = fmt_type_ntfs;

	} else if (fat_rec( dev )){
		if (mode == VDISK_CREATE_NEW) {
			new_mode = FAT_CREATE_NEW;
		} else if (mode == VDISK_OPEN_EXISTING) {
			new_mode = FAT_OPEN_EXISTING;
		}
		new_mode |= (FAT_READ | FAT_WRITE);
		res = fat_fopen( dev, filename, new_mode );
		fd->type = fmt_type_fat32;
	}

	if (res != NULL) {
		
		fd->fd = res;
		return fd;
	} else {
		free( fd );
		return NULL;
	}
}

void vdisk_fclose( void *file )
{
	struct vdisk_fd *fd = (struct vdisk_fd *)file;

	if (fd == NULL)
		return;

	if (fd->type == fmt_type_ntfs) {
		ntfs_fclose( fd->fd );
	} else if (fd->type == fmt_type_fat32) {
		fat_fclose( fd->fd );
	}

	free( file );
}

int vdisk_flseek( void *file, int offset, int whence )
{
	struct vdisk_fd *fd = (struct vdisk_fd *)file;

	if (file == NULL)
		return 0;

	if (fd->type == fmt_type_ntfs) {
		return ntfs_flseek( fd->fd, offset, whence );
	} else if (fd->type == fmt_type_fat32) {
		return fat_flseek( fd->fd, offset, whence );
	} else {
		return 0;
	}
}

int vdisk_fread( void *file, void *buffer, unsigned int bufsize )
{
	struct vdisk_fd *fd = (struct vdisk_fd *)file;

	if (file == NULL || buffer == NULL || bufsize == 0)
		return 0;

	if (fd->type == fmt_type_ntfs) {
		return ntfs_fread( fd->fd, buffer, bufsize );
	} else if (fd->type == fmt_type_fat32) {
		return fat_fread( fd->fd, buffer, bufsize );
	} else {
		return 0;
	}
}

int vdisk_fwrite( void *file, void *buffer, unsigned int bufsize )
{
	struct vdisk_fd *fd = (struct vdisk_fd *)file;

	if (file == NULL || buffer == NULL || bufsize == 0)
		return 0;

	if (fd->type == fmt_type_ntfs) {
		return ntfs_fwrite( fd->fd, buffer, bufsize );
	} else if (fd->type == fmt_type_fat32) {
		return fat_fwrite( fd->fd, buffer, bufsize );
	} else {
		return 0;
	}
}

vdisk_error vdisk_mkdir(void *dev, char * path)
{
	return fat_mkdir( dev, path );
}

vdisk_error vdisk_chdir(void *dev, char * path)
{
	return fat_chdir( dev, path );
}

vdisk_error vdisk_remove(void *dev, char * name)
{
	return fat_remove( dev, name );
}

vdisk_error vdisk_rename(void *dev, char * oldname, char * newname)
{
	return fat_rename( dev, oldname, newname );
}

vdisk_error vdisk_format( void *dev, fmt_type type )
{
	if (type == fmt_type_fat32)
		return (fat_format( dev, PR_SFD, 512 ) == 0) ? VDISK_ERROR_SUCCESS : VDISK_ERROR_UNSUCCESSFUL;
	else if (type == fmt_type_ntfs)
		return (ntfs_format( dev ) == 0) ? VDISK_ERROR_SUCCESS : VDISK_ERROR_UNSUCCESSFUL;

	return VDISK_ERROR_INVALID_PARAMETER;
}

vdisk_error vdisk_clear_diskpart(int nDiskNum, int part)
{
	uint64_t start = 0;
	uint64_t disksize = 0;
	unsigned long dwLoop = 0;
	char szDrivePath[MAX_PATH] = {0};
	HANDLE hDiskHandle = INVALID_HANDLE_VALUE;
	struct _PART_LAYOUT_INFO layoutInfo = { 0 };
	BOOL	bRet = FALSE;
	DWORD nDiskBytesRead = 0;

	sprintf_s( szDrivePath, MAX_PATH - 1, "\\\\.\\PhysicalDrive%d", nDiskNum );
	hDiskHandle = CreateFileA(szDrivePath, GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);

	if (hDiskHandle == INVALID_HANDLE_VALUE) {
		return VDISK_ERROR_INVALID_DEVICE;
	}

	bRet = DeviceIoControl(hDiskHandle,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID) &layoutInfo,(DWORD) sizeof(layoutInfo),(LPDWORD) &nDiskBytesRead,NULL);
	if ( !bRet )
	{
		CloseHandle(hDiskHandle);
		return VDISK_ERROR_INVALID_DEVICE;
	}


	bRet = FALSE;
	for ( dwLoop = 0 ; dwLoop < layoutInfo.layout->PartitionCount ; dwLoop++)
	{
		PPARTITION_INFORMATION_EX tPartInfo = &layoutInfo.layout->PartitionEntry[dwLoop];
		if ( tPartInfo->PartitionNumber == part)
		{
			start = tPartInfo->StartingOffset.QuadPart;
			disksize = tPartInfo->PartitionLength.QuadPart;
			bRet = TRUE;
			break;
		}
	}

	if ( !bRet )
	{
		CloseHandle(hDiskHandle);
		return VDISK_ERROR_FILE_NOT_FOUND; 
	}

	{
		BYTE SECTOR[VDISK_SECTOR_SIZE] = { 0 };
		LARGE_INTEGER FilePoint;
		FilePoint.QuadPart = start;
		SetFilePointer(hDiskHandle, FilePoint.LowPart, &FilePoint.HighPart, FILE_BEGIN);
		//DeviceIoControl(hDiskHandle, FSCTL_LOCK_VOLUME, 0,0,0,0,&nDiskBytesRead, NULL);
		bRet = WriteFile(hDiskHandle, SECTOR, VDISK_SECTOR_SIZE, &nDiskBytesRead, NULL);
		//DeviceIoControl(hDiskHandle, FSCTL_UNLOCK_VOLUME, 0,0,0,0,&nDiskBytesRead, NULL);
	}
	
	if ( !bRet )
	{
		CloseHandle(hDiskHandle);
		return VDISK_ERROR_UNSUCCESSFUL;
	}

	CloseHandle(hDiskHandle);

	return VDISK_ERROR_SUCCESS;
}

int diskpart_create(void *fd, struct rsdisk_header *hdr)
{
	int rc = VDISK_ERROR_UNSUCCESSFUL;
	struct _PART_LAYOUT_INFO layoutInfo = { 0 };
	DWORD nDiskBytesRead = 0;
	DWORD dwLoop = 0;
	do 
	{
		
		if ( DeviceIoControl(fd,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID) &layoutInfo,(DWORD) sizeof(layoutInfo),(LPDWORD) &nDiskBytesRead,NULL) )
		{
			rc = VDISK_ERROR_UNSUCCESSFUL;
			break;
		}

		for ( dwLoop = 0 ; dwLoop < layoutInfo.layout->PartitionCount ; dwLoop++)
		{
			PPARTITION_INFORMATION_EX tPartInfo = &layoutInfo.layout->PartitionEntry[dwLoop];
			if ( tPartInfo->PartitionNumber == hdr->p_hdr.partnum)
			{
				hdr->p_hdr.start= tPartInfo->StartingOffset.QuadPart;
				hdr->p_hdr.size = tPartInfo->PartitionLength.QuadPart;
				rc = VDISK_ERROR_SUCCESS;
				break;
			}
		}

		//清空第一个扇区
		if ( rc == VDISK_ERROR_SUCCESS)
		{
			byte sector[VDISK_SECTOR_SIZE] = { 0 };
			vdisk_file_lseek(fd, hdr->p_hdr.start, 0);
			if ( !(WriteFile(fd, sector, VDISK_SECTOR_SIZE, &nDiskBytesRead, NULL) && nDiskBytesRead == VDISK_SECTOR_SIZE) )
			{
				rc = VDISK_ERROR_UNSUCCESSFUL;
				break;
			}
		}
		
	} while (0);

	return rc;
}

int get_diskpart_info(void* fd, int part, struct rsdisk_header *hdr )
{
	int rc = VDISK_ERROR_UNSUCCESSFUL;
	struct _PART_LAYOUT_INFO layoutInfo = { 0 };
	DWORD nDiskBytesRead = 0;
	DWORD dwLoop = 0;
	do 
	{
		if ( DeviceIoControl(fd,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID) &layoutInfo,(DWORD) sizeof(layoutInfo),(LPDWORD) &nDiskBytesRead,NULL) )
		{
			rc = VDISK_ERROR_UNSUCCESSFUL;
			break;
		}

		for ( dwLoop = 0 ; dwLoop < layoutInfo.layout->PartitionCount ; dwLoop++)
		{
			PPARTITION_INFORMATION_EX tPartInfo = &layoutInfo.layout->PartitionEntry[dwLoop];
			if ( tPartInfo->PartitionNumber == part)
			{
				hdr->p_hdr.start = tPartInfo->StartingOffset.QuadPart;
				hdr->p_hdr.size = tPartInfo->PartitionLength.QuadPart;
				rc = VDISK_ERROR_SUCCESS;
				break;
			}
		}

	} while (0);

	return rc;
}

vdisk_error vdisk_verify(const char* diskfile)
{
	char filepath[MAX_PATH] = { 0 };
	int nDiskNum = 0;
	int nPartNum = 0;

	if (diskfile == NULL) 
	{
		return VDISK_ERROR_INVALID_PARAMETER;
	}


}

vdisk_error vdisk_clear(const char* diskfile)
{

}

/*
vdisk_error vdisk_verify_diskpart(int nDiskNum, int part,bool_t is_encryt, char *userkey)
{
	uint64_t start = 0;
	uint64_t disksize = 0;
	unsigned long dwLoop = 0;
	char szDrivePath[MAX_PATH] = {0};
	HANDLE hDiskHandle = INVALID_HANDLE_VALUE;
	struct _PART_LAYOUT_INFO layoutInfo = { 0 };
	BOOL	bRet = FALSE;
	DWORD nDiskBytesRead = 0;

	vdisk_error rc = VDISK_ERROR_SUCCESS;

	BYTE SECTOR[VDISK_SECTOR_SIZE] = { 0 };
	struct rsdisk_header *hdr = (struct rsdisk_header *)SECTOR;

	int aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	do 
	{
		sprintf_s( szDrivePath, MAX_PATH - 1, "\\\\.\\PhysicalDrive%d", nDiskNum );
		hDiskHandle = CreateFileA(szDrivePath, GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		if (hDiskHandle == INVALID_HANDLE_VALUE) 
		{
			rc =  VDISK_ERROR_INVALID_DEVICE;
			break;
		}

		bRet = DeviceIoControl(hDiskHandle,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL,0,(LPVOID) &layoutInfo,(DWORD) sizeof(layoutInfo),(LPDWORD) &nDiskBytesRead,NULL);
		if ( !bRet )
		{
			rc =  VDISK_ERROR_INVALID_DEVICE;
			break;
		}


		bRet = FALSE;
		for ( dwLoop = 0 ; dwLoop < layoutInfo.layout->PartitionCount ; dwLoop++)
		{
			PPARTITION_INFORMATION_EX tPartInfo = &layoutInfo.layout->PartitionEntry[dwLoop];
			if ( tPartInfo->PartitionNumber == part)
			{
				start = tPartInfo->StartingOffset.QuadPart;
				disksize = tPartInfo->PartitionLength.QuadPart;
				bRet = TRUE;
				break;
			}
		}


		if ( !bRet )
		{
			rc = VDISK_ERROR_INVALID_DEVICE;
		}


		int nLoop = 0;
		bRet = ReadFile(hDiskHandle, SECTOR, VDISK_SECTOR_SIZE, &nDiskBytesRead, NULL);
		if ( !( bRet && nDiskBytesRead == VDISK_SECTOR_SIZE))
		{
			rc =  VDISK_DISKPART_ERROR;
			break;
		}

		if ( hdr->size == 0 )
		{
			rc =  VDISK_DISKPART_NO_INIT;
			break;
		}


	
		if (hdr->magic != RSDISK_MAGIC || hdr->version != RSDISK_VERSION || hdr->disk_type != RSDISK_TYPE_DISKPART) 
		{
			rc = VDISK_DISKPART_ERROR; 
			break;
		}

		if (hdr->cryptmagic != hdr->magic) 
		{

			if (AES_set_encrypt_key( userkey, 128, &vdi->encrypt_key )) {
				rc = VDISK_ERROR_UNSUCCESSFUL; break;
			}

			if (AES_set_decrypt_key( userkey, 128, &vdi->decrypt_key )) {
				rc = VDISK_ERROR_UNSUCCESSFUL; break;
			}

			encrypt_aes_blocks( tmpbuf + 16, tmpbuf + 16, aligned_hdrsize - 16, 0, &vdi->decrypt_key );

			if (hdr->cryptmagic != hdr->magic) {
				rc = VDISK_ERROR_INVALID_KEY; break;
			}

	} while (FALSE);

	

	

	


	if ( !bRet )
	{
		CloseHandle(hDiskHandle);
		return VDISK_ERROR_UNSUCCESSFUL;
	}

	CloseHandle(hDiskHandle);
}

*/