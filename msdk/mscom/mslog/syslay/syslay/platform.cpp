// platform.cpp
//
// 这是windows平台的实现，将来需要有其他平台的实现

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include "mslog/Syslay/platform.h"
#include "mslog/Syslay/memalloc.h"



static LARGE_INTEGER g_PerformanceFrequency;
static LARGE_INTEGER g_MSPerformanceFrequency;
static LARGE_INTEGER g_ClockStart;

static void InitTime()
{
	if( !g_PerformanceFrequency.QuadPart )
	{
		QueryPerformanceFrequency(&g_PerformanceFrequency);
		g_MSPerformanceFrequency.QuadPart = g_PerformanceFrequency.QuadPart / 1000;
		QueryPerformanceCounter(&g_ClockStart);
	}
}

double Sys_RunFloatTime()
{
	InitTime();

	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter( &CurrentTime );

	double fRawSeconds = (double)( CurrentTime.QuadPart - g_ClockStart.QuadPart ) / (double)(g_PerformanceFrequency.QuadPart);

	return fRawSeconds;
}

unsigned long Sys_RunMSTime()
{
	InitTime();

	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter( &CurrentTime );

	return (unsigned long) ( ( CurrentTime.QuadPart - g_ClockStart.QuadPart ) / g_MSPerformanceFrequency.QuadPart);
}


// -------------------------------------------------------------------------------------------------- //
// 默认的内存分配错误处理
// -------------------------------------------------------------------------------------------------- //
void Sys_DefaultAllocErrorFn( unsigned long size )
{
}

Sys_AllocErrorFn g_AllocError = Sys_DefaultAllocErrorFn;


CRITICAL_SECTION g_AllocCS;
class CAllocCSInit
{
public:
	CAllocCSInit()
	{
		InitializeCriticalSection( &g_AllocCS );
	}
	~CAllocCSInit()
	{
	// 不知何时析构，干脆就不析构，所以就会泄漏内存的，不过不要紧的！
	//	DeleteCriticalSection(&g_AllocCS);
	}
} g_AllocCSInit;


void* Sys_Alloc( unsigned long size )
{
	EnterCriticalSection( &g_AllocCS );
		void *pRet = MS_pMemAlloc->Alloc( size );
	LeaveCriticalSection( &g_AllocCS );
	if ( pRet )
	{
		return pRet;
	}
	else
	{
		g_AllocError( size );
		return 0;
	}
}


void* Sys_Realloc( void *ptr, unsigned long size )
{
	EnterCriticalSection( &g_AllocCS );
		void *pRet = MS_pMemAlloc->Realloc( ptr, size );
	LeaveCriticalSection( &g_AllocCS );
	if ( pRet )
	{
		return pRet;
	}
	else
	{
		g_AllocError( size );
		return 0;
	}
}


void Sys_Free( void *ptr )
{
	EnterCriticalSection( &g_AllocCS );
		MS_pMemAlloc->Free( ptr );
	LeaveCriticalSection( &g_AllocCS );
}


void Sys_SetAllocErrorFn( Sys_AllocErrorFn fn )
{
	g_AllocError = fn;
}



