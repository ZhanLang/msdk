// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "filestore.h"
extern "C"
{
	#include "..\vdlib\vdisk_dll.h"
};

BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY(CLSID_FileStore , CFileStore)
	CLIDMAPENTRY_END
END_CLIDMAP
DEFINE_ALL_EXPORTFUN

HMODULE g_hinstance = NULL; 
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved) \
{ 
	switch (ul_reason_for_call) 
	{ 
	case DLL_PROCESS_ATTACH:
		g_hinstance = hModule; 
		/*_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);*/ 
		DisableThreadLibraryCalls((HMODULE)hModule); 
		vdisk_init();
		break;
	case DLL_THREAD_ATTACH: 
	case DLL_THREAD_DETACH: 
		break;
	case DLL_PROCESS_DETACH:
		vdisk_uninit();
		break; 
	} 
	return TRUE; 
}