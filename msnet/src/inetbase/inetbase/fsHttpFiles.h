/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSHTTPFILES_H__50AB10C6_642A_4EEE_96AE_6E16668CF6F0__INCLUDED_)
#define AFX_FSHTTPFILES_H__50AB10C6_642A_4EEE_96AE_6E16668CF6F0__INCLUDED_

#include "fsHttpConnection.h"	
#include "fsinet.h"	
#if _MSC_VER > 1000
#pragma once
#endif 

#include "fsInternetFiles.h"
#include "fsHttpFile.h"	

class fsHttpFiles : public fsHttpFiles_i , public fsInternetFiles , CUnknownImp
{
public:
	
	UNKNOWN_IMP2_(fsHttpFiles_i , fsInternetFiles_i);
	fsInternetResult GetList (LPCSTR pszPath);
	void Abort();

	void SetEventFunc (fsOnHttpFilesEvent_i* pfn, LPVOID lpParam);
	
	void UseSecure (BOOL bUse);
	LPCSTR GetLastError();
	
	void SetServer (fsHttpConnection_i* pServer);
	
	
	void RetreiveInfoWhileGettingList (BOOL b);
	fsHttpFiles();
	virtual ~fsHttpFiles();
protected:
	LPCSTR m_pszBaseURL;
	
	void Event (fsHttpFilesEvent enEvent);
	
	
	void CalcUrl (fsFileInfo* pInfo, LPCSTR pszSomeUrl);
	
	void CheckFolder (fsFileInfo* file);
	
	
	
	fsInternetResult GetUrlInfo (LPCSTR pszUrl, fsFileInfo *pInfo);
	
	fsInternetResult BuildFileList();
	
	fsInternetResult LoadFile();

	UTIL::com_ptr<fsOnHttpFilesEvent_i> m_pfnEvents;	
	LPVOID m_lpEventsParam;				
	BOOL m_bUseSecure;					
	fsString m_strFullPath;				
	BOOL m_bAbort;						
	BOOL m_bGetInfo;					
	LPSTR m_pszFileBuffer;				
	UINT64 m_uFileLen;					
	fsHttpFile m_httpFile;				
	UTIL::com_ptr<fsHttpConnection_i> m_pServer;		

private:
	fsHttpConnection m_hUI_server;		
	fsHttpFile m_hUI_file;				
};

#endif 
