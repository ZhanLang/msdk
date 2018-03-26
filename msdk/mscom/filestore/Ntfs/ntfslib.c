#include "device.h"
#include "mkntfs.h"
#include "utils.h"
#include "dir.h"
#include "misc.h"
#include <sys/stat.h>
#include "ntfslib.h"

/* Command code for disk_ioctrl() */
#define CTRL_SYNC			0	/* Mandatory for write functions */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2	/* Mandatory for multiple sector size cfg */
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */

struct ntfs_device_ops	ntfs_dev_ops;

int ntfs_dev_open(struct ntfs_device *dev, int flags)
{
	dev->offset = 0;
	return 0;
}

int ntfs_dev_close(struct ntfs_device *dev)
{
	return 0;
}

s64 ntfs_dev_seek(struct ntfs_device *dev, s64 offset, int whence)
{

	switch (whence) {

	case SEEK_SET:
		dev->offset = offset;
		break;
	case SEEK_CUR:
		dev->offset += offset;
		break;
	case SEEK_END:
		{
			s32 blksz;
			s64 seccnt;
			ntfs_dev_ops.disk_ioctl( NULL, dev->d_name, GET_SECTOR_COUNT, &seccnt );
			ntfs_dev_ops.disk_ioctl( NULL, dev->d_name, GET_BLOCK_SIZE, &blksz );
			if (seccnt >= 0) {
				dev->offset = (blksz * seccnt) + offset;
			}
		}
		break;
	}

	return 0;
}

s64 ntfs_dev_read(struct ntfs_device *dev, void *buf, s32 count)
{
	if (ntfs_dev_ops.disk_read( NULL, dev->d_name, buf, dev->offset, count ) == 0) {
		dev->offset += count;
		return count;
	}

	return 0;
}

s64 ntfs_dev_write(struct ntfs_device *dev, const void *buf, s32 count)
{
	if (ntfs_dev_ops.disk_write( NULL, dev->d_name, buf, dev->offset, count ) == 0) {
		dev->offset += count;
		return count;
	}

	return 0;
}

s64 ntfs_dev_pread(struct ntfs_device *dev, void *buf, s32 count, s64 offset)
{
	if (ntfs_dev_ops.disk_read( NULL, dev->d_name, buf, offset, count ) == 0)
		return count;

	return 0;
}

s64 ntfs_dev_pwrite(struct ntfs_device *dev, const void *buf, s32 count, s64 offset)
{
	if (ntfs_dev_ops.disk_write( NULL, dev->d_name, buf, offset, count ) == 0)
		return count;
	return 0;
}

int ntfs_dev_sync(struct ntfs_device *dev)
{
	return 0;
}

int ntfs_dev_stat(struct ntfs_device *dev, struct x_stat *buf)
{
	memset(buf, 0, sizeof(struct x_stat));
	buf->st_mode = S_IFBLK;
	buf->st_size = 0;

	return 0;
}



int ntfs_dev_ioctl(struct ntfs_device *dev, int request, void *argp)
{
	switch (request) {

	case BLKGETSIZE:
		{
			s64 blksz;
			ntfs_dev_ops.disk_ioctl( NULL, dev->d_name, GET_SECTOR_COUNT, &blksz );
			if (blksz >= 0) {
				*(int *)argp = (int)(blksz / 512);
				return 0;
			}

			errno = EOPNOTSUPP;
			return -1;
		}
		break;

	case BLKGETSIZE64:
		{
			s32 blksz;
			s64 seccnt;
			ntfs_dev_ops.disk_ioctl( NULL, dev->d_name, GET_SECTOR_COUNT, &seccnt );
			ntfs_dev_ops.disk_ioctl( NULL, dev->d_name, GET_BLOCK_SIZE, &blksz );
			if (seccnt >= 0) {
				*(s64 *)argp = blksz * seccnt;
				return 0;
			}

			errno = EOPNOTSUPP;
			return -1;
		}
		break;

	case HDIO_GETGEO:
		break;
		//ntfs_log_debug("HDIO_GETGEO detected.\n");
		//return ntfs_win32_hdio_getgeo(dev, (struct hd_geometry *)argp);

	case BLKSSZGET:
		break;
		//ntfs_log_debug("BLKSSZGET detected.\n");
		//return ntfs_win32_blksszget(dev, (int *)argp);

	case BLKBSZSET:
		//ntfs_log_debug("BLKBSZSET detected.\n");
		/* Nothing to do on Windows. */
		return 0;

	default:
		ntfs_log_debug("unimplemented ioctl %d.\n", request);
		errno = EOPNOTSUPP;
		return -1;
	}

	return 0;//ntfs_dev_ops.disk_ioctl( NULL, dev->d_name, request, argp );
}

struct ntfs_device_operations ntfs_dops = {

	ntfs_dev_open,
	ntfs_dev_close,
	ntfs_dev_seek,
	ntfs_dev_read,
	ntfs_dev_write,
	ntfs_dev_pread,
	ntfs_dev_pwrite,
	ntfs_dev_sync,
	ntfs_dev_stat,
	ntfs_dev_ioctl,
};


void ntfs_register_io_operations( struct ntfs_device_operations *iops )
{
	ntfs_device_default_io_ops.open			= iops->open;
	ntfs_device_default_io_ops.close		= iops->close;
	ntfs_device_default_io_ops.seek			= iops->seek;
	ntfs_device_default_io_ops.read			= iops->read;
	ntfs_device_default_io_ops.write		= iops->write;
	ntfs_device_default_io_ops.pread		= iops->pread;
	ntfs_device_default_io_ops.pwrite		= iops->pwrite;
	ntfs_device_default_io_ops.sync			= iops->sync;
	ntfs_device_default_io_ops.ioctl		= iops->ioctl;
	ntfs_device_default_io_ops.stat			= iops->stat;
}

typedef enum {
	NF_STREAMS_INTERFACE_NONE,		/* No access to named data streams. */
	NF_STREAMS_INTERFACE_XATTR,		/* Map named data streams to xattrs. */
	NF_STREAMS_INTERFACE_WINDOWS,	/* "file:stream" interface. */
} ntfs_fuse_streams_interface;


char* 
ntfs_fuse_SplitStream(
	IN char* stream_name_mbs,
	OUT char** outStreamName
	)
/*++

Author: sudami [sudami@163.com]
Time  : 2009/03/06 [6:3:2009 - 12:39]

Routine Description:
  分割出文件名&文件流名. 无论如何,调用者需要释放申请的内存 free();

Arguments:
  stream_name_mbs - 要分割的全路径
  outStreamName - 保存文件流名

Return Value:
  返回分割后的文件路径

--*/
{
	int nLength = 0 ;
	char* pdest = NULL ;
	char* szTmp = (char*)malloc( 0x1000 );
	char* szTmpStream = (char*)malloc( 0x1000 );

	if ( NULL == stream_name_mbs || 0 == *stream_name_mbs || NULL == szTmp ) { return NULL; }
	memset( (void*)szTmp, 0, 0x1000 );
	memset( (void*)szTmpStream, 0, 0x1000 );
	*outStreamName = szTmpStream;

	pdest = strrchr( stream_name_mbs, ':' );

	if ( (NULL == pdest) || (*(++pdest) == '\\') ) { 
		strcpy( szTmp, stream_name_mbs );
		return szTmp;

	} else {
		// 表明冒号后面的是流文件,截取之
		strcpy( szTmpStream, pdest );

		nLength = (int)(pdest - stream_name_mbs - 1); 
		if ( 0 == nLength ) { return szTmp; }

		strncpy( szTmp, stream_name_mbs, nLength );
		return szTmp ;
	}

	return NULL ;
}

static int 
ntfs_fuse_parse_path(
    IN const char *org_path, 
    OUT char **path,
	OUT ntfschar **stream_name
	)
/*++

Author: sudami [sudami@163.com]
Time  : 2009/03/06 [6:3:2009 - 9:10]

Routine Description:
  分割字符串为 "文件路径" & "文件流".
  此函数会为@*path & @*stream分配内存,调用者需要释放之

Arguments:
  org_path - 要分割的文件全路径
  path - pointer to buffer in which parsed path saved
  stream_name -	pointer to buffer where stream name in unicode saved

Return Value:
  <0	Error occurred, return -errno;
  ==0	    No stream name, @*stream is not allocated and set to AT_UNNAMED.
  >0	Stream name length in unicode characters.

--*/
{
	char *stream_name_mbs;
	//ntfs_fuse_streams_interface streams;
	//int res;
	// 
	// 	stream_name_mbs = strdup(org_path);
	// 	if ( !stream_name_mbs ){ return -errno; }

	stream_name_mbs = (char*) org_path;
#if 0
	if ( streams == NF_STREAMS_INTERFACE_WINDOWS ) 
	{
		//	调用者需要释放此子函数分配的2处内存 @path 和 @stream_name_mbs
		*path = ntfs_fuse_SplitStream( stream_name_mbs, &stream_name_mbs );
		if (stream_name_mbs) 
		{
			*stream_name = NULL;
			res = ntfs_mbstoucs(stream_name_mbs, stream_name, 0);
			if (res < 0) { return -errno; }

			return res;
		}

	} else 
#endif
	{

		// 反正caller要释放内存,所以这里需要补上
		char* szTmp = (char*)malloc( 0x1000 );

		if ( NULL == szTmp  ) { 
			*path = stream_name_mbs; 
		} else {
			memset( (void*)szTmp, 0, 0x1000 );
			strcpy( szTmp, stream_name_mbs  );
			*path = szTmp;
		}
	}

	*stream_name = AT_UNNAMED;
	return 0;
}

void ntfs_register_io_ops( struct ntfs_device_ops *ops )
{
	ntfs_register_io_operations( &ntfs_dops );
	ntfs_dev_ops.disk_initialize = ops->disk_initialize;
	ntfs_dev_ops.disk_read = ops->disk_read;
	ntfs_dev_ops.disk_write = ops->disk_write;
	ntfs_dev_ops.disk_status = ops->disk_status;
	ntfs_dev_ops.disk_ioctl = ops->disk_ioctl;
}

int ntfs_format( void *dev )
{
	mkntfs_init_options( &opts );
	opts.quick_format = TRUE;
	opts.dev_name = dev;
	return mkntfs_redirect( &opts );
}

BOOL ntfs_rec( void *dev )
{
	ntfs_volume *vol;
	vol = utils_mount_volume( dev, MS_EXCLUSIVE, TRUE );
	if (vol == NULL) {
		return FALSE;
	}
	ntfs_umount( vol, TRUE );

	return TRUE;
}

struct ntfs_fd {
	ntfs_volume *vol;
	ntfs_attr *na;
	s64 offset;
};

ntfs_attr *ntfs_create_file( ntfs_volume *vol, char *org_path )
{
	char *path;
	char *name;
	ntfschar *uname = NULL;
	int uname_len;
	ntfs_inode *dir_ni = NULL, *ni = NULL;
	ntfs_attr *na = NULL;

	path = strdup(org_path);
	if (!path)
		return NULL;

	do {

		name = strrchr(path, '\\');
		name++;
		uname_len = ntfs_mbstoucs(name, &uname, 0);
		if (uname_len < 0) {
			break;
		}

		*name = '\0';
		dir_ni = ntfs_pathname_to_inode( vol, NULL, path );
		if (!dir_ni) {
			break;
		}

		ni = ntfs_create(dir_ni, uname, (u8)uname_len, S_IFREG);
		if (!ni) {
			break;
		}

		na = ntfs_attr_open( ni, AT_DATA, NULL, 0 );
		if (!na) {
			break;
		}

	} while (FALSE);

	if (uname != NULL) {
		free( uname );
	}

	if (dir_ni != NULL)
		ntfs_inode_close(dir_ni);

	if (ni != NULL && na == NULL)
		ntfs_inode_close( ni );

	return na;
	
}

ntfs_attr *ntfs_open_file( ntfs_volume *vol, char *path )
{
	ntfs_inode *ni = NULL;
	ntfs_attr *na = NULL;

	do {

		ni = ntfs_pathname_to_inode( vol, NULL, path );
		if (!ni) {
			break;
		}

		na = ntfs_attr_open( ni, AT_DATA, NULL, 0 );
		if (!na) {
			break;
		}

		if (na->data_size <= sizeof(INTX_FILE_TYPES)) {
			break;
		}
		if (na->data_size > sizeof(INTX_FILE_TYPES) +
			sizeof(ntfschar) * MAX_PATH) {
				break;
		}

	} while (FALSE);

	if (ni != NULL && na == NULL)
		ntfs_inode_close( ni );

	return na;
}

void *ntfs_fopen( void *dev, char *filename, unsigned char mode )
{
	ntfs_volume *vol = NULL;
	ntfs_inode *ni = NULL;
	ntfs_attr *na = NULL;
	struct ntfs_fd *fd = NULL;

	vol = utils_mount_volume( dev, MS_EXCLUSIVE, FALSE );
	if (vol == NULL) {
		return NULL;
	}

	switch (mode) {

	case NTFS_OPEN_EXISTING:
		{
			na = ntfs_open_file( vol, filename );
		}
		break;

	case NTFS_CREATE_NEW:
		{
			na = ntfs_create_file( vol, filename );
		}
		break;

	default:
		{

		}
		break;
	}

	if (na == NULL) {
		ntfs_umount( vol, TRUE );
		return NULL;
	}
	
	fd = malloc( sizeof(struct ntfs_fd) );
	if (fd == NULL) {
		ntfs_umount( vol, TRUE );
		return NULL;
	}

	fd->na = na;
	fd->offset = 0;
	fd->vol = vol;

	return fd;
}

int ntfs_fread( void *file, void *buffer, s32 bufsize )
{
	s32 res = 0;
	s32 total = 0;
	INTX_FILE *intx_file = NULL;
	struct ntfs_fd *fd = (struct ntfs_fd *)file;
	ntfs_attr *na = fd->na;
	s64 bufsize64 = bufsize;

	if (fd->offset > na->data_size) {
		return 0;
	}

	do {

		if (fd->offset + bufsize > na->data_size) { 
			bufsize64 = (na->data_size - fd->offset);
		}

		while (bufsize64) 
		{
			res = (s32)ntfs_attr_pread( na, fd->offset, bufsize64, buffer );
			if (res <= 0) {
				total = 0;
				break;
			}
			if ( (res > (int)bufsize64) && (res < (int)bufsize64 + 512) ) // 比如要读0x3e8,那么按照512字节对齐读出的话,就是0x400了.很正常. 表明成功读取
			{ 
				ntfs_log_debug("-- Read OK! -- \n");
				break ;
			}

			bufsize64 -= res; // res是实际读出的内容大小
			fd->offset += res;
			total += res;

			if ( bufsize64 < 0 || (bufsize64 & 0xff000000) ) { break ; }
		}
		res = total; // 保存实际读出的内容大小

	} while (FALSE);

	return res;
}

int ntfs_fwrite( void *file, void *buffer, s32 bufsize )
{
	s32 res;
	int total;
	struct ntfs_fd *fd = (struct ntfs_fd *)file;
	ntfs_attr *na = fd->na;
	s64 bufsize64 = bufsize;

	total = 0;

	while (bufsize64) 
	{
		res = (s32)ntfs_attr_pwrite( na, fd->offset, bufsize64, buffer );
		if (res < (s64)bufsize64)
			ntfs_log_error("ntfs_attr_pwrite returned less bytes than requested.\n");
		if (res <= 0) {
			break;
		}

		bufsize64 -= res;
		fd->offset += res;
		total += res;
	}
	res = total;

	return res;
}

void ntfs_fclose( void *file )
{
	struct ntfs_fd *fd = (struct ntfs_fd *)file;

	ntfs_inode_close( fd->na->ni );
	
	ntfs_attr_close( fd->na );

	ntfs_umount( fd->vol, TRUE );

	free( fd );
}

int ntfs_flseek( void *file, int offset, int whence)
{
	struct ntfs_fd *fd = (struct ntfs_fd *)file;

	if(whence == SEEK_SET)
		fd->offset = offset;
	else if(whence == SEEK_CUR)
		fd->offset += offset;
	else if(whence == SEEK_END)

		fd->offset = fd->na->data_size + offset;
	else
		return -1;

	if( fd->offset > fd->na->data_size )
		fd->offset = fd->na->data_size;

	return (int)fd->offset;
}