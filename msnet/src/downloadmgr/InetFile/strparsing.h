#ifndef __STR_PARSING__H_
#define __STR_PARSING__H_

 inline BOOL fsStrIsDivider (TCHAR c)
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

inline const TCHAR* fsStrSkipSpaces (const TCHAR* psz)
{
	while (*psz == ' ') 
		psz ++;

	return psz;
}

LPCTSTR fsStrStrNoCase(LPCTSTR pszIn, LPCTSTR pszWhat)
{
	UINT uLen = _tcslen(pszWhat);

	if (pszIn == NULL)
		return NULL;

	while (*pszIn)
	{
		/*_tcsncicmp*/ 

		if (_tcsncicmp (pszIn, pszWhat, uLen) == 0)
			return pszIn;
		else
			pszIn++;
	}

	return NULL;
}

LPCTSTR fsStrGetStrUpToChar (LPCTSTR pszFrom, LPCTSTR pszCharTo, LPTSTR* ppszResult)
{
	int tolen = _tcscspn (pszFrom, pszCharTo);

	*ppszResult = NULL;

	if (pszFrom [tolen] == 0) 
	{
		return NULL;
	}

	fsnew (*ppszResult, TCHAR, tolen + 1);
	CopyMemory (*ppszResult, pszFrom, tolen);
	(*ppszResult) [tolen] = 0;

	return pszFrom + tolen + 1; 
}


#endif