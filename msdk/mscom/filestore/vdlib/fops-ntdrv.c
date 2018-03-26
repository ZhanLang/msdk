#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ntddk.h>
#include "fops.h"

#ifndef SECTOR_SIZE
# define SECTOR_SIZE 512
#endif

struct vdisk_fd {
	void *handle;
	offset_t cur_offset;
};

void *vdisk_file_open(const char *filename, int flags)
{
	NTSTATUS rc;
	HANDLE fd;
	ANSI_STRING ansi_fn;
	UNICODE_STRING uni_fn;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK iosb;
	unsigned long creat_disp = 0, daccess = SYNCHRONIZE;
	struct vdisk_fd *vfd;

	if (flags & O_CREAT) {
		if (flags & O_TRUNC)
			creat_disp = FILE_OVERWRITE_IF;
		else
			creat_disp = FILE_OPEN_IF;
	} else {
		if (flags & O_TRUNC)
			creat_disp = FILE_OVERWRITE;
		else
			creat_disp = FILE_OPEN;
	}

	if ((flags & O_RDONLY) && (flags & O_RDWR))
		return 0;
	if (flags & O_RDONLY)
		daccess |= GENERIC_READ;
	if (flags & O_RDWR)
		daccess |= GENERIC_WRITE;

	RtlInitAnsiString(&ansi_fn, filename);

	rc = RtlAnsiStringToUnicodeString(&uni_fn, &ansi_fn, TRUE);
	if (!NT_SUCCESS(rc))
		return 0;

	InitializeObjectAttributes(&oa, &uni_fn,
				   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0,
				   0);
	rc = ZwCreateFile(&fd, daccess, &oa, &iosb, 0, FILE_ATTRIBUTE_NORMAL,
			  FILE_SHARE_READ, creat_disp,
			  FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
			  | FILE_RANDOM_ACCESS | FILE_WRITE_THROUGH, 0, 0);
	if (!NT_SUCCESS(rc))
		fd = 0;
	RtlFreeUnicodeString(&uni_fn);
	if (!fd)
		return 0;

	vfd = malloc(sizeof(struct vdisk_fd));
	if (!vfd) {
		ZwClose(fd);
		return 0;
	}
	vfd->handle = fd;
	vfd->cur_offset = 0;

	return vfd;
}

void vdisk_file_close(void *fd)
{
	struct vdisk_fd *vfd = fd;
	if (vfd && vfd->handle) {
		ZwClose(vfd->handle);
	}

	free( vfd );
}

offset_t vdisk_file_tell(void *fd)
{
	struct vdisk_fd *vfd = fd;
	if (!vfd || !vfd->handle)
		return -1;
	return vfd->cur_offset;
}

offset_t vdisk_file_lseek(void *fd, offset_t offset, int origin)
{
	struct vdisk_fd *vfd = fd;

	if (!vfd || !vfd->handle)
		return -1;

	if (origin == SEEK_SET) {
		vfd->cur_offset = offset;
	} else if (origin == SEEK_CUR) {
		vfd->cur_offset += offset;
	} else if (origin == SEEK_END) {

		NTSTATUS rc;
		IO_STATUS_BLOCK iosb;
		FILE_STANDARD_INFORMATION stdinfo;

		rc = ZwQueryInformationFile(vfd->handle, &iosb, &stdinfo,
					    sizeof(stdinfo),
					    FileStandardInformation);
		if (!NT_SUCCESS(rc))
			return -1;

		vfd->cur_offset = stdinfo.EndOfFile.QuadPart + offset;
	} else {
		return -1;
	}

	return vfd->cur_offset;
}

long vdisk_file_read(void *fd, void *buffer, unsigned long count)
{
	NTSTATUS rc;
	IO_STATUS_BLOCK iosb;
	LARGE_INTEGER offset;
	struct vdisk_fd *vfd = fd;

	if (!vfd || !vfd->handle || !buffer)
		return 0;

	offset.QuadPart = vfd->cur_offset;
	rc = ZwReadFile(vfd->handle, 0, 0, 0, &iosb, buffer, count, &offset, 0);
	if (!NT_SUCCESS(rc))
		return 0;

	vfd->cur_offset += iosb.Information;
	return (long)iosb.Information;
}

long vdisk_file_write(void *fd, const void *buffer, unsigned long count)
{
	NTSTATUS rc;
	IO_STATUS_BLOCK iosb;
	LARGE_INTEGER offset;
	struct vdisk_fd *vfd = fd;

	if (!vfd || !vfd->handle || !buffer)
		return 0;

	offset.QuadPart = vfd->cur_offset;
	rc = ZwWriteFile(vfd->handle, 0, 0, 0, &iosb, (void *)buffer, count,
			 &offset, 0);
	if (!NT_SUCCESS(rc))
		return 0;
	iosb.Information = count;

	vfd->cur_offset += iosb.Information;
	return iosb.Information;
}

int vdisk_file_truncate(void *fd, offset_t offset)
{
	NTSTATUS rc;
	IO_STATUS_BLOCK iosb;
	FILE_END_OF_FILE_INFORMATION eof;
	struct vdisk_fd *vfd = fd;

	if (!vfd || !vfd->handle)
		return -EINVAL;

	eof.EndOfFile.QuadPart = offset;
	rc = ZwSetInformationFile(vfd->handle, &iosb, &eof, sizeof(eof),
				  FileEndOfFileInformation);

	return NT_SUCCESS(rc) ? 0 : -EACCES;
}
