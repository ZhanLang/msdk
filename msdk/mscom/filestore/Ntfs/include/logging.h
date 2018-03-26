/*
 * logging.h - Centralised logging. Originated from the Linux-NTFS project.
 */

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdarg.h>
#include "types.h"
typedef int (ntfs_log_handler)(const char *function, const char *file, int line,
							   u32 level, void *data, const char *format, va_list args);
#define ntfs_log_debug printf
#define ntfs_log_trace printf
#define ntfs_log_error printf
#define ntfs_log_perror printf
#define ntfs_log_critical printf
#define ntfs_log_info	printf
#define ntfs_log_warning printf
#define ntfs_log_progress printf
#define ntfs_log_verbose printf
#define ntfs_log_progress printf
#define ntfs_log_quiet	printf

#endif /* _LOGGING_H_ */

