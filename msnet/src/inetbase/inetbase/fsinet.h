/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#ifndef __FS_INET_H_
#define __FS_INET_H_

#include <windows.h>
#include <wininet.h>

extern fsInternetResult fsWinInetErrorToIR ();
extern fsInternetResult fsWinInetErrorToIR (DWORD dwErr);
extern fsInternetResult fsHttpStatusCodeToIR (DWORD dwStatusCode);

extern fsInternetResult fsHttpOpenPath (LPCSTR pszPath, class fsHttpConnection *pServer, class fsHttpFile *pFile, LPSTR* ppRedirectedUrl, BOOL *pbRedirInner);

extern fsInternetResult fsHttpOpenUrl (LPCSTR pszUrl, LPCSTR pszUser, LPCSTR pszPassword, class fsHttpConnection *pServer, class fsHttpFile *pFile, LPSTR* ppRedirectedUrl, BOOL *pbRedirInner);

extern BOOL fsUrlToFullUrl (LPCSTR pszUrlParent, LPCSTR pszUrlCurrent, LPSTR *ppszFullUrl);
extern fsInternetResult fsWSAErrorToIR ();

extern BOOL fsIsUrlRelative (LPCSTR pszUrl);
extern fsInternetResult fsWinErrorToIR (DWORD dwErr);
extern fsInternetResult fsWinErrorToIR ();

extern void fsRemoveWWW (LPCSTR pszUrl);

extern BOOL fsIsServersEqual (LPCSTR pszServ1, LPCSTR pszServ2, BOOL bExcludeSubDomainNameFrom2Site = FALSE);

extern fsInternetResult fsDownloadFile (class fsInternetURLFile* file, LPBYTE* ppBuf, UINT* puSize, BOOL* pbAbort);

#endif