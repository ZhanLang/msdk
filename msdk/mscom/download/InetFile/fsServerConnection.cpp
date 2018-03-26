
#include "stdafx.h"
#include "fsServerConnection.h"
#include "fsInternetSession.h"


fsInternetServerConnection::fsInternetServerConnection()
{
	m_hServer = NULL;
	m_pSession = NULL;
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

fsInternetResult fsInternetServerConnection::Initialize(fsInternetSession *pSession)
{
	m_pSession = pSession;
	return IR_SUCCESS;
}

LPCTSTR fsInternetServerConnection::GetLastError()
{
	return m_pszLastError;
}

VOID fsInternetServerConnection::CloseHandle()
{
	if (m_hServer)
	{
		InternetCloseHandle (m_hServer);
		m_hServer = NULL;
	}
}

fsInternetSession* fsInternetServerConnection::GetSession()
{
	return m_pSession;
}

LPCTSTR fsInternetServerConnection::GetServerName()
{
	return m_strServer.c_str();
}

INTERNET_PORT fsInternetServerConnection::GetServerPort()
{
	return m_uPort;
}

