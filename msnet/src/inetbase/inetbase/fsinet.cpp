/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsinet.h"
#include "fsURL.h"
#include "fsHttpConnection.h"
#include "fsHttpFile.h"
#include "system.h"

#include <stdio.h>

fsInternetResult fsWinErrorToIR (DWORD dwErr)
{
	switch (dwErr)
	{
		case ERROR_BAD_NETPATH:
		case ERROR_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
		case ERROR_FILE_NOT_FOUND:
			return IR_FILENOTFOUND;

		default:
			
			return IR_WININETUNKERROR;
	}
}

fsInternetResult fsWinErrorToIR ()
{
	return fsWinErrorToIR (GetLastError ());
}

fsInternetResult fsWinInetErrorToIR (DWORD dwErr)
{
	switch (dwErr)
	{
		case ERROR_SUCCESS:
			return IR_SUCCESS;

		case ERROR_INTERNET_LOGIN_FAILURE:
			return IR_LOGINFAILURE;

		case ERROR_INTERNET_INCORRECT_USER_NAME:
			return IR_INVALIDUSERNAME;

		case ERROR_INTERNET_INCORRECT_PASSWORD:
			return IR_INVALIDPASSWORD;

		case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
			return IR_LOGINFAILURE;

		case ERROR_INTERNET_UNRECOGNIZED_SCHEME:
		case ERROR_INTERNET_INVALID_URL:
			return IR_BADURL;

		case ERROR_INTERNET_CANNOT_CONNECT:
			return IR_CANTCONNECT;

		case ERROR_INTERNET_CONNECTION_RESET:
			return IR_LOSTCONNECTION;

		case ERROR_INTERNET_TIMEOUT:
			return IR_TIMEOUT;

		case ERROR_INTERNET_NAME_NOT_RESOLVED:
			return IR_NAMENOTRESOLVED;

		case ERROR_INTERNET_EXTENDED_ERROR:
			return IR_EXTERROR;

		case ERROR_INTERNET_CONNECTION_ABORTED:
		case ERROR_INTERNET_OPERATION_CANCELLED:
		case ERROR_FTP_DROPPED:
			return IR_CONNECTIONABORTED;

		case ERROR_INTERNET_NO_DIRECT_ACCESS:
			return IR_NODIRECTACCESS;

		case ERROR_INTERNET_ITEM_NOT_FOUND:
			return IR_FILENOTFOUND;

		case ERROR_INTERNET_DISCONNECTED:
			return IR_NOINTERNETCONNECTION;

		case ERROR_INVALID_PARAMETER:
			return IR_INVALIDPARAM;

		case ERROR_HTTP_INVALID_SERVER_RESPONSE:
			return IR_SERVERUNKERROR;

		case ERROR_HTTP_INVALID_HEADER:
			return IR_E_WININET_UNSUPP_RESOURCE;

		default:
			
			return IR_WININETUNKERROR;
	}
}

fsInternetResult fsWinInetErrorToIR ()
{
	return fsWinInetErrorToIR (GetLastError ());
}

fsInternetResult fsHttpStatusCodeToIR (DWORD dwStatusCode)
{
	switch (dwStatusCode)
	{
		case HTTP_STATUS_BAD_REQUEST:
			return IR_SERVERBADREQUEST;

		case HTTP_STATUS_GONE:
		case HTTP_STATUS_NOT_FOUND:
			return IR_FILENOTFOUND;

		case HTTP_STATUS_PROXY_AUTH_REQ:
			return IR_PROXYAUTHREQ;

		case HTTP_STATUS_OK:
			return IR_SUCCESS;

		case HTTP_STATUS_DENIED:
		case HTTP_STATUS_FORBIDDEN:
			return IR_LOGINFAILURE;

		case HTTP_STATUS_REDIRECT:
		case HTTP_STATUS_REDIRECT_METHOD:
		case HTTP_STATUS_MOVED:
		case HTTP_STATUS_REDIRECT_KEEP_VERB:
			return IR_NEEDREDIRECT;

		case HTTP_STATUS_SERVICE_UNAVAIL:
		case HTTP_STATUS_SERVER_ERROR:
			return IR_SERVERUNKERROR;

		case HTTP_STATUS_VERSION_NOT_SUP:
			return IR_HTTPVERNOTSUP;

		default:
			
			return IR_WININETUNKERROR;
	}
}

fsInternetResult fsHttpOpenUrl (LPCSTR pszUrl, LPCSTR pszUser, LPCSTR pszPassword, fsHttpConnection *pServer, fsHttpFile *pFile, LPSTR* ppRedirectedUrl, BOOL *pbRedirInner)
{
	fsURL url;
	fsInternetResult ir;

	ir = url.Crack (pszUrl);
	if (ir != IR_SUCCESS)
		return ir;

	ir = pServer->Connect (url.GetHostName (), pszUser ? pszUser : url.GetUserName (), 
		pszPassword ? pszPassword : url.GetPassword (), url.GetPort ());

	if (ir != IR_SUCCESS)
		return ir;

	pFile->UseSecure (url.GetInternetScheme () == INTERNET_SCHEME_HTTPS);

	return fsHttpOpenPath (url.GetPath (), pServer, pFile, ppRedirectedUrl, pbRedirInner);
}

fsInternetResult fsHttpOpenPath (LPCSTR pszPath, fsHttpConnection *pServer, fsHttpFile *pFile, LPSTR* ppRedirectedUrl, BOOL *pbRedirInner)
{
	fsInternetResult ir;

	pFile->Initialize (pServer);
	
	*ppRedirectedUrl = NULL;

	
	ir = pFile->Open (pszPath, 0);

	if (ir != IR_SUCCESS)
	{
		if (ir == IR_NEEDREDIRECT) 
		{
			fsString strUrl;
			fsURL u;
			BOOL bRelUrl = FALSE;

			if (u.Crack (pFile->GetLastError ()) != IR_SUCCESS)	
				bRelUrl = TRUE;	

			*pbRedirInner = TRUE;

			if (bRelUrl)
			{
				if (*pFile->GetLastError () != '/' && *pFile->GetLastError () != '\\')
				{
					
					strUrl = pszPath;
					int len = strUrl.Length ();
					while (strUrl [len-1] != '/' && strUrl [len-1] != '\\')
						len--;
					strUrl [len] = 0;
					strUrl += pFile->GetLastError ();
					
				}
				else
				{
					strUrl = pFile->GetLastError ();
				}

				

				
				ir = fsHttpOpenPath (strUrl, pServer, pFile, ppRedirectedUrl, pbRedirInner);
			}
			else
			{
				strUrl = pFile->GetLastError (); 
				ir =  fsHttpOpenUrl (strUrl, NULL, NULL, pServer, pFile, ppRedirectedUrl, pbRedirInner); 
			}

			
			if (*ppRedirectedUrl == NULL)
			{
				fsnew (*ppRedirectedUrl, CHAR, strUrl.Length () + 1);
				strcpy (*ppRedirectedUrl, strUrl);
			}

			
			*pbRedirInner = *pbRedirInner && bRelUrl;
		}

		return ir;
	}

	return IR_SUCCESS;
}

BOOL fsUrlToFullUrl (LPCSTR pszUrlParent, LPCSTR pszUrlCurrent, LPSTR *ppszFullUrl)
{
	fsURL url;
	*ppszFullUrl = NULL;

	try {

	if (IR_SUCCESS != url.Crack (pszUrlCurrent, FALSE)) 
	{
		UINT nLenParent = strlen (pszUrlParent);
		UINT nLenUrl = strlen (pszUrlCurrent);

		fsnew (*ppszFullUrl, char, nLenParent + nLenUrl + 10);
		
		if (*pszUrlCurrent == '\\' || *pszUrlCurrent == '/')
		{
			
			
			fsURL url2;
			if (IR_SUCCESS != url.Crack (pszUrlParent))
				return FALSE;
			DWORD dwLen = nLenParent + nLenUrl + 1;
			url2.Create (url.GetInternetScheme (), url.GetHostName (), url.GetPort (), url.GetUserName (), url.GetPassword (),
				pszUrlCurrent, *ppszFullUrl, &dwLen);
		}
		else
		{
			
			
			

			
			lstrcpy (*ppszFullUrl, pszUrlParent);

			
			int pos = nLenParent;

			int posmin = 0;	
			
			LPCSTR pszPath;
			pszPath = strstr (*ppszFullUrl, "://"); 
			if (pszPath == NULL)
				pszPath = strstr (*ppszFullUrl, ":\\\\");
			if (pszPath)
			{
				pszPath += 3; 
				
				while (*pszPath != 0 && *pszPath != '\\' && *pszPath != '/')
					pszPath++;
				
				posmin = pszPath - *ppszFullUrl;
				
				if (*pszPath == 0)
				{
					
					
					lstrcat (*ppszFullUrl, "/");
					posmin++;
				}
				else 
					posmin++; 
			}

			
			if (pos < posmin)
				pos = posmin;
			else while (pos > posmin && pszUrlParent [pos-1] != '\\' && pszUrlParent [pos-1] != '/')
				pos--; 

			
			
			for (UINT i = 0; i < nLenUrl; i++)
			{
				
				if (pszUrlCurrent [i] == '.')
				{
					
					if (pszUrlCurrent [i+1] == '.')
					{
						if (pszUrlCurrent [i+2] == '\\' || pszUrlCurrent [i+2] == '/') 
						{
							

							if (pos > posmin)
								pos--;

							
							
							while (pos > posmin && ((*ppszFullUrl) [pos-1] != '\\' && (*ppszFullUrl) [pos-1] != '/'))
								pos --;

							i += 2; 
						}
						else
						{
							
							(*ppszFullUrl) [pos++] = pszUrlCurrent [i];
						}
					}
					
					else if (pszUrlCurrent [i+1] == '\\' || pszUrlCurrent [i+1] == '/') 
					{
						i += 1; 
					}
					else
					{
						
						(*ppszFullUrl) [pos++] = pszUrlCurrent [i];
					}
				}
				else
				{
					
					(*ppszFullUrl) [pos++] = pszUrlCurrent [i];
				}
			}

			
			(*ppszFullUrl) [pos] = 0;
		}

		return TRUE; 
	}

	
	
	fsnew (*ppszFullUrl, char, strlen (pszUrlCurrent) + 1);
	strcpy (*ppszFullUrl, pszUrlCurrent);
	return FALSE;

	}
	catch (...)
	{
		return FALSE;	
	}
}

fsInternetResult fsWSAErrorToIR ()
{
	DWORD dwError = WSAGetLastError ();

	switch (dwError)
	{
		case NOERROR:
			return IR_SUCCESS;
	}

	return IR_ERROR;
}

BOOL fsIsUrlRelative (LPCSTR pszUrl)
{
	return strnicmp (pszUrl, "http://", 7) && strnicmp (pszUrl, "https://", 8) && strnicmp (pszUrl, "ftp://", 6);
}

void fsRemoveWWW (LPCSTR pszUrl)
{
	
	
	char* psz = const_cast<char*>(max (strstr (pszUrl, "://"), strstr (pszUrl, ":\\\\")));

	if (psz)
		if (strnicmp (psz+3, "www.", 4) == 0)
			strcpy (psz+3, psz+7);
}

BOOL fsIsServersEqual (LPCSTR pszServ1, LPCSTR pszServ2, BOOL bExcludeSubDomainNameFrom2Site)
{
	if (pszServ1 == NULL || pszServ2 == NULL)
		return FALSE;

	UINT n1 = 0, n2 = 0;

	

	if (strnicmp (pszServ1, "www.", 4) == 0)
		n1 = 4;

	if (strnicmp (pszServ2, "www.", 4) == 0)
		n2 = 4;

	if (bExcludeSubDomainNameFrom2Site)
	{
		int l1 = strlen (pszServ1);
		int l2 = strlen (pszServ2);

		
		
		if (l1-n1 < l2-n2)
		{
			
			if (pszServ2 [l2 - (l1 - n1) - 1] == '.')
				n2 = l2 - (l1 - n1);	
		}
	}

	return stricmp (pszServ1 + n1, pszServ2 + n2) == 0;
}

// fsInternetResult fsDownloadFile (fsInternetURLFile* file, LPBYTE* ppBuf, UINT* puSize, BOOL* pbAbort)
// {
// 	UINT uToRead = 1000;		
// 	const FLOAT fInc = 1.2f;	
// 
// 	fsInternetResult ir;
// 
// 	
// 
// 	UINT64 uFileSize = file->GetFileSize ();
// 	UINT uMax = (UINT) uFileSize;	
// 	UINT uPos = 0;	
// 	DWORD dwRead;	
// 
// 	if (uFileSize == _UI64_MAX)	
// 		uMax = 100000;		
// 
// 	*ppBuf = NULL;
// 
// 	LPBYTE pBuf = NULL;
// 	fsnew (pBuf, BYTE, uMax+1);
// 
// 	int cZeroReads = 0;
// 	
// 	do
// 	{
// 		if (uToRead > uFileSize - uPos)
// 			uToRead = UINT (uFileSize - uPos);
// 		if (uToRead == 0)
// 			break;
// 		
// 		if (uPos + uToRead > uMax) 
// 		{
// 			
// 			uMax = UINT (uMax * fInc);
// 
// 			
// 			LPBYTE buf = 0;
// 			fsnew (buf, BYTE, uMax+1);
// 			CopyMemory (buf, pBuf, uPos);
// 			delete [] pBuf;
// 			pBuf = buf;
// 		}
// 
// 		ir = file->Read (LPBYTE (pBuf+uPos), uToRead, &dwRead);
// 		if (ir != IR_SUCCESS)
// 		{
// 			delete pBuf;
// 			return ir;
// 		}
// 
// 		uPos += dwRead;	
// 
// 		if (dwRead == 0)
// 		{
// 			cZeroReads ++;
// 			if (cZeroReads < 3 && uFileSize != _UI64_MAX)
// 				dwRead = 1; 
// 		}
// 		else
// 			cZeroReads = 0;
// 	}
// 	while (dwRead && *pbAbort == FALSE);
// 
// 	if (*pbAbort)
// 		return IR_S_FALSE;
// 
// 	if (uFileSize != _UI64_MAX && uFileSize != uPos)
// 		return IR_WININETUNKERROR;
// 
// 	pBuf [uPos] = 0;	
// 
// 	*ppBuf = pBuf;
// 
// 	if (puSize)
// 		*puSize = uPos;
// 
// 	return IR_SUCCESS;
// }