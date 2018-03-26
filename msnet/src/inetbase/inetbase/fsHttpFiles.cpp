/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsHttpFiles.h"
#include "fsURL.h"
#include "common.h"
#include <limits.h>
#include "fsHTMLParser.h"
#include "fsinet.h"

fsHttpFiles::fsHttpFiles()
{
	m_pszFileBuffer = NULL;
	m_bGetInfo = TRUE;
	m_pfnEvents = INULL;
}

fsHttpFiles::~fsHttpFiles()
{
	SAFE_DELETE_ARRAY (m_pszFileBuffer);
}

fsInternetResult fsHttpFiles::GetList(LPCSTR pszPath)
{
	fsInternetResult ir;
	BOOL bRedirected = FALSE;

	m_bAbort = FALSE;

	m_strPath = pszPath;
	LPSTR pszNewUrl;

	m_httpFile.UseSecure (m_bUseSecure);

	BOOL bRel;	
	
	
	ir = fsHttpOpenPath (pszPath, (fsHttpConnection*)m_pServer.m_p, &m_httpFile, &pszNewUrl, &bRel);
	if (ir != IR_SUCCESS)
		return ir;

	Event (HFE_CONNECTED);	

	if (pszNewUrl)	
	{
		if (bRel == FALSE)	
		{
			

			fsURL url;
			ir = url.Crack (pszNewUrl);
			if (ir != IR_SUCCESS)
				return ir; 

			m_strPath = url.GetPath ();	

			bRedirected = TRUE;
		}
		else
			m_strPath = pszNewUrl;	

		delete [] pszNewUrl;
	}

	
	
	ir = LoadFile ();
	if (ir != IR_SUCCESS)
		return ir;

	Event (HFE_FILELISTREAD);	

	Event (HFE_STARTBUILDLIST);	

	ir = BuildFileList ();		
	if (ir != IR_SUCCESS)
		return ir;

	Event (HFE_FINISHBUILDLIST); 

	return bRedirected ? IR_S_REDIRECTED : IR_SUCCESS;	
}

fsInternetResult fsHttpFiles::LoadFile()
{
	UINT uToRead = 1000;		
	const FLOAT fInc = 1.2f;	

	fsInternetResult ir;

	UINT64 uFileSize = m_httpFile.GetFileSize ();
	UINT64 uMax = uFileSize;	
	UINT64 uPos = 0;	
	DWORD dwRead;	

	if (uMax == _UI64_MAX)	
		uMax = 100000;		

	SAFE_DELETE_ARRAY (m_pszFileBuffer);

	fsnew (m_pszFileBuffer, char, int (uMax+1));

	int cZeroReads = 0;
	
	do
	{
		if (uToRead > uFileSize - uPos)
			uToRead = UINT (uFileSize - uPos);
		
		if (uPos + uToRead > uMax) 
		{

			
			uMax = UINT64 ((INT64)uMax * fInc);

			
			LPSTR psz = 0;
			fsnew (psz, char, int (uMax+1));
			CopyMemory (psz, m_pszFileBuffer, UINT (uPos));
			delete [] m_pszFileBuffer;
			m_pszFileBuffer = psz;
		}

		ir = m_httpFile.Read (LPBYTE (m_pszFileBuffer+uPos), uToRead, &dwRead);
		if (ir != IR_SUCCESS)
		{
			delete m_pszFileBuffer;
			return ir;
		}

		uPos += dwRead;	

		if (dwRead == 0)
		{
			cZeroReads ++;
			if (cZeroReads < 3 && uFileSize != _UI64_MAX)
				dwRead = 1; 
		}
		else
			cZeroReads = 0; 

	}
	while (dwRead && m_bAbort == FALSE);

	if (m_bAbort)
		return IR_S_FALSE;

	if (uFileSize != _UI64_MAX && uFileSize != uPos)
		return IR_WININETUNKERROR;

	m_pszFileBuffer [m_uFileLen = uPos] = 0;	

	m_httpFile.CloseHandle ();

	return IR_SUCCESS;
}

fsInternetResult fsHttpFiles::BuildFileList()
{
	fsHTMLParser html;

	html.ParseHTML (m_pszFileBuffer); 

	m_pszBaseURL = html.Get_BaseURL ();

	UINT cUrls = html.GetUrlCount ();

	m_vFiles.clear ();

	fsURL url;
	CHAR szUrl [10000];
	DWORD dwLen = 10000;
	url.Create (INTERNET_SCHEME_HTTP, ((fsHttpConnection*)m_pServer.m_p)->GetServerName (), ((fsHttpConnection*)m_pServer.m_p)->GetServerPort (),
		NULL, NULL, m_strPath, szUrl, &dwLen);
	m_strFullPath = szUrl;

	for (UINT i = 0; i < cUrls && m_bAbort == FALSE; i++)
	{
		fsFileInfo file;
		LPCSTR pszUrl = html.GetUrl (i);
		fsURL url;
		if (url.Crack (pszUrl, FALSE) == IR_SUCCESS) 
		{
			INTERNET_SCHEME scheme = url.GetInternetScheme ();
			
			if (scheme != INTERNET_SCHEME_HTTP && scheme != INTERNET_SCHEME_HTTPS && scheme != INTERNET_SCHEME_FTP)
				continue;
		}

		file.bAvailable = TRUE;
		file.bFolder = FALSE;

		
		LPCSTR ppszBadUrls [] = {"/..", "../", "./", "/.", "\\..", "..\\", ".\\", "\\."};
		BOOL bBad = FALSE;

		for (int s = 0; s < sizeof (ppszBadUrls) / sizeof (LPSTR); s++)
		{
			if (strcmp (pszUrl, ppszBadUrls [s]) == 0)
			{
				bBad = TRUE;
				break;
			}
		}

		if (bBad)
			continue;

		CalcUrl (&file, pszUrl);
		CheckFolder (&file); 

		
		if (m_bGetInfo == FALSE || GetUrlInfo (pszUrl, &file) != IR_SUCCESS)
		{
			CalcUrl (&file, pszUrl);
			file.date.dwHighDateTime = file.date.dwLowDateTime = UINT_MAX; 
			file.uSize = _UI64_MAX;
		}

		
		if (file.strName.size() == 0)
			continue;

		m_vFiles.add (file);
	}

	m_strFullPath = NULL; 

	if (m_bAbort)
		m_vFiles.clear ();

	return m_bAbort ? IR_S_FALSE : IR_SUCCESS;
}

fsInternetResult fsHttpFiles::GetUrlInfo(LPCSTR pszUrl, fsFileInfo *pInfo)
{
	fsInternetResult ir;

	pInfo->strName = pszUrl;

	if (pInfo->bFolder)	
		return IR_SUCCESS;

	m_hUI_server.Initialize (((fsHttpConnection*)m_pServer.m_p)->GetSession ());
	

	if (m_bAbort)
		return IR_S_FALSE;

	m_hUI_file.UseSecure (m_bUseSecure);

	fsURL url;
	
	LPSTR pszNewUrl = NULL; 
	BOOL bRedirInner;

	if (url.Crack (pszUrl) == IR_SUCCESS) 
	{
		
		ir = fsHttpOpenUrl (pszUrl, url.GetUserName (), url.GetPassword (), &m_hUI_server, &m_hUI_file, &pszNewUrl, &bRedirInner);
		m_hUI_server.CloseHandle ();
	}
	else
	{
		
		ir = fsHttpOpenPath (pszUrl, (fsHttpConnection*)m_pServer.m_p, &m_hUI_file, &pszNewUrl, &bRedirInner);
	}

	
	m_hUI_file.CloseHandle ();
	
	if (ir != IR_SUCCESS)	
	{
		pInfo->bAvailable = FALSE;	
		return ir;
	}

	if (pszNewUrl)	
		CalcUrl (pInfo, pszNewUrl);
	else
		CalcUrl (pInfo, pszUrl);

	CheckFolder (pInfo);

	
	if (!m_hUI_file.GetLastModifiedDate (&pInfo->date))
		pInfo->date.dwHighDateTime = pInfo->date.dwLowDateTime = UINT_MAX; 

	pInfo->uSize = m_hUI_file.GetFileSize ();

	SAFE_DELETE_ARRAY (pszNewUrl);

	return IR_SUCCESS;
}

void fsHttpFiles::RetreiveInfoWhileGettingList(BOOL b)
{
	m_bGetInfo = b;
}

void fsHttpFiles::CheckFolder(fsFileInfo *file)
{
	UINT uLen = file->strName.size();

	
	if (file->strName [uLen-1] == '\\' || file->strName [uLen - 1] == '/')
	{
		file->strName [uLen - 1] = 0;
		file->bFolder = TRUE;
	}

	LPCSTR pszSlash = strrchr (file->strName.c_str(), '\\');
	pszSlash = max (pszSlash, strrchr (file->strName.c_str(), '/'));
	LPCSTR pszExt = strrchr (file->strName.c_str(), '.');

	
	file->bFolder = pszSlash >= pszExt;

	
	
}

void fsHttpFiles::Abort()
{
	m_bAbort = TRUE;
	m_httpFile.CloseHandle ();
}

void fsHttpFiles::SetServer(fsHttpConnection_i *pServer)
{
	m_pServer = pServer;
}

void fsHttpFiles::CalcUrl(fsFileInfo *pInfo, LPCSTR pszSomeUrl)
{
	fsURL url1, url2;

	fsString strResUrl;

	fsInternetResult ir = url2.Crack (pszSomeUrl);

	if (ir != IR_SUCCESS && m_pszBaseURL && *m_pszBaseURL)
	{
		
		fsURL urlBase;
		if (IR_SUCCESS != urlBase.Crack (m_pszBaseURL))
		{
			strResUrl = "http://";
			strResUrl += m_pszBaseURL;
		}
		else
		{
			strResUrl = m_pszBaseURL;
		}

		if (strResUrl [strResUrl.Length ()-1] != '/' && strResUrl [strResUrl.Length ()-1] != '\\')
			strResUrl += "/";

		ir = urlBase.Crack (strResUrl);
		if (ir == IR_SUCCESS)
			strResUrl += pszSomeUrl;
		else
			strResUrl = pszSomeUrl;
	}
	else
	{
		strResUrl = pszSomeUrl;
	}

	if (ir == IR_SUCCESS)
	{
		if (fsIsServersEqual (url1.GetHostName (), url2.GetHostName (), FALSE) &&
			url1.GetInternetScheme () == url2.GetInternetScheme () &&
			url1.GetPort () == url2.GetPort ())
		{
			LPCSTR p1 = url1.GetPath ();
			LPCSTR p2 = url2.GetPath ();

			while (*p1 && *p1++ == *p2)
				p2++;

			
			strResUrl = p2;
		}
	}

	
	
	
	pInfo->strName = strResUrl;

	
	if (pInfo->strName.at(pInfo->strName.size() - 1) == '\\' ||  pInfo->strName.at(pInfo->strName.size()- 1) == '/')
	{
		
		pInfo->strName [pInfo->strName.size() - 1] = 0;
		pInfo->bFolder = TRUE;	
	}
}

LPCSTR fsHttpFiles::GetLastError()
{
	return ((fsHttpConnection*)m_pServer.m_p)->GetLastError ();
}

void fsHttpFiles::UseSecure(BOOL bUse)
{
	m_bUseSecure = bUse;
}

void fsHttpFiles::SetEventFunc(fsOnHttpFilesEvent_i* pfn, LPVOID lpParam)
{
	m_pfnEvents = pfn;
	m_lpEventsParam = lpParam;
}

void fsHttpFiles::Event(fsHttpFilesEvent enEvent)
{
	if (m_pfnEvents)
		m_pfnEvents->OnOnHttpFilesEvent(this, enEvent, m_lpEventsParam);
}
