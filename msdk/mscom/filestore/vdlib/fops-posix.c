#ifdef WIN32
# include <io.h>
#else
# include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fops.h"

#ifndef WIN32
# define O_BINARY 0
#else
# include <windows.h>
#endif

void *vdisk_file_open(const char *filename, int flags)
{
	int fd = open(filename, flags | O_BINARY, S_IREAD | S_IWRITE);
	if (fd < 0)
		fd = 0;
	return (void *)fd;
}

void vdisk_file_close(void *fd)
{
	close((int)(size_t) fd);
}

offset_t vdisk_file_tell(void *fd)
{
	return tell((int)(size_t) fd);
}

offset_t vdisk_file_lseek(void *fd, offset_t offset, int origin)
{
	long offset_lo, offset_hi;

	void *osfh = (void *)_get_osfhandle((int)(size_t) fd);
	if (!osfh)
		return -1;

	offset_hi = offset >> 32;
	offset_lo = SetFilePointer(osfh, offset, &offset_hi, origin);
	if (offset_lo < 0)
		return -1;

	return	(((offset_t)(unsigned long)offset_hi) << 32) | 
		(offset_t)(unsigned long)offset_lo;
}

long vdisk_file_read(void *fd, void *buffer, unsigned long count)
{
	return (long)read((int)(size_t) fd, buffer, count);
}

long vdisk_file_write(void *fd, const void *buffer, unsigned long count)
{
	return (long)write((int)(size_t) fd, buffer, count);
}

int vdisk_file_truncate(void *fd, offset_t offset)
{
	void *osfh = (void *)_get_osfhandle((int)(size_t) fd);
	if (!osfh)
		return -EINVAL;
	vdisk_file_lseek(fd, offset, SEEK_SET);
	SetEndOfFile(osfh);
	return 0;
}
