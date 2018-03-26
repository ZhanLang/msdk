#include "StdAfx.h"
#include "WinFileSystems.h"

#include "luahost/luahost.h"

#if defined(_MEM_CHECK_)

		#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
		#endif

#endif

namespace winfunc
{

CWinFileSystems::CWinFileSystems(void)
{
}

CWinFileSystems::~CWinFileSystems(void)
{
}

bool CWinFileSystems::InternalDeletePath(LPCTSTR lpszPath, bool bRecursive, bool bOnlyDelDir, bool bErrContinue, bool bMoveAnyway)
{
	//RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("CWinFileSystems->InternalDeletePath(%s, %d, %d, %d, %d)"), lpszPath?lpszPath:_T("null"), bRecursive, bOnlyDelDir, bErrContinue, bMoveAnyway);
	WIN32_FIND_DATA FindFileData;
	HANDLE		hFind = NULL;
	TCHAR		szFileName[MAX_PATH] = {0};
	bool	fFinished = false;
	
	//	参数判断
	ATLASSERT(lpszPath != NULL);

	// 是文件,非目录
	if (!(GetFileAttributes(lpszPath) & FILE_ATTRIBUTE_DIRECTORY))
	{
		SetFileAttributes(lpszPath, FILE_ATTRIBUTE_NORMAL);
		if (::DeleteFile(lpszPath) == FALSE) {
			return bMoveAnyway ? ::MoveFileEx(lpszPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT) == TRUE
							   : false;
		}
		return true;
	}
		
	if ( lstrlen(lpszPath) == 0 )// add by sys 避免删除当前目录盘符下所有文件
		return false;
	
	//	溢出判断
	if (lstrlen(lpszPath)>sizeof(szFileName)-5) {
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return false;
	}
	lstrcpyn(szFileName, lpszPath, sizeof(szFileName)-1);
	if (_tcsrchr(szFileName, _T('\\')) != &szFileName[lstrlen(szFileName)]) 
	{
		lstrcat(szFileName, _T("\\"));
	}
	lstrcat(szFileName, _T("*.*"));
	
	//	搜索文件
	hFind = FindFirstFile(szFileName, &FindFileData);
	
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return false;
	}
	
	while (!fFinished) 
	{ 
		lstrcpyn(szFileName, lpszPath, sizeof(szFileName)-1);
		
		//	溢出判断
		if (lstrlen(FindFileData.cFileName) + lstrlen(szFileName) <= sizeof(szFileName)-1) 
		{
			lstrcat(szFileName, _T("\\"));
			lstrcat(szFileName, FindFileData.cFileName); 			
		}
		//	判断是否是目录
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//	判断是否是"."和".."
			if (FindFileData.cFileName[0] != _T('.'))
			{
				if (bRecursive) {
					//	递归调用
					if(!DeletePath(szFileName, bRecursive, bOnlyDelDir) && !bErrContinue)
						return false;
				}
			}
		}
		else
		{
			SetFileAttributes(szFileName, FILE_ATTRIBUTE_NORMAL);
			if (!bOnlyDelDir) 
			{
				//	清理文件
				if (::DeleteFile(szFileName) == FALSE) {
					if(bMoveAnyway) {
						RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("CWinFileSystems->InternalDeletePath : MoveFileEx(%s)"), szFileName?szFileName:_T("null"));
						::MoveFileEx(szFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
					}
				}
			}
		}
		
		if (!FindNextFile(hFind, &FindFileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = true; 
			} 
			else 
			{ 
				FindClose(hFind);
				return false;
			}
		}
	} 
	
	// Close the search handle. 	
	if (!FindClose(hFind)) 
	{ 
		return false;
	}
	
	SetFileAttributes(lpszPath, FILE_ATTRIBUTE_NORMAL);
	if (::RemoveDirectory(lpszPath) != TRUE) {
		if (bMoveAnyway) {
			RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("CWinFileSystems->InternalDeletePath : MoveFileEx(%s)"), lpszPath?lpszPath:_T("null"));
			return ::MoveFileEx(lpszPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT) == TRUE;
		}
		return false;
	}
	return true;
}


bool CWinFileSystems::DeletePath(LPCTSTR lpszPath, bool bRecursive, bool bOnlyDelDir, bool bErrContinue) {
	return InternalDeletePath(lpszPath, bRecursive, bOnlyDelDir, bErrContinue, false);
}

bool CWinFileSystems::DeletePathEx(LPCTSTR lpszPath, bool bRecursive, bool bOnlyDelDir, bool bErrContinue) {
	return InternalDeletePath(lpszPath, bRecursive, bOnlyDelDir, bErrContinue, true);
}

ULONG CWinFileSystems::GetFileSize(LPCTSTR lpszFileName)
{
	ULONG ulSize = 0;
	HANDLE hFile = CreateFile( lpszFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						0L,
						NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) 
	{
		return 0;
	}

	ulSize = ::GetFileSize( hFile, NULL );
	CloseHandle(hFile);
	return ulSize;
}

bool CWinFileSystems::CopyFile(
				LPCTSTR lpExistingFileName, // name of an existing file
				LPCTSTR lpNewFileName,      // name of new file
				bool bFailIfExists,   // operation if file exists
				bool bCreateDir,	  //  Create directory default
				DWORD dwFileAttributes//	new file's attributes
				)
{
// 	GrpError(RSLOG_DETAIL, MsgLevel_Msg,_T("CWinFileSystems->CopyFile(%s, %s, %s, %s, %d)"), 
// 		lpExistingFileName?lpExistingFileName:_T("null"),
// 		lpNewFileName?lpNewFileName:_T("null"),
// 		bFailIfExists?_T("true"):_T("false"),
// 		bCreateDir?_T("true"):_T("false"),
// 		dwFileAttributes
// 		);
	TCHAR	szNewFilePath[MAX_PATH]={0};
	bool	fRet = false;
	//	参数判断
	ATLASSERT(NULL!=lpExistingFileName && NULL!=lpNewFileName);
	
	if (lstrlen(lpNewFileName)>sizeof(szNewFilePath)-1)
	{
		GrpMsg(GRP_NAME, MsgLevel_Error, _T("CWinFileSystems::CopyFile lstrlen(lpNewFileName)>sizeof(szNewFilePath)-1"));
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return fRet;
	}
	
	lstrcpyn(szNewFilePath, lpNewFileName, sizeof(szNewFilePath)-1);
	TCHAR*	pChar = _tcsrchr(szNewFilePath, _T('\\'));
	if (pChar) 
	{
		*pChar = 0;
        if (lstrlen(szNewFilePath) < 3)
            lstrcat(szNewFilePath, _T("\\"));
	}
	else
	{
		GrpMsg(GRP_NAME, MsgLevel_Error, _T("CWinFileSystems::CopyFile(1)"));
		SetLastError(ERROR_INVALID_PARAMETER);
		return fRet;
	}
	
	//	判断文件是否存在
	if (!PathFileExists(lpExistingFileName)) 
	{
		GrpMsg(GRP_NAME, MsgLevel_Error, _T("CWinFileSystems::CopyFile(2)"));
		SetLastError(ERROR_FILE_NOT_FOUND);
		return fRet;
	}
	
	//	创建文件目录
	if (bCreateDir && !CreatePath(szNewFilePath)) 
	{
		GrpMsg(GRP_NAME, MsgLevel_Error, _T("CWinFileSystems::CopyFile bCreateDir && !CreatePath(%s)"),szNewFilePath);
		SetLastError(ERROR_CANNOT_MAKE);
		return fRet;
	}
	
	//	设置文件属性
	if (PathFileExists(lpNewFileName)) 
	{
		SetFileAttributes(lpNewFileName, FILE_ATTRIBUTE_NORMAL);
	}
	//	拷贝文件
	fRet = BOOL2bool(::CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists));
	//	设置文件属性
	if (fRet && dwFileAttributes)
	{
		SetFileAttributes(lpNewFileName, dwFileAttributes);		
	}
	return fRet;
}

bool CWinFileSystems::CreatePath( LPCTSTR lpFileName )
{
	//RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("CWinFileSystems->CreatePath(%s)"), lpFileName?lpFileName:_T("null"));

	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	bool	fRet = true;
	TCHAR	szFileName[MAX_PATH] = { 0 };
	TCHAR*	lpChr = NULL;
	TCHAR*	lpHead = NULL;

	ATLASSERT(lpFileName != NULL && *lpFileName != '\0');
	if (lpFileName == NULL || lstrlen(lpFileName) <= 0)
		return false;
		
	if (lstrlen(lpFileName) > sizeof(szFileName))
	{
		GrpMsg(GRP_NAME, MsgLevel_Error, _T("CWinFileSystems::CreatePath lstrlen(lpFileName) > sizeof(szFileName)"));
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return false;
	}
	
	lstrcpyn(szFileName, lpFileName, sizeof(szFileName)-1);
	lpHead = PathSkipRoot(szFileName);
	while (true)
	{
        lpChr = lpHead ? _tcschr(lpHead, _T('\\')) : NULL;
		DWORD dwAttrib = 0;
		if (NULL == lpChr)
		{
			dwAttrib = GetFileAttributes(szFileName);
			// 如果存在同名文件则删除
            if ( (INVALID_FILE_ATTRIBUTES == dwAttrib) || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
            {
                if (!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    SetFileAttributes(szFileName, FILE_ATTRIBUTE_NORMAL);
                    DeleteFile(szFileName);
                }

				fRet &= BOOL2bool(::CreateDirectory(szFileName, lpSecurityAttributes));
                if (!fRet && (GetLastError() == ERROR_ALREADY_EXISTS))
                    fRet = true;                
            }

			break;
		}
		else
		{
			*lpChr = NULL;
			dwAttrib = GetFileAttributes(szFileName);
			// 如果存在同名文件则删除
			if ( (INVALID_FILE_ATTRIBUTES == dwAttrib) || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
			{
                if (!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
				    SetFileAttributes(szFileName, FILE_ATTRIBUTE_NORMAL);
				    DeleteFile(szFileName);
                }

				fRet &= BOOL2bool(::CreateDirectory(szFileName, lpSecurityAttributes));
                 if (!fRet && (GetLastError() == ERROR_ALREADY_EXISTS))
                     fRet = true;                
			}

			*lpChr = _T('\\');
			lpHead = lpChr+1;
		}
	}
	if (!fRet)
	{
		fRet = msapi::CreateDirectoryEx(lpFileName) ? true : false;
	}
	if (!fRet)
	{
		GrpMsg(GRP_NAME, MsgLevel_Error, _T("CWinFileSystems::CreatePath"));
	}
	return fRet;
}

bool CWinFileSystems::CopyPath(LPCTSTR lpszSrcPath, LPCTSTR lpszDestPath, DWORD dwDestFileAttributes)
{
	TCHAR		szFileName[MAX_PATH];	
	TCHAR		szNewFile[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE		hFind = NULL;
	bool		fFinished = false;
	bool		fRet = true;
	
	//	参数判断
	ATLASSERT(NULL!=lpszSrcPath && NULL!=lpszDestPath);
	if (!PathFileExists(lpszSrcPath))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}
	
	//	溢出判断
	if (lstrlen(lpszSrcPath)>sizeof(szFileName)-5 ||
		lstrlen(lpszDestPath)>sizeof(szNewFile)-5) {
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return false;
	}
	lstrcpyn(szFileName, lpszSrcPath, sizeof(szFileName)-1);
	if (_tcsrchr(szFileName, _T('\\')) != &szFileName[lstrlen(szFileName)-1]) 
	{
		lstrcat(szFileName, _T("\\"));
	}
	lstrcat(szFileName, _T("*.*"));
	
	//	搜索文件
	hFind = FindFirstFile(szFileName, &FindFileData);
	
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return false;
	}
	
	while (!fFinished) 
	{ 
		lstrcpyn(szFileName, lpszSrcPath, sizeof(szFileName)-1);	
		lstrcpyn(szNewFile, lpszDestPath, sizeof(szNewFile)-1);
		
		if (_tcsrchr(szFileName, _T('\\')) != &szFileName[lstrlen(szFileName)]) 
		{
			lstrcat(szFileName, _T("\\"));
		}
		if (_tcsrchr(szNewFile, _T('\\')) != &szNewFile[lstrlen(szNewFile)]) 
		{
			lstrcat(szNewFile, _T("\\"));
		}
		
		//	溢出判断
		if (lstrlen(FindFileData.cFileName) + lstrlen(szFileName) <= sizeof(szFileName)-1 &&
			lstrlen(FindFileData.cFileName) + lstrlen(szNewFile) <= sizeof(szNewFile)-1) 
		{
			lstrcat(szFileName, FindFileData.cFileName);
			lstrcat(szNewFile, FindFileData.cFileName);
		}
		else
		{
			FindClose(hFind);
			SetLastError(ERROR_BUFFER_OVERFLOW);
			return false;
		}
		//	判断是否是目录
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//	判断是否是"."和".."
			if (FindFileData.cFileName[0] != _T('.'))
			{
				if (!PathFileExists(szNewFile))
				{
					CreatePath(szNewFile);
				}
				//	递归调用
				fRet &= CopyPath(szFileName, szNewFile, dwDestFileAttributes);
			}
		}
		else
		{
			//	复制文件
			if (PathFileExists(szFileName)) 
			{				
				SetFileAttributes(szNewFile, FILE_ATTRIBUTE_NORMAL);
			}
			fRet &= CopyFile(szFileName, szNewFile, false);
			if (fRet && dwDestFileAttributes)
			{				
				SetFileAttributes(szNewFile, dwDestFileAttributes);
			}
		}
		
		if (!FindNextFile(hFind, &FindFileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = true; 
			} 
			else 
			{ 
				FindClose(hFind);
				return false;
			} 
		}
	} 
	
	// Close the search handle. 	
	if (!FindClose(hFind)) 
	{ 
		return false;
	}
	
	return fRet;
}

bool CWinFileSystems::GetDiskFreeSpaceEx(LPCTSTR lpszDriver, ULONGLONG& ullFreeSize)
{
	BOOL	 fResult = FALSE;
	ATLASSERT(lpszDriver != NULL);

	if (lstrlen(lpszDriver)<3)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return	false;
	}	
	
	TCHAR	szBoot[4]={0};
	ULARGE_INTEGER	i64FreeBytesToCaller;
	ULARGE_INTEGER	i64TotalBytes;
	ULARGE_INTEGER	i64FreeBytes;
	
	lstrcpyn(szBoot, lpszDriver, 3);	
	
	fResult = ::GetDiskFreeSpaceEx (szBoot,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);
		ullFreeSize = i64FreeBytes.QuadPart;

	if (!fResult)
	{   
		DWORD	dwSectPerClust=0;
		DWORD	dwBytesPerSect=0;
		DWORD	dwFreeClusters=0;
		DWORD	dwTotalClusters=0;
		
		fResult = ::GetDiskFreeSpace (szBoot, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);
		if (fResult) 
		{
			ullFreeSize = dwSectPerClust*dwBytesPerSect*dwFreeClusters;
		}
	}
	
	return BOOL2bool(fResult);
}

HRESULT CWinFileSystems::DelFileEx(LPCTSTR lpszFileName)
{
	HRESULT hr = ERROR_SUCCESS;

	TCHAR szPath[2 *MAX_PATH] = { 0 };

	//StringCchCopy(szPath, MAX_PATH, lpszFileName);
	lstrcpy(szPath, lpszFileName);

	TCHAR* p = _tcsrchr(szPath, _T('\\'));
	if (p)
	{
		*p = _T('\0');
	}
	else
	{
		return E_INVALIDARG;
	}

	WIN32_FIND_DATA finddata;
	ZeroMemory(&finddata, sizeof(WIN32_FIND_DATA));
	HANDLE		hFind = NULL;
	TCHAR		szFileName[MAX_PATH] = { 0 };
	ZeroMemory(szFileName, sizeof(TCHAR)*MAX_PATH);
	bool	fFinished = false;

	//	参数判断
	ATLASSERT(lpszFileName != NULL);
	if ( lstrlen(szPath) == 0 )// add by sys 避免删除当前目录盘符下所有文件
	{
		return E_INVALIDARG;
	}

	//	搜索文件
	hFind = FindFirstFile(lpszFileName, &finddata);	
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return GetLastError();
	}

	while (!fFinished) 
	{ 		
// 		if (FAILED(hr = StringCbPrintf(szFileName, MAX_PATH, _T("%s\\%s"), szPath, finddata.cFileName)))
// 		{
// 			FindClose(hFind);
// 			return hr;
// 		}		
		wsprintf(szFileName, _T("%s\\%s"), szPath, finddata.cFileName);

		//	判断是否是目录
		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			;
		}
		else
		{			
			SetFileAttributes(szFileName, FILE_ATTRIBUTE_NORMAL);
			//	清理文件
			DeleteFile(szFileName);
		}

		if (!FindNextFile(hFind, &finddata)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = true; 
			} 
			else 
			{ 
				hr = GetLastError();
				FindClose(hFind);
				return hr;
			} 
		}
	} 

	// Close the search handle. 	
	FindClose(hFind);
	return S_OK;
}

bool CWinFileSystems::MoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("MoveFile %s to %s\n"), lpExistingFileName, lpNewFileName);
	return BOOL2bool(::MoveFile(lpExistingFileName, lpNewFileName));
}

bool CWinFileSystems::MoveFileEx(
				LPCTSTR lpExistingFileName,
				LPCTSTR lpNewFileName,
				DWORD dwFlags
				)
{
	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("MoveFileEx %s to %s , arg=%d\n"), lpExistingFileName, lpNewFileName, dwFlags);
	return BOOL2bool(::MoveFileEx(lpExistingFileName, lpNewFileName, dwFlags));
}

ULONGLONG CWinFileSystems::GetDiskFreeSpace(LPCTSTR lpszDriver)
{
	ULONGLONG llSize = 0;
	GetDiskFreeSpaceEx(lpszDriver, llSize);
	return llSize;
}



bool CWinFileSystems::PathFileExists(LPCTSTR lpszPath) {
	return ::PathFileExists(lpszPath) == TRUE;
}

void  CWinFileSystems::RsLog(DWORD dwLevel, LPCTSTR lpszScriptFileName, LPTSTR lpszInfo)
{
	if (lpszInfo == NULL) {
		//RSLOG(lpszScriptFileName, dwLevel, _T(""));
		return;
	}

	int len  = _tcslen(lpszInfo),
		used = 0;
	while (len >= DEBUG_MESSAGE_BUFLEN_MAX) {
		lpszInfo[used + DEBUG_MESSAGE_BUFLEN_MAX - 1] = _T('\0');
		//RSLOG(lpszScriptFileName, dwLevel, &lpszInfo[used]);
		used += DEBUG_MESSAGE_BUFLEN_MAX;
		len  -= DEBUG_MESSAGE_BUFLEN_MAX;
	}
	//RSLOG(lpszScriptFileName, dwLevel, &lpszInfo[used]);
}



/////////////////////////////////////////////////////////////////////////////
bool CWinComDll::InstallCom(LPCTSTR lpszFileName, bool bInstall)
{
	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("Rs%sInstallCom(%s)...\n"), bInstall ? _T("") : _T("Un"), lpszFileName);
	bool bSuccess = false;
	LPCSTR pszDllEntryPoint = bInstall ? "DllRegisterServer" : "DllUnregisterServer";

	if (FAILED(::OleInitialize(NULL)))
	{
		RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("OleInitialize Failed!\n"));
		return false;
	}

	HMODULE hDll = LoadLibrary(lpszFileName);
	if (hDll)
	{
		HRESULT (WINAPI *lpDllEntryPoint)(void);
		(FARPROC&)lpDllEntryPoint = GetProcAddress(hDll, pszDllEntryPoint);
		if (lpDllEntryPoint)
		{
			HRESULT hr = (*lpDllEntryPoint)();
			if (SUCCEEDED(hr))
			{
				bSuccess = true;
			}
			else
			{
				LPVOID lpMsgBuf;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					hr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)(&lpMsgBuf),
					0,
					NULL);
				RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("%s Failed! ErrMsg: %s\n"), pszDllEntryPoint, lpMsgBuf);
				LocalFree(lpMsgBuf);
			}
		}
		else
		{
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("GetProcAddress(%s) Failed!\n"), pszDllEntryPoint);
		}
		FreeLibrary(hDll);
	}
	else
	{
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("LoadLibrary(%s) Failed!\n"), lpszFileName);
	}

	::OleUninitialize();

	RSLOG(RSLOG_DETAIL, MsgLevel_Error,_T("Rs%sInstallCom(%s) Return: %d\n"), bInstall ? _T("") : _T("Un"), lpszFileName, bSuccess);
	return bSuccess;
}

BOOL CWinComDll::BindToLua( ILuaVM* pLuaVM )
{
GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("WinComDll")
		+mluabind::Function("InstallCom", &CWinComDll::InstallCom)
		);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// CWinFileFind implementation

CWinFileFind::CWinFileFind()
{
	m_pFoundInfo = NULL;
	m_pNextInfo = NULL;
	m_hContext = NULL;
	m_chDirSeparator = _T('\\');
}

CWinFileFind::~CWinFileFind()
{
	Close();
}

void CWinFileFind::Close()
{
	if (m_pFoundInfo != NULL)
	{
		delete m_pFoundInfo;
		m_pFoundInfo = NULL;
	}

	if (m_pNextInfo != NULL)
	{
		delete m_pNextInfo;
		m_pNextInfo = NULL;
	}

	if (m_hContext != NULL)
	{
		if (m_hContext != INVALID_HANDLE_VALUE)
			CloseContext();
		m_hContext = NULL;
	}
}

void CWinFileFind::CloseContext()
{
	::FindClose(m_hContext);
	return;
}

bool CWinFileFind::FindFile(LPCTSTR pstrName /* = NULL */,
						 DWORD dwUnused /* = 0 */)
{
	UNUSED_ALWAYS(dwUnused);
	Close();

	if (pstrName == NULL)
		pstrName = _T("*.*");
	else if (lstrlen(pstrName) >= (_countof(((WIN32_FIND_DATA*) m_pNextInfo)->cFileName)))
	{
		::SetLastError(ERROR_BAD_ARGUMENTS);
		return false;		
	}

	m_pNextInfo = new WIN32_FIND_DATA;

	WIN32_FIND_DATA *pFindData = (WIN32_FIND_DATA *)m_pNextInfo;

	Checked::tcscpy_s(pFindData->cFileName, _countof(pFindData->cFileName), pstrName);

	m_hContext = ::FindFirstFile(pstrName, (WIN32_FIND_DATA*) m_pNextInfo);

	if (m_hContext == INVALID_HANDLE_VALUE)
	{
		DWORD dwTemp = ::GetLastError();
		Close();
		::SetLastError(dwTemp);
		return false;
	}
	
	//LPTSTR pstrRoot =m_strRoot.GetBufferSetLength(_MAX_PATH);
	m_strRoot.resize(_MAX_PATH);
	LPCTSTR pstr = _tfullpath(&m_strRoot[0], pstrName, _MAX_PATH);

	// passed name isn't a valid path but was found by the API
	ATLASSERT(pstr != NULL);
	if (pstr == NULL)
	{
		m_strRoot.resize(0);	//ReleaseBuffer(0);
		Close();
		::SetLastError(ERROR_INVALID_NAME);
		return false;
	}
	else
	{
		TCHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR];
		Checked::tsplitpath_s(m_strRoot.c_str(), strDrive, _MAX_DRIVE, strDir, _MAX_DIR, NULL, 0, NULL, 0);
		Checked::tmakepath_s(&m_strRoot[0], _MAX_PATH, strDrive, strDir, NULL, NULL);
		//m_strRoot.ReleaseBuffer(-1);
		m_strRoot.erase(lstrlen(m_strRoot.c_str()));
	}
	return true;
}

bool CWinFileFind::MatchesMask(DWORD dwMask) const
{
	ATLASSERT(m_hContext != NULL);
	

	if (m_pFoundInfo != NULL)
		return BOOL2bool(!!(((LPWIN32_FIND_DATA) m_pFoundInfo)->dwFileAttributes & dwMask));
	else
		return false;
}

bool CWinFileFind::GetLastAccessTime(FILETIME* pTimeStamp) const
{
	ATLASSERT(m_hContext != NULL);
	ATLASSERT(pTimeStamp != NULL);
	

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastAccessTime;
		return true;
	}
	else
		return false;
}

bool CWinFileFind::GetLastWriteTime(FILETIME* pTimeStamp) const
{
	ATLASSERT(m_hContext != NULL);
	ATLASSERT(pTimeStamp != NULL);
	

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastWriteTime;
		return true;
	}
	else
		return false;
}

bool CWinFileFind::GetCreationTime(FILETIME* pTimeStamp) const
{
	ATLASSERT(m_hContext != NULL);
	

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftCreationTime;
		return true;
	}
	else
		return false;
}

bool CWinFileFind::GetLastAccessTime(CTime& refTime) const
{
	ATLASSERT(m_hContext != NULL);
	

	if (m_pFoundInfo != NULL)
	{
		if (CTime::IsValidFILETIME(((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastAccessTime))
		{
			refTime = CTime(((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastAccessTime);
		}
		else
		{
			refTime = CTime();
		}
		return true;
	}
	else
		return false;
}

bool CWinFileFind::GetLastWriteTime(CTime& refTime) const
{
	ATLASSERT(m_hContext != NULL);
	

	if (m_pFoundInfo != NULL)
	{
		if (CTime::IsValidFILETIME(((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastWriteTime))
		{
			refTime = CTime(((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastWriteTime);
		}
		else
		{
			refTime = CTime();
		}
		return true;
	}
	else
		return false;
}

bool CWinFileFind::GetCreationTime(CTime& refTime) const
{
	ATLASSERT(m_hContext != NULL);
	

	if (m_pFoundInfo != NULL)
	{
		if (CTime::IsValidFILETIME(((LPWIN32_FIND_DATA) m_pFoundInfo)->ftCreationTime))
		{
			refTime = CTime(((LPWIN32_FIND_DATA) m_pFoundInfo)->ftCreationTime);
		}
		else
		{
			refTime = CTime();
		}
		return true;
	}
	else
		return false;
}

bool CWinFileFind::IsDots() const
{
	ATLASSERT(m_hContext != NULL);
	

	// return true if the file name is "." or ".." and
	// the file is a directory

	bool bResult = false;
	if (m_pFoundInfo != NULL && IsDirectory())
	{
		LPWIN32_FIND_DATA pFindData = (LPWIN32_FIND_DATA) m_pFoundInfo;
		if (pFindData->cFileName[0] == _T('.'))
		{
			if (pFindData->cFileName[1] == _T('\0') ||
				(pFindData->cFileName[1] == _T('.') &&
				pFindData->cFileName[2] == _T('\0')))
			{
				bResult = true;
			}
		}
	}

	return bResult;
}

bool CWinFileFind::FindNextFile()
{
	ATLASSERT(m_hContext != NULL);

	if (m_hContext == NULL)
		return false;
	if (m_pFoundInfo == NULL)
		m_pFoundInfo = new WIN32_FIND_DATA;

	

	void* pTemp = m_pFoundInfo;
	m_pFoundInfo = m_pNextInfo;
	m_pNextInfo = pTemp;

	return BOOL2bool(::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo));
}

TString CWinFileFind::GetFileURL() const
{
	ATLASSERT(m_hContext != NULL);
	

	TString strResult(_T("file://"));
	strResult += GetFilePath();
	return strResult;
}

TString CWinFileFind::GetRoot() const
{
	ATLASSERT(m_hContext != NULL);
	

	return m_strRoot;
}

TString CWinFileFind::GetFilePath() const
{
	ATLASSERT(m_hContext != NULL);
	

	TString strResult = m_strRoot;
	LPCTSTR pszResult;
	LPCTSTR pchLast;
	pszResult = strResult.c_str();
	pchLast = _tcsdec( pszResult, pszResult+strResult.size() );
	ATLASSERT(pchLast!=NULL);
	if ((*pchLast != _T('\\')) && (*pchLast != _T('/')))
		strResult += m_chDirSeparator;
	strResult += GetFileName();
	return strResult;
}

TString CWinFileFind::GetFileTitle() const
{
	ATLASSERT(m_hContext != NULL);
	

	TString strFullName = GetFileName();
	TString strResult;
	strResult.resize(_MAX_PATH);;

	Checked::tsplitpath_s(strFullName.c_str(), NULL, 0, NULL, 0, 
		//strResult.GetBuffer(_MAX_FNAME), _MAX_FNAME, NULL, 0);
		&strResult[0], _MAX_FNAME, NULL, 0);
	
	//strResult.ReleaseBuffer();
	strResult.erase(lstrlen(strResult.c_str()));
	return strResult;
}



TString CWinFileFind::GetFileExt()const
{
		TString strFullName = this->GetFilePath();
		TCHAR ext[MAX_PATH];
		_tsplitpath_s(strFullName.c_str(), NULL, 0, NULL, 0, NULL, 0, ext, MAX_PATH);
		return ext;
}

TString CWinFileFind::GetFileName() const
{
	ATLASSERT(m_hContext != NULL);
	

	TString ret;

	if (m_pFoundInfo != NULL)
		ret = ((LPWIN32_FIND_DATA) m_pFoundInfo)->cFileName;
	return ret;
}

ULONGLONG CWinFileFind::GetLength() const
{
	ATLASSERT(m_hContext != NULL);
	

	ULARGE_INTEGER nFileSize;

	if (m_pFoundInfo != NULL)
	{
		nFileSize.LowPart = ((LPWIN32_FIND_DATA) m_pFoundInfo)->nFileSizeLow;
		nFileSize.HighPart = ((LPWIN32_FIND_DATA) m_pFoundInfo)->nFileSizeHigh;
	}
	else
	{
		nFileSize.QuadPart = 0;
	}

	return nFileSize.QuadPart;
}

bool CWinFileFind::IsReadOnly() const
{ return MatchesMask(FILE_ATTRIBUTE_READONLY); }
bool CWinFileFind::IsDirectory() const
{ return MatchesMask(FILE_ATTRIBUTE_DIRECTORY); }
bool CWinFileFind::IsCompressed() const
{ return MatchesMask(FILE_ATTRIBUTE_COMPRESSED); }
bool CWinFileFind::IsSystem() const
{ return MatchesMask(FILE_ATTRIBUTE_SYSTEM); }
bool CWinFileFind::IsHidden() const
{ return MatchesMask(FILE_ATTRIBUTE_HIDDEN); }
bool CWinFileFind::IsTemporary() const
{ return MatchesMask(FILE_ATTRIBUTE_TEMPORARY); }
bool CWinFileFind::IsNormal() const
{ return MatchesMask(FILE_ATTRIBUTE_NORMAL); }
bool CWinFileFind::IsArchived() const
{ return MatchesMask(FILE_ATTRIBUTE_ARCHIVE); }



};//namespace winfunc
