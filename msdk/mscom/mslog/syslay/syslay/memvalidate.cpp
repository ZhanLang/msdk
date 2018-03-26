// memvalidate.cpp
// 说明：
// 1、这是一个堆内存的有效性检查的类
// 2、原理是在内存块前后做标记并跟踪之
// 3、通过 SYSLAY_VALIDATE_HEAP 宏来启用的
// 4、本身并不做内存分配器
//

#ifdef SYSLAY_VALIDATE_HEAP

#include <malloc.h>
#include "Syslay/dbg.h"
#include "Syslay/memalloc.h"


extern IMemAlloc *g_pActualAlloc;

//-----------------------------------------------------------------------------
// 一个堆内存的有效性检查的类
//-----------------------------------------------------------------------------
class CValidateAlloc : public IMemAlloc
{
public:
	enum
	{
		HEAP_PREFIX_BUFFER_SIZE = 12,
		HEAP_SUFFIX_BUFFER_SIZE = 8,
	};

	CValidateAlloc();

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

	// Crt调试版功能
	virtual long CrtSetBreakAlloc( long lNewBreakAlloc );
	virtual	int CrtSetReportMode( int nReportType, int nReportMode );
	virtual int CrtIsValidHeapPointer( const void *pMem );
	virtual int CrtCheckMemory( void );
	virtual int CrtSetDbgFlag( int nNewFlag );
	virtual void CrtMemCheckpoint( _CrtMemState *pState );
	void* CrtSetReportFile( int nRptType, void* hFile );
	void* CrtSetReportHook( void* pfnNewHook );
	int CrtDbgReport( int nRptType, const char * szFile,
			int nLine, const char * szModule, const char * pMsg );
	virtual int heapchk();

	// 设置新的调试输出函数、打印内存状态
	virtual void DumpStatus(int iType = 0);
	virtual void DbgOutputFunc( HeapReportFunc_t func , HeapReportFuncEx_t funcex = NULL );

private:
	struct HeapPrefix_t
	{
		HeapPrefix_t *m_pPrev;
		HeapPrefix_t *m_pNext;
		int m_nSize;
		unsigned char m_Prefix[HEAP_PREFIX_BUFFER_SIZE];
	};

	struct HeapSuffix_t
	{
		unsigned char m_Suffix[HEAP_SUFFIX_BUFFER_SIZE];
	};

private:
	HeapSuffix_t *Suffix( HeapPrefix_t *pPrefix );
	void *AllocationStart( HeapPrefix_t *pBase );
	HeapPrefix_t *PrefixFromAllocation( void *pAlloc );
	const HeapPrefix_t *PrefixFromAllocation( const void *pAlloc );

	// 加入链表
	void AddToList( HeapPrefix_t *pHeap, int nSize );

	// 从链表中删除
	void RemoveFromList( HeapPrefix_t *pHeap );

	// 堆是否有效
	bool ValidateAllocation( HeapPrefix_t *pHeap );

private:
	HeapPrefix_t *m_pFirstAllocation;
	char m_pPrefixImage[HEAP_PREFIX_BUFFER_SIZE];
	char m_pSuffixImage[HEAP_SUFFIX_BUFFER_SIZE];
};


//-----------------------------------------------------------------------------
// 定义一个全局实例
//-----------------------------------------------------------------------------
static CValidateAlloc s_ValidateAlloc;
IMemAlloc *MS_pMemAlloc = &s_ValidateAlloc;


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
CValidateAlloc::CValidateAlloc()
{
	m_pFirstAllocation = 0;
	memset( m_pPrefixImage, 0xBE, HEAP_PREFIX_BUFFER_SIZE );
	memset( m_pSuffixImage, 0xAF, HEAP_SUFFIX_BUFFER_SIZE );
}


//-----------------------------------------------------------------------------
// 简单属性
//-----------------------------------------------------------------------------
inline CValidateAlloc::HeapSuffix_t *CValidateAlloc::Suffix( HeapPrefix_t *pPrefix )
{
	return reinterpret_cast<HeapSuffix_t *>( (unsigned char*)( pPrefix + 1 ) + pPrefix->m_nSize );
}

inline void *CValidateAlloc::AllocationStart( HeapPrefix_t *pBase )
{
	return static_cast<void *>( pBase + 1 );
}

inline CValidateAlloc::HeapPrefix_t *CValidateAlloc::PrefixFromAllocation( void *pAlloc )
{
	if ( !pAlloc )
		return NULL;

	return ((HeapPrefix_t*)pAlloc) - 1;
}

inline const CValidateAlloc::HeapPrefix_t *CValidateAlloc::PrefixFromAllocation( const void *pAlloc )
{
	return ((const HeapPrefix_t*)pAlloc) - 1;
}


//-----------------------------------------------------------------------------
// 加入链表
//-----------------------------------------------------------------------------
void CValidateAlloc::AddToList( HeapPrefix_t *pHeap, int nSize )
{
	pHeap->m_pPrev = NULL;
	pHeap->m_pNext = m_pFirstAllocation;
	if ( m_pFirstAllocation )
	{
		m_pFirstAllocation->m_pPrev = pHeap;
	}
	pHeap->m_nSize = nSize;

	m_pFirstAllocation = pHeap;

	HeapSuffix_t *pSuffix = Suffix( pHeap );
	memcpy( pHeap->m_Prefix, m_pPrefixImage, HEAP_PREFIX_BUFFER_SIZE );
	memcpy( pSuffix->m_Suffix, m_pSuffixImage, HEAP_SUFFIX_BUFFER_SIZE );
}


//-----------------------------------------------------------------------------
// 从链表中删除
//-----------------------------------------------------------------------------
void CValidateAlloc::RemoveFromList( HeapPrefix_t *pHeap )
{
	if ( !pHeap )
		return;

	ValidateAllocation( pHeap );
	if ( pHeap->m_pPrev )
	{
		pHeap->m_pPrev->m_pNext = pHeap->m_pNext;
	}
	else
	{
		m_pFirstAllocation = pHeap->m_pNext;
	}

	if ( pHeap->m_pNext )
	{
		pHeap->m_pNext->m_pPrev = pHeap->m_pPrev;
	}
}


//-----------------------------------------------------------------------------
// 堆是否有效
//-----------------------------------------------------------------------------
bool CValidateAlloc::ValidateAllocation( HeapPrefix_t *pHeap )
{
	HeapSuffix_t *pSuffix = Suffix( pHeap );

	bool bOk = true;
	if ( memcmp( pHeap->m_Prefix, m_pPrefixImage, HEAP_PREFIX_BUFFER_SIZE ) )
	{
		bOk = false;
	}

	if ( memcmp( pSuffix->m_Suffix, m_pSuffixImage, HEAP_SUFFIX_BUFFER_SIZE ) )
	{
		bOk = false;
	}

	if ( !bOk )
	{
		Warning("Memory trash detected in allocation %X!\n", (void*)(pHeap+1) );
		Assert( 0 );
	}

	return bOk;
}

//-----------------------------------------------------------------------------
// 注意：
//		g_pActualAlloc 才是真正的内存分配器，因此只需调用即可
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Release versions
//-----------------------------------------------------------------------------
void *CValidateAlloc::Alloc( size_t nSize )
{
	int nActualSize = nSize + sizeof(HeapPrefix_t) + sizeof(HeapSuffix_t);
	HeapPrefix_t *pHeap = (HeapPrefix_t*)g_pActualAlloc->Alloc( nActualSize );
	AddToList( pHeap, nSize );
	return AllocationStart( pHeap );
}

void *CValidateAlloc::Realloc( void *pMem, size_t nSize )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	RemoveFromList( pHeap );

	int nActualSize = nSize + sizeof(HeapPrefix_t) + sizeof(HeapSuffix_t);
	pHeap = (HeapPrefix_t*)g_pActualAlloc->Realloc( pHeap, nActualSize );
	AddToList( pHeap, nSize );

	return AllocationStart( pHeap );
}

void CValidateAlloc::Free( void *pMem )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	RemoveFromList( pHeap );

	g_pActualAlloc->Free( pHeap );
}

void *CValidateAlloc::Expand( void *pMem, size_t nSize )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	RemoveFromList( pHeap );

	int nActualSize = nSize + sizeof(HeapPrefix_t) + sizeof(HeapSuffix_t);
	pHeap = (HeapPrefix_t*)g_pActualAlloc->Expand( pHeap, nActualSize );
	AddToList( pHeap, nSize );

	return AllocationStart( pHeap );
}


//-----------------------------------------------------------------------------
// Debug versions
//-----------------------------------------------------------------------------
void *CValidateAlloc::Alloc( size_t nSize, const char *pFileName, int nLine )
{
	int nActualSize = nSize + sizeof(HeapPrefix_t) + sizeof(HeapSuffix_t);
	HeapPrefix_t *pHeap = (HeapPrefix_t*)g_pActualAlloc->Alloc( nActualSize, pFileName, nLine );
	AddToList( pHeap, nSize );
	return AllocationStart( pHeap );
}

void *CValidateAlloc::Realloc( void *pMem, size_t nSize, const char *pFileName, int nLine )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	RemoveFromList( pHeap );

	int nActualSize = nSize + sizeof(HeapPrefix_t) + sizeof(HeapSuffix_t);
	pHeap = (HeapPrefix_t*)g_pActualAlloc->Realloc( pHeap, nActualSize, pFileName, nLine );
	AddToList( pHeap, nSize );

	return AllocationStart( pHeap );
}

void  CValidateAlloc::Free( void *pMem, const char *pFileName, int nLine )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	RemoveFromList( pHeap );

	g_pActualAlloc->Free( pHeap, pFileName, nLine );
}

void *CValidateAlloc::Expand( void *pMem, size_t nSize, const char *pFileName, int nLine )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	RemoveFromList( pHeap );

	int nActualSize = nSize + sizeof(HeapPrefix_t) + sizeof(HeapSuffix_t);
	pHeap = (HeapPrefix_t*)g_pActualAlloc->Expand( pHeap, nActualSize, pFileName, nLine );
	AddToList( pHeap, nSize );

	return AllocationStart( pHeap );
}


//-----------------------------------------------------------------------------
// 获得分配大小
//-----------------------------------------------------------------------------
size_t CValidateAlloc::GetSize( void *pMem )
{
	HeapPrefix_t *pHeap	= PrefixFromAllocation( pMem );
	return pHeap->m_nSize;
}


//-----------------------------------------------------------------------------
// 强制在分配位置上产生file + line的信息
//-----------------------------------------------------------------------------
void CValidateAlloc::PushAllocDbgInfo( const char *pFileName, int nLine )
{
	g_pActualAlloc->PushAllocDbgInfo( pFileName, nLine );
}

void CValidateAlloc::PopAllocDbgInfo()
{
	g_pActualAlloc->PopAllocDbgInfo( );
}

//-----------------------------------------------------------------------------
// Crt调试功能实现
//-----------------------------------------------------------------------------
long CValidateAlloc::CrtSetBreakAlloc( long lNewBreakAlloc )
{
	return g_pActualAlloc->CrtSetBreakAlloc( lNewBreakAlloc );
}

int CValidateAlloc::CrtSetReportMode( int nReportType, int nReportMode )
{
	return g_pActualAlloc->CrtSetReportMode( nReportType, nReportMode );
}

int CValidateAlloc::CrtIsValidHeapPointer( const void *pMem )
{
	const HeapPrefix_t *pHeap = PrefixFromAllocation( pMem );
	return g_pActualAlloc->CrtIsValidHeapPointer( pHeap );
}

int CValidateAlloc::CrtCheckMemory( void )
{
	return g_pActualAlloc->CrtCheckMemory( );
}

int CValidateAlloc::CrtSetDbgFlag( int nNewFlag )
{
	return g_pActualAlloc->CrtSetDbgFlag( nNewFlag );
}

void CValidateAlloc::CrtMemCheckpoint( _CrtMemState *pState )
{
	g_pActualAlloc->CrtMemCheckpoint( pState );
}

void* CValidateAlloc::CrtSetReportFile( int nRptType, void* hFile )
{
	return g_pActualAlloc->CrtSetReportFile( nRptType, hFile );
}

void* CValidateAlloc::CrtSetReportHook( void* pfnNewHook )
{
	return g_pActualAlloc->CrtSetReportHook( pfnNewHook );
}

int CValidateAlloc::CrtDbgReport( int nRptType, const char * szFile,
		int nLine, const char * szModule, const char * pMsg )
{
	return g_pActualAlloc->CrtDbgReport( nRptType, szFile, nLine, szModule, pMsg );
}

int CValidateAlloc::heapchk()
{
	bool bOk = true;

	// Validate the heap
	HeapPrefix_t *pHeap = m_pFirstAllocation;
	for( pHeap = m_pFirstAllocation; pHeap; pHeap = pHeap->m_pNext )
	{
		if ( !ValidateAllocation( pHeap ) )
		{
			bOk = false;
		}
	}

#ifdef _WIN32
	return bOk ? _HEAPOK : 0;
#elif _LINUX
	return bOk;
#endif
}

//-----------------------------------------------------------------------------
// 设置新的调试输出函数、打印内存状态
//-----------------------------------------------------------------------------
void CValidateAlloc::DumpStatus(int iType)
{
	g_pActualAlloc->DumpStatus(iType);
}

void CValidateAlloc::DbgOutputFunc( HeapReportFunc_t func , HeapReportFuncEx_t funcex/* = NULL */)
{
	g_pActualAlloc->DbgOutputFunc(func, funcex);
}

#endif // SYSLAY_VALIDATE_HEAP