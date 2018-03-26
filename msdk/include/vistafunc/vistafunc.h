// VistaTools.h

#if !defined(_INCLUDE_VISTAFUNC_H__)
#define _INCLUDE_VISTAFUNC_H__

#include <UserEnv.h>
#include <WtsApi32.h>
#include "Psapi.h"

#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib , "Psapi.lib")

namespace vistafunc{
class CVistaTools
{
public:
	static BOOL IsVista()
	{
		OSVERSIONINFO osver;

		osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		if (	::GetVersionEx( &osver ) &&
			osver.dwPlatformId == VER_PLATFORM_WIN32_NT &&
			(osver.dwMajorVersion >= 6 ) )
			return TRUE;

		return FALSE;
	}

	//用来判断当前程序的权限，dwIntegrityLevel的值就是
	// SECURITY_MANDATORY_HIGH_RID 同组值
	static HRESULT GetProcessIntergrityLevel(DWORD& dwIntegrityLevel)
	{
		dwIntegrityLevel = SECURITY_MANDATORY_UNTRUSTED_RID;

		HRESULT hResult = E_FAIL; // assume an error occured
		HANDLE hToken	= NULL;

		if ( !::OpenProcessToken(
			::GetCurrentProcess(),
			TOKEN_QUERY | TOKEN_QUERY_SOURCE,
			&hToken ) )
		{
			ATLASSERT( FALSE );
			return hResult;
		}

		//get the integrity level
		PTOKEN_MANDATORY_LABEL pTIL = NULL;
		DWORD dwReturnLength = 0;

		if ( !GetTokenInformation(
			hToken,
			TokenIntegrityLevel,
			NULL,
			0,
			&dwReturnLength ) )
		{
			if(ERROR_INSUFFICIENT_BUFFER == GetLastError())
			{
				pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, dwReturnLength);
				if(GetTokenInformation(
					hToken,
					TokenIntegrityLevel,
					pTIL, dwReturnLength, &dwReturnLength))
				{
					dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid,
						(DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

					hResult = S_OK;
				}

				LocalFree(pTIL);
				pTIL = NULL;
			}
		}

		return hResult;
	}

	static HANDLE CreateProcess(LPCTSTR lpApplicationName, LPCTSTR lpszCmdLine, LPCTSTR lpCurrentDirectory, DWORD dwSessionID = 0xFFFFFFFF, DWORD dwRidSecurity = SECURITY_MANDATORY_HIGH_RID)
	{
		BOOL bRunAsSvc = FALSE;

		if(SECURITY_MANDATORY_SYSTEM_RID == dwRidSecurity) //启成服务
			bRunAsSvc = TRUE;
		else if(0xFFFFFFFF == dwSessionID)
		{
			dwSessionID = FindMostActiveSession();
			if (0xFFFFFFFF == dwSessionID)
			{
				bRunAsSvc = TRUE;
			}
		}

		if(!bRunAsSvc)
		{
			HANDLE hToken = NULL;
			BOOL bResult = WTSQueryUserToken(dwSessionID, &hToken);
			if (!bResult)
			{
				bRunAsSvc = TRUE;	//还是用服务模式启吧，也可能本进程就不是服务
			}

			if(hToken)
			{
				CloseHandle(hToken);
				hToken = NULL;
			}
		}

		if(bRunAsSvc)
			return CreateProcessAsSvc(lpApplicationName, lpszCmdLine, lpCurrentDirectory, 0);
		else
			return CreateProcessAsLogon(lpApplicationName, lpszCmdLine, lpCurrentDirectory, dwSessionID, dwRidSecurity);
	}

	/////////////////////////////////////////////////////////////////////////////
	static DWORD FindMostActiveSession()
	{
		DWORD dwSessionID = 0xFFFFFFFF;
		DWORD dwCount = 0;
		PWTS_SESSION_INFO pSessionInfo = NULL;
		if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount))
		{
			for (DWORD i = 0; i < dwCount; ++i)
			{
				if (WTSActive == pSessionInfo[i].State)
				{
					dwSessionID = pSessionInfo[i].SessionId;
					break;
				}
				if (WTSConnected == pSessionInfo[i].State)
				{
					dwSessionID = pSessionInfo[i].SessionId;
				}
			}

			WTSFreeMemory(pSessionInfo);
		}

		return dwSessionID;
	}

	static HANDLE CreateProcessAsSvc(LPCTSTR lpApplicationName, LPCTSTR lpszCmdLine, LPCTSTR lpCurrentDirectory, DWORD dwSessionID)
	{
		HANDLE hToken = NULL;
		BOOL bResult = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
		if (!bResult)
		{
			return NULL;
		}

		HANDLE hDuplicateToken = NULL;
		bResult = DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityDelegation, TokenPrimary, &hDuplicateToken);
		if (bResult)
		{
			SetTokenInformation(hDuplicateToken, TokenSessionId, &dwSessionID, sizeof(DWORD));
		}
		CloseHandle(hToken);
		hToken = hDuplicateToken;

		STARTUPINFO si = {sizeof(STARTUPINFO)};
		si.lpDesktop   = _T("WinSta0\\Default");
		si.wShowWindow = SW_SHOWNORMAL;
		si.dwFlags     = STARTF_USESHOWWINDOW;
		PROCESS_INFORMATION pi;

		LPTSTR lpszTempCmdLine = NULL;
		if(lpszCmdLine)
		{
			DWORD dwLen = _tcslen(lpszCmdLine);
			lpszTempCmdLine = new TCHAR[dwLen + 1];
			_tcscpy_s(lpszTempCmdLine, dwLen + 1, lpszCmdLine);
		}

		DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_SEPARATE_WOW_VDM;
		bResult = CreateProcessAsUser(hToken, lpApplicationName, lpszTempCmdLine, NULL, NULL, FALSE, dwCreateFlag, NULL, lpCurrentDirectory, &si, &pi);
		if(lpszTempCmdLine)
		{
			delete[] lpszTempCmdLine;
			lpszTempCmdLine = NULL;
		}
		CloseHandle(hToken);
		if (bResult)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}

		return pi.hProcess;
	}

	/////////////////////////////////////////////////////////////////////////////
	static HANDLE CreateProcessAsLogon(LPCTSTR lpApplicationName, LPCTSTR lpszCmdLine, LPCTSTR lpCurrentDirectory, DWORD dwSessionID, DWORD dwRidSecurity = SECURITY_MANDATORY_HIGH_RID)
	{
		HANDLE hToken = NULL;
		BOOL bResult = WTSQueryUserToken(dwSessionID, &hToken);
		if (!bResult)
		{
			return NULL;
		}

		if (LOBYTE(LOWORD(GetVersion())) > 5)
		{
			TOKEN_LINKED_TOKEN tlt = {0};
			DWORD dwLen;
			bResult = GetTokenInformation(hToken, TokenLinkedToken, &tlt, sizeof(tlt), &dwLen);
			if (bResult)
			{
				CloseHandle(hToken);
				hToken = tlt.LinkedToken;
			}

			SID_IDENTIFIER_AUTHORITY sia = SECURITY_MANDATORY_LABEL_AUTHORITY;
			PSID pSidHigh = NULL;
			bResult = AllocateAndInitializeSid(&sia, 1,
				dwRidSecurity, 0, 0, 0, 0, 0, 0, 0, &pSidHigh);
			if (bResult)
			{
				HANDLE hDuplicateToken = NULL;
				bResult = DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityDelegation, TokenPrimary, &hDuplicateToken);
				if (bResult)
				{
					CloseHandle(hToken);
					hToken = hDuplicateToken;
				}

				TOKEN_MANDATORY_LABEL tml = {0};
				tml.Label.Attributes = SE_GROUP_INTEGRITY;
				tml.Label.Sid = pSidHigh;
				SetTokenInformation(hToken, TokenIntegrityLevel, &tml, sizeof(tml));
			}
		}

		LPVOID lpEnv;
		bResult = CreateEnvironmentBlock(&lpEnv, hToken, FALSE);
		if (!bResult)
		{
			CloseHandle(hToken);
			return NULL;
		}

		STARTUPINFO si = {sizeof(STARTUPINFO)};
		si.lpDesktop   = _T("WinSta0\\Default");
		si.wShowWindow = SW_SHOWNORMAL;
		si.dwFlags     = STARTF_USESHOWWINDOW;
		PROCESS_INFORMATION pi;

		LPTSTR lpszTempCmdLine = NULL;
		if(lpszCmdLine)
		{
			DWORD dwLen = _tcslen(lpszCmdLine);
			lpszTempCmdLine = new TCHAR[dwLen + 1];
			_tcscpy_s(lpszTempCmdLine, dwLen + 1, lpszCmdLine);
		}
		DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT | CREATE_SEPARATE_WOW_VDM;
		bResult = CreateProcessAsUser(hToken, lpApplicationName, lpszTempCmdLine, NULL, NULL, FALSE, dwCreateFlag, lpEnv, lpCurrentDirectory, &si, &pi);
		if(lpszTempCmdLine)
		{
			delete[] lpszTempCmdLine;
			lpszTempCmdLine = NULL;
		}
		DestroyEnvironmentBlock(lpEnv);
		CloseHandle(hToken);
		if (bResult)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}

		return pi.hProcess;
	}

	//dwSessionID = 0xFFFFFFFF,取当前活动session
	static BOOL GetSessionUser(LPTSTR szUserName, DWORD nNameLen, DWORD dwSessionID = 0xFFFFFFFF)
	{
		if(0xFFFFFFFF == dwSessionID)
		{
			dwSessionID = FindMostActiveSession();
			if (0xFFFFFFFF == dwSessionID)
			{
				return FALSE;
			}
		}

		BOOL bResult = FALSE;
		HANDLE hToken = NULL;

		DWORD dwIntegrityLevel;
		if(SUCCEEDED(GetProcessIntergrityLevel(dwIntegrityLevel)) &&
			SECURITY_MANDATORY_SYSTEM_RID == dwIntegrityLevel)
		{
			if (WTSQueryUserToken(dwSessionID, &hToken))
			{
				return FALSE;
			}
		}
		else
		{
			DWORD dwProcessSessionId;
			if(!ProcessIdToSessionId(GetCurrentProcessId(), &dwProcessSessionId) || dwProcessSessionId != dwSessionID)
			{
				return FALSE;
			}

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			{
				return FALSE;
			}
		}

		if(NULL == hToken)
			return FALSE;

		DWORD dwNeedLen = 0;
		TOKEN_USER *pTokenUser = NULL;
		bResult = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwNeedLen);
		if (dwNeedLen > 0)
		{
			pTokenUser = (TOKEN_USER*)new BYTE[dwNeedLen];
			bResult = GetTokenInformation(hToken,
				TokenUser,
				pTokenUser,
				dwNeedLen,
				&dwNeedLen);
		}

		if(bResult)
		{
			SID_NAME_USE sn;
			TCHAR szDomainName[MAX_PATH];
			DWORD dwDmLen = MAX_PATH;
			bResult = LookupAccountSid(NULL,
				pTokenUser->User.Sid,
				szUserName,
				&nNameLen,
				szDomainName,
				&dwDmLen,
				&sn);
		}

		if(hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}

		return bResult;
	}
	
	//检查是否为管理员启动
	static BOOL IsRunAsAdministrator()
	{
		SID_IDENTIFIER_AUTHORITY SystemSidAuthority = {SECURITY_NT_AUTHORITY};
		PSID psid = NULL;
		BOOL IsMember = FALSE;

		if(!AllocateAndInitializeSid(&SystemSidAuthority,2,
			SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_ADMINS,0,0,0,0,0,0,&psid)){
				return FALSE;
		}

		if(!CheckTokenMembership(NULL,psid,&IsMember)){
			if(psid) FreeSid(psid);
			return FALSE;
		}

		if(psid) FreeSid(psid);
		return IsMember;
	}
	

	static BOOL ServiceExecute(LPCTSTR wstrCmdLine, INT32& n32ExitResult)  
	{  
		DWORD dwProcesses = 0;  
		BOOL bResult = FALSE;  
		DWORD dwSid = FindMostActiveSession();  

		DWORD dwRet = 0;  
		PROCESS_INFORMATION pi;  
		STARTUPINFO si;  
		HANDLE hProcess = NULL, hPToken = NULL, hUserTokenDup = NULL;  
		if (!WTSQueryUserToken(dwSid, &hPToken))  
		{  
			PROCESSENTRY32 procEntry;  
			DWORD dwPid = 0;  
			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
			if (hSnap == INVALID_HANDLE_VALUE)  
			{  
				return FALSE;  
			}  

			procEntry.dwSize = sizeof(PROCESSENTRY32);  
			if (Process32First(hSnap, &procEntry))  
			{  
				do  
				{  
					if (_tcsicmp(procEntry.szExeFile, _T("explorer.exe")) == 0)  
					{  
						DWORD exeSessionId = 0;  
						if (ProcessIdToSessionId(procEntry.th32ProcessID, &exeSessionId) && exeSessionId == dwSid)  
						{  
							dwPid = procEntry.th32ProcessID;  
							break;  
						}  
					}  

				} while (Process32Next(hSnap, &procEntry));  
			}  
			CloseHandle(hSnap);  

			// explorer进程不存在  
			if (dwPid == 0)  
			{  
				return FALSE;  
			}  

			hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);  
			if (hProcess == NULL)  
			{  
				return FALSE;  
			}  

			if(!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS_P,&hPToken))  
			{  
				CloseHandle(hProcess);  
				return FALSE;  
			}  
		}  

		if (hPToken == NULL)  
			return FALSE;  

		TOKEN_LINKED_TOKEN admin;  
		bResult = GetTokenInformation(hPToken, (TOKEN_INFORMATION_CLASS)19, &admin, sizeof(TOKEN_LINKED_TOKEN), &dwRet);  

		if (!bResult) // vista 以前版本不支持TokenLinkedToken  
		{  
			TOKEN_PRIVILEGES tp;  
			LUID luid;  
			if (LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))  
			{  
				tp.PrivilegeCount =1;  
				tp.Privileges[0].Luid =luid;  
				tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;  
			}  
			DuplicateTokenEx(hPToken,MAXIMUM_ALLOWED,NULL,SecurityIdentification,TokenPrimary,&hUserTokenDup);  
		}  
		else  
		{  
			hUserTokenDup = admin.LinkedToken;  
		}  

		LPVOID pEnv =NULL;  
		DWORD dwCreationFlags = CREATE_PRESERVE_CODE_AUTHZ_LEVEL;  

		if(CreateEnvironmentBlock(&pEnv,hUserTokenDup,TRUE))  
		{  
			dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;  
		}  
		else  
		{  
			pEnv = NULL;  
		}  

		ZeroMemory( &si, sizeof(si) );  
		si.cb = sizeof(si);  
		si.dwFlags = STARTF_USESHOWWINDOW;  
		si.wShowWindow = SW_SHOWNORMAL;  
		ZeroMemory( &pi, sizeof(pi) );  

		bResult = CreateProcessAsUser(  
			hUserTokenDup,                     // client's access token  
			NULL,    // file to execute  
			(LPTSTR) wstrCmdLine,                 // command line  
			NULL,            // pointer to process SECURITY_ATTRIBUTES  
			NULL,               // pointer to thread SECURITY_ATTRIBUTES  
			FALSE,              // handles are not inheritable  
			dwCreationFlags,     // creation flags  
			pEnv,               // pointer to new environment block  
			NULL,               // name of current directory  
			&si,               // pointer to STARTUPINFO structure  
			&pi                // receives information about new process  
			);    

		if(pi.hProcess)  
		{  
			if(WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess, 180000))  
			{  
				DWORD dwResult = 0;  
				if(GetExitCodeProcess(pi.hProcess,  &dwResult))  
				{  
					n32ExitResult = dwResult;  
				}  
				else  
				{  
					n32ExitResult = -1;  
				}  

				CloseHandle(pi.hThread);  
				CloseHandle(pi.hProcess);  
			}  
			else  
			{  
				CloseHandle(pi.hThread);  
				CloseHandle(pi.hProcess);  
				n32ExitResult = -1;  
			}  
		}  



		if (hUserTokenDup != NULL)  
			CloseHandle(hUserTokenDup);  
		if (hProcess != NULL)  
			CloseHandle(hProcess);  
		if (hPToken != NULL)  
			CloseHandle(hPToken);  
		if (pEnv != NULL)  
			DestroyEnvironmentBlock(pEnv);  

		return TRUE;  
	}  

	
};

static BOOL DisableWow64FsRedirection(PVOID* pOldValue)
{
	BOOL bReturn = FALSE;
	BOOL (WINAPI* pWow64DisableWow64FsRedirection)(PVOID*) = NULL;
	HMODULE hKernel = GetModuleHandle(_T("Kernel32.dll"));
	if (hKernel)
	{
		(FARPROC&)pWow64DisableWow64FsRedirection = GetProcAddress(hKernel, "Wow64DisableWow64FsRedirection");
		if (pWow64DisableWow64FsRedirection)
			bReturn = pWow64DisableWow64FsRedirection(pOldValue);
	}

	return bReturn;
}

static BOOL RevertWow64FsRedirection(PVOID OldValue)
{
	BOOL bReturn = FALSE;
	BOOL (WINAPI* pWow64RevertWow64FsRedirection)(PVOID) = NULL;
	HMODULE hKernel = GetModuleHandle(_T("Kernel32.dll"));
	if (hKernel)
	{
		(FARPROC&)pWow64RevertWow64FsRedirection = GetProcAddress(hKernel, "Wow64RevertWow64FsRedirection");
		if (pWow64RevertWow64FsRedirection)
			bReturn = pWow64RevertWow64FsRedirection(OldValue);
	}

	return bReturn;
}

};// namespace vastafunc
#endif // !defined(_INCLUDE_VISTAFUNC_H__)
