#pragma once

#include <Windows.h>
extern "C"{

typedef  void* LZMA_HANDLE;


LZMA_HANDLE LZMA_OpenFileW(LPCWSTR lpszFile);
LZMA_HANDLE LZMA_OpenFileA(LPCSTR  lpszFile);
LZMA_HANDLE LZMA_OpenMem(LPVOID pMem, DWORD dwLength);

VOID		LZMA_Close(LZMA_HANDLE _handle);

DWORD		LZMA_GetFileCount(LZMA_HANDLE _handle);

/*!
	返回文件名称长度，-1失败
*/
DWORD		LZMA_GetFileNameW(LZMA_HANDLE _handle, DWORD dwIndex, LPWSTR lpszName, DWORD dwLenght);
DWORD		LZMA_GetFileNameA(LZMA_HANDLE _handle, DWORD dwIndex, LPSTR  lpszName, DWORD dwLenght);



/*!
	返回长度，解压缩后的长度，-1失败
*/
DWORD       LZMA_ExtractW(LZMA_HANDLE _handle, DWORD dwIndex, LPCWSTR lpszFile);
DWORD		LZMA_ExtractA(LZMA_HANDLE _handle, DWORD dwIndex, LPCSTR  lpszFile);

};


