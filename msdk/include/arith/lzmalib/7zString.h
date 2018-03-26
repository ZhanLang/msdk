/* 7zString.h -- String IO
2013-07-26 : Igor Pavlov : Public domain */

#ifndef __7Z_STRING_H
#define __7Z_STRING_H

#include <windows.h>

#include "Types.h"

EXTERN_C_BEGIN

/* ---------- String ---------- */

typedef struct
{
	Byte *data;
	size_t size;
	size_t pos;
	size_t cur;
} CSzString;

void String_Construct(CSzString *p);

WRes String_Open(CSzString *p, const void *src, size_t* size);

WRes String_Close(CSzString *p);

/* reads max(*size, remain file's size) bytes */
WRes String_Read(CSzString *p, void *data, size_t *size);

WRes String_Seek(CSzString *p, Int64 *pos, ESzSeek origin);

WRes String_GetLength(CSzString *p, UInt64 *length);


/* ---------- StringInStream ---------- */
typedef struct
{
	ISeekInStream s;
	CSzString file;
} CStringInStream;

void StringInStream_CreateVTable(CStringInStream *p);

EXTERN_C_END

#endif
