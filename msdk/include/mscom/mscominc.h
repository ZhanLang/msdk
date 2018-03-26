#pragma once
/* 使用方法
USE_DEFAULT_DLL_MAIN;//需要替换原来的DllMain
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		//CLIDMAPENTRY(CLSID_...,CLASS)
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN;
*/

#include <windows.h>

#include <mscom/msbase.h>
#include <util/utility.h>

#include <mscom/mscombase.h>
#include <mscom/comregiest.h>
#include <mscom/msstdcom.h>
#ifndef DISABLE_NAMESPACE_msdk
using namespace msdk;
#include <mscom/msplugin.h>
#include <mscom/mscom_ex.h>
#include <combase/IMsBuffer.h>
#include <combase/IMsString.h>
#include <mscom/srvbase.h>

#include <crtdbg.h>
extern HMODULE  g_hinstance;


#define USE_DEFAULT_DLL_MAIN \
HMODULE g_hinstance = NULL; \
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved) \
{ \
	switch (ul_reason_for_call) \
	{ \
	case DLL_PROCESS_ATTACH: \
		g_hinstance = hModule; \
		/*_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);*/ \
		DisableThreadLibraryCalls((HMODULE)hModule); \
		break;\
	case DLL_THREAD_ATTACH: \
	case DLL_THREAD_DETACH: \
	case DLL_PROCESS_DETACH: \
		break; \
	} \
return TRUE; \
}

#endif


