// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "SoftReportPerson.h"

HINSTANCE g_hInst = NULL;
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
			g_hinstance = g_hInst = (HINSTANCE)hModule;
			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		}
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
	CLIDMAPENTRY_NOROT(CLSID_SoftReportPerson,CSoftReportPerson)
	CLIDMAPENTRY_END
	END_CLIDMAP_AND_EXPORTFUN;

#include "SoftScan.h"

extern "C"
{
	 ISoftScan* WINAPI CreateSoftEng(ISoftCallback* pCallback)
	{
		return new CSoftScan(pCallback);
	}
	 VOID WINAPI DestorySoftEng( ISoftScan* pSoftScan)
	{
		CSoftScan* pSoft = (CSoftScan*)pSoftScan;
		if ( pSoft )
		{
			delete pSoft;
		}
	}
}

