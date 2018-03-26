// rsdebug.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "rs_debug.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

static CTraceMessage s_trace;

DbgRetval_t DbgMsg( DbgType_t spewType, const char* pMsg )
{
	return s_trace.DbgMsgToDebugView(spewType, pMsg);
}

DBG_INTERFACE void RegistDebugInfo(const char* szProductName, const char* szModName, IN unsigned int dwFormat, IN const char* szWorkPath)
{
	s_trace.ReadDebugInfoSetting(szProductName, szModName, dwFormat, szWorkPath);	
}

DBG_INTERFACE void SetDebugEnv(IN DbgOutputFunc_t fn)
{
	if(fn==NULL)
		DbgOutputFunc(DbgMsg);
}
