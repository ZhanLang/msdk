

#pragma once

#define OTL_ODBC

// generic macros
#ifdef _UNICODE

#define OTL_UNICODE
//#define OTL_UNICODE_CHAR	WCHAR
#define OTL_BIGINT			LONGLONG

// macros for generic ODBC
#ifdef OTL_ODBC
#define OTL_UNICODE_USE_ANSI_ODBC_FUNCS_FOR_DATA_DICT
#endif

#endif

#include <otl/otlv4.h>

#define OTL_BUFF_SIZE	1
#define LOB_CHUNK_SIZE	4096
#define LOB_CHAR_COUNT	( LOB_CHUNK_SIZE/sizeof(TCHAR) )
#define OTL_STR_VAR_LEN	8196