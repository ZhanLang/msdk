#include <windows.h>
#include <winioctl.h>
#include <locale.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <conio.h>

#include "vdisk.h"
#include "dyndisk.h"
#include "fops.h"
#include "crc32.h"
#include "vdisk_dll.h"

crc32_t get_userid( void )
{
	char user_name[1024] = "\0";
	uint32_t renlen = 1024;

	if (GetUserName( user_name, &renlen )) {
		return crc32_do_hash( user_name, renlen );
	}

	return (crc32_t)0;
}

int vdisk_mount(struct vdisk_info *vdi, char DriveLetter)
{
	BOOL  bRet = FALSE;
	char VolumeName[] = " :";
	char DeviceName[255] = "\0";
	HANDLE cdoDevice;
	DWORD BytesReturned;
	char drvletter[2] = "\0";

	//create CDO first
	cdoDevice = CreateFileW(
		DEVICE_WIN32_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (cdoDevice == INVALID_HANDLE_VALUE) {
		return -1;
	}

	drvletter[0] = DriveLetter;
	_strupr( drvletter );

	vdi->letter = drvletter[0] - 'A';

	bRet = DeviceIoControl(cdoDevice, IOCTL_VDISK_CREATE_MOUNT_POINT, vdi, sizeof(struct vdisk_info) + vdi->filename_len - 1, DeviceName, 255, &BytesReturned, NULL);

	if (!bRet) {
		CloseHandle(cdoDevice);			
		return -1;
	}
	CloseHandle(cdoDevice);
	//end create CDO

	VolumeName[0] = DriveLetter;
	if (!DefineDosDevice( DDD_RAW_TARGET_PATH, VolumeName, DeviceName )) {
		return -1;
	}

	return 0;
}

int vdisk_umount( uint32_t userid, char DriveLetter )
{
	BOOL  bRet = FALSE;
	char VolumeName[] = "\\\\.\\ :";
	HANDLE Device;
	DWORD BytesReturned;
	struct vdisk_info vdi;

	VolumeName[4] = DriveLetter;
	strupr(VolumeName);

	Device = CreateFile(VolumeName,
			    GENERIC_READ | GENERIC_WRITE,
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);

	if (Device == INVALID_HANDLE_VALUE) {
		return -1;
	}

	if (!DeviceIoControl(Device,
			     FSCTL_LOCK_VOLUME,
			     NULL, 0, NULL, 0, &BytesReturned, NULL)) {
		CloseHandle(Device);
		return -1;
	}

	if (!DeviceIoControl(Device,
			     FSCTL_DISMOUNT_VOLUME,
			     NULL, 0, NULL, 0, &BytesReturned, NULL)) {
		CloseHandle(Device);
		return -1;
	}
	if (!DeviceIoControl(Device,
			     FSCTL_UNLOCK_VOLUME,
			     NULL, 0, NULL, 0, &BytesReturned, NULL)) {
		CloseHandle(Device);
		return -1;
	}
	CloseHandle(Device);

	if (!DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL)) {
		return -1;
	}

	//DEL VDO
	Device = CreateFileW(
		DEVICE_WIN32_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (Device == INVALID_HANDLE_VALUE) {		
		return -1;
	}

	vdi.letter = VolumeName[4] - 'A';
	vdi.userid = userid;

	bRet = DeviceIoControl(Device, IOCTL_VDISK_DELETE_MOUNT_POINT, &vdi, sizeof(struct vdisk_info), NULL, 0, &BytesReturned, NULL);

	if (!bRet) {
		CloseHandle(Device);			
		return -1;
	}
	CloseHandle(Device);	

	return 0;
}

int FileDiskStatus(char DriveLetter)
{
	char VolumeName[] = "\\\\.\\ :";
	HANDLE Device;
	struct vdisk_info *vdi;
	DWORD BytesReturned;

	VolumeName[4] = DriveLetter;

	Device = CreateFile(VolumeName,
			    GENERIC_READ,
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);

	if (Device == INVALID_HANDLE_VALUE) {
		return -1;
	}

	vdi = malloc(sizeof(struct vdisk_info) + MAX_PATH);

	if (!DeviceIoControl(Device,
			     IOCTL_VDISK_QUERY_FILE,
			     NULL,
			     0,
			     vdi,
			     sizeof(struct vdisk_info) + MAX_PATH,
			     &BytesReturned, NULL)) {
		return -1;
	}

	if (BytesReturned < sizeof(struct vdisk_info)) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return -1;
	}

	printf("%c: %.*s Size: %I64u bytes%s\n",
	       DriveLetter,
	       vdi->filename_len,
	       vdi->filename, vdi->filesize, vdi->readonly ? ", ReadOnly" : "");

	return 0;
}

int __cdecl create_main(int argc, char *argv[])
{


	


	char *option;
	int64_t disksize;
	char *disktype;
	int disktypeid = RSDISK_TYPE_FIXED;
	char *encrypt;
	int encryptid = RSDISK_CRYPT_NONE;
	struct rsdisk_header hdr;
	unsigned char key[16];
	AES_KEY encrypt_key;

	int ret = -EINVAL;
	int i;
	void *fd;
	uint8_t *inbuf;
	uint8_t *outbuf;

	unsigned long aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	memset(key, 0, 16);
	memset(&encrypt_key, 0, sizeof(AES_KEY));

	if (argc < 5)
		return -EINVAL;

	option = argv[2];
	if (option[strlen(option) - 1] == 'G')
		disksize = _atoi64(option) * 1024 * 1024 * 1024 / VDISK_SECTOR_SIZE;
	else if (option[strlen(option) - 1] == 'M')
		disksize = _atoi64(option) * 1024 * 1024 / VDISK_SECTOR_SIZE;
	else if (option[strlen(option) - 1] == 'K')
		disksize = _atoi64(option) * 1024 / VDISK_SECTOR_SIZE;
	else
		disksize = _atoi64(option) / VDISK_SECTOR_SIZE;

	disktype = argv[3];
	disktypeid = atoi(disktype);

	encrypt = argv[4];
	encryptid = atoi(encrypt);

	// fill rs disk herader
	memset(&hdr, 0, sizeof(struct rsdisk_header));
	hdr.magic = hdr.cryptmagic = RSDISK_MAGIC;
	hdr.version = RSDISK_VERSION;
	hdr.crypt_method = encryptid;
	if (hdr.crypt_method < RSDISK_CRYPT_NONE
	    || hdr.crypt_method > RSDISK_CRYPT_AES) {
		return -EINVAL;
	}
	hdr.disk_type = disktypeid;
	if (hdr.disk_type < RSDISK_TYPE_FIXED
	    || hdr.disk_type > RSDISK_TYPE_DYNAMIC) {
		return -EINVAL;
	}

	// allocate file object
	fd = vdisk_file_open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
	if (!fd) {
		return -EINVAL;
	}

	inbuf = malloc(aligned_hdrsize);
	if (!inbuf) {
		return -EINVAL;
	}
	outbuf = malloc(aligned_hdrsize);
	if (!outbuf) {
		return -EINVAL;
	}
	memset(inbuf, 0, aligned_hdrsize);
	memset(outbuf, 0, aligned_hdrsize);

	if (hdr.disk_type == RSDISK_TYPE_FIXED) {
		ret = vdisk_file_truncate(fd, (disksize + 1) * VDISK_SECTOR_SIZE);	//first sector for ourself
	} else if (hdr.disk_type == RSDISK_TYPE_DYNAMIC) {
		hdr.d_hdr.size = disksize * VDISK_SECTOR_SIZE;
		ret = dyndisk_create(fd, &hdr);
	}

	if (ret == 0) {
		memcpy(inbuf, &hdr, sizeof(struct rsdisk_header));
		memcpy(outbuf, &hdr, sizeof(struct rsdisk_header));
		if (hdr.crypt_method == RSDISK_CRYPT_AES) {
			printf("input password: ");
			for (i = 0; i < 16; ++i) {
				int c = getch();
				if (c == '\r' || c == '\n') {
					printf("\n");
					break;
				}
				key[i] = c;
				printf("*");
			}
			if (AES_set_encrypt_key(key, 128, &encrypt_key)) {
				vdisk_file_close(fd);
				free(inbuf);
				free(outbuf);
				return -EINVAL;
			}
			encrypt_aes_blocks(outbuf + 16, inbuf + 16,
					   aligned_hdrsize - 16, 1, &encrypt_key);
		}
		vdisk_file_lseek(fd, 0, SEEK_SET);
		vdisk_file_write(fd, outbuf, aligned_hdrsize);
	}

	vdisk_file_close(fd);
	free(inbuf);
	free(outbuf);
	return ret;

}

/* format */
#define FMIFS_DONE	0xB
#define FMIFS_HARDDISK	0xC

typedef BOOLEAN (__stdcall * PFMIFSCALLBACK) (int command, DWORD subCommand,
					      PVOID parameter);
typedef VOID (__stdcall * PFORMATEX) (PWCHAR DriveRoot, DWORD MediaFlag,
				      PWCHAR Format, PWCHAR Label,
				      BOOL QuickFormat, DWORD ClusterSize,
				      PFMIFSCALLBACK Callback);

typedef struct {
	DWORD Lines;
	PCHAR Output;
} TEXTOUTPUT, *PTEXTOUTPUT;

typedef enum {
	PROGRESS,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	UNKNOWN7,
	UNKNOWN8,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	UNKNOWNC,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS
} CALLBACKCOMMAND;

BOOLEAN format_result;
BOOLEAN __stdcall format_ex_callback(CALLBACKCOMMAND Command, DWORD Modifier,
				     PVOID Argument)
{
	PDWORD percent;
	PTEXTOUTPUT output;

	switch (Command) {

	case PROGRESS:
		percent = (PDWORD) Argument;
		printf("%d percent completed.\r", *percent);
		break;

	case OUTPUT:
		output = (PTEXTOUTPUT) Argument;
		printf("%s", output->Output);
		break;

	case DONE:
		format_result = *(PBOOLEAN) Argument;
		printf("format_result=%d\n", format_result);
		break;
	}
	return TRUE;
}

int __cdecl format_main(int argc, char *argv[])
{
	int i, if_quick_format = 0;
	wchar_t drive_char, format[64], label[260], drive_path[64] = L"x:\\";
	int ret = -EINVAL;
	UINT rc = 0xFF;
	HMODULE fmifs_lib = NULL;
	PFORMATEX format_ex;
	struct vdisk_info *vdi = NULL;

	int cc = 0;
	unsigned char key[16];
	struct rsdisk_header *hdr = NULL;
	uint8_t *tmpbuf = NULL;
	void *fd = NULL;

	unsigned long aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	if (argc < 3)
		return -EINVAL;

	vdi = malloc(sizeof(struct vdisk_info) + strlen(argv[1]) + 7);
	if (!vdi) {
		goto fail;
	}
	memset(vdi, 0, sizeof(struct vdisk_info) + strlen(argv[1]) + 7);
	strcpy(vdi->filename, "\\??\\");
	strcat(vdi->filename, argv[1]);
	vdi->filename_len = (USHORT) strlen(vdi->filename) * sizeof(wchar_t);

	if (strstr(argv[2], "NTFS") || strstr(argv[2], "FAT32")
	    || strstr(argv[2], "FAT16")) {
		mbstowcs(format, argv[2], 64);
	} else {
		goto fail;
	}
	if (strlen(argv[3]) < 260 && strlen(argv[3]) > 0) {
		mbstowcs(label, argv[3], 260);
	} else {
		goto fail;
	}
	if (argc == 5) {
		if (argv[4][0] == 'q') {
			if_quick_format = TRUE;
		} else {
			goto fail;
		}
	}

	fmifs_lib = LoadLibraryA("fmifs.dll");
	if (!fmifs_lib) {
		goto fail;
	}
	format_ex = (PFORMATEX)GetProcAddress(fmifs_lib, "FormatEx");
	if (!format_ex) {
		goto fail;
	}

	//mount
	memset(key, 0, 16);
	fd = vdisk_file_open(argv[1], O_RDONLY);
	if (!fd) {
		goto fail;
	}
	tmpbuf = malloc(aligned_hdrsize);
	if (!tmpbuf) {
		vdisk_file_close(fd);
		goto fail;
	}
	memset(tmpbuf, 0, aligned_hdrsize);
	hdr = (struct rsdisk_header *)tmpbuf;

	vdisk_file_lseek(fd, 0, SEEK_SET);
	vdisk_file_read(fd, tmpbuf, aligned_hdrsize);

	if (hdr->magic != RSDISK_MAGIC || hdr->version != RSDISK_VERSION) {
		vdisk_file_close(fd);
		goto fail;
	}
	if (hdr->cryptmagic != hdr->magic) {
		printf("input password: ");
		for (i = 0; i < 16; ++i) {
			int c = getch();
			if (c == '\r' || c == '\n') {
				printf("\n");
				break;
			}
			key[i] = c;
			printf("*");
		}

		if (AES_set_encrypt_key(key, 128, &vdi->encrypt_key)) {
			vdisk_file_close(fd);
			goto fail;
		}

		if (AES_set_decrypt_key(key, 128, &vdi->decrypt_key)) {
			vdisk_file_close(fd);
			goto fail;
		}

		encrypt_aes_blocks(tmpbuf + 16, tmpbuf + 16, aligned_hdrsize - 16, 0,
				   &vdi->decrypt_key);

		if (hdr->cryptmagic != hdr->magic) {
			vdisk_file_close(fd);
			goto fail;
		}
	}
	vdisk_file_close(fd);

	for (drive_char = L'Z'; drive_char >= L'A'; --drive_char) {
		drive_path[0] = drive_char;
		rc = GetDriveTypeW(drive_path);
		if (rc == DRIVE_NO_ROOT_DIR) {
			break;
		}
	}
	if (rc != DRIVE_NO_ROOT_DIR) {
		goto fail;
	}

	vdi->userid = (uint32_t)get_userid();

	vdi->readonly = FALSE;
	ret = vdisk_mount(vdi, (char)drive_char);
	if (ret != 0) {
		goto fail;
	}

	format_result = 0;
	for (i = 0; i < 500 && !format_result; i++) {
		format_ex(drive_path, FMIFS_HARDDISK, format, label,
			  if_quick_format, 0, format_ex_callback);
	}

	// unmount
	ret = vdisk_umount( get_userid(), (char)drive_char);

 fail:
	if (vdi)
		free(vdi);
	if (tmpbuf)
		free(tmpbuf);
	if (fmifs_lib)
		FreeLibrary(fmifs_lib);
	return ret;
}

int __cdecl mount_main(int argc, char *argv[])
{
	int i, rc = -EINVAL;
	struct vdisk_info *vdi = NULL;
	unsigned char key[16];
	struct rsdisk_header *hdr = NULL;
	uint8_t *tmpbuf = NULL;
	void *fd = NULL;
	char *rwtype;
	BOOLEAN brwtype = FALSE;

	unsigned long aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	memset(key, 0, 16);

	if (argc < 4)
		return -EINVAL;

	fd = vdisk_file_open(argv[1], O_RDWR);
	if (!fd) {
		fd = vdisk_file_open(argv[1], O_RDONLY);
	}
	if (!fd) {
		return -EINVAL;
	}

	vdi = malloc(sizeof(struct vdisk_info) + strlen(argv[1]) + 7);
	if (!vdi) {
		goto fail;
	}
	memset(vdi, 0, sizeof(struct vdisk_info) + strlen(argv[1]) + 7);
	strcpy(vdi->filename, "\\??\\");
	strcat(vdi->filename, argv[1]);
	vdi->filename_len = (USHORT) strlen(vdi->filename);

	rwtype = argv[3];
	if (*rwtype == '1') {
		brwtype = TRUE;
	}
	vdi->readonly = brwtype;

	tmpbuf = malloc(aligned_hdrsize);
	if (!tmpbuf) {
		vdisk_file_close(fd);
		goto fail;
	}
	memset(tmpbuf, 0, aligned_hdrsize);
	hdr = (struct rsdisk_header *)tmpbuf;

	vdisk_file_lseek(fd, 0, SEEK_SET);
	vdisk_file_read(fd, tmpbuf, aligned_hdrsize);

	if (hdr->magic != RSDISK_MAGIC || hdr->version != RSDISK_VERSION) {
		vdisk_file_close(fd);
		goto fail;
	}

	if (hdr->cryptmagic != hdr->magic) {
		printf("input password: ");
		for (i = 0; i < 16; ++i) {
			int c = getch();
			if (c == '\r' || c == '\n') {
				printf("\n");
				break;
			}
			key[i] = c;
			printf("*");
		}

		if (AES_set_encrypt_key(key, 128, &vdi->encrypt_key)) {
			vdisk_file_close(fd);
			goto fail;
		}

		if (AES_set_decrypt_key(key, 128, &vdi->decrypt_key)) {
			vdisk_file_close(fd);
			goto fail;
		}

		encrypt_aes_blocks(tmpbuf + 16, tmpbuf + 16, aligned_hdrsize - 16, 0,
				   &vdi->decrypt_key);

		if (hdr->cryptmagic != hdr->magic) {
			vdisk_file_close(fd);
			goto fail;
		}
	}

	
	vdi->userid = (uint32_t)get_userid();

	vdisk_file_close(fd);
	rc = vdisk_mount(vdi, argv[2][0]);

 fail:
	if (vdi)
		free(vdi);
	if (tmpbuf)
		free(tmpbuf);
	return rc;

}

int __cdecl umount_main(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	return vdisk_umount( get_userid(), argv[1][0] );
}

struct sub_cmd_dispatch {
	const char *sub_command;
	int (__cdecl * dispatch) (int argc, char *argv[]);
} main_dispatch[] = {
	{ "create", create_main }, 
	{ "format", format_main }, 
	{ "mount", mount_main }, 
	{ "umount", umount_main },
};

void usage()
{
	printf("evd <sub command> <arguments>\n");
	printf("sub command:\n");
	printf("    create: create virtual disk\n");
	printf("            usage: create <filename> <size> <disktype:1=Dyn;0=Static>\n");
	printf("                          <encrypttype:1=AES Encrypt;0=No Encrypt>\n");
	printf("            eg: create c:\\virtdisk.img 1G 0 1\n");
	printf("    format: format virtual disk\n");
	printf("            usage: format <filename> <format:NTFS;FAT32;FAT16> <lable>\n");
	printf("                          <quick(optional):q?>\n");
	printf("            eg: format c:\\virtdisk.img NTFS worldcup q\n");
	printf("    mount : mount virtual disk image\n");
	printf("            usage: mount <filename> <drive><readonly:1=ROnly;0=RWOnly>\n");
	printf("            eg: mount c:\\virtdisk.img e 0\n");
	printf("    umount: umount virtual disk image\n");
	printf("            eg: umount e\n");
}

int __cdecl main(int argc, char *argv[])
{
	int i;
	int cc = 0;
	if (argc < 2) {
		usage();
		return -EINVAL;
	}

	setlocale(LC_ALL, ".ACP");

	crc32_initial( __crc32_poly__ );

	for (i = 0; i < sizeof(main_dispatch) / sizeof(struct sub_cmd_dispatch); i++) {
		if (!strcmp(main_dispatch[i].sub_command, argv[1]))
			return main_dispatch[i].dispatch(argc - 1, argv + 1);
	}

	usage();
	return -EINVAL;
}
