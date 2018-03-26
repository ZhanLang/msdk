/*********************************************************
文件描述:此文件定义瑞星虚拟磁盘驱动与三环的接口描述
时间    :2010/07/12
         内核安全研究组
*********************************************************/

/****************************************************
       产生虚拟磁盘镜像文件:
       [create] [filename] [size] [disktype:1=Dyn;0=Static] [encrypttype:1=AES Encrypt;0=No Encrypt] [encryptcode:AES code <=16 char]
	 |        |           |        |                         |                                        |
       *argv[0]  *argv[1]    *argv[2]  *argv[3]                 *argv[4]                                 *argv[5]

       eg:create c:\rsdisk.img 1G 0 1 123456
                               xM
			       xK

       格式化虚拟磁盘:（在加载虚拟磁盘操作前做此命令格式化虚拟磁盘为指定文件格式）
       [format] [filename] [format:NTFS;FAT32;FAT16] [lable] [quick:q=quick;n=normal] [encryptcode:AES code <=16 char]
       |         |           |                        |          |                         |
       *argv[0]  *argv[1]   *argv[2]                 *argv[3]   *argv[4]                  *argv[5]

       eg:format c:\rsdisk.img NTFS worldcup q 123456


       加载虚拟磁盘:
       [mount] [filename] [drive] [readonly:1=ROnly;0=RWOnly] [encryptcode:AES code <=16 char]
       |         |          |           |                         |
       *argv[0]  *argv[1]   *argv[2]    *argv[3]                  *argv[4]

       eg:mount c:\rsdisk.img Z 0 123456

       卸载虚拟磁盘:
       [umount] [drive]
       |         |
       *argv[0]  *argv[1]

       eg:umount Z
****************************************************/



/****************************************************
函数名称:rsvdisk_create
描述    :产生虚拟磁盘镜像文件。
参数    :AES key为小于16字节。下同
返回值  :TRUE  成功
	 FALSE 失败
****************************************************/
typedef __int64 int64_t;

typedef enum {
	STATIC_DISK = 0,
	DYN_DISK
} DISKTYPE;

typedef enum {
	NO_ENCRYPT = 0,
	AES_ENCRYPT
} ENCRYPTTYPE;

BOOL WINAPI rsvdisk_create(const char *filename, int64_t disksize,
	       DISKTYPE disktype, ENCRYPTTYPE encrypttype, const char *key);


/****************************************************
函数名称:rsvdisk_format
描述    :格式化虚拟磁盘。在虚拟磁盘未加载时使用
参数    :
返回值  :TRUE  成功
	 FALSE 失败
****************************************************/
typedef enum {
	NTFS = 0,
	FAT32,
	FAT16
} FSFORMAT;

typedef enum {
	QUICK = 0,
	NO_QUICK	
} FORMATTYPE;

BOOL WINAPI rsvdisk_format(const char *filename, FSFORMAT fsformat, 
	       const char *disklabel, FORMATTYPE formattype, const char *key);


/****************************************************
函数名称:rsvdisk_mount
描述    :加载虚拟磁盘。
参数    :
返回值  :TRUE  成功
	 FALSE 失败
****************************************************/
typedef enum {
	RWONLY = 0,
	RONLY
} RWTYPE;

BOOL WINAPI rsvdisk_mount(const char *filename, char driverletter, 
			   RWTYPE rwtype, const char *key);


/****************************************************
函数名称:rsvdisk_umount
描述    :卸载虚拟磁盘。
参数    :
返回值  :TRUE  成功
	 FALSE 失败
****************************************************/
BOOL WINAPI rsvdisk_umount(char driverletter);