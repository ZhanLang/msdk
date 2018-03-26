// viewdbg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <assert.h> // for assert
#include <stdio.h> // for printf

#define PIPE_BUFFER_SIZE 1024


int _tmain(int argc, _TCHAR* argv[])
{
	SECURITY_DESCRIPTOR sd;
	BOOL b = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	assert(b);

	// This is our SID for group "Everyone"
	BYTE sidEveryone[12] = {1,1,0,0,0,0,0,1,0,0,0,0};

	// Determine size for DACL, allocate and initialize it.
	DWORD dwSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + sizeof(sidEveryone) - sizeof(DWORD);
	PACL pDacl = (PACL) malloc(dwSize);
	b = InitializeAcl(pDacl, dwSize, ACL_REVISION);
	assert(b);

	// Allow access for everyone
	b = AddAccessAllowedAce(pDacl, ACL_REVISION, GENERIC_ALL, sidEveryone);
	assert(b);

	// Insert DACL into security descriptor
	b = SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE);
	assert(b);

	// Initialize SECURITY_ATTRIBUTES structure
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;

	DWORD dwErr = 0;
	do 
	{
		// Create named pipe with access for everyone
		HANDLE hPipe = ::CreateNamedPipe(_T("\\\\.\\pipe\\DebugWindow"),	// pipe name
			PIPE_ACCESS_INBOUND,			// open  mode
			PIPE_TYPE_MESSAGE | 
			PIPE_READMODE_MESSAGE | 
			PIPE_WAIT,						// type, read and wait modes
			1,								// mux number of instances
			PIPE_BUFFER_SIZE,				// Size of output buffer
			PIPE_BUFFER_SIZE,				// Size of input buffer
			INFINITE,						// Default time-out in ms
			&sa);							// Security attributes
		assert(INVALID_HANDLE_VALUE != hPipe);
	
		// Wait indefinitely until client connects
		printf("Waiting for client to connect...\n");
		BOOL bResult = ::ConnectNamedPipe(hPipe, NULL);
		assert(bResult);

		// The client has connected
		printf("The client has connected. Waiting for messages...\n");

		// Read and display client messages in an infinite loop
		char buf[PIPE_BUFFER_SIZE];
		DWORD dwBytesRead;
		while (TRUE)
		{
			bResult = ReadFile(hPipe,				// Handle of file to read
				buf,				// Pointer to buffer that receives data
				PIPE_BUFFER_SIZE,	// Number of bytes to read
				&dwBytesRead,		// Number of bytes read
				NULL);				// Pointer to OVERLAPPED structrue
			if (bResult)
			{
				printf(buf);	// Output message on the screen			
			}
			else
			{
				dwErr = GetLastError();
				printf("Error reading from pipe. Last operating system error: %d\n", dwErr);
				break;
			}
		}
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
	} while(dwErr==109);//109表示发送端主动关闭了管道，我们继续ConnectNamedPipe

	// Do clean up and exit
	free(pDacl);	
	return 0;
}

