// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "CDBSrvRemote.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		//_CrtSetBreakAlloc(186);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY(CLSID_ITaskDBFastRemote, CDBFastSrvRemote)		
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN

