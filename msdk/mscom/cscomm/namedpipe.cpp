// hgctrl.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "namedpipe.h"
#include <process.h>
#include <malloc.h>



CNamedPipe::CNamedPipe()
{
	m_hThreadHandle = NULL;
	m_dwThreadId = 0;
	m_hEvent = NULL;
	m_hNamedPipe = NULL;
	m_pCallBack = NULL;
	m_pThis = NULL;
	m_bExit = FALSE;
}

CNamedPipe::~CNamedPipe()
{

}


BOOL CNamedPipe::EnablePrivilege( LPCTSTR lpszPrivilege)
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tp;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,lpszPrivilege , &sedebugnameValue))
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = sedebugnameValue;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

BOOL CNamedPipe::SetObjectToLowIntegrity( HANDLE hObject, SE_OBJECT_TYPE type ) 
{ 

	LPCTSTR LOW_INTEGRITY_SDDL_SACL = L"S:(ML;;NW;;;LW)"; 
	bool bRet = false; 
	DWORD dwErr = ERROR_SUCCESS; 
	PSECURITY_DESCRIPTOR pSD = NULL; 
	PACL pSacl = NULL; 
	BOOL fSaclPresent = FALSE; 
	BOOL fSaclDefaulted = FALSE; 

	if ( ConvertStringSecurityDescriptorToSecurityDescriptor ( 
		LOW_INTEGRITY_SDDL_SACL, SDDL_REVISION_1, &pSD, NULL ) ) 
	{ 
		if ( GetSecurityDescriptorSacl ( 
			pSD, &fSaclPresent, &pSacl, &fSaclDefaulted ) ) 
		{ 
			dwErr = SetSecurityInfo ( 
				hObject, type, LABEL_SECURITY_INFORMATION,
				NULL, NULL, NULL, pSacl ); 

			bRet = (ERROR_SUCCESS == dwErr); 
		} 

		LocalFree ( pSD ); 
	} 

	return bRet; 
}

DWORD CNamedPipe::NPStartModule(LPWSTR lpPipeName,DWORD dwMaxConn,BOOL bIsFirst,pPipeNotifyCallBack callback,LPVOID lpThis)
{
	DWORD dwRet = 0,dwOpenMode = 0 ;
	SECURITY_ATTRIBUTES     sa;
	PSECURITY_DESCRIPTOR    pSD = NULL;

	if(lpPipeName == NULL)
	{
		dwRet = ERROR_INVALID_PARAMETER;
		goto exit;
	}

	if(m_hNamedPipe)
	{
		goto exit;
	}

	dwOpenMode = PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED;

	if(bIsFirst)
	{
		EnablePrivilege(SE_SECURITY_NAME);
		EnablePrivilege(SE_DEBUG_NAME);
		dwOpenMode |= WRITE_OWNER /*| ACCESS_SYSTEM_SECURITY*/;
	}

	
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
		SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
	{
		dwRet = GetLastError();
		goto exit;
	}
	if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
	{
		dwRet = GetLastError();
		goto exit;
	}

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = TRUE;

	m_hNamedPipe = CreateNamedPipeW(lpPipeName,dwOpenMode,
		PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,dwMaxConn,NULL,NULL,1000,&sa);
	if(INVALID_HANDLE_VALUE == m_hNamedPipe)
	{
		dwRet = GetLastError();
		goto exit;
	}

	if(bIsFirst)
	{
		if(SetObjectToLowIntegrity(m_hNamedPipe))
		{
			OutputDebugStringA("zzh : ok \n");
		}
		else 
		{
			OutputDebugStringA("zzh : false \n");
		}
	}

	m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(m_hEvent == NULL)
	{
		dwRet = GetLastError();
		goto exit;
	}

	m_hThreadHandle = (HANDLE)_beginthreadex(NULL,0,StartThread,this,0,&m_dwThreadId);
	if(m_hThreadHandle == NULL)
	{
		dwRet = GetLastError();
		goto exit;
	}

	m_pCallBack = callback;
	m_pThis = lpThis;

exit:
	if(pSD)
		LocalFree((HLOCAL)pSD);

	return dwRet;
}

DWORD CNamedPipe::NPStopModule()
{
	m_bExit = TRUE;

	if(m_hEvent)
	{
		SetEvent(m_hEvent);
	}

	if(m_hThreadHandle)
	{
		DWORD dwIndex = 0,dwExitCode = 0;
		while(dwIndex++ < 5 && GetExitCodeThread(m_hThreadHandle,&dwExitCode) && dwExitCode == STILL_ACTIVE)
		{
			Sleep(200);
		}	
		TerminateThread(m_hThreadHandle,0);
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle = NULL;
	}
	m_dwThreadId = 0;

	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	if(m_hNamedPipe)
	{
		CloseHandle(m_hNamedPipe);
		m_hNamedPipe = NULL;
	}
	return 0;
}

UINT _stdcall CNamedPipe::StartThread( LPVOID lParam )
{
	UINT uInt = 0;
	CNamedPipe* pNamedPipe = (CNamedPipe*)lParam;
	if(pNamedPipe)
	{
		uInt = pNamedPipe->ThreadRoutine();
	}
	return uInt;
}

#define MAX_BUFFER_SIZE				64*1024

UINT CNamedPipe::ThreadRoutine()
{
	UINT uInt = 0;
	DWORD dwBytes = 0,dwReadByte = 0,dwWriteByte,dwRet = 0,dwPipeSize = 0;
	LPVOID	lpInBuff = NULL,lpOutBuff = NULL;
	DWORD	dwInBuffSize = 0,dwOutBuffSize = 0;
	OVERLAPPED OverLapStruct = {0};

	OverLapStruct.hEvent = m_hEvent;
	lpInBuff = (LPVOID)malloc(MAX_BUFFER_SIZE);
	lpOutBuff = (LPVOID)malloc(MAX_BUFFER_SIZE);

	while (TRUE)
	{
		dwBytes = 0;
		dwReadByte = 0;
		dwPipeSize = 0;
		dwInBuffSize = 0;
		dwOutBuffSize = 0;
		ZeroMemory(lpInBuff,MAX_BUFFER_SIZE);
		ZeroMemory(lpOutBuff,MAX_BUFFER_SIZE);

		ConnectNamedPipe(m_hNamedPipe,&OverLapStruct);
		WaitForSingleObject(m_hEvent,INFINITE);

		if(m_bExit)
			break;

		if(ReadFile(m_hNamedPipe,lpInBuff,MAX_BUFFER_SIZE,&dwReadByte,NULL) == FALSE)
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				break;
		}

		if(m_pCallBack)
		{
			dwOutBuffSize = MAX_BUFFER_SIZE;
			dwRet = m_pCallBack(m_pThis,lpInBuff,dwReadByte,lpOutBuff,dwOutBuffSize);
		}

		WriteFile(m_hNamedPipe,lpOutBuff,MAX_BUFFER_SIZE,&dwWriteByte,NULL);

		DisconnectNamedPipe(m_hNamedPipe);
	}

	if(lpInBuff)
	{
		free(lpInBuff);
	}
	if(lpOutBuff)
	{
		free(lpOutBuff);
	}

	return uInt;
}

DWORD CNamedPipe::NPSendMessage( LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize )
{
	DWORD dwRet = 0,dwReadSize = 0,dwWriteSize = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE;

	if(lpPipeName == NULL || lpInBuff == NULL)
	{
		dwRet = ERROR_INVALID_PARAMETER;
		goto exit;
	}

	hPipe = CreateFileW(lpPipeName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hPipe == INVALID_HANDLE_VALUE)
	{
		dwRet = GetLastError();
		goto exit;
	}
	
	
	if(WriteFile(hPipe,lpInBuff,dwInBuffSize,&dwWriteSize,NULL) == FALSE)
	{
		dwRet = GetLastError();
		goto exit;
	}

	if(lpOutBuff && dwOutBuffSize)
	{
		if(ReadFile(hPipe,lpOutBuff,dwOutBuffSize,&dwReadSize,NULL) == FALSE)
		{
			dwRet = GetLastError();
			goto exit;
		}
		dwOutBuffSize = dwReadSize;
	}

exit:
	if(hPipe != INVALID_HANDLE_VALUE)
		CloseHandle(hPipe);

	return dwRet;
}

DWORD CNamedPipe::NPNamedPipeExists( LPWSTR lpPipeName )
{
	DWORD dwRet = 0,dwReadSize = 0,dwWriteSize = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	NPMESSAGEHEADER pItem;
	pItem.dwSize = sizeof(NPMESSAGEHEADER);
	pItem.dwType = URLTYPE_UNKNOWN;
	pItem.dwError = 0;
	pItem.dwReserved = 0;

	if(lpPipeName == NULL)
	{
		dwRet = ERROR_INVALID_PARAMETER;
		goto exit;
	}

	hPipe = CreateFileW(lpPipeName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hPipe == INVALID_HANDLE_VALUE)
	{
		dwRet = GetLastError();
		goto exit;
	}

	if(WriteFile(hPipe,&pItem,sizeof(NPMESSAGEHEADER),&dwWriteSize,NULL) == FALSE)
	{
		dwRet = GetLastError();
		goto exit;
	}

exit:
	if(hPipe != INVALID_HANDLE_VALUE)
		CloseHandle(hPipe);

	return dwRet;
}
