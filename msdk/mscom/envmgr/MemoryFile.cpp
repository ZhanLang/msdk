#include "stdafx.h"
#include "MemoryFile.h"
#include "mhook/mhook.h"
#include "HookContext.h"
#include "Des.h"
#define LogToFile 
// void LogToFile(const TCHAR* szFormat, ...)
// {
// 	extern FILE* pFile;
// 	if (pFile != NULL)
// 	{
// 		TCHAR sz[1024];
// 		va_list vl;
// 		va_start(vl, szFormat);
// 		_vsntprintf(sz, 1024, szFormat, vl);
// 		va_end(vl);
// 		sz[1023] = '\0';
// 		_ftprintf(pFile, _T("%s\n"), sz);
// 		fflush(pFile);
// 	}
// }
HANDLE (__stdcall *CMemoryFile::tCreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = &::CreateFileA;
HANDLE (__stdcall *CMemoryFile::tCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = &::CreateFileW;
BOOL (__stdcall *CMemoryFile::tReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) = &::ReadFile;
DWORD (__stdcall *CMemoryFile::tSetFilePointer)( HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod) = &::SetFilePointer; 
DWORD (__stdcall *CMemoryFile::tGetFileSize)( HANDLE hFile, LPDWORD lpFileSizeHigh) = &::GetFileSize; 
BOOL (__stdcall *CMemoryFile::tGetFileSizeEx)( HANDLE hFile, PLARGE_INTEGER lpFileSizeHigh) = &::GetFileSizeEx;
DWORD (__stdcall *CMemoryFile::tGetFileAttributesW)(LPCWSTR) = &::GetFileAttributesW; 
BOOL (__stdcall *CMemoryFile::tCloseHandle)(HANDLE hHandle) = &::CloseHandle;
DWORD (__stdcall *CMemoryFile::tGetFullPathNameW)( LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR *lpFilePart) = &::GetFullPathNameW;
DWORD (__stdcall *CMemoryFile::tGetFullPathNameA)( LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart) = &::GetFullPathNameA;
//BOOL (__stdcall *CMemoryFile::tGetFileAttributesExW)( LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation) = &::GetFileAttributesExW;
HANDLE (__stdcall *CMemoryFile::tFindFirstFileW)( LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) = ::FindFirstFileW;
HANDLE (__stdcall *CMemoryFile::tFindFirstFileA)( LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) = ::FindFirstFileA;
BOOL (__stdcall *CMemoryFile::tFindNextFileW)( HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) = ::FindNextFileW;
BOOL (__stdcall *CMemoryFile::tFindNextFileA)( HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData) = ::FindNextFileA;
BOOL (__stdcall *CMemoryFile::tFindClose)( HANDLE hFindFile) = ::FindClose;
DWORD (WINAPI *CMemoryFile::tGetFileType)(__in  HANDLE hFile)= ::GetFileType;
BOOL (WINAPI *CMemoryFile::tPathFileExistsA)(LPCSTR lpszFileName) = ::PathFileExistsA;
BOOL (WINAPI *CMemoryFile::tPathFileExistsW)(LPCWSTR lpszFileName) = ::PathFileExists;

CRITICAL_SECTION CMemoryFile::sCSFind;
CRITICAL_SECTION CMemoryFile::sCSFile;
CRITICAL_SECTION CMemoryFile::sCSHandle;
CMemoryFile::MFFINDHANDLE* CMemoryFile::spFindHandleHead = NULL;
CMemoryFile* CMemoryFile::spHead = NULL;

HANDLE __stdcall CMemoryFile::mfCreateFileA(
									  LPCSTR lpFileName,          // pointer to name of the file
									  DWORD dwDesiredAccess,       // access (read-write) mode
									  DWORD dwShareMode,           // share mode
									  LPSECURITY_ATTRIBUTES lpSecurityAttributes,    // pointer to security attributes
									  DWORD dwCreationDisposition,  // how to create
									  DWORD dwFlagsAndAttributes,  // file attributes
									  HANDLE hTemplateFile         // handle to file with attributes to copy
									  )
{
	LogToFile("CreateFileA");
	int nLen = strlen(lpFileName);
	WCHAR* lpwFileName = new WCHAR[nLen + 1];
	memset(lpwFileName, 0, (nLen + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, lpFileName, -1, lpwFileName, nLen);
	HANDLE hRet = mfCreateFileW(lpwFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
			dwFlagsAndAttributes, hTemplateFile);
	delete[] lpwFileName;
	return hRet;
}


HANDLE CMemoryFile::mfCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
								  LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, 
								  DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	LogToFile(_T("CreateFileW"));
	HANDLE hRet = INVALID_HANDLE_VALUE;
	CMemoryFile *pMemFile = NULL;

	WCHAR szPath[MAX_PATH * 4] = { 0 };
	DWORD dLen = 0;
	GetFullPathNameW(lpFileName, MAX_PATH, szPath, NULL);
	lpFileName = szPath;

	if(pMemFile = IsMemoryFile(lpFileName))
	{
		EnterCriticalSection(&sCSHandle);
		MFHANDLE *pHandle = new MFHANDLE;
		pHandle->pCurPointer = pMemFile->mlpFileBuffer;
		pHandle->pNext = pMemFile->mpHandleHead;
		if(pMemFile->mpHandleHead == NULL)
		{
			pHandle->hHandle = (HANDLE)(~size_t(pMemFile->mlpFileBuffer)&(~WORD(0)<<((sizeof(HANDLE) - sizeof(WORD))*8)));
		}
		else
		{
			pHandle->hHandle = (HANDLE)((LPBYTE)pMemFile->mpHandleHead->hHandle + 1);
		}

		LONG lRet = InterlockedExchange(&(LONG&)(pMemFile->mpHandleHead), (LONG&)pHandle);
		hRet = pHandle->hHandle;
		LeaveCriticalSection(&sCSHandle);
	}
	else
	{
		hRet = tCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	LogToFile(_T("->0x%08X"), hRet);
	return hRet;
}

BOOL CMemoryFile::mfReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, \
							 LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	LogToFile("ReadFile");
	BOOL bRet = FALSE;
	MFHANDLE *pMfHandle = NULL;
	CMemoryFile *pMemFile = NULL;
	if(pMemFile = IsMemoryFile(hFile, &pMfHandle))
	{
		LPBYTE lpFileEnd = LPBYTE(pMemFile->mlpFileBuffer + pMemFile->mdwFileSizeLow);
		
		DWORD dwNumOfByteRead = 0;
		if(pMfHandle->pCurPointer >= lpFileEnd)
		{
			*lpNumberOfBytesRead = 0;
		//	return FALSE;
		}
		else if((pMfHandle->pCurPointer + nNumberOfBytesToRead) < lpFileEnd)
		{
			dwNumOfByteRead = nNumberOfBytesToRead;
		}
		else
		{
			dwNumOfByteRead = (DWORD)(lpFileEnd - pMfHandle->pCurPointer);
		}
		memcpy(lpBuffer, pMfHandle->pCurPointer, dwNumOfByteRead);
		pMfHandle->pCurPointer = pMfHandle->pCurPointer + dwNumOfByteRead;
		*lpNumberOfBytesRead = dwNumOfByteRead;
		bRet = TRUE;
	}
	else
	{
		bRet = tReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}
	return bRet;
}
DWORD CMemoryFile::mfGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{       
	LogToFile("GetFileSize");
	DWORD dwRet = 0;
	MFHANDLE *pMfHandle = NULL;
	CMemoryFile *pMemFile = IsMemoryFile(hFile, &pMfHandle);
	if(pMemFile)
	{
		*lpFileSizeHigh = pMemFile->mdwFileSizeHigh;
		dwRet = pMemFile->mdwFileSizeLow;
	}
	else
	{
		dwRet = tGetFileSize(hFile, lpFileSizeHigh);
	}
	return dwRet;
}


BOOL CMemoryFile::mfGetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSizeHigh)
{       
	lpFileSizeHigh->LowPart = GetFileSize(hFile, (DWORD*)&(lpFileSizeHigh->HighPart));
	return GetLastError() == 0;
}


DWORD CMemoryFile::mfSetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
	LogToFile("SetFilePointer");
	DWORD dwRet;
	MFHANDLE *pMfHandle = NULL;
	CMemoryFile *pMemFile;
	if(pMemFile = IsMemoryFile(hFile, &pMfHandle))
	{
		switch(dwMoveMethod)
		{
		case FILE_BEGIN:
			pMfHandle->pCurPointer = pMemFile->mlpFileBuffer + lDistanceToMove;
			break;
		case FILE_CURRENT:
			pMfHandle->pCurPointer = pMfHandle->pCurPointer + lDistanceToMove;
			break;
		case FILE_END:
			pMfHandle->pCurPointer = pMemFile->mlpFileBuffer + pMemFile->mdwFileSizeLow + lDistanceToMove;
			//dwRet = pMemFile->mdwFileSizeLow;
			break;
		}
		if(pMfHandle->pCurPointer <= pMemFile->mlpFileBuffer)
		{
			dwRet = /*lDistanceToMove - */ DWORD(pMemFile->mlpFileBuffer - pMfHandle->pCurPointer);
			//pMfHandle->pCurPointer = pMemFile->mlpFileBuffer;
		}
		if(pMfHandle->pCurPointer <= (pMemFile->mlpFileBuffer + pMemFile->mdwFileSizeLow))
		{
			dwRet = /*lDistanceToMove -*/ DWORD(pMfHandle->pCurPointer - pMemFile->mlpFileBuffer);
			//pMfHandle->pCurPointer = LPBYTE(pMemFile->mlpFileBuffer) + pMemFile->mdwFileSizeLow;
		}
		
	}
	else
	{
		dwRet = tSetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
	}
	return dwRet;
}
DWORD CMemoryFile::mfGetFileAttributesW(LPCWSTR lpFileName)
{
	LogToFile("GetFileAttributesW");
	DWORD dwRet;
	WCHAR szPath[MAX_PATH * 4] = { 0 };
	DWORD dLen = 0;
	GetFullPathNameW(lpFileName, MAX_PATH, szPath, NULL);
	lpFileName = szPath;

	CMemoryFile *pMemFile = NULL;
	if(pMemFile = IsMemoryFile(lpFileName))
	{
		dwRet = pMemFile->mdwFileAttributes;
	}
	else
	{
		dwRet = tGetFileAttributesW(lpFileName);
	}
	return dwRet;
}
BOOL CMemoryFile::mfCloseHandle(HANDLE hObject)
{
	LogToFile("CloseHandle");
	CMemoryFile* pMemFile = NULL;
	MFHANDLE *pPreHandle = NULL;
	MFHANDLE *pHandle = NULL;         
	EnterCriticalSection(&sCSHandle);
	for(pMemFile = spHead; pMemFile != NULL; pMemFile = pMemFile->mpNext)
	{
		for(pHandle = pMemFile->mpHandleHead; pHandle != NULL; pPreHandle = pHandle, pHandle = pHandle->pNext)
		{
			if(pHandle->hHandle == hObject)
			{
				if(pPreHandle)
				{
					//pPreHandle->pNext = pMemFile->mpNext;
					::InterlockedExchange(&(LONG&)pPreHandle->pNext, (LONG&)pMemFile->mpNext);
				}
				else
				{
					//pMemFile->mpHandleHead = NULL;
					::InterlockedExchange(&(LONG&)pMemFile->mpHandleHead, NULL);
				}
				delete pHandle;
				LeaveCriticalSection(&sCSHandle);
				return TRUE;
			}
		}
	}
	LeaveCriticalSection(&sCSHandle);
	return tCloseHandle(hObject);
}

DWORD CMemoryFile::mfGetFullPathNameW(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR *lpFilePart)
{
	LogToFile("GetFullPathNameW");
	DWORD dwRet = 0;
	CMemoryFile *pMemFile = IsMemoryFile(lpFileName);
	if(pMemFile)
	{
		int nStrLen = lstrlenW(lpFileName);
		LPWSTR pStr;
		if(nBufferLength < (nStrLen + 1) * sizeof(WCHAR))
			return 0;
		lstrcpyW((LPWSTR)lpBuffer, lpFileName);
		for(pStr = lpBuffer + nStrLen; (*pStr != '\\') && (pStr >= lpBuffer); pStr--);
		if (lpFilePart != NULL)
			*lpFilePart = ++pStr;
		dwRet = nStrLen;
	}
	else
	{
		dwRet = tGetFullPathNameW(lpFileName, nBufferLength, lpBuffer, lpFilePart);
	}
	return dwRet;

}


DWORD CMemoryFile::mfGetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart)
{
	LogToFile("GetFullPathNameA");
	int nLen = strlen(lpFileName);
	WCHAR* lpwFileName = new WCHAR[nLen + 1];
	memset(lpwFileName, 0, (nLen + 1) * sizeof(WCHAR));

	MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, lpFileName, -1, lpwFileName, nLen);

	DWORD dwRet;
	CMemoryFile *pMemFile = IsMemoryFile(lpwFileName);
	if(pMemFile)
	{
		int nStrLen = lstrlenW(lpwFileName);
		LPSTR pStr;
		if(nBufferLength < (nStrLen + 1) * sizeof(CHAR))
			return 0;

		BOOL bUseDef = FALSE;
		CHAR* szFileName = new CHAR[nStrLen + 1];
		WideCharToMultiByte(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, lpwFileName, -1, szFileName, nLen, "", &bUseDef);
		lstrcpyA((LPSTR)lpBuffer, szFileName);
		for(pStr = lpBuffer + nStrLen; (*pStr != '\\') && (pStr >= lpBuffer); pStr--);
		*lpFilePart = ++pStr;
		dwRet = nStrLen;
	}
	else
	{
		dwRet = tGetFullPathNameA(lpFileName, nBufferLength, lpBuffer, lpFilePart);
	}
	return dwRet;
}
//BOOL CMemoryFile::mfGetFileAttributesExW(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
//{
//       //CMemoryFile *pMemoryFile = IsMemoryFile(lpFileName);
//       //if(pMemoryFile)
//       //{
//       //       return 0x21;
//       //       //return pMemoryFile->mdwFileAttributes;
//       //}
//       BOOL bRet = tGetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
//       return bRet;
//}
HANDLE CMemoryFile::mfFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
	LogToFile(_T("FindFileW "));
	HANDLE hRet = NULL;
	CMemoryFile *pMemFile = NULL;
	if(pMemFile = IsMemoryFile(lpFileName, TRUE))
	{
		MFFINDHANDLE *pFindHandle = new MFFINDHANDLE;
		EnterCriticalSection(&sCSFind);
		//pFindHandle->hHandle = pMemFile->mlpFileBuffer;
		pFindHandle->pCurMemoryFile = pMemFile;
		pFindHandle->pNext = spFindHandleHead;
		if(spFindHandleHead == NULL)
		{
			pFindHandle->hHandle = (HANDLE)(~size_t(&spFindHandleHead)&(~WORD(0)<<((sizeof(HANDLE) - sizeof(WORD))*8)));//用高位作为内存文件间的标志，而低位用来标识不同的句柄
		}
		else
		{
			pFindHandle->hHandle = (HANDLE)(PBYTE(spFindHandleHead->hHandle) + 1);
		}
		LONG lRet = InterlockedExchange(&(LONG&)spFindHandleHead, (LONG&)pFindHandle);
		hRet = pFindHandle->hHandle;

		CString strFileName = pMemFile->mlpFileName;
		strFileName = strFileName.Mid(strFileName.ReverseFind('\\') + 1);

		lstrcpyW(lpFindFileData->cFileName,strFileName);
		//lstrcpyW(lpFindFileData->cAlternateFileName, L".SWF");
		lpFindFileData->nFileSizeHigh = 0;
		lpFindFileData->nFileSizeLow = pMemFile->mdwFileSizeLow;
		lpFindFileData->dwFileAttributes = pMemFile->mdwFileAttributes;

		LeaveCriticalSection(&sCSFind);
	}
	else
	{
		hRet = tFindFirstFileW(lpFileName, lpFindFileData);
	}
	return hRet;
}

HANDLE CMemoryFile::mfFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
	LogToFile("FindFirstFileA");
	int nLen = strlen(lpFileName);
	WCHAR* lpwFileName = new WCHAR[nLen + 1];

	memset(lpwFileName, 0, (nLen + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, lpFileName, -1, lpwFileName, nLen);

	HANDLE hRet = NULL;
	CMemoryFile *pMemFile = NULL;
	CHAR szPath[MAX_PATH * 4] = { 0 };
	DWORD dLen = 0;
	GetFullPathNameA(lpFileName, MAX_PATH, szPath, NULL);
	lpFileName = szPath;

	if(pMemFile = IsMemoryFile(lpwFileName))
	{
		MFFINDHANDLE *pFindHandle = new MFFINDHANDLE;
		EnterCriticalSection(&sCSFind);
		//pFindHandle->hHandle = pMemFile->mlpFileBuffer;
		pFindHandle->pCurMemoryFile = pMemFile;
		pFindHandle->pNext = spFindHandleHead;
		if(spFindHandleHead == NULL)
		{
			pFindHandle->hHandle = (HANDLE)(~size_t(&spFindHandleHead)&(~WORD(0)<<((sizeof(HANDLE) - sizeof(WORD))*8)));//用高位作为内存文件间的标志，而低位用来标识不同的句柄
		}
		else
		{
			pFindHandle->hHandle = (HANDLE)(PBYTE(spFindHandleHead->hHandle) + 1);
		}

		CStringA strFileName = lpFileName;
		strFileName = strFileName.Mid(strFileName.ReverseFind('\\') + 1);

		strcpy(lpFindFileData->cFileName, strFileName);
		//strcpy(lpFindFileData->cAlternateFileName, ".SWF");
		lpFindFileData->nFileSizeHigh = 0;
		lpFindFileData->nFileSizeLow = pMemFile->mdwFileSizeLow;
		lpFindFileData->dwFileAttributes = pMemFile->mdwFileAttributes;

		

		LONG lRet = InterlockedExchange(&(LONG&)spFindHandleHead, (LONG&)pFindHandle);
		hRet = pFindHandle->hHandle;
		LeaveCriticalSection(&sCSFind);
	}
	else
	{
		hRet = tFindFirstFileA(lpFileName, lpFindFileData);
	}
	delete[] lpwFileName;
	return hRet;
}

BOOL CMemoryFile::mfFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
{
	LogToFile("FindNextFileW");
	BOOL bRet = FALSE;
	CMemoryFile* pMemFile = NULL;
	MFFINDHANDLE *pHandle = NULL;          
	EnterCriticalSection(&sCSFind);
	for(pMemFile = spHead; pMemFile != NULL; pMemFile = pMemFile->mpNext)
	{
		for(pHandle = spFindHandleHead; pHandle != NULL; pHandle = pHandle->pNext)
		{
			if(pHandle->hHandle == hFindFile)
			{
				CString strFileName = pMemFile->mlpFileName;
				strFileName = strFileName.Mid(strFileName.ReverseFind('\\') + 1);
				lstrcpyW(lpFindFileData->cFileName, strFileName);
				lpFindFileData->nFileSizeHigh = 0;
				lpFindFileData->nFileSizeLow = pMemFile->mdwFileSizeLow;
				LeaveCriticalSection(&sCSFind);
				return TRUE;
			}
		}
	}
	LeaveCriticalSection(&sCSFind);
	bRet = tFindNextFileW(hFindFile, lpFindFileData);
	return bRet;
}


BOOL CMemoryFile::mfFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
	LogToFile("FindNextFileA");
	BOOL bRet = FALSE;
	CMemoryFile* pMemFile = NULL;
	MFFINDHANDLE *pHandle = NULL;          
	EnterCriticalSection(&sCSFind);
	for(pMemFile = spHead; pMemFile != NULL; pMemFile = pMemFile->mpNext)
	{
		for(pHandle = spFindHandleHead; pHandle != NULL; pHandle = pHandle->pNext)
		{
			if(pHandle->hHandle == hFindFile)
			{
				int nLen = wcslen(pMemFile->mlpFileName);
				CHAR* szFileName = new CHAR[nLen + 1];
				memset(szFileName, 0, (nLen + 1) * sizeof(CHAR));

				BOOL bUseDefChar = FALSE;
				WideCharToMultiByte(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, pMemFile->mlpFileName, -1, szFileName, nLen, "", &bUseDefChar);

				CStringA strFileName = szFileName;
				strFileName = strFileName.Mid(strFileName.ReverseFind('\\') + 1);

				lstrcpyA(lpFindFileData->cFileName, strFileName);
				lpFindFileData->nFileSizeHigh = 0;
				lpFindFileData->nFileSizeLow = pMemFile->mdwFileSizeLow;
				LeaveCriticalSection(&sCSFind);
				return TRUE;
			}
		}
	}
	LeaveCriticalSection(&sCSFind);
	bRet = tFindNextFileA(hFindFile, lpFindFileData);
	return bRet;
}
BOOL CMemoryFile::mfFindClose(HANDLE hFindFile)
{
	LogToFile("FindClose");
	BOOL bRet = FALSE;
	CMemoryFile* pMemFile = NULL;
	MFFINDHANDLE *pPreHandle = NULL;
	MFFINDHANDLE *pHandle = NULL;          
	EnterCriticalSection(&sCSFind);
	for(pMemFile = spHead; pMemFile != NULL; pMemFile = pMemFile->mpNext)
	{
		for(pHandle = spFindHandleHead; pHandle != NULL; pPreHandle = pHandle, pHandle = pHandle->pNext)
		{
			if(pHandle->hHandle == hFindFile)
			{
				if(pPreHandle)
				{
					pPreHandle->pNext = pHandle->pNext;
					::InterlockedExchange(&(LONG&)pPreHandle->pNext, (LONG&)pMemFile->mpNext);
				}
				else
				{
					spFindHandleHead = NULL;
					::InterlockedExchange(&(LONG&)spFindHandleHead,NULL);
				}
				delete pHandle;
				LeaveCriticalSection(&sCSFind);
				return TRUE;
			}
		}
	}
	LeaveCriticalSection(&sCSFind);
	bRet = tFindClose(hFindFile);
	return bRet;
}

CMemoryFile::CMemoryFile(  LPCSTR pFileName, LPVOID lpFileBuffer, int nFileSize, DWORD dwFileAttributes)
{
	if( (lstrlenA(pFileName) == 0) || (lpFileBuffer == NULL ))
		throw -1;
	if(::IsBadReadPtr(lpFileBuffer, nFileSize))
	{
		throw -2;
	}
	int nRet = 0;
	LONG lRet = 0;
	int nLen = (int)(lstrlenA(pFileName));
	LPWSTR lpwFileName = new WCHAR[2 * nLen + 1];
	memset(lpwFileName, 0, (nLen * 2 + 1) * sizeof(WCHAR));
	nRet = MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, pFileName, -1, lpwFileName, nLen * 2);
	if(!nRet)
	{
		delete[] lpwFileName;
		DWORD dw = GetLastError();
		throw GetLastError();
	}
	if(IsMemoryFile(lpwFileName))
	{
		delete[] lpwFileName;
		throw -2;
	}
	mdwFileAttributes = dwFileAttributes;
	mdwFileSizeHigh = 0;
	mdwFileSizeLow = nFileSize;
	mlpFileBuffer = (LPBYTE)lpFileBuffer;
	mlpFileName = lpwFileName;
	mpHandleHead = NULL;
	CMemoryFile *tp = this;
	if(TryEnterCriticalSection(&sCSFile))
	{
		mpNext = spHead;
		spHead = this;
		LeaveCriticalSection(&sCSFile);
	}
	else
	{
		if(spHead == NULL)
		{
			mpNext = spHead;
			lRet = InterlockedExchange(&(LONG&)spHead, (LONG&)tp);
		}
		else
		{
			throw -1;
		}
	}
}
CMemoryFile::CMemoryFile( LPCWSTR pFileName, LPVOID lpFileBuffer, int nFileSize, DWORD dwFileAttributes)
{   
	if( (lstrlenW(pFileName) == 0) || (lpFileBuffer == NULL ))
	{
		throw -1;
	}
	if(::IsBadReadPtr(lpFileBuffer, nFileSize))
	{
		throw -2;
	}
	if(IsMemoryFile(pFileName))
	{
		throw -2;
	}
	LONG lRet = 0;
	mdwFileAttributes = dwFileAttributes;
	mdwFileSizeHigh = 0;
	mdwFileSizeLow = nFileSize;
	mlpFileBuffer = (LPBYTE)lpFileBuffer;
	mlpFileName = new WCHAR[lstrlenW(pFileName) + 1];
	lstrcpyW(mlpFileName, pFileName);
	mpHandleHead = NULL;
	CMemoryFile *tp = this;
	if(TryEnterCriticalSection(&sCSFile))
	{
		mpNext = spHead;
		lRet = InterlockedExchange(&(LONG&)spHead, (LONG&)tp);
		LeaveCriticalSection(&sCSFile);
	}
	else
	{
		if(spHead == NULL)
		{
			mpNext = spHead;
			lRet = InterlockedExchange(&(LONG&)spHead, (LONG&)tp);
		}
		else
		{
			throw -1;
		}
	}
}
CMemoryFile::~CMemoryFile(void)
{
	EnterCriticalSection(&sCSHandle);
	if(mpHandleHead == NULL)
	{
		delete[] mlpFileName;
		mlpFileName = NULL;
	}
	
	SAFE_DELETE_BUFFER(mlpFileBuffer);
	

	LeaveCriticalSection(&sCSHandle);
	CMemoryFile *pMemFile, *pPreMemFile;
	EnterCriticalSection(&sCSFile);
	for(pPreMemFile = NULL, pMemFile = spHead; pMemFile != NULL; pPreMemFile = pMemFile, pMemFile = pMemFile->mpNext)
	{
		if(pPreMemFile != NULL)
		{
			//pPreMemFile->pNext = pMemFile->pNext;
			::InterlockedExchange(&(LONG&)pPreMemFile->mpNext, (LONG&)pMemFile->mpNext);
		}
		else
		{
			//spHead = NULL;
			::InterlockedExchange(&(LONG&)spHead, (LONG&)pMemFile->mpNext);
		}
	}
	LeaveCriticalSection(&sCSFile);
}
CMemoryFile* CMemoryFile::IsMemoryFile(HANDLE hHandle, MFHANDLE** ppMfHandle)
{
	CMemoryFile* pMemFile = NULL;
	MFHANDLE *pHandle = NULL;
	for(pMemFile = spHead; pMemFile != NULL; pMemFile = pMemFile->mpNext)
	{
		for(pHandle = pMemFile->mpHandleHead; pHandle != NULL; pHandle = pHandle->pNext)
		{
			if(pHandle->hHandle == hHandle)
			{
				*ppMfHandle = pHandle;
				return pMemFile;
			}
		}
	}
	ppMfHandle = NULL;
	return NULL;
}
CMemoryFile* CMemoryFile::IsMemoryFile(LPCWSTR lpFileName,BOOL bFind)
{
	if (spHead == NULL)
		return NULL;

	WCHAR wszFileName[1024];
	WCHAR* wszTemp = wszFileName;
	lstrcpyW(wszFileName, lpFileName);
	if (wcsncmp(wszFileName, L"\\\\?\\", 4) == 0)
	{
		wszTemp = wszFileName + 4;
	}


	CMemoryFile* p = spHead; 
	do 
	{
		if (bFind && !PathFileExists(lpFileName))
		{
			//首先判断路径
			CString strPath = lpFileName;
			strPath = strPath.Left(strPath.Find('*'));

			CString strExt = lpFileName;
			int n = strExt.ReverseFind('.');
			strExt = strExt.Mid(strExt.ReverseFind('.') + 1);
			
			CString strMemExt = p->mlpFileName;
			strMemExt = strMemExt.Mid(strMemExt.ReverseFind('.') + 1);

			if (wcsnicmp(p->mlpFileName , strPath.GetBuffer(), strPath.GetLength()) == 0 &&
				strMemExt.CompareNoCase(strExt) == 0)
			{
				return p;
			}
			
		}
		if (wcsncmp(p->mlpFileName, wszTemp, wcslen(p->mlpFileName)) == 0)
			return p;
		p = p->mpNext;
	} while (p != NULL);
	
	return NULL;
}
//CMemoryFile::CMemoryFile(CMemoryFile& obj)
//{
//       int nLen = lstrlenW(obj.lpFileName) + 2;
//       lpFileName = new WCHAR[nLen];
//       lstrcpyW(lpFileName, obj.lpFileName);
//
//       lpFileBuffer = obj.lpFileBuffer;
////     lpCurPoint = lpFileBuffer;
//       dwFileSizeLow = obj.dwFileSizeLow;
//       dwFileSizeHigh = obj.dwFileSizeHigh;
//       dwFileAttributes = obj.dwFileAttributes;
////     hHandle = obj.hHandle;
////     lpCurPoint = lpFileBuffer;
//       pNext = NULL;
//}

static VOID GrpDbgOutput( TCHAR const *pGroupName, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX] = {0};

	DWORD dwCurrentProcessID = GetCurrentProcessId();
	DWORD dwCurrentThreadID  = GetCurrentThreadId();

	SYSTEMTIME systemTime = { 0 };
	GetLocalTime(&systemTime);

	//int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[%-8s]:"), pGroupName);

	int len = _stprintf_s(pTempBuffer,
		DEBUG_MESSAGE_BUFLEN_MAX ,
		_T("[%4d]%4d-%02d-%02d %02d:%02d:%02d:%03d: [%4d][%s][%s]:"),
		dwCurrentProcessID,
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute,
		systemTime.wSecond,
		systemTime.wMilliseconds,
		dwCurrentThreadID,
		_T("DBG"),
		pGroupName);

	va_list args;
	va_start( args, pMsg );
	len += _vstprintf_s ( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );
	len += _stprintf_s ( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, _T("\r\n"));
	::OutputDebugString(pTempBuffer);

	va_end( args );
}

BOOL CMemoryFile::Release(void)
{
	if(spHead != NULL)
		return FALSE;

	//打印日志
	for (CMemoryFile* pHead = spHead ; pHead ; pHead = pHead->mpNext)
	{
		GrpDbgOutput(_T("MemoryFile"), _T("the file %s not close."), pHead->mlpFileName);
	}

	LONG bRet = TRUE;

	bRet |= Mhook_Unhook(&(PVOID&)tFindClose);
	bRet |= Mhook_Unhook(&(PVOID&)tFindNextFileW);
	bRet |= Mhook_Unhook(&(PVOID&)tFindNextFileA);
	bRet |= Mhook_Unhook(&(PVOID&)tFindFirstFileW);
	bRet |= Mhook_Unhook(&(PVOID&)tFindFirstFileA);
	bRet |= Mhook_Unhook(&(PVOID&)tGetFullPathNameW);
	bRet |= Mhook_Unhook(&(PVOID&)tGetFullPathNameA);
	bRet |= Mhook_Unhook(&(PVOID&)tCloseHandle);
	bRet |= Mhook_Unhook(&(PVOID&)tGetFileAttributesW);
	bRet |= Mhook_Unhook(&(PVOID&)tGetFileSize);
	bRet |= Mhook_Unhook(&(PVOID&)tSetFilePointer);
	bRet |= Mhook_Unhook(&(PVOID&)tReadFile);
	bRet |= Mhook_Unhook(&(PVOID&)tCreateFileW);
	bRet |= Mhook_Unhook(&(PVOID&)tCreateFileA);
	bRet |= Mhook_Unhook(&(PVOID&)tGetFileType);
	bRet |= Mhook_Unhook(&(PVOID&)tPathFileExistsA);
	bRet |= Mhook_Unhook(&(PVOID&)tPathFileExistsW);

	DeleteCriticalSection(&sCSHandle);
	DeleteCriticalSection(&sCSFile);
	DeleteCriticalSection(&sCSFind);
	
	
	return bRet;
}

BOOL CMemoryFile::Init(void)
{
	BOOL bRet = TRUE;
	InitializeCriticalSection(&sCSFind);
	InitializeCriticalSection(&sCSFile);
	InitializeCriticalSection(&sCSHandle);

	
	bRet |= Mhook_SetHook(&(PVOID&)tCreateFileW,				mfCreateFileW);
	bRet |= Mhook_SetHook(&(PVOID&)tCreateFileA,				mfCreateFileA);
	bRet |= Mhook_SetHook(&(PVOID&)tReadFile,				mfReadFile);
	bRet |= Mhook_SetHook(&(PVOID&)tSetFilePointer,			mfSetFilePointer);
	bRet |= Mhook_SetHook(&(PVOID&)tGetFileSize,				mfGetFileSize);
	bRet |= Mhook_SetHook(&(PVOID&)tGetFileAttributesW,		mfGetFileAttributesW);
	bRet |= Mhook_SetHook(&(PVOID&)tCloseHandle,				mfCloseHandle);
	bRet |= Mhook_SetHook(&(PVOID&)tGetFullPathNameW,		mfGetFullPathNameW);
	bRet |= Mhook_SetHook(&(PVOID&)tGetFullPathNameA,		mfGetFullPathNameA);
	bRet |= Mhook_SetHook(&(PVOID&)tFindFirstFileW,			mfFindFirstFileW);
	bRet |= Mhook_SetHook(&(PVOID&)tFindFirstFileA,			mfFindFirstFileA);
	bRet |= Mhook_SetHook(&(PVOID&)tFindNextFileW,			mfFindNextFileW);
	bRet |= Mhook_SetHook(&(PVOID&)tFindNextFileA,			mfFindNextFileA);
	bRet |= Mhook_SetHook(&(PVOID&)tFindClose,				mfFindClose);
	bRet |= Mhook_SetHook(&(PVOID&)tGetFileType,			mfGetFileType);
	bRet |= Mhook_SetHook(&(PVOID&)tPathFileExistsA,		mfPathFileExistsA);
	bRet |= Mhook_SetHook(&(PVOID&)tPathFileExistsW,		mftPathFileExistsW);
	return bRet;
}


BOOL CMemoryFile::IsFirst22MilesFile(const TCHAR* szFile)
{
	FILE* pFile = NULL;
	_tfopen_s(&pFile, szFile, _T("rb"));
	if (pFile != NULL)
	{
		fseek(pFile, -HASH_CODE_LEN-3, SEEK_END);
		char szSep[4];
		fread(szSep, sizeof(char), 4,pFile);
		fclose(pFile);
		szSep[3] = '\0';
		if (strncmp(szSep, SEPARATOR,3) == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

DWORD CMemoryFile::mfGetFileType(__in HANDLE hFile)
{
	MFHANDLE *pMfHandle = NULL;
	CMemoryFile *pMemFile = NULL;
	if(pMemFile = IsMemoryFile(hFile, &pMfHandle))
	{
		return FILE_TYPE_CHAR;
	}

	return tGetFileType(hFile);
}

BOOL WINAPI CMemoryFile::mfPathFileExistsA( LPCSTR lpszFileName )
{
	USES_CONVERSION;
	return mftPathFileExistsW(A2W(lpszFileName));
}

BOOL WINAPI CMemoryFile::mftPathFileExistsW( LPCWSTR lpszFileName )
{
	WCHAR szPath[MAX_PATH * 4] = { 0 };
	DWORD dLen = 0;
	GetFullPathNameW(lpszFileName, MAX_PATH, szPath, NULL);

	CMemoryFile* pMemFile = IsMemoryFile(szPath);
	if (pMemFile)
	{
		return TRUE;
	}
	return tPathFileExistsW(lpszFileName);
}
