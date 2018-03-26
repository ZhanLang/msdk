//////////////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400)
#	define _WIN32_WINNT 0x0400 //
#endif
#if defined(_LIB)
#	include <windows.h>
#else
#	include "StdAfx.h"

#endif


class CMemoryFile
{
protected:
	// structures
	struct MFHANDLE
	{
		HANDLE hHandle;
		LPBYTE pCurPointer;
		MFHANDLE *pNext;
	};
	struct MFFINDHANDLE
	{
		HANDLE hHandle;
		CMemoryFile *pCurMemoryFile;
		MFFINDHANDLE *pNext;
	};
	static MFFINDHANDLE *spFindHandleHead;
	MFHANDLE *mpHandleHead;
	static CMemoryFile *spHead;
	WCHAR* mlpFileName;
	LPBYTE mlpFileBuffer;
	DWORD mdwFileSizeLow;
	DWORD mdwFileSizeHigh;
	DWORD mdwFileAttributes;
	CMemoryFile *mpNext;
protected:
	// static data members
	// tCreateFileW record the Ture entrypoint to CreateFileW
	static HANDLE	(__stdcall *tCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	static HANDLE	(__stdcall *tCreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	static BOOL  (__stdcall *tReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
	static DWORD (__stdcall *tSetFilePointer)( HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
	static DWORD (__stdcall *tGetFileSize)( HANDLE hFile, LPDWORD lpFileSizeHigh);
	static BOOL (__stdcall *tGetFileSizeEx)( HANDLE hFile, PLARGE_INTEGER lpFileSizeHigh);
	static DWORD (__stdcall *tGetFileAttributesW)(LPCWSTR);
	static BOOL (__stdcall *tCloseHandle)(HANDLE hHandle);
	static DWORD (__stdcall *tGetFullPathNameW)( LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR *lpFilePart);
	static DWORD (__stdcall *tGetFullPathNameA)( LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart);
	// static BOOL (__stdcall *tGetFileAttributesExW)( LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation);
	static HANDLE (__stdcall *tFindFirstFileW)( LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
	static HANDLE (__stdcall *tFindFirstFileA)( LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
	static BOOL (__stdcall *tFindNextFileW)( HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
	static BOOL (__stdcall *tFindNextFileA)( HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
	static BOOL (__stdcall *tFindClose)( HANDLE hFindFile);
	static DWORD (WINAPI *tGetFileType)(__in  HANDLE hFile);
	static BOOL (WINAPI *tPathFileExistsA)(LPCSTR lpszFileName);
	static BOOL (WINAPI *tPathFileExistsW)(LPCWSTR lpszFileName);
	// some synchronization objects
	static CRITICAL_SECTION sCSFind;
	static CRITICAL_SECTION sCSFile;
	static CRITICAL_SECTION sCSHandle;
	
	// some addition functions
	static CMemoryFile* IsMemoryFile(HANDLE hHandle, MFHANDLE** ppMfHandle);
	// static method
	static HANDLE __stdcall mfCreateFileW(
		LPCWSTR lpFileName,          // pointer to name of the file
		DWORD dwDesiredAccess,       // access (read-write) mode
		DWORD dwShareMode,           // share mode
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,    // pointer to security attributes
		DWORD dwCreationDisposition,  // how to create
		DWORD dwFlagsAndAttributes,  // file attributes
		HANDLE hTemplateFile         // handle to file with attributes to copy
		);
	
	static HANDLE __stdcall mfCreateFileA(
		LPCSTR lpFileName,          // pointer to name of the file
		DWORD dwDesiredAccess,       // access (read-write) mode
		DWORD dwShareMode,           // share mode
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,    // pointer to security attributes
		DWORD dwCreationDisposition,  // how to create
		DWORD dwFlagsAndAttributes,  // file attributes
		HANDLE hTemplateFile         // handle to file with attributes to copy
		);
	static BOOL __stdcall mfReadFile(
		HANDLE hFile,                // handle of file to read
		LPVOID lpBuffer,             // pointer to buffer that receives data
		DWORD nNumberOfBytesToRead,  // number of bytes to read
		LPDWORD lpNumberOfBytesRead, // pointer to number of bytes read
		LPOVERLAPPED lpOverlapped    // pointer to structure for data
		);
	static DWORD __stdcall mfSetFilePointer(
		HANDLE hFile,          // handle of file
		LONG lDistanceToMove,  // number of bytes to move file pointer
		PLONG lpDistanceToMoveHigh,     // pointer to high-order DWORD of distance to move
		DWORD dwMoveMethod     // how to move
		);
	static DWORD __stdcall mfGetFileSize(
		HANDLE hFile,  // handle of file to get size of
		LPDWORD lpFileSizeHigh     // pointer to high-order word for file size
		);

	static BOOL __stdcall mfGetFileSizeEx( HANDLE hFile, PLARGE_INTEGER lpFileSizeHigh);
	static BOOL __stdcall mfCloseHandle(
		HANDLE hObject   // handle to object to close
		);
	static DWORD __stdcall mfGetFileAttributesW(
		LPCWSTR lpFileName   // handle to object to close
		);
	static DWORD __stdcall mfGetFullPathNameW(
		LPCWSTR lpFileName,  // pointer to name of file to find path for
		DWORD nBufferLength, // size, in characters, of path buffer
		LPWSTR lpBuffer,     // pointer to path buffer
		LPWSTR *lpFilePart   // pointer to filename in path
		);

	static DWORD __stdcall mfGetFullPathNameA(
		LPCSTR lpFileName,  // pointer to name of file to find path for
		DWORD nBufferLength, // size, in characters, of path buffer
		LPSTR lpBuffer,     // pointer to path buffer
		LPSTR *lpFilePart   // pointer to filename in path
		);
	/*      static BOOL __stdcall mfGetFileAttributesExW(
	LPCWSTR lpFileName,        // pointer to string that specifies a file or directory
	GET_FILEEX_INFO_LEVELS fInfoLevelId, // value that specifies the type of attribute information to obtain
	LPVOID lpFileInformation   // pointer to buffer to receive attribute information
	);
	*/
	static HANDLE __stdcall mfFindFirstFileW(
	LPCWSTR lpFileName,  // pointer to name of file to search for
	LPWIN32_FIND_DATAW lpFindFileData // pointer to returned information
	);
	static HANDLE __stdcall mfFindFirstFileA(
	LPCSTR lpFileName,  // pointer to name of file to search for
	LPWIN32_FIND_DATAA lpFindFileData // pointer to returned information
	);
	static BOOL __stdcall mfFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
	static BOOL __stdcall mfFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
	static BOOL __stdcall mfFindClose(HANDLE hFindFile);
	static HMODULE (__stdcall CMemoryFile::mfLoadLibraryW)(LPCWSTR lpFileName);
	static BOOL __stdcall mfCreateProcessW(LPCWSTR pszImageName, LPWSTR pszCmdLine, LPSECURITY_ATTRIBUTES psaProcess, LPSECURITY_ATTRIBUTES psaThread, 
		BOOL fInheritHandles, DWORD fdwCreate, LPVOID pvEnvironment, LPCWSTR pszCurDir, LPSTARTUPINFOW psiStartInfo, LPPROCESS_INFORMATION pProcInfo); 

	static BOOL __stdcall mfCreateProcessA(LPCSTR pszImageName, LPSTR pszCmdLine, LPSECURITY_ATTRIBUTES psaProcess, LPSECURITY_ATTRIBUTES psaThread, 
		BOOL fInheritHandles, DWORD fdwCreate, LPVOID pvEnvironment, LPCSTR pszCurDir, LPSTARTUPINFOA psiStartInfo, LPPROCESS_INFORMATION pProcInfo);
	static BOOL	__stdcall mfDeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
	static DWORD WINAPI mfGetFileType(__in  HANDLE hFile);

	static BOOL WINAPI mfPathFileExistsA(LPCSTR lpszFileName);
	static BOOL WINAPI mftPathFileExistsW(LPCWSTR lpszFileName);
public:
	static BOOL IsFirst22MilesFile(const TCHAR* szFile);
	static CMemoryFile* IsMemoryFile(LPCWSTR lpFileName,BOOL bFind =FALSE);
	static BOOL Init(void);
	static BOOL Release(void);
	static BOOL AttatchToProcess(DWORD dwProcId);
	CMemoryFile( LPCSTR          pFileName, LPVOID lpFileBuffer, int nFileSize, DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY);
	CMemoryFile( LPCWSTR pFileName, LPVOID lpFileBuffer, int nFileSize, DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY);
	virtual ~CMemoryFile(void);
private:
	// prevent the follow operator and function;
	CMemoryFile(CMemoryFile& obj);
	CMemoryFile& operator=(CMemoryFile& obj);
public:
	//       void operator delete(CMemoryFile& obj);
};
