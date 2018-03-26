/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSSERVERCONNECTION_H__0CD5653D_2D57_4456_B496_204556B72225__INCLUDED_)
#define AFX_FSSERVERCONNECTION_H__0CD5653D_2D57_4456_B496_204556B72225__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <windows.h>
#include <wininet.h>
#include "fsinet.h"	
#include <fsString.h>
#include "fsSpeaking.h"
#include "fsServerConnection_i.h"
class fsInternetServerConnection : public fsInternetServerConnection_i, public fsSpeaking
{
public:
	
	INTERNET_PORT GetServerPort();
	
	LPCSTR GetServerName();
	
	
	interface fsInternetSession_i* GetSession();
	
	void CloseHandle();
	
	LPCSTR GetLastError ();
	
	

	
	virtual fsInternetResult Connect (LPCSTR pszServer, LPCSTR pszUser, LPCSTR pszPassword, INTERNET_PORT nPort) = NULL;
	fsInternetResult Initialize (interface fsInternetSession_i *pSession);
	
	HINTERNET GetHandle();
	fsInternetServerConnection();
	virtual ~fsInternetServerConnection();

protected:
	INTERNET_PORT m_uPort;	
	fsString m_strServer;	
	char *m_pszLastError;	
	HINTERNET m_hServer;	
	UTIL::com_ptr<fsInternetSession_i> m_pSession;	
};

#endif 
