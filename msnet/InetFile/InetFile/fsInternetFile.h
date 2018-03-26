
#pragma once
#include <windows.h>
#include <wininet.h>
#include "fsServerConnection.h"

enum fsResumeSupportType
{
	RST_PRESENT,	
	RST_NONE,		
	RST_UNKNOWN,	
};

class fsInternetFile
{
public:
	
	HINTERNET GetRawHandle();
	virtual fsInternetResult QuerySize(LPCSTR pszFilePath);
	virtual fsInternetResult Open (LPCTSTR pszFilePath, UINT64 uStartPos) = NULL;
	virtual fsInternetResult OpenEx (LPCTSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize, UINT64 uUploadTotalSize) = NULL;
	virtual fsInternetResult Read (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwRead) = NULL;
	virtual fsInternetResult Write (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwWritten) = NULL;
	LPCTSTR GetSuggestedFileName ();
	fsResumeSupportType IsResumeSupported ();
	BOOL GetContentType (LPTSTR pszType);
	BOOL GetLastModifiedDate (FILETIME* pTime);
	virtual VOID CloseHandle();
	LPCTSTR GetLastError();
	virtual UINT64 GetFileSize () {return m_uFileSize;};
	virtual fsInternetResult Initialize (fsInternetServerConnection *pServer);
	fsInternetFile();
	virtual ~fsInternetFile();
protected:
	fsResumeSupportType m_enRST;		
	LPTSTR m_pszLastError;		
	fsString m_strContentType;	
	BOOL m_bContentTypeValid;	
	FILETIME m_date;			
	BOOL m_bDateValid;			
	UINT64 m_uFileSize;			
	HINTERNET m_hFile;			
	fsInternetServerConnection *m_pServer;	
	fsString m_strSuggFileName;	
};

