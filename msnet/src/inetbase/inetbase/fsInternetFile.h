/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSINTERNETFILE_H__3693BB45_3388_4234_821D_FE3961082A7C__INCLUDED_)
#define AFX_FSINTERNETFILE_H__3693BB45_3388_4234_821D_FE3961082A7C__INCLUDED_

#include "fsinet.h"	
#include <limits.h>
#if _MSC_VER > 1000
#pragma once
#endif 

#include <windows.h>
#include <wininet.h>
#include "fsServerConnection.h"
#include <fsString.h>
#include "fsSpeaking.h"

#include "fsInternetFile_i.h"

class fsInternetFile : public fsInternetFile_i, public fsSpeaking
{
public:
	
	HINTERNET GetRawHandle();
	
	
	virtual fsInternetResult QuerySize(LPCSTR pszFilePath);
	
	
	
	
	virtual fsInternetResult Open (LPCSTR pszFilePath, UINT64 uStartPos) = NULL;
	virtual fsInternetResult OpenEx (LPCSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize, UINT64 uUploadTotalSize) = NULL;
	
	
	
	
	virtual fsInternetResult Read (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwRead) = NULL;
	virtual fsInternetResult Write (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwWritten) = NULL;
	
	
	
	
	LPCSTR GetSuggestedFileName ();
	
	fsResumeSupportType IsResumeSupported ();
	
	
	BOOL GetContentType (LPSTR pszType);
	
	
	
	BOOL GetLastModifiedDate (FILETIME* pTime);
	
	virtual void CloseHandle();
	
	LPCSTR GetLastError();
	
	virtual UINT64 GetFileSize () {return m_uFileSize;};
	
	
	virtual fsInternetResult Initialize (fsInternetServerConnection_i *pServer);
	fsInternetFile();
	virtual ~fsInternetFile();

protected:
	fsResumeSupportType m_enRST;		
	LPSTR m_pszLastError;		
	fsString m_strContentType;	
	BOOL m_bContentTypeValid;	
	FILETIME m_date;			
	BOOL m_bDateValid;			
	UINT64 m_uFileSize;			
	HINTERNET m_hFile;			
	UTIL::com_ptr<fsInternetServerConnection_i>m_pServer;	
	fsString m_strSuggFileName;	
};

#endif 
