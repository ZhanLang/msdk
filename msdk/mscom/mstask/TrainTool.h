#pragma once

namespace train
{
	static LPCTSTR RsGetCurentPath(TCHAR*szPath,DWORD dwMaxSize)
	{
		if(szPath == NULL && dwMaxSize <MAX_PATH)
			return NULL;
		memset((VOID*)szPath,0,dwMaxSize*sizeof(TCHAR));
		GetModuleFileName( NULL, szPath, dwMaxSize);
		LPTSTR lpszToken = _tcsrchr(szPath, _T('\\'));
		if (lpszToken == NULL)
			return NULL;
		*lpszToken = _T('\0');
		return szPath;
	}

	static BOOL IsDirectoryExist(LPCTSTR lpszPathName)
	{
		DWORD dwRt = GetFileAttributes(lpszPathName);
		if(dwRt!=INVALID_FILE_ATTRIBUTES && (dwRt & FILE_ATTRIBUTE_DIRECTORY))
		{
			return TRUE;	
		}
		return FALSE;
	}

	static BOOL IsFileExist(LPCTSTR lpFileName)
	{
		DWORD dwFileAttrib = GetFileAttributes(lpFileName);
		if ( dwFileAttrib == INVALID_FILE_ATTRIBUTES || dwFileAttrib&FILE_ATTRIBUTE_DIRECTORY )
		{
			return FALSE;
		}
		return TRUE;
	}
	static BOOL IsXPAbove()
	{
		OSVERSIONINFO osver;
		osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		if ( ! ::GetVersionEx( &osver ))
			return FALSE;

		if(	osver.dwPlatformId == VER_PLATFORM_WIN32_NT 
			&& ( osver.dwMajorVersion >= 6	|| (osver.dwMajorVersion>=5 && osver.dwMinorVersion>=1)) )
			return TRUE;
		return FALSE;
	}

	static BOOL  ConvertToSystemtime(IN LPCTSTR lpBuff, OUT SYSTEMTIME* pDate)
	{
		if(lpBuff==NULL || pDate ==NULL  ) 
			return FALSE;

		memset(pDate,0,sizeof(SYSTEMTIME));
		int nLen = _tcslen(lpBuff);
		if( nLen < 19 ) 
			return FALSE;
		DWORD dwYear=0;
		DWORD dwMonth=0;
		DWORD dwDay=0;
		DWORD dwData[6]={0};

		TCHAR szFormat[] = _T("%04d-%02d-%02d %02d:%02d:%02d");
		_sntscanf_s(lpBuff, nLen, szFormat, &(dwData[0]),&(dwData[1]),&(dwData[2]),&(dwData[3]),&(dwData[4]),&(dwData[5]));
		pDate->wYear = (WORD)dwData[0];
		pDate->wMonth = (WORD)dwData[1];
		pDate->wDay = (WORD)dwData[2];
		pDate->wHour = (WORD)dwData[3];
		pDate->wMinute = (WORD)dwData[4];
		pDate->wSecond = (WORD)dwData[5];
		return TRUE;
	}

}
