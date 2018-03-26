/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#ifndef __STR_PARSING__H_
#define __STR_PARSING__H_

inline BOOL fsStrIsDivider (char c)
{
	if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
		return TRUE;
	
	return FALSE;
}

inline LPCSTR fsStrSkipDividers (LPCSTR psz)
{
	while (fsStrIsDivider (*psz)) 
		psz ++;

	return psz;
}

inline const char* fsStrSkipSpaces (const char* psz)
{
	while (*psz == ' ') 
		psz ++;

	return psz;
}

extern LPCSTR fsStrStrNoCase(LPCSTR pszIn, LPCSTR pszWhat);

extern LPCSTR fsStrGetStrUpToChar (LPCSTR pszFrom, LPCSTR pszCharTo, LPSTR* ppszResult);

#endif