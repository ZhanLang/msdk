// RsDebugWindow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 1024


int _tmain(int argc, TCHAR *argv[]) 
{ 
	DWORD dwWaitTime = 3000;
	HANDLE hPipe; 
	TCHAR chBuf[BUFSIZE]; 
	BOOL fSuccess; 
	DWORD cbRead, dwMode; 
	TCHAR	lpszPipename[_MAX_PATH] = _T("\\\\.\\pipe\\");//LPTSTR lpszPipename = _T("\\\\.\\pipe\\DebugWindow"); 
	
	LPTSTR szPipeName = _T("DebugWindow");
	if(argc>1)
		szPipeName = argv[1];
	_tcscat_s(lpszPipename, _MAX_PATH-_tcslen(lpszPipename), szPipeName);

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

	// Try to open a named pipe; wait for it, if necessary. 
tryopen: 
	_tprintf_s(_T("Try to connect pipe: %s\n"), lpszPipename); 
	while (1) 
	{ 
		hPipe = CreateFile( 
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing 
			&sa,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE) 
			break; 

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY) 
		{
			_tprintf_s(_T("Could not open pipe, so wait for %d seconds.\n"), dwWaitTime/1000); 
			Sleep(dwWaitTime);
			continue;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, dwWaitTime)) 
		{ 
			_tprintf_s(_T("All pipe instances are busy, so wait for %d seconds.\n"), dwWaitTime/1000); 
			if (GetLastError() == ERROR_SEM_TIMEOUT) 
		 {
			 continue;
		 }
			return 0;
		} 
	} 

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE; 
	fSuccess = SetNamedPipeHandleState( 
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess) 
	{
		_tprintf_s(_T("SetNamedPipeHandleState failed")); 
		return 0;
	}

	_tprintf_s(_T("%s is connected!\n"), lpszPipename); 
	
	do 
	{ 
		// Read from the pipe. 

		fSuccess = ReadFile( 
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE*sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (! fSuccess && GetLastError() != ERROR_MORE_DATA)
		{
			//break; 
			CloseHandle(hPipe); 
			goto tryopen;
		}

		_tprintf_s( _T("%s"), chBuf ); 
	} while (fSuccess||GetLastError() == ERROR_MORE_DATA);  // repeat loop if ERROR_MORE_DATA 


	CloseHandle(hPipe); 

	return 0; 
}
