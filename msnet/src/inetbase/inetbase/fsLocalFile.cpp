/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsLocalFile.h"
#include "fsURL.h"

fsLocalFile::fsLocalFile()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_enRST = RST_PRESENT;
}

fsLocalFile::~fsLocalFile()
{
	CloseHandle ();
	m_hFile = NULL;
}

namespace inetfile {
bool fsSetFilePointer (HANDLE hFile, __int64 iPosition, DWORD dwMethod)
{
	LARGE_INTEGER li;
	li.QuadPart = iPosition;

	if (INVALID_SET_FILE_POINTER == SetFilePointer (hFile, li.LowPart, &li.HighPart, dwMethod) &&
		NO_ERROR != GetLastError ())
		return false;

	return true;
}
}

fsInternetResult fsLocalFile::Open(LPCSTR pszFilePath, UINT64 uStartPos)
{
	CloseHandle ();

	fsString strPath;
	if (m_strServer != "")
	{
		
		strPath = "\\\\";
		strPath += m_strServer;
	}
	strPath += pszFilePath;

	m_hFile = CreateFile (strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		0, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
		return fsWinErrorToIR ();

	DWORD dw1, dw2;
	dw1 = ::GetFileSize (m_hFile, &dw2);
	m_uFileSize = dw1 | UINT64 (dw2) << 32;

	if (uStartPos)
	{
		if (!inetfile::fsSetFilePointer (m_hFile, (__int64)uStartPos, FILE_BEGIN))
			return IR_RANGESNOTAVAIL;
	}
	
	return IR_SUCCESS;
}

fsInternetResult fsLocalFile::Read(LPBYTE pBuffer, DWORD dwSize, DWORD *pdwRead)
{
	if (FALSE == ReadFile (m_hFile, pBuffer, dwSize, pdwRead, NULL))
		return fsWinErrorToIR ();

	return IR_SUCCESS;
}

void fsLocalFile::Initialize(LPCSTR pszServer)
{
	m_strServer = pszServer;
}

void fsLocalFile::CloseHandle()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle (m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

fsInternetResult fsLocalFile::QuerySize(LPCSTR pszFilePath)
{
	HANDLE hFile = CreateFile (pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return fsWinErrorToIR ();

	DWORD dw1, dw2;
	dw1 = ::GetFileSize (hFile, &dw2);
	m_uFileSize = dw1 | UINT64 (dw2) << 32;

	::CloseHandle (hFile);

	return IR_SUCCESS;
}

fsInternetResult fsLocalFile::OpenEx(LPCSTR pszFilePath, UINT64 uStartPos, UINT64 , UINT64 uUploadTotalSize)
{
	if (uUploadTotalSize != _UI64_MAX)
		return IR_ERROR;
	return Open (pszFilePath, uStartPos);
}

fsInternetResult fsLocalFile::Write(LPBYTE , DWORD , DWORD* )
{
	return IR_ERROR;
}
