#include <fcntl.h>

#ifndef _FILE_OPS_H_
#define _FILE_OPS_H_

#ifdef WIN32
typedef __int64 offset_t;
#else
# include <stdint.h>
typedef int64_t offset_t;
#endif

void *vdisk_file_open(const char *filename, int flags);
void vdisk_file_close(void *fd);
offset_t vdisk_file_tell(void *fd);
offset_t vdisk_file_lseek(void *fd, offset_t offset, int origin);
long vdisk_file_read(void *fd, void *buffer, unsigned long count);
long vdisk_file_write(void *fd, const void *buffer, unsigned long count);
int vdisk_file_truncate(void *fd, offset_t offset);

#endif				/* _FILE_OPS_H_ */
