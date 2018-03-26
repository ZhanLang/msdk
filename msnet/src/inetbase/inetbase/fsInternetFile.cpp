/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsInternetFile.h"
#include "common.h"

fsInternetFile::fsInternetFile()
{
	m_hFile = NULL;
	m_pServer = INULL;
	m_pszLastError = NULL;
	m_enRST = RST_UNKNOWN;
	m_uFileSize = _UI64_MAX;

	m_bContentTypeValid = m_bDateValid = FALSE;
}

fsInternetFile::~fsInternetFile()
{
	CloseHandle ();

	SAFE_DELETE_ARRAY (m_pszLastError);
}

fsInternetResult fsInternetFile::Initialize(fsInternetServerConnection_i *pServer)
{
	m_pServer = pServer;
	return IR_SUCCESS;
}

LPCSTR fsInternetFile::GetLastError()
{
	return m_pszLastError;
}

void fsInternetFile::CloseHandle()
{
	if (m_hFile)
	{
		InternetCloseHandle (m_hFile);
		m_hFile = NULL;
	}
}

BOOL fsInternetFile::GetLastModifiedDate(FILETIME *pTime)
{
	if (m_bDateValid)
		CopyMemory (pTime, &m_date, sizeof (m_date));
	return m_bDateValid;
}

BOOL fsInternetFile::GetContentType(LPSTR pszType)
{
	*pszType = 0;

	if (m_bContentTypeValid)
		strcpy (pszType, m_strContentType);
		
	return m_bContentTypeValid;
}

fsResumeSupportType fsInternetFile::IsResumeSupported()
{
	return m_enRST;
}

LPCSTR fsInternetFile::GetSuggestedFileName()
{
	return m_strSuggFileName;
}

fsInternetResult fsInternetFile::QuerySize(LPCSTR)
{
	
	return IR_S_FALSE;
}

HINTERNET fsInternetFile::GetRawHandle()
{
	return m_hFile;
}
