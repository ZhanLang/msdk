#include "StdAfx.h"
#include ".\check.h"
#include "WtsApi32.h"

CCheck::CCheck(void)
{
	m_hModPSAPI = NULL;
	m_pfnEnumProcesses = NULL;
	m_pfnEnumProcessModules = NULL;
	m_pfnGetModuleFileNameEx = NULL;
	m_hKernel = NULL;

	m_pCreateToolhelp32Snapshot = NULL;
	m_pProcess32First = NULL;
	m_pProcess32Next = NULL;

	m_bXpAbove = IsXPAbove();

	ZeroMemory(m_szFileName,sizeof(m_szFileName));
	GetWindowsDirectory(m_szFileName,MAX_PATH);
	lstrcat(m_szFileName,_T("\\explorer.exe"));
	_tcslwr(m_szFileName);

	RsGetCurentPath(m_szPath,MAX_PATH);
	lstrcat(m_szPath,_T("\\tray.exe"));
	_tcslwr(m_szPath);

	GetDllFunc();
	//CheckExplorer();
}

CCheck::~CCheck(void)
{
	if(m_hModPSAPI)
	{
		FreeLibrary(m_hModPSAPI);
		m_hModPSAPI = NULL;
	}
	if(m_hKernel)
	{
		FreeLibrary(m_hKernel);
		m_hKernel = NULL;
	}

}

BOOL CCheck::GetDllFunc()
{
	if( GetVersion() < 0x80000000)
	{
		return GetDllFuncNT();
	}
	else
	{
		return GetDllFunc9x();
	}
	return TRUE;
}

BOOL CCheck::GetDllFunc9x()
{
	if (NULL == m_hKernel)
		m_hKernel = ::LoadLibrary(_T("kernel32.dll"));

	if(NULL == m_hKernel)
		return FALSE;

	m_pCreateToolhelp32Snapshot = (xCreateToolhelp32Snapshot)
		::GetProcAddress(m_hKernel,"CreateToolhelp32Snapshot");

	m_pProcess32First = (xProcess32First)
		::GetProcAddress(m_hKernel, "Process32First");

	m_pProcess32Next = (xProcess32Next)
		::GetProcAddress(m_hKernel, "Process32Next");
	return TRUE;
}

BOOL CCheck::GetDllFuncNT()
{
	if (NULL == m_hModPSAPI)
		m_hModPSAPI = ::LoadLibrary(_T("PSAPI.DLL"));

	if(m_hModPSAPI == NULL)
		return FALSE;

	m_pfnEnumProcesses = (PFNENUMPROCESSES)
		::GetProcAddress(m_hModPSAPI,"EnumProcesses");

	m_pfnEnumProcessModules = (PFNENUMPROCESSMODULES)
		::GetProcAddress(m_hModPSAPI, "EnumProcessModules");

#ifdef _UNICODE
	m_pfnGetModuleFileNameEx = (PFNGETMODULEFILENAMEEX)
		::GetProcAddress(m_hModPSAPI, "GetModuleFileNameExW");
#else
	m_pfnGetModuleFileNameEx = (PFNGETMODULEFILENAMEEX)
		::GetProcAddress(m_hModPSAPI, "GetModuleFileNameExA");
#endif
	return TRUE;
	
}

DWORD CCheck::GetCurrentSession(DWORD dwPid)
{
	HMODULE hKernal32 = LoadLibrary(_T("kernel32.dll"));
	if(hKernal32 == NULL)
	{
		return -1;
	}

	typedef BOOL (WINAPI *FNPTS )(DWORD , DWORD*);
	DWORD dwSid = -1;
	FNPTS fnPts = NULL;
	fnPts = (FNPTS)GetProcAddress(hKernal32 , "ProcessIdToSessionId");
	if(fnPts)
	{
		//dwPid = GetCurrentProcessId();
		fnPts(dwPid , &dwSid);
	}
	FreeLibrary(hKernal32);
	return dwSid;
}



DWORD CCheck::CheckExplorerNT(DWORD dwSessionID)
{
	RASSERTP(m_pfnEnumProcesses,0); 
	RASSERTP(m_pfnEnumProcessModules,0); 
	RASSERTP(m_pfnGetModuleFileNameEx,0); 

	DWORD dwExplorerID  = 0; 
	BOOL  b64 = FALSE;
	DWORD pidArray[1024];
	DWORD cbNeeded;
	DWORD nProcesses;

	if ( ! m_pfnEnumProcesses(pidArray, sizeof(pidArray), &cbNeeded))
		return 0;

	nProcesses = cbNeeded / sizeof(DWORD); 
	for (DWORD i = 0; i < nProcesses; i++)
	{
		HMODULE hModuleArray[1024];
		HANDLE hProcess;
		DWORD pid = pidArray[i];
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |	PROCESS_VM_READ,FALSE, pid);
		if (!hProcess)
			continue;

		if (!m_pfnEnumProcessModules(hProcess, hModuleArray,sizeof(hModuleArray), &cbNeeded) )
		{
			::CloseHandle(hProcess);
			continue;
		}

		TCHAR szModuleName[MAX_PATH]={0};
		m_pfnGetModuleFileNameEx(hProcess, hModuleArray[0],szModuleName, sizeof(szModuleName));
		::CloseHandle(hProcess);

		if(_tcscmp(_tcslwr(szModuleName),m_szFileName) == 0 || _tcscmp(_tcslwr(szModuleName),m_szPath) == 0 )
		{	
			dwExplorerID =  pidArray[i];
			if(dwSessionID != 0xFFFFFFFF  && m_bXpAbove)
			{
				DWORD dwSid = GetCurrentSession(dwExplorerID);
				if(dwSid == dwSessionID) 
					break;
				dwExplorerID = 0;
			}
			else
			   break;
		}

	} // for
	return dwExplorerID;
}

DWORD CCheck::CheckExplorer9x(DWORD dwSessionID)
{
	DWORD  dwExplorerID  = 0; 
	BOOL  b64 = FALSE;
	RASSERTP(m_pCreateToolhelp32Snapshot,0); 
	RASSERTP(m_pProcess32First,0); 
	RASSERTP(m_pProcess32Next,0); 

	HANDLE         hProcessSnap = NULL; 
	PROCESSENTRY32 pe32      = {0}; 

	hProcessSnap = m_pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		RSLOG(RSXML_DETAIL, _T("CreateToolhelp32Snapshot error =%d\n"),GetLastError());
		return 0; 
	}

	pe32.dwSize = sizeof(PROCESSENTRY32); 
	if (! m_pProcess32First(hProcessSnap, &pe32)) 
	{
		CloseHandle (hProcessSnap); 
		return 0;
	}
	
	do 
	{ 
		if(_tcscmp(_tcslwr(pe32.szExeFile),m_szFileName)==0 || _tcscmp(_tcslwr(pe32.szExeFile),m_szPath)==0)
		{
			dwExplorerID = pe32.th32ProcessID; 
			break;
		}
	} 
	while (m_pProcess32Next(hProcessSnap, &pe32)); 
   
	CloseHandle (hProcessSnap); 
	return dwExplorerID;
}

DWORD CCheck::CheckExplorer(DWORD dwSessionID)
{
	if( GetVersion() < 0x80000000 )
	{
		return CheckExplorerNT(dwSessionID);
	}
	return CheckExplorer9x(dwSessionID);
}

typedef BOOL (WINAPI * FnWTSEnumerateSessions)(
	HANDLE hServer,
	DWORD Reserved,
	DWORD Version,
	PWTS_SESSION_INFO* ppSessionInfo,
	DWORD* pCount
	);
typedef  void (WINAPI *FnWTSFreeMemory)(PVOID pMemory);

DWORD  CCheck::GetActiveExplorer(DWORD& dwSessionID)
{
	RASSERTP(m_pfnEnumProcesses,0); 
	RASSERTP(m_pfnEnumProcessModules,0); 
	RASSERTP(m_pfnGetModuleFileNameEx,0); 

	DWORD    dwExplorerID  = 0; 
	BOOL     b64 = FALSE;
	WTS_SESSION_INFO* pSesionBuf = NULL; 
	DWORD dwSessionNum =0;
	dwSessionID = 0;

	if( ! m_bXpAbove )
		return 0;

	HMODULE hWtsapi32 = LoadLibrary(_T("Wtsapi32.dll"));
	RASSERTP(hWtsapi32,0); 
	FnWTSEnumerateSessions pWTSEnumerateSessions = (FnWTSEnumerateSessions) GetProcAddress(hWtsapi32, "WTSEnumerateSessionsA");
	FnWTSFreeMemory pWTSFreeMemory = (FnWTSFreeMemory) GetProcAddress(hWtsapi32, "WTSFreeMemory");
	RASSERTP(pWTSEnumerateSessions,0); 
	RASSERTP(pWTSFreeMemory,0); 

	if(! pWTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE,0,1,&pSesionBuf,&dwSessionNum))
	{
		FreeLibrary(hWtsapi32);
		return 0;
	}

	DWORD pidArray[1024];
	DWORD cbNeeded;
	DWORD nProcesses;
	if ( ! m_pfnEnumProcesses(pidArray, sizeof(pidArray), &cbNeeded))
	{
		if(pSesionBuf)
		{
			pWTSFreeMemory(pSesionBuf);
		}
		FreeLibrary(hWtsapi32);
		return 0;
	}
	
	nProcesses = cbNeeded / sizeof(DWORD); 
	for (DWORD i = 0; i < nProcesses; i++)
	{
		HMODULE hModuleArray[1024];
		HANDLE hProcess;
		DWORD pid = pidArray[i];
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |	PROCESS_VM_READ,FALSE, pid);
		if (!hProcess)
			continue;

		if (!m_pfnEnumProcessModules(hProcess, hModuleArray,	sizeof(hModuleArray), &cbNeeded) )
		{
			::CloseHandle(hProcess);
			continue;
		}
		TCHAR szModuleName[MAX_PATH]={0};
		m_pfnGetModuleFileNameEx(hProcess, hModuleArray[0],szModuleName, sizeof(szModuleName));
		::CloseHandle(hProcess);  

		if(_tcscmp(_tcslwr(szModuleName),m_szFileName) == 0 || _tcscmp(_tcslwr(szModuleName),m_szPath) == 0 )
		{	
			dwExplorerID =  pidArray[i];

			DWORD dwSid = GetCurrentSession(dwExplorerID);

			BOOL bFind = FALSE;
			for(DWORD j=0;j<dwSessionNum ;j++)
			{
				WTS_SESSION_INFO*ppBuf =(WTS_SESSION_INFO * ) ( pSesionBuf +j);

				if( ppBuf->SessionId == dwSid && ppBuf->State == WTSActive )
				{
					dwSessionID = dwSid;
					bFind = TRUE;
					//break;
				}
			}
			if(bFind) 
				break;
			dwExplorerID = 0;			
		}

	} // for

	if(pSesionBuf)
	{
		pWTSFreeMemory(pSesionBuf);
	}
	FreeLibrary(hWtsapi32);

	return dwExplorerID;
}