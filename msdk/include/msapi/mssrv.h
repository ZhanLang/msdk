#pragma once
namespace msdk{;
namespace msapi{;

static BOOL IsSrvExisted(LPCTSTR lpszServiceName)
{
	if ( !(lpszServiceName && _tcslen(lpszServiceName)))
		return FALSE;

	BOOL bRet = FALSE;
	SC_HANDLE schSCManager = NULL, schService = NULL;
	do 
	{
		schSCManager = ::OpenSCManager(NULL, NULL, SERVICE_QUERY_STATUS);    
		if(!schSCManager)
			break;

		schService = ::OpenService(schSCManager,  lpszServiceName, SERVICE_QUERY_STATUS);
		if ( schService )
			bRet = TRUE;

	} while (0);
	
	if ( schService )
		CloseServiceHandle(schService);

	if ( schSCManager )
		CloseServiceHandle(schSCManager);

	return bRet;
}

static BOOL IsSrvRunning(LPCTSTR lpszServiceName)
{
	if ( !(lpszServiceName && _tcslen(lpszServiceName)))
		return FALSE;

	BOOL bRet = FALSE;
	SC_HANDLE schSCManager = NULL, schService = NULL;
	do 
	{
		schSCManager = ::OpenSCManager(NULL, NULL, SERVICE_QUERY_STATUS);    
		if(!schSCManager)
			break;

		schService = ::OpenService(schSCManager,  lpszServiceName, SERVICE_QUERY_STATUS);
		if ( !schService )
			break;

		SERVICE_STATUS	ssStatus			= {0};
		if ( !::QueryServiceStatus(schService, &ssStatus) )
			break;


		if (SERVICE_RUNNING == ssStatus.dwCurrentState)
			bRet = TRUE;


	} while (0);


	if ( schService )
		CloseServiceHandle(schService);

	if ( schSCManager )
		CloseServiceHandle(schSCManager);
	

	return bRet;
}


static BOOL IsSrvStoped(LPCTSTR lpszServiceName)
{
	if ( !(lpszServiceName && _tcslen(lpszServiceName)))
		return FALSE;

	BOOL bRet = FALSE;
	SC_HANDLE schSCManager = NULL, schService = NULL;
	do 
	{
		schSCManager = ::OpenSCManager(NULL, NULL, SERVICE_QUERY_STATUS);    
		if(!schSCManager)
			break;

		schService = ::OpenService(schSCManager,  lpszServiceName, SERVICE_QUERY_STATUS);
		if ( !schService )
			break;

		SERVICE_STATUS	ssStatus			= {0};
		if ( !::QueryServiceStatus(schService, &ssStatus) )
			break;


		if (SERVICE_STOPPED == ssStatus.dwCurrentState)
			bRet = TRUE;


	} while (0);


	if ( schService )
		CloseServiceHandle(schService);

	if ( schSCManager )
		CloseServiceHandle(schSCManager);


	return bRet;
}


};};