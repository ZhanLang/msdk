/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsServerConnection.h"
#include "fsInternetSession.h"
#include "common.h"

fsInternetServerConnection::fsInternetServerConnection()
{
	m_hServer = NULL;
	m_pSession = INULL;
	m_pszLastError = NULL;
}

fsInternetServerConnection::~fsInternetServerConnection()
{
	CloseHandle ();
	SAFE_DELETE_ARRAY (m_pszLastError);
}

HINTERNET fsInternetServerConnection::GetHandle()
{
	return m_hServer;
}

fsInternetResult fsInternetServerConnection::Initialize(fsInternetSession_i *pSession)
{
	m_pSession = pSession;
	return IR_SUCCESS;
}

LPCSTR fsInternetServerConnection::GetLastError()
{
	return m_pszLastError;
}

void fsInternetServerConnection::CloseHandle()
{
	if (m_hServer)
	{
		InternetCloseHandle (m_hServer);
		m_hServer = NULL;
	}
}

interface fsInternetSession_i* fsInternetServerConnection::GetSession()
{
	return m_pSession;
}

LPCSTR fsInternetServerConnection::GetServerName()
{
	return m_strServer;
}

INTERNET_PORT fsInternetServerConnection::GetServerPort()
{
	return m_uPort;
}

