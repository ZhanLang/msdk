// memstd.cpp

#ifndef _DEBUG

#include <malloc.h>
#include "mslog/Syslay/dbg.h"
#include "mslog/Syslay/memalloc.h"


//-----------------------------------------------------------------------------
// Release版内存分配器
//-----------------------------------------------------------------------------
class CStdMemAlloc : public IMemAlloc
{
public:
	// Release versions
	virtual void *Alloc( size_t nSize );
	virtual void *Realloc( void *pMem, size_t nSize );
	virtual void  Free( void *pMem );
    virtual void *Expand( void *pMem, size_t nSize );

	// Debug versions
    virtual void *Alloc( size_t nSize, const char *pFileName, int nLine );
    virtual void *Realloc( void *pMem, size_t nSize, const char *pFileName, int nLine );
    virtual void  Free( void *pMem, const char *pFileName, int nLine );
    virtual void *Expand( void *pMem, size_t nSize, const char *pFileName, int nLine );

	// 获得分配大小
	virtual size_t GetSize( void *pMem );

    // 强制在分配位置上产生file + line的信息
    virtual void PushAllocDbgInfo( const char *pFileName, int nLine );
    virtual void PopAllocDbgInfo();

	// Crt调试功能实现
	virtual long CrtSetBreakAlloc( long lNewBreakAlloc );
	virtual	int CrtSetReportMode( int nReportType, int nReportMode );
	virtual int CrtIsValidHeapPointer( const void *pMem );
	virtual int CrtCheckMemory( void );
	virtual int CrtSetDbgFlag( int nNewFlag );
	virtual void CrtMemCheckpoint( _CrtMemState *pState );

	// Crt的堆调试跟踪报告接口
	void* CrtSetReportFile( int nRptType, void* hFile );
	void* CrtSetReportHook( void* pfnNewHook );
	int CrtDbgReport( int nRptType, const char * szFile,
			int nLine, const char * szModule, const char * pMsg );
	virtual int heapchk();

	// 设置新的调试输出函数、打印内存状态
	virtual void DumpStatus() {}
	virtual void DbgOutputFunc( HeapReportFunc_t func ) {}

private:
};


//-----------------------------------------------------------------------------
// 定义一个全局实例
//-----------------------------------------------------------------------------
static CStdMemAlloc s_StdMemAlloc;

#ifndef SYSLAY_VALIDATE_HEAP
IMemAlloc *MS_pMemAlloc = &s_StdMemAlloc;
#else
IMemAlloc *g_pActualAlloc = &s_StdMemAlloc;
#endif


//-----------------------------------------------------------------------------
// Release versions
//-----------------------------------------------------------------------------
void *CStdMemAlloc::Alloc( size_t nSize )
{
	return malloc( nSize );
}

void *CStdMemAlloc::Realloc( void *pMem, size_t nSize )
{
	return realloc( pMem, nSize );
}

void CStdMemAlloc::Free( void *pMem )
{
	free( pMem );
}

void *CStdMemAlloc::Expand( void *pMem, size_t nSize )
{
#ifdef _WIN32
	return _expand( pMem, nSize );
#elif _LINUX
	return realloc( pMem, nSize );
#endif
}


//-----------------------------------------------------------------------------
// Debug versions
//-----------------------------------------------------------------------------
void *CStdMemAlloc::Alloc( size_t nSize, const char *pFileName, int nLine )
{
	return malloc( nSize );
}

void *CStdMemAlloc::Realloc( void *pMem, size_t nSize, const char *pFileName, int nLine )
{
	return realloc( pMem, nSize );
}

void  CStdMemAlloc::Free( void *pMem, const char *pFileName, int nLine )
{
	free( pMem );
}

void *CStdMemAlloc::Expand( void *pMem, size_t nSize, const char *pFileName, int nLine )
{
#ifdef _WIN32
	return _expand( pMem, nSize );
#elif _LINUX
	return realloc( pMem, nSize );
#endif
}


//-----------------------------------------------------------------------------
// 获得分配大小
//-----------------------------------------------------------------------------
size_t CStdMemAlloc::GetSize( void *pMem )
{
#ifdef _WIN32
	return _msize( pMem );
#elif _LINUX
	Assert( "GetSize() not implemented");
#endif
}


//-----------------------------------------------------------------------------
// 强制在分配位置上产生file + line的信息
//-----------------------------------------------------------------------------
void CStdMemAlloc::PushAllocDbgInfo( const char *pFileName, int nLine )
{
}

void CStdMemAlloc::PopAllocDbgInfo()
{
}

//-----------------------------------------------------------------------------
// Crt调试功能实现， Release下没有用。将来或许自己写个堆调度。。。
//-----------------------------------------------------------------------------
long CStdMemAlloc::CrtSetBreakAlloc( long lNewBreakAlloc )
{
	return 0;
}

int CStdMemAlloc::CrtSetReportMode( int nReportType, int nReportMode )
{
	return 0;
}

int CStdMemAlloc::CrtIsValidHeapPointer( const void *pMem )
{
	return 1;
}

int CStdMemAlloc::CrtCheckMemory( void )
{
	return 1;
}

int CStdMemAlloc::CrtSetDbgFlag( int nNewFlag )
{
	return 0;
}

void CStdMemAlloc::CrtMemCheckpoint( _CrtMemState *pState )
{
}

// Crt的堆调试跟踪报告接口
void* CStdMemAlloc::CrtSetReportFile( int nRptType, void* hFile )
{
	return 0;
}

void* CStdMemAlloc::CrtSetReportHook( void* pfnNewHook )
{
	return 0;
}

int CStdMemAlloc::CrtDbgReport( int nRptType, const char * szFile,
		int nLine, const char * szModule, const char * pMsg )
{
	return 0;
}

int CStdMemAlloc::heapchk()
{
#ifdef _WIN32
	return _HEAPOK;
#elif _LINUX
	return 1;
#endif
}


#endif
