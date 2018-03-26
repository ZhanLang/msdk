#pragma once
unsigned long update_crc32(unsigned long CurCRC, const char * Buf, DWORD BufSize);
unsigned long std_crc32(const char *data, size_t lens);
