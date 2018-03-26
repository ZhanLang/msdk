// 7Zip.cpp : 定义应用程序的入口点。
//

#include "7Zip/7Zip.h"
#include <Windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include <atlbase.h>

extern "C"
{
#include "lzmalib/7z.h"
#include "lzmalib/7zAlloc.h"
#include "lzmalib/7zCrc.h"
#include "lzmalib/7zFile.h"
#include "lzmalib/7zString.h"

};


typedef struct _LZMA_CONTENT_
{
	CSzArEx		ArExDb;
	CLookToRead lookStream;
	ISzAlloc	allocImp;
	CFileInStream   FileStream;
	CStringInStream ArchiveStream;
	BOOL		 isFile;
}LZMA_CONTENT,* LPLZMA_CONTENT;

LZMA_HANDLE LZMA_OpenFileW( LPCWSTR lpszFile )
{
	if(!lpszFile){
		return NULL;
	}

	USES_CONVERSION;
	return LZMA_OpenFileA(W2A(lpszFile));
}

LZMA_HANDLE LZMA_OpenFileA( LPCSTR lpszFile )
{
	if (!lpszFile){
		return NULL;
	}

	LPLZMA_CONTENT pContent = new LZMA_CONTENT();
	memset(pContent, 0, sizeof(LZMA_CONTENT));
	pContent->isFile = TRUE;
	
	ISzAlloc allocTempImp;

	pContent->allocImp.Alloc = SzAlloc;
	pContent->allocImp.Free  = SzFree;

	allocTempImp.Alloc		 = SzAllocTemp;
	allocTempImp.Free		 = SzFreeTemp;

	//pContent->lookStream.s.Seek = 

	BOOL bRes = False;

	if (!InFile_Open(&pContent->FileStream.file, lpszFile))
	{
		
		FileInStream_CreateVTable(&pContent->FileStream);
		LookToRead_CreateVTable(&pContent->lookStream, False);

		pContent->lookStream.realStream = &pContent->FileStream.s;
		LookToRead_Init(&pContent->lookStream);

		CrcGenerateTable();

		SzArEx_Init(&pContent->ArExDb);
		SRes res = SzArEx_Open(&pContent->ArExDb, &pContent->lookStream.s, &pContent->allocImp, &allocTempImp);
		if (res == SZ_OK )
		{
			return pContent;
		}
	}

	if (pContent)
	{
		LZMA_Close(pContent);
	}
	
	return NULL;
}

LZMA_HANDLE LZMA_OpenMem( LPVOID pMem, DWORD dwLength )
{
	if (!(pMem && dwLength)){
		return NULL;
	}

	LPLZMA_CONTENT pContent = new LZMA_CONTENT();
	memset(pContent, 0, sizeof(LZMA_CONTENT));
	pContent->isFile = FALSE;

	ISzAlloc allocTempImp;

	pContent->allocImp.Alloc = SzAlloc;
	pContent->allocImp.Free  = SzFree;

	allocTempImp.Alloc = SzAllocTemp;
	allocTempImp.Free = SzFreeTemp;


	//初始化
	String_Construct(&pContent->ArchiveStream.file);
	String_Open(&pContent->ArchiveStream.file,pMem, (size_t*)&dwLength);

	StringInStream_CreateVTable(&pContent->ArchiveStream);
	LookToRead_CreateVTable(&pContent->lookStream, False);

	pContent->lookStream.realStream = &pContent->ArchiveStream.s;
	LookToRead_Init(&pContent->lookStream);

	CrcGenerateTable();
	BOOL bRes = FALSE;
	SzArEx_Init(&pContent->ArExDb);

	SRes res = SzArEx_Open(&pContent->ArExDb, &pContent->lookStream.s, &pContent->allocImp, &allocTempImp);
	if (res == SZ_OK)
	{
		return pContent;
	}

	if (pContent)
	{
		LZMA_Close(pContent);
	}

	return NULL;
}


VOID LZMA_Close( LZMA_HANDLE _handle )
{
	if (_handle){
		LPLZMA_CONTENT pHandle = static_cast<LPLZMA_CONTENT>(_handle);
		 SzArEx_Free(&pHandle->ArExDb, &pHandle->allocImp);
		 if (!pHandle->isFile)
		 {
			 String_Close(&pHandle->ArchiveStream.file);
		 }
		 else
		 {
			 File_Close(&pHandle->FileStream.file);
		 }
		delete pHandle;
	}
}

DWORD LZMA_GetFileCount( LZMA_HANDLE _handle )
{
	if(!_handle){
		return -1;
	}

	LPLZMA_CONTENT pContent = (LPLZMA_CONTENT)_handle;
	return pContent->ArExDb.db.NumFiles;
}


DWORD LZMA_GetFileNameW( LZMA_HANDLE _handle, DWORD dwIndex, LPWSTR lpszName, DWORD dwLenght )
{
	if (!_handle){
		return -1;
	}

	LPLZMA_CONTENT pContent = (LPLZMA_CONTENT)_handle;
	if (dwIndex > pContent->ArExDb.db.NumFiles)
	{
		return -1;
	}


	DWORD dwSize = SzArEx_GetFileNameUtf16(&pContent->ArExDb, dwIndex, NULL);
	if (!(lpszName && dwLenght))
	{
		return dwSize;
	}


	UInt16* pTemp = (UInt16*)SzAlloc(NULL, dwSize * sizeof(pTemp[0]));
	if (!pTemp)
	{
		return -1;
	}

	SzArEx_GetFileNameUtf16(&pContent->ArExDb, dwIndex, pTemp);
	wcscpy_s(lpszName, dwLenght, (const wchar_t*)pTemp);
	for (int j = 0; lpszName[j] != 0; j++)
	{
		if (lpszName[j] == '/')
		{
			lpszName[j] = 0;
			lpszName[j] = CHAR_PATH_SEPARATOR;
		}
	}

	SzFree(NULL, pTemp);
	return 0;
}

DWORD LZMA_GetFileNameA( LZMA_HANDLE _handle, DWORD dwIndex, LPSTR lpszName, DWORD dwLenght )
{
	WCHAR szName[MAX_PATH * 2] = {0};
	DWORD dwRet = LZMA_GetFileNameW(_handle, dwIndex, szName, MAX_PATH * 2);
	if (dwRet == -1 || !(lpszName && dwLenght) || dwLenght < wcslen(szName))
	{
		return dwRet;
	}

	USES_CONVERSION;
	strcpy_s(lpszName, dwLenght, W2A(szName));
	return 0;
}

DWORD LZMA_ExtractW( LZMA_HANDLE _handle, DWORD dwIndex, LPCWSTR lpszFile )
{
	USES_CONVERSION;
	return LZMA_ExtractA(_handle, dwIndex, W2A(lpszFile));
}

//创建一个完整目录
static BOOL CreateDirectoryEx(const TCHAR* szPath)
{
	if (!(szPath && lstrlen(szPath)))
	{
		return FALSE;
	}

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

DWORD LZMA_ExtractA( LZMA_HANDLE _handle, DWORD dwIndex, LPCSTR lpszFile )
{
	if (!(_handle && lpszFile)){
		return -1;
	}

	LPLZMA_CONTENT pContent = (LPLZMA_CONTENT)_handle;
	if (dwIndex > pContent->ArExDb.db.NumFiles){
		return -1;
	}


	UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
	Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
	size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
	size_t offset = 0;
	size_t outSizeProcessed = 0;
	size_t processedSize;
	ISzAlloc allocTempImp = { 0 };
	allocTempImp.Alloc	  = SzAllocTemp;
	allocTempImp.Free	  = SzFreeTemp;

	USES_CONVERSION;
	CreateDirectoryEx(A2W(lpszFile));

	const CSzFileItem *pFileItem = pContent->ArExDb.db.Files + dwIndex;
	if (pFileItem->IsDir) {
		return 0;
	}

	CSzFile outFile = { 0 };
	if(OutFile_Open(&outFile, lpszFile) != SZ_OK){
		return -1;
	}

	
	
	
	SRes res = SzArEx_Extract(&pContent->ArExDb, &pContent->lookStream.s, dwIndex,
		&blockIndex, &outBuffer, &outBufferSize,
		&offset, &outSizeProcessed,
		&pContent->allocImp, &allocTempImp);

	processedSize = outSizeProcessed;

	if (res != SZ_OK)
	{
		File_Close(&outFile);
		return -1;
	}

	if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
	{
		File_Close(&outFile);
		return -1;
	}

	if (pFileItem->AttribDefined){
		SetFileAttributesA(lpszFile, pFileItem->Attrib);
	}

	if(pFileItem->MTimeDefined){
		//SetFileTime(outFile.handle, pFileItem->MTime.)
	}
	IAlloc_Free(&pContent->allocImp, outBuffer);
	File_Close(&outFile);

	return 0;

}

