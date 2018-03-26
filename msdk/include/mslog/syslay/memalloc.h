//
// 项目名称: 
//
// 实现功能: 作为内存分配与管理模块，定义内存分配器接口，用于跟踪内存的分配。
//
// 文件名称: memalloc.h
//
// 创建作者: 
//
// 创建日期: 
//
// 修改作者: 
//
// 修改日期: 
//
// （如有新的修改人请在这里添加）
//
// 使用说明: 
// CValidateAlloc类，用来在分配的内存的前后加入保护标记，是个可选类
// CStdMemAlloc类，用于Release版的调用系统内存分配接口
// CDbgMemAlloc类，用于Debug版的调用带调试信息的系统内存分配接口
//
// 关于MS_pMemAlloc的实例指针：
// 1、默认情况下（没有定义SYSLAY_VALIDATE_HEAP宏）
//	Release版：	MS_pMemAlloc = CStdMemAlloc；
//  Debug版：	MS_pMemAlloc = CDbgMemAlloc；
// 2、定义了SYSLAY_VALIDATE_HEAP宏
//	Release版：	MS_pMemAlloc = CValidateAlloc - > CStdMemAlloc；
//  Debug版：	MS_pMemAlloc = CValidateAlloc - > CDbgMemAlloc；
//
// 将来我们可以写自己的内存堆的调度管理函数，这些类可以相应的简化。。。
//=============================================================================

#ifndef _MEMALLOC_INCLUDE_H_
#define _MEMALLOC_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif


#include <stddef.h>
#include "mem.h"


struct _CrtMemState;


enum
{
	ReportType_MemTotals = 0,
};

enum
{
	BYTE_COUNT_16 = 0,
	BYTE_COUNT_32,
	BYTE_COUNT_128,
	BYTE_COUNT_1024,
	BYTE_COUNT_GREATER,
	
	NUM_BYTE_COUNT_BUCKETS
};

struct st_Totals_MemInfo_
{
	st_Totals_MemInfo_()
	{
		memset( this, 0, sizeof(*this) );
	}
	
	// Size in bytes
	int m_nCurrentSize;
	int m_nPeakSize;
	int m_nTotalSize;
	int m_nOverheadSize;
	int m_nPeakOverheadSize;
	
	// Count in terms of # of allocations
	int m_nCurrentCount;
	int m_nPeakCount;
	int m_nTotalCount;
	
	// Count in terms of # of allocations of a particular size
	int m_pCount[NUM_BYTE_COUNT_BUCKETS];
	
	// Time spent allocating + deallocating	(microseconds)
	__int64 m_nTime;
};

struct _Crt_Mem_Status_Item : public st_Totals_MemInfo_
{
	char m_szFilename[MAX_PATH];
	int m_nLine;
};

typedef struct _Crt_Mem_Status_Item	CRT_MEM_ITEM, * PCRT_MEM_ITEM;

typedef struct _Crt_Mem_Status_Head
{
	int ReportType;
	int length;
	int count;
	int Reserved;
	CRT_MEM_ITEM item[1];
}CRT_MEM_HEAD, * PCRT_MEM_HEAD;

//-----------------------------------------------------------------------------
// 定义内存分配器接口
//-----------------------------------------------------------------------------
class IMemAlloc
{
public:
	// Release versions
	virtual void *Alloc( size_t nSize ) = 0;
	virtual void *Realloc( void *pMem, size_t nSize ) = 0;
	virtual void Free( void *pMem ) = 0;
    virtual void *Expand( void *pMem, size_t nSize ) = 0;

	// Debug versions
    virtual void *Alloc( size_t nSize, const char *pFileName, int nLine ) = 0;
    virtual void *Realloc( void *pMem, size_t nSize, const char *pFileName, int nLine ) = 0;
    virtual void  Free( void *pMem, const char *pFileName, int nLine ) = 0;
    virtual void *Expand( void *pMem, size_t nSize, const char *pFileName, int nLine ) = 0;

	// 获得分配大小
	virtual size_t GetSize( void *pMem ) = 0;

	// 强制在分配位置上产生file + line的信息
    virtual void PushAllocDbgInfo( const char *pFileName, int nLine ) = 0;
    virtual void PopAllocDbgInfo() = 0;

	// Crt调试功能实现
	virtual long CrtSetBreakAlloc( long lNewBreakAlloc ) = 0;
	virtual	int CrtSetReportMode( int nReportType, int nReportMode ) = 0;
	virtual int CrtIsValidHeapPointer( const void *pMem ) = 0;
	virtual int CrtCheckMemory( void ) = 0;
	virtual int CrtSetDbgFlag( int nNewFlag ) = 0;
	virtual void CrtMemCheckpoint( _CrtMemState *pState ) = 0;
	
	// 设置新的调试输出函数、打印内存状态	
	typedef void (*HeapReportFunc_t)( unsigned char * pBuffer, int iLen );
	virtual void DbgOutputFunc( HeapReportFunc_t func) = 0;	
	virtual void DumpStatus() = 0;

	// Crt的堆调试跟踪报告接口
	virtual void* CrtSetReportFile( int nRptType, void* hFile ) = 0;
	virtual void* CrtSetReportHook( void* pfnNewHook ) = 0;
	virtual int CrtDbgReport( int nRptType, const char * szFile,
			int nLine, const char * szModule, const char * pMsg ) = 0;

	virtual int heapchk() = 0;
};


//-----------------------------------------------------------------------------
// 导出一个全局的内存分配对象
//-----------------------------------------------------------------------------
MEM_INTERFACE IMemAlloc *MS_pMemAlloc;


#endif // _MEMALLOC_INCLUDE_H_
