#ifndef __CRC32_H__
#define __CRC32_H__

#ifndef _WINDOWS_
	#include "windows.h"
#endif

DWORD Calc_CRC32(const UCHAR *buf, UINT len);
DWORD upx_crc32(UINT c, const UCHAR *buf, UINT len);

#endif	

