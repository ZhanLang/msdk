/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsInternetSession.h"
#include "fsinet.h"

fsInternetSession::fsInternetSession()
{
	m_hSession = NULL;
	m_strProxyUser = "";
	m_strProxyPassword = "";
}

fsInternetSession::~fsInternetSession()
{
	CloseHandle ();
}

fsInternetResult fsInternetSession::Create(LPCSTR pszAgent, fsInternetAccessType accType, LPCSTR pszProxy)
{
	static BOOL bInit = FALSE;
	if (!bInit)
	{
		InitializeWinInet();
		bInit =TRUE;
	}
	CloseHandle ();

	DWORD dwAccessType;

	switch (accType)
	{
		case IAT_NOPROXY:
			m_strProxy = "";
			dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
			break;

		case IAT_PRECONFIGPROXY:
			m_strProxy = "Internet Explorer";
			dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
			break;

		case IAT_MANUALPROXY:
			dwAccessType = INTERNET_OPEN_TYPE_PROXY;
			if (pszProxy == NULL || *pszProxy == 0)
				return IR_INVALIDPARAM;
			m_strProxy = pszProxy;
			break;

		default:
			return IR_INVALIDPARAM;
	}

	m_strUserAgent = pszAgent;

	m_hSession = InternetOpen (pszAgent, dwAccessType, pszProxy, 
		dwAccessType == INTERNET_OPEN_TYPE_PROXY ? "<local>" : NULL, 0);

	if (m_hSession == NULL)
		return fsWinInetErrorToIR ();

	
	BOOL bDisable = TRUE;
	InternetSetOption (m_hSession, INTERNET_OPTION_DISABLE_AUTODIAL, &bDisable, sizeof (bDisable));

	return IR_SUCCESS;
}

HINTERNET fsInternetSession::GetHandle()
{
	return m_hSession;
}

void fsInternetSession::SetTimeout(UINT uTimeout)
{
	if (m_hSession == NULL)
		return;

	InternetSetOption (m_hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &uTimeout, sizeof (uTimeout));
	InternetSetOption (m_hSession, INTERNET_OPTION_SEND_TIMEOUT, &uTimeout, sizeof (uTimeout));
	InternetSetOption (m_hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &uTimeout, sizeof (uTimeout));
}

void fsInternetSession::SetProxyAuth(LPCSTR pszUser, LPCSTR pszPassword)
{
	m_strProxyUser = pszUser;
	m_strProxyPassword = pszPassword;
}

fsInternetResult fsInternetSession::ApplyProxyAuth(HINTERNET hInetHandle)
{
	if (m_strProxyUser.Length () == 0)
		return IR_SUCCESS;

	if (!InternetSetOption (hInetHandle, INTERNET_OPTION_PROXY_USERNAME, 
		(LPVOID) m_strProxyUser, m_strProxyUser.Length ()+1))
		return fsWinInetErrorToIR ();

	if (!InternetSetOption (hInetHandle, INTERNET_OPTION_PROXY_PASSWORD, 
		(LPVOID) m_strProxyPassword, m_strProxyPassword.Length ()+1))
		return fsWinInetErrorToIR ();

	return IR_SUCCESS;
}

void fsInternetSession::CloseHandle()
{
	if (m_hSession)
	{
		InternetCloseHandle (m_hSession);
		m_hSession = NULL;
	}
}

void fsInternetSession::InitializeWinInet()
{
	ULONG ul = 500;
	InternetSetOption (NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &ul, sizeof (ul));
	InternetSetOption (NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &ul, sizeof (ul));

	
}

LPCSTR fsInternetSession::get_UserAgent()
{
	return m_strUserAgent;
}

void fsInternetSession::get_Proxy(LPCSTR &pszProxyName, LPCSTR &pszProxyUser, LPCSTR &pszProxyPwd)
{
	pszProxyName = m_strProxy;
	pszProxyUser = m_strProxyUser;
	pszProxyPwd  = m_strProxyPassword;
}
