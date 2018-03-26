#include "stdafx.h"
#include "fsInternetFile.h"
fsInternetFile::fsInternetFile()
{
	m_hFile = NULL;
	m_pServer = NULL;
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

fsInternetResult fsInternetFile::Initialize(fsInternetServerConnection *pServer)
{
	m_pServer = pServer;
	return IR_SUCCESS;
}

LPCTSTR fsInternetFile::GetLastError()
{
	return m_pszLastError;
}

VOID fsInternetFile::CloseHandle()
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

BOOL fsInternetFile::GetContentType(LPTSTR pszType)
{
	*pszType = 0;

	if (m_bContentTypeValid)
		lstrcpy (pszType, m_strContentType.c_str());
		
	return m_bContentTypeValid;
}

fsResumeSupportType fsInternetFile::IsResumeSupported()
{
	return m_enRST;
}

LPCTSTR fsInternetFile::GetSuggestedFileName()
{
	return m_strSuggFileName.c_str();
}

fsInternetResult fsInternetFile::QuerySize(LPCSTR)
{
	
	return IR_S_FALSE;
}

HINTERNET fsInternetFile::GetRawHandle()
{
	return m_hFile;
}
