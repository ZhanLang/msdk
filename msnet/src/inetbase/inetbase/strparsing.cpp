/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include <windows.h>
#include "common.h"

LPCSTR fsStrStrNoCase(LPCSTR pszIn, LPCSTR pszWhat)
{
	UINT uLen = strlen (pszWhat);

	if (pszIn == NULL)
		return NULL;

	while (*pszIn)
	{
		if (strnicmp (pszIn, pszWhat, uLen) == 0)
			return pszIn;
		else
			pszIn++;
	}

	return NULL;
}

LPCSTR fsStrGetStrUpToChar (LPCSTR pszFrom, LPCSTR pszCharTo, LPSTR* ppszResult)
{

	
	
	int tolen = strcspn (pszFrom, pszCharTo);

	*ppszResult = NULL;

	if (pszFrom [tolen] == 0) 
	{

		return NULL;
	}

	fsnew (*ppszResult, char, tolen + 1);
	CopyMemory (*ppszResult, pszFrom, tolen);
	(*ppszResult) [tolen] = 0;

	return pszFrom + tolen + 1; 
}
