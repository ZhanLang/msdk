// stdafx.cpp : source file that includes just the standard includes
//	comx3.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#include <time.h>

//-----------------------------------------------------------------------------
// 目的 : 这是本日志输出模块出错后的错误的输出
// 输入 : msgDescriptor - 
//-----------------------------------------------------------------------------
void ErrorMsg(const char * msgDescriptor, ... )
{
	char szBuf[1024];

	time_t t = time(NULL);
	va_list marker;
	va_start( marker, msgDescriptor );

#if _MSC_VER >= 1400
	vsprintf_s( szBuf, 1024, msgDescriptor, marker );
#else
	vsprintf( szBuf, msgDescriptor, marker );
#endif

	va_end( marker );

#if _MSC_VER >= 1400
	FILE *f;
	fopen_s(&f, "c:\\rs_error.txt", "a+t");
#else
	FILE *f=fopen("c:\\rs_error.txt", "a+t");
#endif
	if(f)
	{
		char tmptime[128];
		char tmpdate[128];
#if _MSC_VER >= 1400
		_strtime_s( tmptime, 128 );
		_strdate_s( tmpdate, 128 );
#else
#endif

		fprintf(f, "[%s %s] %s", tmpdate, tmptime, szBuf);
		//fputs(szBuf, f);
		fclose(f);
	}
	::OutputDebugString(szBuf);
}