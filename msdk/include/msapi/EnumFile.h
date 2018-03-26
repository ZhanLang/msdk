#pragma once
namespace msdk{;
namespace msapi{;



class CEnumFile
{
public:
	//·µ»ØFALSEÌø³öÃ¶¾Ù
	typedef BOOL(*FileEnumFunction)(LPCTSTR, LPVOID);

	CEnumFile(FileEnumFunction pFileEnum, LPVOID lParam, BOOL bDir = FALSE, INT Depth = -1)
	{
		m_bDir = bDir;
		m_lParam = lParam;
		m_pFileEnum = pFileEnum;
		m_nDepth = Depth;
	}
	~CEnumFile() {};

	

	BOOL EnumFile(LPCTSTR szPath, LPCTSTR szSuppix)
	{
		INT nDepth = 0;
		return EnumFile(szPath, szSuppix, nDepth);
	}

private:
	BOOL EnumFile(LPCTSTR szPath, LPCTSTR szSuppix, INT& nDepth)
	{

		WIN32_FIND_DATA  wData;
		TCHAR szTargetPath[MAX_PATH] = {0};
		szSuppix =  (szSuppix && _tcslen(szSuppix)) ? szSuppix : _T("*.*");
		wsprintf(szTargetPath, _T("%s\\%s"), szPath,szSuppix);

		HANDLE hFindFile = FindFirstFile(szTargetPath, &wData);
		if ( hFindFile == INVALID_HANDLE_VALUE)
		{
			return TRUE;
		}

		BOOL bRet = TRUE;
		BOOL bReturn = TRUE;
		while( bRet )
		{

			if( 0 == _tcsicmp(wData.cFileName, _T(".")) 
				|| 0 == _tcsicmp(wData.cFileName, _T("..")) 
				|| wData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT )
			{
				bRet = FindNextFile(hFindFile, &wData);
				continue;
			}

			TCHAR szTempFilePath[MAX_PATH] = {0};
			wsprintf(szTempFilePath, _T("%s\\%s"), szPath, wData.cFileName);
			if( wData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( m_bDir )
				{
					bReturn = m_pFileEnum(szTempFilePath, m_lParam);
					if ( !bReturn )
						break;
				}


				if ( nDepth <= m_nDepth )
				{
					nDepth++;
					bReturn = EnumFile(szTempFilePath, szSuppix, nDepth);
					if ( !bReturn )
						break;
				}
			}
			else
			{
				bReturn = m_pFileEnum(szTempFilePath, m_lParam);
				if ( !bReturn )
				{
					break;
				}
			}
			bRet = FindNextFile(hFindFile, &wData);
		}
		if ( hFindFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFindFile);
		}

		return bReturn;
	}
private:
	LPVOID m_lParam;
	FileEnumFunction m_pFileEnum;
	BOOL m_bDir;
	INT m_nDepth;
};
};};