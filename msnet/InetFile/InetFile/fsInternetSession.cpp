
#include "stdafx.h"
#include "fsInternetSession.h"

fsInternetSession::fsInternetSession()
{
	m_hSession = NULL;
	m_strProxyUser = _T("");
	m_strProxyPassword = _T("");
}

fsInternetSession::~fsInternetSession()
{
	CloseHandle ();
}

fsInternetResult fsInternetSession::Create(LPCTSTR pszAgent, fsInternetAccessType accType, LPCTSTR pszProxy)
{
	CloseHandle ();

	DWORD dwAccessType;

	switch (accType)
	{
		case IAT_NOPROXY:
			m_strProxy = _T("");
			dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
			break;

		case IAT_PRECONFIGPROXY:
			m_strProxy = _T("Internet Explorer");
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
		dwAccessType == INTERNET_OPEN_TYPE_PROXY ? _T("<local>") : NULL, 0);

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

VOID fsInternetSession::SetTimeout(UINT uTimeout)
{
	if (m_hSession == NULL)
		return;

	InternetSetOption (m_hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &uTimeout, sizeof (uTimeout));
	InternetSetOption (m_hSession, INTERNET_OPTION_SEND_TIMEOUT, &uTimeout, sizeof (uTimeout));
	InternetSetOption (m_hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &uTimeout, sizeof (uTimeout));
}

VOID fsInternetSession::SetProxyAuth(LPCTSTR pszUser, LPCTSTR pszPassword)
{
	m_strProxyUser = pszUser;
	m_strProxyPassword = pszPassword;
}

fsInternetResult fsInternetSession::ApplyProxyAuth(HINTERNET hInetHandle)
{
	if (m_strProxyUser.length() == 0)
		return IR_SUCCESS;

	if (!InternetSetOption (hInetHandle, INTERNET_OPTION_PROXY_USERNAME, 
		(LPVOID) m_strProxyUser.c_str(), m_strProxyUser.length()+1))
		return fsWinInetErrorToIR ();

	if (!InternetSetOption (hInetHandle, INTERNET_OPTION_PROXY_PASSWORD, 
		(LPVOID) m_strProxyPassword.c_str(), m_strProxyPassword.length()+1))
		return fsWinInetErrorToIR ();

	return IR_SUCCESS;
}

VOID fsInternetSession::CloseHandle()
{
	if (m_hSession)
	{
		InternetCloseHandle (m_hSession);
		m_hSession = NULL;
	}
}

VOID fsInternetSession::InitializeWinInet()
{
	AdjustWinInetConnectionLimit ();
	
}

VOID fsInternetSession::AdjustWinInetConnectionLimit()
{
	ULONG ul = 500;
	InternetSetOption (NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &ul, sizeof (ul));
	InternetSetOption (NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &ul, sizeof (ul));
}

LPCTSTR fsInternetSession::get_UserAgent()
{
	return m_strUserAgent.c_str();
}

VOID fsInternetSession::get_Proxy(LPCTSTR &pszProxyName, LPCTSTR &pszProxyUser, LPCTSTR &pszProxyPwd)
{
	pszProxyName = m_strProxy.c_str();
	pszProxyUser = m_strProxyUser.c_str();
	pszProxyPwd  = m_strProxyPassword.c_str();
}
