// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "CDBSrv.h"

HINSTANCE g_hinstance = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hinstance = hModule;
#ifdef _DEBUG
		//_CrtSetBreakAlloc(186);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		{
			//线程 Detach时，清理指定的内存。
			CExprNodeAllocatorTlsMgr::ReleaseOne();
		}
		break;
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}

BEGIN_CLIDMAP

//#ifdef _DEBUG
//
//	CLIDMAPENTRY_BEGIN
//	CLIDMAPENTRY(CLSID_ITaskDBFast, CDBFastSrv)
//	CLIDMAPENTRY_END
//#endif

#ifdef DISKLESS
	CLIDMAPENTRY_BEGIN
	CLIDMAPENTRY(CLSID_ITaskDBFastDiskLess, CDBFastSrv)
	CLIDMAPENTRY_END
	#pragma message("DISKLESS")
#else
	CLIDMAPENTRY_BEGIN
 		CLIDMAPENTRY(CLSID_ITaskDBFast, CDBFastSrv)
	CLIDMAPENTRY_END

	#pragma message("DISKHAS")
#endif

//#endif

	


END_CLIDMAP_AND_EXPORTFUN

