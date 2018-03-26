
#pragma once
#include <windows.h>
#include <wininet.h>
#include "fsinet.h"	


class fsInternetServerConnection
{
public:
	
	INTERNET_PORT GetServerPort();
	LPCTSTR GetServerName();
	class fsInternetSession* GetSession();
	VOID CloseHandle();
	LPCTSTR GetLastError ();
	virtual fsInternetResult Connect (LPCTSTR pszServer, LPCTSTR pszUser, LPCTSTR pszPassword, INTERNET_PORT nPort) = NULL;
	fsInternetResult Initialize (class fsInternetSession *pSession);
	HINTERNET GetHandle();

	fsInternetServerConnection();
	virtual ~fsInternetServerConnection();

protected:
	INTERNET_PORT m_uPort;	
	fsString m_strServer;	
	TCHAR *m_pszLastError;	
	HINTERNET m_hServer;	
	class fsInternetSession* m_pSession;	
};


