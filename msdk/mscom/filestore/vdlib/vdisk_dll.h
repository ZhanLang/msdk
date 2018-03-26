
#ifndef __vdisk_dll_h__
#define __vdisk_dll_h__

#define VDISK_SECTOR_SIZE 512

/* 设备名头部字符串，用于简单识别设备 */
#define VDISK_DEVICE_NAME_PREFIX _T("\\Device\\VDisk")

/* 错误代码 */

/* 成功 */
#define VDISK_ERROR_SUCCESS			0

/* 文件没有发现，或者打开文件失败 */
#define VDISK_ERROR_FILE_NOT_FOUND		1

/* 参数错误 */
#define VDISK_ERROR_INVALID_PARAMETER		2

/* 没有足够的资源，通常为内存分配失败 */
#define VDISK_ERROR_INSUFFICIENT_RESOURCES	3

/* 执行失败 */
#define VDISK_ERROR_UNSUCCESSFUL		4

/* 密码错误 */
#define VDISK_ERROR_INVALID_KEY			5

/* 无效的设备，通常是打开设备失败 */
#define VDISK_ERROR_INVALID_DEVICE		6

/* 用户提供的缓冲区太小 */
#define VDISK_ERROR_BUFFER_TOO_SMALL		7

/* 用户提供的缓冲区大小，超出可以存储范围 */
#define VDISK_ERROR_BUFFER_OVERFLOW		8

typedef unsigned long vdisk_error;


/* 定义基本类型 */
typedef int bool_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

#ifndef FALSE
#define FALSE ((bool_t)0)
#endif

#ifndef TRUE
#define TRUE ((bool_t)1)
#endif

#define VDISK_MAX_KEY_LENGTH	128

/* 磁盘信息结构 */

#define VDISK_MAX_FILE_PATH	0x200

struct vdisk_info_entry {

	char diskfile[VDISK_MAX_FILE_PATH];	/* 磁盘映射文件路径 */
	uint32_t userid;			/* 用户标识 */
	char drv_letter;			/* 驱动器字母，如 'c', 'd', ... */
	uint64_t local_time;			/* 最后使用时间 */
};



#define VDISK_MAGIC_HI ((uint64_t)('R' << 24) | (uint64_t)('S' << 16) | (uint64_t)('V' << 8) | (uint64_t)('D'))
#define VDISK_MAGIC_LO ((uint64_t)('S' << 24) | (uint64_t)('I' << 16) | (uint64_t)('G' << 8) | (uint64_t)(0xfe))
#define VDISK_MAGIC ((VDISK_MAGIC_HI << 32) | VDISK_MAGIC_LO)
#define VDISK_VERSION 1

/* 磁盘镜像文件头 */

struct vdisk_header {
	//non crypt info
	uint64_t magic;	/* 标志，见 VDISK_MAGIC */
	uint64_t version; /* 版本 VDISK_VERSION */
};


vdisk_error vdisk_init();

vdisk_error vdisk_uninit();
/************************************************************************
 * 功能：
 *	挂载镜像文件
 * 参数：
 *	【IN】diskfile - 磁盘镜像文件名称
 *	【IN】readonly - 是否挂载为只读磁盘，TRUE 为只读
 *	【IN】key - 磁盘密码，最大长度 VDISK_MAX_KEY_LENGTH
 *	【IN】userid - 用户ID
 *	【IN】drv_letter - 磁盘驱动器字母
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_mount( char *diskfile, bool_t readonly, char *key, uint32_t userid, char drv_letter );


/************************************************************************
 * 功能：
 *	卸载镜像文件
 * 参数：
 *	【IN】userid - 用户ID
 *	【IN】drv_letter - 磁盘驱动器字母
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_umount( uint32_t userid, char drv_letter );


/* 加密方式 */
#define VDISK_CRYPT_NONE	0
#define VDISK_CRYPT_AES		1
#define VDISK_CRYPT_CAST5	2
#define VDISK_CRYPT_TWOFISH	3
#define VDISK_CRYPT_MAX		3

/* 用户数据大小 */
#define VDISK_USER_DATA_SIZE 0x80

#pragma pack (push, 1)

struct vdisk_diskfile_info {

	uint64_t disksize;		/* 磁盘大小，单位为字节 */
	uint32_t crypt_method;
	uint8_t user_data[VDISK_USER_DATA_SIZE];	/* 自定义数据，可由 vdisk_set_diskfile_info 函数设置，大小为 VDISK_USER_DATA_SIZE */
};

#pragma pack (pop)


/************************************************************************
 * 功能：
 *	获取磁盘文件信息
 * 参数：
 *	【IN】diskfile - 磁盘镜像文件名称
 *	【IN】info - 信息内容，见 vdisk_diskfile_info
 *	【IN】info_len - info 内容的总大小   sizeof (struct vdisk_diskfile_info)
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_get_diskfile_info( char *diskfile, struct vdisk_diskfile_info *info, uint32_t info_len );

/************************************************************************
 * 功能：
 *	设置用户自定义数据，写入文件
 * 参数：
 *	【IN】diskfile - 磁盘镜像文件名称
 *	【IN】info - 信息内容，为自定义数据，大小不超过 VDISK_USER_DATA_SIZE
 *	【IN】info_len - info 内容的总大小
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_set_diskfile_info( char *diskfile, void *info, uint32_t info_len );

/************************************************************************
 * 功能：
 *	创建镜像文件
 * 参数：
 *	【IN】diskfile - 磁盘镜像文件名称
 *	【IN】disksize - 磁盘镜像大小，单位为 扇区数，见 VDISK_SECTOR_SIZE 宏
 *	【IN】crypt_method - 加密方式
 *	【IN】key - 磁盘密码，最大长度 VDISK_MAX_KEY_LENGTH
 *	【IN】is_dyndisk - 是否是动态磁盘，TRUE 为 动态磁盘，否则为静态磁盘
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_create( char *diskfile, uint64_t disksize, int crypt_method, char *key, bool_t is_dyndisk );


/************************************************************************
 * 功能：
 *	设置用户数据
 * 参数：
 *	【IN】userid - 用户ID
 *	【IN】user_data - 用户自定义数据
 *	【IN】user_data_len - 数据长度
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_set_user_data( uint32_t userid, uint8_t *user_data, size_t user_data_len );


/************************************************************************
 * 功能：
 *	获取用户数据
 * 参数：
 *	【IN】 userid - 用户ID
 *	【OUT】user_data - 用户自定义数据
 *	【IN】 user_data_len - 数据长度
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_get_user_data( uint32_t userid, uint8_t *user_data, size_t user_data_len );


/************************************************************************
 * 功能：
 *	获取当前信息
 * 参数：
 *	【OUT】info - vdisk_info_entry 结构的数组，返回当前信息
 *	【IN】 innr - info 数组个数
 *	【OUT】outnr - 返回实际的个数
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_get_info( struct vdisk_info_entry *info, size_t innr, size_t *outnr );


/************************************************************************
 * 功能：
 *	打开磁盘，用于文件操作
 * 参数：
 *	【IN】diskfile - 磁盘文件路径
 *	【IN】 key - 磁盘密码
 *	【OUT】dev - 返回一个设备句柄，用于操作文件
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_open( char *diskfile, char *key, void **dev );

/************************************************************************
 * 功能：
 *	关闭磁盘
 * 参数：
 *	【IN】dev - 设备句柄，见 vdisk_open
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
void vdisk_close( void *dev );


typedef enum {
	fmt_type_unknow,	/* 未知文件系统 */
	fmt_type_ntfs,
	fmt_type_fat32,
} fmt_type;




/************************************************************************
 * 功能：
 *	格式化磁盘
 * 参数：
 *	【IN】dev - 设备句柄，见 vdisk_open
 *	【IN】type - 文件系统类型，见 fmt_type
 *	【IN】lable - 卷标
 * 返回值：
 *	vdisk_error 见宏定义
/************************************************************************/ 
vdisk_error vdisk_format( void *dev, fmt_type type );

#define	VDISK_OPEN_EXISTING	0x01	/* Opens the file. The function fails if the file is not existing. */
#define	VDISK_CREATE_NEW	0x02	/* Creates a new file. The function fails if the file is already existing. */

/************************************************************************
 * 功能：
 *	打开/创建文件
 * 参数：
 *	【IN】dev - 设备句柄，见 vdisk_open
 *	【IN】filename - 文件名称，如 "\\abc.txt"
 *	【IN】mode - VDISK_OPEN_EXISTING 或 VDISK_CREATE_NEW
 * 返回值：
 *	文件指针，如果失败则文件指针为 NULL
/************************************************************************/ 
void *vdisk_fopen( void *dev, char *filename, unsigned char mode );

/************************************************************************
 * 功能：
 *	读取文件
 * 参数：
 *	【IN】file - 文件指针，见 vdisk_fopen
 *	【IN】buffer - 读取缓冲区
 *	【IN】bufsize - 缓冲区大小
 * 返回值：
 *	实际读取长度
/************************************************************************/ 
int vdisk_fread( void *file, void *buffer, unsigned int bufsize );

/************************************************************************
 * 功能：
 *	写入文件
 * 参数：
 *	【IN】file - 文件指针，见 vdisk_fopen
 *	【IN】buffer - 写入缓冲区
 *	【IN】bufsize - 缓冲区大小
 * 返回值：
 *	实际写入长度
/************************************************************************/ 
int vdisk_fwrite( void *file, void *buffer, unsigned int bufsize );

/************************************************************************
 * 功能：
 *	移动文件指针
 * 参数：
 *	【IN】file - 文件指针，见 vdisk_fopen
 *	【IN】offset - 偏移
 *	【IN】whence - 位置 SEEK_SET， SEEK_CUR， SEEK_END
 * 返回值：
 *	当前文件偏移
/************************************************************************/ 
int vdisk_flseek( void *file, int offset, int whence );

/************************************************************************
 * 功能：
 *	关闭文件
 * 参数：
 *	【IN】file - 文件指针，见 vdisk_fopen
 * 返回值：
 *	无
/************************************************************************/ 
void vdisk_fclose( void *file );

vdisk_error vdisk_mkdir(void *dev, char * path);
vdisk_error vdisk_remove(void *dev, char * name);
vdisk_error vdisk_rename(void *dev, char * oldname, char * newname);
#endif