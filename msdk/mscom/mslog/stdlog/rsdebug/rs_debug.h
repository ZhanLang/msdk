
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tchar.h>
#include <rsdebugex.h>
//#include "rspathfun.h"

#pragma once

static BOOL GetAppDataPathFromReg(LPCTSTR lpSubKey, LPTSTR pszPath, UINT nMaxSize)
{
	ULONG	lLen = 0;
	HKEY	hOpenKey = NULL;
	DWORD	dwType = REG_SZ;
	CHAR	szRegKey[_MAX_PATH]={0};
#ifdef _STRSAFE_H_INCLUDED_
	StringCchPrintf(szRegKey, sizeof(szRegKey), "SOFTWARE\\Rising\\%s", lpSubKey);
#else
	wsprintf(szRegKey, "SOFTWARE\\Rising\\%s", lpSubKey);
#endif

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegKey, 0, KEY_READ, &hOpenKey) )
	{
		lLen = nMaxSize;
		if(ERROR_SUCCESS == RegQueryValueEx(hOpenKey, TEXT("datapath"), 0, &dwType, LPBYTE(pszPath), &lLen))
		{
			if (lstrlen(pszPath))
			{
#ifdef _STRSAFE_H_INCLUDED_
				StringCchCat(pszPath, nMaxSize, "\\");
#else
				_tcsncat(pszPath, "\\", nMaxSize - _tcslen(pszPath) - 1);
#endif
				RegCloseKey(hOpenKey);
				return TRUE;
			}
		}
		RegCloseKey(hOpenKey);
	}
	return FALSE;
}

static DWORD GetAppLogFromReg(LPCTSTR lpSubKey, LPCTSTR szModName)
{
	DWORD	dwRet = RSD_DEFAULT_FORMAT;
	ULONG	lLen = 0;
	HKEY	hOpenKey = NULL;
	DWORD	dwType = REG_DWORD;	
	CHAR	szRegKey[MAX_PATH] = {0};

	if(lpSubKey==NULL || *lpSubKey == NULL)
		return dwRet;

#ifdef _STRSAFE_H_INCLUDED_
	StringCchPrintf(szRegKey, sizeof(szRegKey), "SOFTWARE\\Rising\\%s", lpSubKey);
#else
	wsprintf(szRegKey, "SOFTWARE\\Rising\\%s", lpSubKey);
#endif	
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegKey, 0, KEY_READ, &hOpenKey) )
	{
		lLen = sizeof(dwRet);
		if(szModName!=NULL && *szModName != NULL)
		{
			if(ERROR_SUCCESS == RegQueryValueEx(hOpenKey, szModName, 0, &dwType, (LPBYTE)&dwRet, &lLen))
			{
				RegCloseKey(hOpenKey);
				return dwRet;
			}
		}
		RegQueryValueEx(hOpenKey, lpSubKey, 0, &dwType, (LPBYTE)&dwRet, &lLen);
		RegCloseKey(hOpenKey);
	}
	return dwRet;
}

class CTraceMessage
{
public:	
	DWORD m_dwType;
	HANDLE m_hPipe;
	CHAR m_szPid[16];
	CHAR m_szProductName[32];
	CHAR m_szModName[MAX_PATH];
	CHAR m_szTempPath[MAX_PATH];
	HANDLE m_hOutput;
	HANDLE m_hInput;

	char* _Format(const char* pFormat, char* szOut, int nLen)
	{
		time_t t = time(NULL);
		struct tm* ptmTemp = localtime(&t);
		if (ptmTemp == NULL || !strftime(szOut, nLen, pFormat, ptmTemp))//_tcsftime
			return NULL;
		return szOut;
		/*SYSTEMTIME t;
		GetSystemTime(&t);

		strInfo.Format("[%d-%02d-%02d][%02d:%02d:%02d %3d]:%s \n"
		, t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,t.wMilliseconds
		, pMsg);*/
	}
public:
	CTraceMessage():m_hPipe(INVALID_HANDLE_VALUE),
					m_hOutput(INVALID_HANDLE_VALUE),
					m_hInput(INVALID_HANDLE_VALUE),
					m_dwType(RSD_DEFAULT_FORMAT)
	{
		m_szPid[0]='[';
		itoa(GetCurrentProcessId(), m_szPid+1, 10);
		lstrcat(m_szPid, "]");

		m_szProductName[0] = 0;
		m_szModName[0] = 0;
		m_szTempPath[0] = 0;
		GetModuleFileName(NULL, m_szTempPath, MAX_PATH);			

		LPCSTR lpPos = _tcsrchr(m_szTempPath, '\\');
		if(lpPos)
			lstrcpy(m_szModName, lpPos+1);
		else
			lstrcpy(m_szModName, "unkown");

		lstrcat(m_szTempPath, ".txt");
	}

	~CTraceMessage()
	{
		DbgMsgToDebugView(DBG_LOG, "Debug is closed!\n");

		if(m_hPipe!=INVALID_HANDLE_VALUE && m_hPipe!=NULL) 
		{						
			::CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
		}
	}
	DWORD ReadType(LPCSTR szModName)
	{
		//from register
		return GetAppLogFromReg(m_szProductName, szModName);
	}
	BOOL ReadPath(LPSTR szPath, DWORD dwSize)
	{
		//from register
		if(m_szProductName[0]==0)
			return FALSE;

		return GetAppDataPathFromReg(m_szProductName, szPath, dwSize);
	}
public:	
	void ReadDebugInfoSetting(const char* szProductName, LPCSTR szModName, DWORD dwType, IN const char* szWorkPath)
	{
		if(szProductName&&*szProductName)
			lstrcpy(m_szProductName, szProductName);

		if(szModName&&*szModName)
			lstrcpy(m_szModName, szModName);

		if(dwType==0)
			m_dwType = ReadType(szModName);
		else
			m_dwType = dwType;

		if(szWorkPath && *szWorkPath)
		{
			lstrcpy(m_szTempPath, szWorkPath);
			//补一个'\\'
			LPCSTR lpPos = _tcsrchr(m_szTempPath, '\\');
			if(lpPos && *(lpPos+1)!=0)
				lstrcat(m_szTempPath, "\\");
			lstrcat(m_szTempPath, m_szModName);
			lstrcat(m_szTempPath, ".txt");
		}
		else
		{
			if(ReadPath(m_szTempPath, sizeof(m_szTempPath)))
			{
				lstrcat(m_szTempPath, m_szModName);
				lstrcat(m_szTempPath, ".txt");
			}
		}
		if(m_dwType&RSD_TYPE_CONSOLE_WIN)
		{
			m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
			if(INVALID_HANDLE_VALUE == m_hOutput)
			{
				if(::AllocConsole())
					m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
			}			
			m_hInput = ::GetStdHandle(STD_INPUT_HANDLE);
			::SetConsoleTitle( m_szModName );
		}
	}

	DbgRetval_t DbgMsgToDebugView( DbgType_t spewType, LPCSTR pMsg )
	{
		static LPCSTR szType[DBG_TYPE_COUNT] = 
		{
			"Msg",
			"Warning",
			"Assert",
			"Error",
			"Log",
			"Report",
		};
		CHAR pTempBuffer[512] = {0};

		//输出内容
		if(m_dwType&RSD_INFO_OUT_TIME)
		{
			CHAR szTime[128] = {0};
			if(_Format("%Y-%m-%d %H:%M:%S", szTime, sizeof(szTime)))
			{
				lstrcpy(pTempBuffer, szTime);
			}
		}
		if(m_dwType&RSD_INFO_OUT_TYPE)
		{
			lstrcat(pTempBuffer, "<");
			lstrcat(pTempBuffer, szType[spewType]);
			lstrcat(pTempBuffer, ">");
		}
		if(m_dwType&RSD_INFO_OUT_PID)
		{
			lstrcat(pTempBuffer, m_szPid);		
		}
		if(m_dwType&RSD_INFO_OUT_TID)
		{
			CHAR szTid[16] = "[";
			itoa(GetCurrentThreadId(), szTid+1, 10);
			lstrcat(szTid, "]");
			lstrcat(pTempBuffer, szTid);
		}
		if(m_dwType&RSD_INFO_OUT_MODNAME)
		{
		}

		lstrcat(pTempBuffer, ": ");

		//输出方式
		if(m_dwType&RSD_TYPE_DEBUG_VIEW)
		{
			OutputDebugString(pTempBuffer);
			OutputDebugString(pMsg);
			OutputDebugString("\n");
		}
		if(m_dwType&RSD_TYPE_TEXT_FILE)
		{
			FILE *fp = fopen(m_szTempPath, "a+");
			if(fp)
			{
				fputs(pTempBuffer,fp);
				fputs(pMsg,fp);
				fputs("\n",fp);
				fclose(fp);
			}
		}
		if(m_dwType&RSD_TYPE_PIPE_WINDOW)
		{
			if(m_hPipe==INVALID_HANDLE_VALUE)
			{
				SECURITY_DESCRIPTOR sd;
				BOOL b = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);				

				// This is our SID for group "Everyone"
				BYTE sidEveryone[12] = {1,1,0,0,0,0,0,1,0,0,0,0};

				// Determine size for DACL, allocate and initialize it.
				DWORD dwSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + sizeof(sidEveryone) - sizeof(DWORD);
				PACL pDacl = (PACL) malloc(dwSize);
				b = InitializeAcl(pDacl, dwSize, ACL_REVISION);				

				// Allow access for everyone
				b = AddAccessAllowedAce(pDacl, ACL_REVISION, GENERIC_ALL, sidEveryone);				

				// Insert DACL into security descriptor
				b = SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE);				

				// Initialize SECURITY_ATTRIBUTES structure
				SECURITY_ATTRIBUTES sa;
				sa.nLength = sizeof(SECURITY_ATTRIBUTES);
				sa.lpSecurityDescriptor = &sd;
				sa.bInheritHandle = FALSE;
				m_hPipe = ::CreateFile("\\\\.\\pipe\\DebugWindow",	// Pipe name
					GENERIC_WRITE,				// Access type to the pipe
					0,							// Share mode
					&sa,						// Security attributes
					OPEN_EXISTING,				// Creation/disposition flags
					FILE_ATTRIBUTE_NORMAL,		// Attributes and flags for the file
					NULL);						// Template		
			}
			if(m_hPipe!=INVALID_HANDLE_VALUE)
			{
				DWORD dwBytesWritten;
				if(!::WriteFile(m_hPipe,			// Handle to the pipe
					pTempBuffer,				// Pointer to data to write to file
					lstrlen(pTempBuffer)+1,	// Number of bytes to write
					&dwBytesWritten,	// Number of bytes written
					NULL))				// Pointer to OVERLAPPED structure
				{
					CloseHandle(m_hPipe);
					m_hPipe = INVALID_HANDLE_VALUE;
				}
				if((m_hPipe!=INVALID_HANDLE_VALUE) && !::WriteFile(m_hPipe,			// Handle to the pipe
					pMsg,				// Pointer to data to write to file
					lstrlen(pMsg)+1,		// Number of bytes to write
					&dwBytesWritten,	// Number of bytes written
					NULL))				// Pointer to OVERLAPPED structure
				{
					CloseHandle(m_hPipe);
					m_hPipe = INVALID_HANDLE_VALUE;
				}
				if((m_hPipe!=INVALID_HANDLE_VALUE) && !::WriteFile(m_hPipe,			// Handle to the pipe
					"\r\n",				// Pointer to data to write to file
					lstrlen("\r\n")+1,	// Number of bytes to write
					&dwBytesWritten,	// Number of bytes written
					NULL))				// Pointer to OVERLAPPED structure
				{
					CloseHandle(m_hPipe);
					m_hPipe = INVALID_HANDLE_VALUE;
				}
			}
		}
		if(m_dwType&RSD_TYPE_CONSOLE_WIN)
		{
			if(m_hOutput!=INVALID_HANDLE_VALUE)
			{
				DWORD dwBytesWritten;
				::WriteConsole( m_hOutput, pTempBuffer, lstrlen(pTempBuffer), &dwBytesWritten, NULL );
				::WriteConsole( m_hOutput, pMsg, lstrlen(pMsg), &dwBytesWritten, NULL );
				::WriteConsole( m_hOutput, "\n", lstrlen("\n"), &dwBytesWritten, NULL );
			}
		}
		//...

		return DBG_RETURNED_CONTINUES;
	}
};

