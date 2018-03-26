/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "common.h"
#include "fsSpeaking.h"

fsSpeaking::fsSpeaking()
{
	m_pfnDlgFunc = NULL;
	m_bMute = FALSE;
}

fsSpeaking::~fsSpeaking()
{

}

void fsSpeaking::SetDialogFunc(fntInetFileDialogFunc pfn, LPVOID lpParam1, LPVOID lpParam2)
{
	m_pfnDlgFunc = pfn;
	m_lpDlgParam1 = lpParam1;
	m_lpDlgParam2 = lpParam2;
}

void fsSpeaking::Dialog(fsInetFileDialogDirection enDir, LPCSTR pszMsg)
{
	
	if (m_pfnDlgFunc && m_bMute == FALSE)
		m_pfnDlgFunc (enDir, pszMsg, m_lpDlgParam1, m_lpDlgParam2);
}

void fsSpeaking::DialogFtpResponse()
{
	if (m_pfnDlgFunc && m_bMute == FALSE)
	{
		char sz [10000];
		DWORD dwErr, dwLen = sizeof (sz);
		
		
		if (InternetGetLastResponseInfo (&dwErr, sz, &dwLen))
			m_pfnDlgFunc (IFDD_FROMSERVER, sz, m_lpDlgParam1, m_lpDlgParam2);
	}
}

void fsSpeaking::DialogHttpResponse(HINTERNET hInet)
{
	if (m_pfnDlgFunc && m_bMute == FALSE)
	{
		char sz [10000];
		DWORD dwLen = sizeof (sz), dwIndex = 0;
		
		
		if (HttpQueryInfo (hInet, HTTP_QUERY_RAW_HEADERS_CRLF, sz, &dwLen, &dwIndex))
			m_pfnDlgFunc (IFDD_FROMSERVER, sz, m_lpDlgParam1, m_lpDlgParam2);
	}
}

void fsSpeaking::Mute(BOOL bMute)
{
	m_bMute = bMute;
}
