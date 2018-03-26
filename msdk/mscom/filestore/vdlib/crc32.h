#include "aes.h"

#ifndef __crc32_h__
#define __crc32_h__

/* tricky defination */
typedef unsigned int 	crc32_t;

/* polynomial to calculate crc32 */
#define __crc32_poly__		0xedb88320

/* C'tor(), build initial crc32 table */
void crc32_initial (int poly /*= __crc32_poly__*/);

/* calculator */
crc32_t crc32_do_hash (const uint8_t *stream, uint64_t length);

#endif  /* !defined(__crc32_h__) */

