#pragma once
#include <tchar.h>
#include <shlwapi.h>
#include <Shellapi.h>
#include <vector>
#include <string>

namespace msdk{;
namespace msapi{


static LPCTSTR GetPathFilePath(LPCTSTR lpszPathFile, LPTSTR lpszPath, DWORD dwCch)
{
	ZeroMemory(lpszPath, sizeof(TCHAR)*dwCch);
	_tcscpy_s(lpszPath, dwCch, lpszPathFile);

	TCHAR* pTail = _tcsrchr(lpszPath, _T('\\'));
	if (NULL != pTail){
		*pTail = 0;
	}
	return lpszPath;
}

static LPCTSTR GetPathFileName(LPCTSTR lpszPathFile, LPTSTR lpszName, DWORD dwCch)
{
	LPCTSTR pTail = _tcsrchr(lpszPathFile, _T('\\'));
	if ( pTail )
	{
		_tcscpy_s(lpszName, dwCch, pTail + 1);
	}
	
	return lpszName;
}

static LPCTSTR GetModulePath(HMODULE hModule , LPTSTR path , DWORD dwLen)
{
	ZeroMemory(path, sizeof(TCHAR)*dwLen);
	GetModuleFileName(hModule, path, dwLen);
	TCHAR* pTail = _tcsrchr(path, _T('\\'));
	if (NULL != pTail){
		*pTail = 0;
	}
	return path;
}

//获取当进程路径
static LPCTSTR GetCurrentPath(LPTSTR path , DWORD dwLen)
{
	return GetModulePath(NULL , path , dwLen);
}

//创建一个完整目录
static BOOL CreateDirectoryEx(const TCHAR* szPath)
{
	if ( !(szPath && lstrlen(szPath)) )
		return FALSE;

	TCHAR szMyPath[MAX_PATH ] = {0};
	_tcscpy_s(szMyPath ,MAX_PATH , szPath);
	TCHAR szCurPath[MAX_PATH] = {0};
	for (UINT i = 0; i < _tcslen(szMyPath); i++){
		szCurPath[i] = szMyPath[i];
		if (szCurPath[i] == _T('\\')){
			if (!PathFileExists(szCurPath)){
				CreateDirectory(szCurPath, NULL);
			}
		}
	}

	DWORD dwFileAtt = ::GetFileAttributes(szPath);
	return ((INVALID_FILE_ATTRIBUTES != dwFileAtt) && (0 != (dwFileAtt & FILE_ATTRIBUTE_DIRECTORY)));
}


// 检测指定路径是否目录
static BOOL IsDirectory(LPCTSTR lpszPath)
{
	if ( !( lpszPath && lstrlen(lpszPath)) ) return FALSE;
	DWORD dwAttr = ::GetFileAttributes(lpszPath);
	return (((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE);
}

// 检测指定文件是否存在
static BOOL IsFileExist(LPCTSTR lpszFileName)
{
	if( !(lpszFileName && lstrlen(lpszFileName)) ) return FALSE;
	DWORD dwAttr = ::GetFileAttributes(lpszFileName);
	return (((dwAttr != 0xFFFFFFFF) && (!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))) ? TRUE : FALSE);
}

static BOOL DeleteFileForce(LPCTSTR lpszPath)
{
	DWORD dwFileAttr = GetFileAttributes(lpszPath);
	if (!( dwFileAttr != -1 && !(dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)))
		return FALSE;

	if (DeleteFile(lpszPath))
		return TRUE;

	SYSTEMTIME sysTime = { 0 };
	GetLocalTime(&sysTime);

	TCHAR szMovePath[MAX_PATH] = { 0 } ;
	_stprintf_s(szMovePath, _T("%s-%d-%d-%d-%d-%d-%d-%d-%d"), lpszPath, 
		sysTime.wYear, 
		sysTime.wMonth, 
		sysTime.wDay,
		sysTime.wHour, 
		sysTime.wMinute, 
		sysTime.wSecond, 
		sysTime.wMilliseconds,GetTickCount());

	BOOL bRet = MoveFile(lpszPath, szMovePath);
	bRet = MoveFileEx(szMovePath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	
	return bRet;
}

static UINT64 GetFileSize(LPCTSTR lpName)
{
	if ( !lpName )	return 0;

	WIN32_FIND_DATA fileData = {0};
	HANDLE hFind = FindFirstFile(lpName , &fileData);
	if ( hFind == INVALID_HANDLE_VALUE) return 0;
	FindClose(hFind);
	LARGE_INTEGER llSize;
	llSize.LowPart = fileData.nFileSizeLow;
	llSize.HighPart = fileData.nFileSizeHigh;
	return llSize.QuadPart;
}




static BOOL DeletePath(LPCTSTR lpszPath, BOOL bRecursive, BOOL bOnlyDelDir, BOOL bFaildContinue = TRUE)
{

	if( !lpszPath ) return FALSE;

	WIN32_FIND_DATA FindFileData;
	HANDLE		hFind = NULL;
	TCHAR		szFileName[MAX_PATH] = {0};
	BOOL		fFinished = FALSE;


	// 是文件,非目录
	if (!(GetFileAttributes(lpszPath) & FILE_ATTRIBUTE_DIRECTORY))
	{
		SetFileAttributes(lpszPath, FILE_ATTRIBUTE_NORMAL);
		if ( DeleteFile(lpszPath))
			return TRUE;
		else
			return FALSE;
	}

	if ( _tcslen(lpszPath) == 0 )// add by sys 避免删除当前目录盘符下所有文件
		return FALSE;

	//	溢出判断
	if (_tcslen(lpszPath)>sizeof(szFileName)-5) {
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}	
	_tcscpy_s(szFileName,MAX_PATH, lpszPath);
	if (_tcsrchr(szFileName, _T('\\')) != &szFileName[lstrlen(szFileName)]) 
	{
		_tcscat_s(szFileName, MAX_PATH, _T("\\"));
	}
	_tcscat_s(szFileName, MAX_PATH, _T("*.*"));

	//	搜索文件
	hFind = FindFirstFile(szFileName, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return FALSE;
	}

	while (!fFinished) 
	{ 
		_tcscpy_s(szFileName, MAX_PATH, lpszPath);

		//	溢出判断
		if (_tcslen(FindFileData.cFileName) + lstrlen(szFileName) <= sizeof(szFileName)-1) 
		{
			_tcscat_s(szFileName, MAX_PATH, _T("\\"));
			_tcscat_s(szFileName, MAX_PATH, FindFileData.cFileName); 			
		}
		//	判断是否是目录
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//	判断是否是"."和".."
			if (FindFileData.cFileName[0] != _T('.'))
			{
				if (bRecursive) {
					//	递归调用
					if (!DeletePath(szFileName, bRecursive, bOnlyDelDir) && !bFaildContinue)
						return FALSE;
				}
			}
		}
		else
		{
			SetFileAttributes(szFileName, FILE_ATTRIBUTE_NORMAL);
			if (!bOnlyDelDir) 
			{
				//	清理文件
				DeleteFile(szFileName);
			}
		}

		if (!FindNextFile(hFind, &FindFileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = TRUE; 
			} 
			else 
			{ 
				FindClose(hFind);
				return FALSE;
			} 
		}
	} 

	// Close the search handle. 	
	if (!FindClose(hFind)) 
	{ 
		return FALSE;
	}

	SetFileAttributes(lpszPath, FILE_ATTRIBUTE_NORMAL);
	return RemoveDirectory(lpszPath);

}

//递归删除目录，如果目录中存在文件则失败
static BOOL TyrRemoveDirectoryEx(LPCTSTR lpszFileDir)
{
	TCHAR lpszPath[MAX_PATH] = { 0 };
	_tcscpy_s(lpszPath, MAX_PATH, lpszFileDir);

	for (LPTSTR lpszTemp = _tcsrchr(lpszPath, '\\') ; lpszTemp ; lpszTemp = _tcsrchr(lpszPath, '\\'))
	{
		RemoveDirectory(lpszPath);
		*lpszTemp = '\0';
	}

	return TRUE;
}

static BOOL IsDirectoryExist(LPCTSTR lpszPath)
{
	if(!(lpszPath && lstrlen(lpszPath))) return FALSE;
	DWORD dwAttr = ::GetFileAttributes(lpszPath);
	return (((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE);
}

static BOOL DeleteFileEx(const TCHAR* szPath)
{
	if( !(szPath && lstrlen(szPath)) ) return FALSE;

	DWORD dwFileAttr = GetFileAttributes( szPath );
	if ( dwFileAttr == 0xFFFFFFFF )
		return TRUE;

	
	if ( !(dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) )
		return ::DeleteFile(szPath);
		

	TCHAR strDirSpec[MAX_PATH] = {0};
	lstrcpy(strDirSpec,szPath);
	lstrcat(strDirSpec, strDirSpec[lstrlen(strDirSpec)-1] == '\\' ? _T("*"):_T("\\*"));

	WIN32_FIND_DATA FindFileData = {0};
	HANDLE hFind = FindFirstFile(strDirSpec, &FindFileData);
	if( hFind == INVALID_HANDLE_VALUE) return FALSE;

	do 
	{
		LPCTSTR lpFileName = FindFileData.cFileName;
		if (lstrcmp(lpFileName,_T("..")) == 0 || lstrcmp(lpFileName,_T(".")) == 0)
			continue;

		TCHAR strTemp[MAX_PATH] = {0};
		lstrcpy(strTemp,szPath);
		lstrcat(strTemp,_T("\\"));
		lstrcat(strTemp,FindFileData.cFileName);

		if ( !DeleteFileEx(strTemp) )
			return FALSE;

	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);

	
	//到这里了必然是目录
	TyrRemoveDirectoryEx(szPath);
	

	return TRUE;
}


static BOOL ShellDeleteFile(LPCTSTR szPath)
{
	TCHAR szBuffer[1024] = {0};
	_tcsncpy(szBuffer, szPath, _tcslen(szPath));

	SHFILEOPSTRUCT shfos = { 0 };
	shfos.hwnd = NULL;
	shfos.wFunc = FO_DELETE;
	shfos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
	shfos.pFrom = szBuffer;
	shfos.pTo = NULL;
	shfos.lpszProgressTitle = NULL;
	return SHFileOperation(&shfos) == 0;
}

static BOOL ShellCopyFile( LPCTSTR lpszSrc, LPCTSTR lpszDes)
{
	TCHAR szSrc[MAX_PATH] = {0};
	_tcsncpy(szSrc, lpszSrc, _countof(szSrc));

	TCHAR szDes[MAX_PATH] = {0};
	_tcsncpy(szDes, lpszDes, _countof(szDes));

	SHFILEOPSTRUCT shfos = { 0 };
	shfos.hwnd = NULL;
	shfos.wFunc = FO_COPY;
	shfos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
	shfos.pFrom = szSrc;
	shfos.pTo = szDes;
	shfos.lpszProgressTitle = NULL;
	return SHFileOperation(&shfos) == 0;
}

static BOOL CopyPath( LPCTSTR lpszSrc, LPCTSTR lpszDes, BOOL bFailIfExists )
{
	if ( !( GetFileAttributes( lpszSrc) & FILE_ATTRIBUTE_DIRECTORY ) )
	{
		CreateDirectoryEx(lpszDes);
		if ( CopyFile( lpszSrc, lpszDes, bFailIfExists) )
			return TRUE;

		if ( !bFailIfExists )
			return TRUE;

		return FALSE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE		hFind = NULL;
	TCHAR		szFileName[MAX_PATH] = {0};
	TCHAR		szDesFileName[MAX_PATH] = {0};
	BOOL		fFinished = FALSE;

	_tcscpy_s(szFileName,MAX_PATH, lpszSrc);

	if (_tcsrchr(szFileName, _T('\\')) != &szFileName[lstrlen(szFileName)]) 
	{
		_tcscat_s(szFileName, MAX_PATH, _T("\\"));
	}
	_tcscat_s(szFileName, MAX_PATH, _T("*.*"));


	_tcscpy_s(szDesFileName,MAX_PATH, lpszDes);
	if (_tcsrchr(szDesFileName, _T('\\')) != &szDesFileName[lstrlen(szDesFileName)]) 
	{
		_tcscat_s(szDesFileName, MAX_PATH, _T("\\"));
	}


	//	搜索文件
	hFind = FindFirstFile(szFileName, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return FALSE;
	}

	while (!fFinished) 
	{ 
		_tcscpy_s(szFileName, MAX_PATH, lpszSrc);
		_tcscpy_s(szDesFileName, MAX_PATH, lpszDes);
		//	溢出判断
		if (_tcslen(FindFileData.cFileName) + lstrlen(szFileName) <= sizeof(szFileName)-1) 
		{
			_tcscat_s(szFileName, MAX_PATH, _T("\\"));
			_tcscat_s(szFileName, MAX_PATH, FindFileData.cFileName); 
		}

		if (_tcslen(FindFileData.cFileName) + lstrlen(szDesFileName) <= sizeof(szDesFileName)-1) 
		{
			_tcscat_s(szDesFileName, MAX_PATH, _T("\\"));
			_tcscat_s(szDesFileName, MAX_PATH, FindFileData.cFileName); 
		}

		if (FindFileData.cFileName[0] != _T('.'))
		{
			if ( !CopyPath(szFileName, szDesFileName, bFailIfExists) )
			{
				return FALSE;
			}

			SetFileAttributes( szDesFileName,  GetFileAttributes(szFileName));
		}



		if (!FindNextFile(hFind, &FindFileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = TRUE; 
			} 
			else 
			{ 
				FindClose(hFind);
				return FALSE;
			} 
		}
	} 

	// Close the search handle. 	
	if (!FindClose(hFind)) 
	{ 
		return FALSE;
	}

	return TRUE;
}


};//namespace mspath
};