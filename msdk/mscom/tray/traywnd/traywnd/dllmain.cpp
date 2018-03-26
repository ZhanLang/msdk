// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "tray\traymenudef.h"

HINSTANCE g_hinstance = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hinstance = (HINSTANCE)hModule;
			//_CrtSetBreakAlloc(195);
			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#include "tray.h"
#include "TraySrv.h"
#include "TrayMenuBase.h"

BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_PROGID(ClSID_CTrayWnd, CTray, _T("TrayWnd"))
		CLIDMAPENTRY_PROGID(ClSID_CTraySrv, CTraySrv, _T("TraySrv"))
		CLIDMAPENTRY_PROGID(ClSID_CTrayMenuBase, CTrayMenuBase, _T("TrayMenuBase"))
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN

