/* 7zString.c -- String IO
2013-07-26 : Igor Pavlov : Public domain */

#include "7zString.h"

void  String_Construct(CSzString *p)
{
	p->data = NULL;
	p->pos = 0;
	p->size = 0;
	p->cur = 0;
}

WRes String_Open(CSzString *p, const void *src, size_t* size)
{
	size_t size_ = *size;
	p->data = malloc(size_);
	if(p->data != NULL)
	{
		memcpy(p->data,src,size_);
		p->size = size_;
	}
	return 0;
}

WRes String_Close(CSzString *p)
{
	if (p->data != NULL)
	{
		free(p->data);
		String_Construct(p);
	}
	return 0;
}

__inline WRes String_Read(CSzString *p, void *data, size_t *size)
{
	size_t originalSize = *size;
	if (originalSize == 0)
		return 0;

	memcpy(data,p->data + p->pos,originalSize);
	p->pos += originalSize;

	return 0;
}

__inline WRes String_Seek(CSzString *p, Int64 *pos, ESzSeek origin)
{
	switch (origin)
	{
	case SZ_SEEK_SET: 
		p->cur = p->pos;
		p->pos = (DWORD)*pos;
		break;
	case SZ_SEEK_CUR: 
		p->pos = p->cur;
		break;
	case SZ_SEEK_END: 
		p->pos = p->size;
		*pos = p->pos;
		break;
	default: return ERROR_INVALID_PARAMETER;
	}
	return 0;
}

__inline WRes String_GetLength(CSzString *p, UInt64 *length)
{
	*length = p->size;
	return 0;
}

/* ---------- StringInStream ---------- */

static SRes StringInStream_Read(void *pp, void *buf, size_t *size)
{
	CStringInStream *p = (CStringInStream *)pp;
	return (String_Read(&p->file, buf, size) == 0) ? SZ_OK : SZ_ERROR_READ;
}

static SRes StringInStream_Seek(void *pp, Int64 *pos, ESzSeek origin)
{
	CStringInStream *p = (CStringInStream *)pp;
	return String_Seek(&p->file, pos, origin);
}

void StringInStream_CreateVTable(CStringInStream *p)
{
	p->s.Read = StringInStream_Read;
	p->s.Seek = StringInStream_Seek;
}
