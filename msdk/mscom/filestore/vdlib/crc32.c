
#include "crc32.h"


crc32_t crc32_table[256];

/* C'tor(), build initial crc32 table */
void crc32_initial (int poly /*= __crc32_poly__*/)
{
	crc32_t crc;
	int i, j;
	for (i = 0; i < 256; ++i) {
		crc = (crc32_t)i;
		for (j = 0; j < 8; ++j) {
			if (crc & 1)
				crc = (crc >> 1) ^ poly;
			else
				crc >>= 1;
		}
		crc32_table[i] = crc;
	}
}

/* calculator */
crc32_t crc32_do_hash (const uint8_t *stream, uint64_t length)
{
	uint64_t i;
	crc32_t crc = (crc32_t)-1;
	for (i = 0; i < length; ++i) {
		uint8_t tmp = (uint8_t)(stream[i] ^ (char)crc);
		crc = (crc >> 8) ^ crc32_table[tmp];
	}
	return ~crc;
}

