/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSLOCALFILE_H__1DDB29EB_F880_4374_BF2A_7C3A1A12F898__INCLUDED_)
#define AFX_FSLOCALFILE_H__1DDB29EB_F880_4374_BF2A_7C3A1A12F898__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "fsInternetFile.h"
#include "fsinet.h"	
#include "fsInternetSession.h"	
#include "fsLocalFile_i.h"

class fsLocalFile :public fsLocalFile_i, public fsInternetFile  , CUnknownImp
{
public:
	UNKNOWN_IMP2_(fsInternetFile_i,fsLocalFile_i);
	fsInternetResult Write (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwWritten);
	fsInternetResult OpenEx (LPCSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize = _UI64_MAX, UINT64 uUploadTotalSize = _UI64_MAX);
	
	virtual fsInternetResult QuerySize (LPCSTR pszFilePath);
	virtual void CloseHandle();
	void Initialize (LPCSTR pszServer);
	fsInternetResult Read (LPBYTE pBuffer, DWORD dwSize, DWORD *pdwRead);
	fsInternetResult Open (LPCSTR pszFilePath, UINT64 uStartPos);
	fsLocalFile();
	virtual ~fsLocalFile();

protected:
	
	fsString m_strServer;
};

#endif 
