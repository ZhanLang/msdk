#include "stdafx.h"
#include "WinService.h"
#include <assert.h>
#include <algorithm>

#include <msapi/mssrv.h>

#include "util\globalevent.h"
#include "ServiceOpt.h"


#if defined(_MEM_CHECK_)

		#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
		#endif

#endif


namespace winfunc
{

/////////////////////////////////////////////////////////////////////////////
CWinService::CWinService()
{
}

/////////////////////////////////////////////////////////////////////////////
CWinService::~CWinService()
{

}

/////////////////////////////////////////////////////////////////////////////
bool CWinService::SetServiceAutoStart(LPCTSTR lpszServiceName)
{
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->SetServiceAutoStart(%s)"), lpszServiceName?lpszServiceName:_T("null"));

	if(lpszServiceName == NULL) {
		return false;
	}

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);    
	if(NULL == schSCManager)
		return false;

	SC_HANDLE schService = ::OpenService(schSCManager,  lpszServiceName, SERVICE_ALL_ACCESS);

	bool ret = false;
	if(schService != NULL) {
		if (!::ChangeServiceConfig(
			schService,            
			SERVICE_NO_CHANGE,    
			SERVICE_AUTO_START,            
			SERVICE_NO_CHANGE,            
			NULL,    
			NULL,        
			NULL,            
			NULL,            
			NULL,        
			NULL,        
			NULL))
		{
			RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("ChangeServiceConfig Failed! Err Code: %d\n"), ::GetLastError());
		}
		else {
			ret = true;
		}
	}
	if(schService)        
		::CloseServiceHandle(schService);

	CloseServiceHandle(schSCManager);

	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->SetServiceAutoStart(%s) Return: %s\n"),  lpszServiceName?lpszServiceName:_T("null"), ret?_T("successed"):_T("failed"));
	return ret;
}

bool CWinService::InstallService(LPCTSTR lpszServiceName, LPCTSTR lpszDisplayName, DWORD dwServiceType, DWORD dwStartType, LPCTSTR lpszBinaryPathName/*, LPCTSTR lpLoadOrderGroup, LPCTSTR lpDependencies, DWORD dwErrorControl, LPCTSTR lpServiceStartName, LPCTSTR lpPassword*/)
{
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->InstallService(%s, %s, %d, %d, %s)"), lpszServiceName?lpszServiceName:_T("null"), lpszDisplayName?lpszDisplayName:_T("null"), dwServiceType, dwStartType, lpszBinaryPathName?lpszBinaryPathName:_T("null"));

	LPCTSTR lpLoadOrderGroup	= NULL, 
			lpDependencies		= NULL, 
			lpServiceStartName	= NULL,
			lpPassword			= NULL;
	DWORD	dwErrorControl		= SERVICE_ERROR_NORMAL;

	if (NULL == lpszServiceName || 0 == lpszServiceName[0])
	{
		return false;
	}

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);    
    if(NULL == schSCManager)
    {
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenSCManager Failed! Err Code: %d\n"), ::GetLastError());
        return false;
    }

    bool bRet = true;
    SC_HANDLE schService = ::OpenService(schSCManager, lpszServiceName, SERVICE_ALL_ACCESS);

    if (NULL == schService) 
    {
        schService = ::CreateService(
            schSCManager,
            lpszServiceName,
            lpszDisplayName,
            SERVICE_ALL_ACCESS,
            dwServiceType,
            dwStartType,
            dwErrorControl,
            lpszBinaryPathName,
            lpLoadOrderGroup,
            NULL,
            lpDependencies,
            lpServiceStartName,
            lpPassword);
        if (!schService)
        {
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("CreateService Failed! Err Code: %d\n"), ::GetLastError());
            bRet = false;
        }
    }
    else
    {
        if (!::ChangeServiceConfig(
            schService,            
            dwServiceType,    
            dwStartType,            
            dwErrorControl,            
            lpszBinaryPathName,    
            lpLoadOrderGroup,        
            NULL,            
            lpDependencies,            
            lpServiceStartName,        
            lpPassword,        
            lpszDisplayName))
        {
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("ChangeServiceConfig Failed! Err Code: %d\n"), ::GetLastError());
            bRet = false;
        }
    }

    if(schService)
    {
        ::CloseServiceHandle(schService);
    }
    ::CloseServiceHandle(schSCManager);

	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->InstallService(%s) Return: %s\n"),  lpszServiceName?lpszServiceName:_T("null"), bRet?_T("successed"):_T("failed"));
    return bRet;
}

/////////////////////////////////////////////////////////////////////////////
bool CWinService::UninstallService(LPCTSTR lpszServiceName)
{
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->UninstallService(%s)"), lpszServiceName?lpszServiceName:_T("null"));

	if (NULL == lpszServiceName || 0 == lpszServiceName[0])
	{
		return false;
	}

    SC_HANDLE schServiceMgr = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == schServiceMgr)
    {
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("OpenSCManager Failed! Err Code: %d\n"), ::GetLastError());
        return false;
    }
    bool bRet = true;
    SC_HANDLE schService = ::OpenService(schServiceMgr, lpszServiceName, SERVICE_ALL_ACCESS);
    if (NULL == schService)
    {
        if (ERROR_SERVICE_DOES_NOT_EXIST != ::GetLastError())
        {
			RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenService Failed And Service Already Exist! Err Code: %d\n"), ::GetLastError());
            bRet = false;
        }
    }
    else
    {
        bRet = BOOL2bool(::DeleteService(schService));
		if (!bRet)
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("DeleteService Failed! Err Code: %d\n"), ::GetLastError());
		}
        ::CloseServiceHandle(schService);
    }

    ::CloseServiceHandle(schServiceMgr);

	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->UninstallService(%s) Return: %s\n"),  lpszServiceName?lpszServiceName:_T("null"), bRet?_T("successed"):_T("failed"));	
    return bRet;
}

/////////////////////////////////////////////////////////////////////////////
bool CWinService::SetServiceFailureAction(LPCTSTR lpszServiceName, DWORD dwResetPeriod, LPTSTR lpCommand, DWORD dwFirstAction, DWORD dwFirstDelay, DWORD dwSecondAction, DWORD dwSecondDelay, DWORD dwAfterAction, DWORD dwAfterDelay)
{
	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->SetServiceFailureAction(%s, %d, %s, %d, %d, %d, %d, %d, %d)"), lpszServiceName?lpszServiceName:_T("null"), dwResetPeriod, lpCommand?lpCommand:_T("null"), dwFirstAction, dwFirstDelay, dwSecondAction, dwSecondDelay, dwAfterAction, dwAfterDelay);

	if (NULL == lpszServiceName || 0 == lpszServiceName[0])
	{
		return false;
	}

    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(NULL == schSCManager)
    {
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenSCManager Failed! Err Code: %d\n"), ::GetLastError());
        return false;
    }

    bool bRet = false;
    SC_HANDLE schService = ::OpenService(schSCManager, lpszServiceName, SERVICE_ALL_ACCESS);

    if (schService) 
    {
        HMODULE hAdvapi32 = LoadLibrary(_T("Advapi32.dll"));
        if (hAdvapi32)
        {
			typedef BOOL (WINAPI* PFNChangeServiceConfig2)(SC_HANDLE, DWORD, LPVOID);
#ifndef _UNICODE
			LPCSTR lpszDllEntryPoint = "ChangeServiceConfig2A";
#else
			LPCSTR lpszDllEntryPoint = "ChangeServiceConfig2W";
#endif
			PFNChangeServiceConfig2 pfnChangeServiceConfig2 = (PFNChangeServiceConfig2)GetProcAddress(hAdvapi32, lpszDllEntryPoint);
            if (pfnChangeServiceConfig2)
            {
                SERVICE_FAILURE_ACTIONS stServiceFailAction = {0};
                stServiceFailAction.dwResetPeriod = dwResetPeriod;
                stServiceFailAction.lpRebootMsg = NULL;
                stServiceFailAction.lpCommand = lpCommand;
                SC_ACTION stSCAction[3];

                stSCAction[0].Type = (SC_ACTION_TYPE)dwFirstAction;
                stSCAction[0].Delay = dwFirstDelay;

                stSCAction[1].Type = (SC_ACTION_TYPE)dwSecondAction;
                stSCAction[1].Delay = dwSecondDelay;

                stSCAction[2].Type = (SC_ACTION_TYPE)dwAfterAction;
                stSCAction[2].Delay = dwAfterDelay;

                stServiceFailAction.cActions = 3;
                stServiceFailAction.lpsaActions = stSCAction;

                if (pfnChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &stServiceFailAction))
                {
                    bRet = true;
                }
				else
				{
					RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("ChangeServiceConfig2 Failed! Err Code: %d\n"), ::GetLastError());
				}
            }
			else
			{
				RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("GetProcAddress(%s) Failed!\n"), lpszDllEntryPoint);
			}
            FreeLibrary(hAdvapi32);
        }
		else
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("LoadLibrary(Advapi32.dll) Failed!\n"));
		}
    }
	else
	{
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("OpenService Failed! Err Code: %d\n"), ::GetLastError());
	}

    if(schService)
    {        
        ::CloseServiceHandle(schService);
    }
    ::CloseServiceHandle(schSCManager);

	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->SetServiceFailureAction(%s) Return: %s\n"),  lpszServiceName?lpszServiceName:_T("null"), bRet?_T("successed"):_T("failed"));
    return bRet;
}

/////////////////////////////////////////////////////////////////////////////
bool CWinService::StartService(LPCTSTR lpszServiceName, DWORD dwNumServiceArgs, LPCTSTR* lpServiceArgVectors)
{
	/*
	CServiceOpt srvOpt;
	srvOpt.SetServiceName(lpszServiceName);
	if ( FAILED(srvOpt.Init()) )
	{
			RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("StartService CServiceOpt::Init %d\n"), ::GetLastError());
			return false;
	}
	
	if ( FAILED(srvOpt.Start(FALSE)) )
	{
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("StartService CServiceOpt::Start %d\n"), ::GetLastError());
		return false;
	}

	return true;
	*/
	
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->StartService(%s, %d)"), lpszServiceName?lpszServiceName:_T("null"), dwNumServiceArgs);
	for(DWORD i=0;i<dwNumServiceArgs;i++)
	{
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("			ServiceArg: %s"), lpServiceArgVectors+i);
	}	

	if (NULL == lpszServiceName || 0 == lpszServiceName[0])
	{
		return false;
	}

	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("OpenSCManager..."));
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == schSCManager)
	{
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenSCManager Failed! Err Code: %d\n"), ::GetLastError());
		return false;
	}

	SERVICE_STATUS	ssStatus			= {0};
	DWORD			dwOldCheckPoint		= 0;
	DWORD			dwStartTickCount	= 0;
	DWORD			dwWaitTime			= 0;

	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("OpenService(%s)..."), lpszServiceName?lpszServiceName:_T("null"));
	SC_HANDLE schService = ::OpenService(schSCManager, lpszServiceName, SERVICE_ALL_ACCESS);
	if (NULL == schService)
	{
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenService Failed! Err Code: %d\n"), ::GetLastError());
		::CloseServiceHandle(schSCManager);
		return false;
	}

	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("QueryServiceStatus(%s)..."), lpszServiceName?lpszServiceName:_T("null"));
	::QueryServiceStatus(schService, &ssStatus);

	if (SERVICE_RUNNING == ssStatus.dwCurrentState)
	{
		RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("Service is Running...\n"));
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		return true;
	}
	else if (SERVICE_START_PENDING != ssStatus.dwCurrentState)
	{
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("StartService(%s) at Status(0x%x)..."), lpszServiceName?lpszServiceName:_T("null"), ssStatus.dwCurrentState);
		if (!::StartService(schService, dwNumServiceArgs, lpServiceArgVectors))
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("StartService Failed! Err Code: %d\n"), ::GetLastError());
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return false;
		}

		// Check the status until the service is no longer start pending.
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("QueryServiceStatus(%s) after StartService..."), lpszServiceName?lpszServiceName:_T("null"));
		if (!::QueryServiceStatus(schService, &ssStatus))
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("QueryServiceStatus Failed! Err Code: %d\n"), ::GetLastError());
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return false;
		}
	}

	// Save the tick count and initial checkpoint.
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is
		// one tenth the wait hint, but no less than 1 second and no
		// more than 10 seconds.
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000)
		{
			dwWaitTime = 10000;
		}

		::Sleep(dwWaitTime);

		// Check the status again.
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("QueryServiceStatus(%s) after StartService..."), lpszServiceName?lpszServiceName:_T("null"));
		if (!::QueryServiceStatus(schService, &ssStatus))
		{
			RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("QueryServiceStatus Failed! Err Code: %d\n"), ::GetLastError());
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// The service is making progress.
			dwStartTickCount = ::GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (::GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint
				RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("No Progress...\n"));
				//break; 这里取消掉，因为有的服务启动慢的时候会直接导致失败
			}
		}
	}

	if (ssStatus.dwCurrentState != SERVICE_RUNNING)
	{
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Invalid Service Status(0x%x)!\n"), ssStatus.dwCurrentState);
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		return false;
	}

	::CloseServiceHandle(schService);
	::CloseServiceHandle(schSCManager);
	
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->StartService(%s) successed!"), lpszServiceName?lpszServiceName:_T("null"));
	return true;
	
}

/////////////////////////////////////////////////////////////////////////////

BOOL CWinService::InternalStopService (SC_HANDLE schService, SERVICE_STATUS* lpssStatus, LPCTSTR lpszServiceName) {
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->InternalStopService(%s)"), lpszServiceName?lpszServiceName:_T("null"));

	if (!::QueryServiceStatus(schService, lpssStatus)) {
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("QueryServiceStatus(%s) Failed! Err Code: %d\n"), lpszServiceName, ::GetLastError());
		return FALSE;
	}

	
	BOOL result = FALSE;/*
	if ((lpssStatus->dwControlsAccepted & SERVICE_ACCEPT_STOP) == 0) {
		USES_GUIDCONVERSION;
		TString	strEventName = GUID2S(CLSID_RSSRV);
		strEventName += _T("-");
		strEventName += lpszServiceName;

		CGlobalEvent globalEvent;
		if(globalEvent.OpenEvent(EVENT_MODIFY_STATE, FALSE, strEventName.c_str()))
			RSLOG(RSLOG_ALERT, _T("CGlobalEvent(%s) SetEvent"), strEventName.c_str());
			result = globalEvent.SetEvent();
	}
	else {
	*/
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("ControlService(%s) SERVICE_CONTROL_STOP"), lpszServiceName);
		result = ::ControlService(schService, SERVICE_CONTROL_STOP, lpssStatus);/*
	}
	*/
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->InternalStopService %s"), lpszServiceName?lpszServiceName:_T("null"),
																  result?_T("succeed"):_T("failed"));

	return result;
}

/////////////////////////////////////////////////////////////////////////////
bool CWinService::StopService(LPCTSTR lpszServiceName, DWORD dwTimeOut, bool bStopDependencies)
{
	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->StopService(%s, %d, %s)"), lpszServiceName?lpszServiceName:_T("null"), dwTimeOut, bStopDependencies?_T("StopDependencies"):_T("NoStopDependencies"));

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager)
	{
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("OpenSCManager Failed! Err Code: %d\n"), ::GetLastError());
		return false;
	}

    SC_HANDLE schService = ::OpenService(schSCManager, lpszServiceName, SERVICE_ALL_ACCESS);
	if (NULL == schService)
	{
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenService(%s) Failed! Err Code: %d\n"), lpszServiceName, ::GetLastError());
		::CloseServiceHandle(schSCManager);
		return false;
	}

	SERVICE_STATUS	ssStatus = {0};
	DWORD			dwStartTime = GetTickCount();

	// Make sure the service is not already stopped
	if (!::QueryServiceStatus(schService, &ssStatus))
	{
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("QueryServiceStatus(%s) Failed! Err Code: %d\n"), lpszServiceName, ::GetLastError());
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		return false;
	}

	if (ssStatus.dwCurrentState == SERVICE_STOPPED)
	{
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Service %s is Stopped...\n"), lpszServiceName);
		return true;
	}

	// If a stop is pending, just wait for it
	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// RSLOG(RSLOG_DETAIL, _T("%s's Stop is Pending...\n"), lpszServiceName);
		::Sleep(ssStatus.dwWaitHint);
		if (!::QueryServiceStatus(schService, &ssStatus))
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("QueryServiceStatus(%s) Failed! Err Code: %d\n"), lpszServiceName, ::GetLastError());
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return false;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
		{
			RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Service %s is Stopped...\n"), lpszServiceName);
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return true;
		}

		if (::GetTickCount() - dwStartTime > dwTimeOut)
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("Wait for Service %s Time Out!\n"), lpszServiceName);
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return false;
		}
	}

	// If the service is running, dependencies must be stopped first
	if (bStopDependencies)
	{
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Stop Service %s Dependencies...\n"), lpszServiceName);
		DWORD dwBytesNeeded = 0;
		DWORD dwCount		= 0;

		LPENUM_SERVICE_STATUS   lpDependencies	= NULL;
		ENUM_SERVICE_STATUS     ess;
		SC_HANDLE               hDepService		= NULL;

		// Pass a zero-length buffer to get the required buffer size
		if (::EnumDependentServices(schService, SERVICE_ACTIVE, lpDependencies, 0, &dwBytesNeeded, &dwCount ))
		{
			// If the Enum call succeeds, then there are no dependent
			// services so do nothing
		}
		else
		{
			if (::GetLastError() != ERROR_MORE_DATA)
			{
				RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("EnumDependentServices Failed! Err Code: %d\n"), ::GetLastError());
				::CloseServiceHandle(schService);
				::CloseServiceHandle(schSCManager);
				return false;
			}

			// Allocate a buffer for the dependencies
			lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

			if (!lpDependencies)
			{
				RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("HeapAlloc Failed! Err Code: %d\n"), ::GetLastError());
				::CloseServiceHandle(schService);
				::CloseServiceHandle(schSCManager);
				return false;
			}

			__try {
				// Enumerate the dependencies
				if (!::EnumDependentServices(schService, SERVICE_ACTIVE, lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
				{
					RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("EnumDependentServices Failed! Err Code: %d\n"), ::GetLastError());
					::CloseServiceHandle(schService);
					::CloseServiceHandle(schSCManager);
					return false;
				}

				for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++ )
				{
					ess = *(lpDependencies + dwIndex);

					// Open the service
					hDepService = ::OpenService(schSCManager, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
					if (!hDepService)
					{
						RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OpenService(%s) Failed! Err Code: %d\n"), ess.lpServiceName, ::GetLastError());
						::CloseServiceHandle(schService);
						::CloseServiceHandle(schSCManager);
						return false;
					}

					__try {
						// Send a stop code
						if (!InternalStopService(hDepService, &ssStatus, ess.lpServiceName))
						{
							RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("InternalStopService(%s) Failed! Err Code: %d\n"), ess.lpServiceName, ::GetLastError());
							::CloseServiceHandle(schService);
							::CloseServiceHandle(schSCManager);
							return false;
						}

						// Wait for the service to stop
						while (ssStatus.dwCurrentState != SERVICE_STOPPED)
						{
							::Sleep(ssStatus.dwWaitHint);
							if (!::QueryServiceStatus(hDepService, &ssStatus))
							{
								RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("QueryServiceStatus(%s) Failed! Err Code: %d\n"), ess.lpServiceName, ::GetLastError());
								::CloseServiceHandle(schService);
								::CloseServiceHandle(schSCManager);
								return false;
							}

							if (ssStatus.dwCurrentState == SERVICE_STOPPED)
							{
								RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Service %s is Stopped...\n"), ess.lpServiceName);
								break;
							}

							if (::GetTickCount() - dwStartTime > dwTimeOut)
							{
								RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("Wait for Service %s Time Out!\n"), ess.lpServiceName);
								::CloseServiceHandle(schService);
								::CloseServiceHandle(schSCManager);
								return false;
							}
						}
					}
					__finally
					{
						// Always release the service handle
						::CloseServiceHandle(hDepService);
					}
				}
			}
			__finally
			{
				// Always free the enumeration buffer
				HeapFree(GetProcessHeap(), 0, lpDependencies);
			}
		}
	}

	// Send a stop code to the main service
	if (!InternalStopService(schService, &ssStatus, lpszServiceName))
	{
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("InternalStopService(%s) Failed! Err Code: %d\n"), lpszServiceName, ::GetLastError());
		::CloseServiceHandle(schService);
		::CloseServiceHandle(schSCManager);
		return false;
	}

	// Wait for the service to stop
	while (ssStatus.dwCurrentState != SERVICE_STOPPED)
	{
		::Sleep(ssStatus.dwWaitHint);
		if (!::QueryServiceStatus(schService, &ssStatus)) {
			RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("QueryServiceStatus(%s) Failed! Err Code: %d\n"), lpszServiceName, ::GetLastError());
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return false;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)	{
			RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Service %s is Stopped...\n"), lpszServiceName);
			break;
		}

		if (::GetTickCount() - dwStartTime > dwTimeOut)
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("Wait for Service %s Time Out!\n"), lpszServiceName);
			::CloseServiceHandle(schService);
			::CloseServiceHandle(schSCManager);
			return false;
		}
	}

	// Return success
	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->StopService(%s) successed!"), lpszServiceName?lpszServiceName:_T("null"));
	::CloseServiceHandle(schService);
	::CloseServiceHandle(schSCManager);
	return true;
}



bool CWinService::StartServiceForLua(LPCTSTR lpszServiceName, LPTSTR lpszServiceArgs)
{
	static LPCTSTR args[20] = {0};
	LPTSTR   temp = lpszServiceArgs;
	int		num  = 0;
	if ( lpszServiceArgs && !_tcslen( lpszServiceArgs ) )
		temp = NULL;
	

	while (temp != NULL) {
		args[num++] = temp;
		if ((temp = _tcschr(temp, _T(' '))) != NULL) {
			*(temp++) = '\0';
		}
	}
	return CWinService::StartService(lpszServiceName, num, args);
}


bool CWinService::IsExistedService(LPCTSTR lpszServiceName)
{
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->IsExistedService(%s)"), lpszServiceName?lpszServiceName:_T("null"));

	if(lpszServiceName == NULL) {
		return false;
	}

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);    
	if(NULL == schSCManager)
		return false;

	SC_HANDLE schService = ::OpenService(schSCManager,  lpszServiceName, SERVICE_ALL_ACCESS);
	if(schService == NULL) {
		DWORD err = GetLastError();
		if (err ==  ERROR_SUCCESS || err == ERROR_SERVICE_DOES_NOT_EXIST )
		{
			CloseServiceHandle(schSCManager);
			return false;
		}
	}
	else {		
		CloseServiceHandle(schService);
	}
	CloseServiceHandle(schSCManager);
	

	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->IsExistedService(%s) Return: Existed!"),  lpszServiceName?lpszServiceName:_T("null"));
	return true;
}

bool CWinService::IsSrvRunning(LPCTSTR lpszServiceName)
{
	return BOOL2bool(msapi::IsSrvRunning(lpszServiceName));
}

bool CWinService::VerifyServiceInfomations(LPCTSTR lpszServiceName, DWORD dwStartType, DWORD dwCurrentState) {
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->VerifyServiceInfomations(%s, %d, %d)"), lpszServiceName?lpszServiceName:_T("null"), dwStartType, dwCurrentState);
	if (lpszServiceName == NULL)
		return false;

	SC_HANDLE				schSCManager = NULL;
	SC_HANDLE				schService   = NULL;
	SERVICE_STATUS			ssStatus	 = {0};
	LPQUERY_SERVICE_CONFIG	lpConfig	 = NULL;
	bool					done		 = false;
	do {
		lpConfig = (LPQUERY_SERVICE_CONFIG)::LocalAlloc(LPTR, 4096);
		if (lpConfig == NULL)
			break;

		schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
		if(NULL == schSCManager)
			break;

		schService = ::OpenService(schSCManager,  lpszServiceName, SERVICE_QUERY_STATUS|SERVICE_QUERY_CONFIG);
		if (schService == NULL)
			break;

		if (::QueryServiceStatus(schService, &ssStatus) == 0)
			break;

		DWORD dwBytesNeeded	= 0;
		if (::QueryServiceConfig(schService, lpConfig, 4096, &dwBytesNeeded) == 0)
			break;
		done = true;
	} while(0);

	bool result = false;
	if (!done) {
		RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("WinService->VerifyServiceInfomations(%s, %d, %d) Failed, errCode = %d"), 
							lpszServiceName, 
							dwStartType, 
							dwCurrentState,
							::GetLastError()
			 );
	}
	else {
		result = (ssStatus.dwCurrentState == dwCurrentState) 
				 && (lpConfig->dwStartType == dwStartType);
		RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("WinService->VerifyServiceInfomations(%s, %d, %d) Succeed, %s"), 
							lpszServiceName, 
							dwStartType, 
							dwCurrentState,
							result ? _T("Verified") : _T("not Verified")
			 );
	}
	if (schSCManager != NULL)
		::CloseServiceHandle(schSCManager);
	if (schService != NULL)
		::CloseServiceHandle(schService);
	return result;
}

BOOL CWinService::BindToLua( ILuaVM* pLuaVM )
{
	GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("WinService")
		+mluabind::Function("InstallService",			&winfunc::CWinService::InstallService)
		+mluabind::Function("UninstallService",			&winfunc::CWinService::UninstallService)
		+mluabind::Function("SetServiceFailureAction",  &winfunc::CWinService::SetServiceFailureAction)
		+mluabind::Function("StartService",				&winfunc::CWinService::StartServiceForLua)
		+mluabind::Function("StopService",				&winfunc::CWinService::StopService)		
		+mluabind::Function("IsExistedService",			&winfunc::CWinService::IsExistedService)
		+mluabind::Function("SetServiceAutoStart",		&winfunc::CWinService::SetServiceAutoStart)
		+mluabind::Function("VerifyServiceInfomations",	&winfunc::CWinService::VerifyServiceInfomations)
		+mluabind::Function("IsSrvRunning", &winfunc::CWinService::IsSrvRunning)

		

		);

	return TRUE;
}


};//namespace winfunc