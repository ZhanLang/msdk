/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsHttpConnection.h"
#include "fsInternetSession.h"

fsHttpConnection::fsHttpConnection()
{

}

fsHttpConnection::~fsHttpConnection()
{

}

fsInternetResult fsHttpConnection::Connect(LPCSTR pszServer, LPCSTR pszUser, LPCSTR pszPassword, INTERNET_PORT nPort)
{
	if (!m_pSession)
		return IR_NOTINITIALIZED;

	HINTERNET hSession = ((fsInternetSession*)m_pSession.m_p)->GetHandle ();

	if (!hSession)
		return IR_NOTINITIALIZED;

	if (m_hServer)
		InternetCloseHandle (m_hServer);

	m_hServer = InternetConnect (hSession, pszServer, nPort,
					pszUser, pszPassword, INTERNET_SERVICE_HTTP, 0, NULL);

	if (!m_hServer)
		return fsWinInetErrorToIR ();

	m_strServer = pszServer;
	m_uPort = nPort;

	return IR_SUCCESS;
}
