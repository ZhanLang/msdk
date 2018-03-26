// filestore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "filestore.h"


extern "C"
{
#include "..\vdlib\vdisk_dll.h"
#include "..\vdlib\vdisk.h"
};


HRESULT CFileStore::OpenStore(LPCWSTR lpszFilePath, ULONGLONG disksize, BOOL bCreateNoExist /*= TRUE*/)
{
	RASSERT(lpszFilePath && wcslen(lpszFilePath), E_INVALIDARG);

	if ( m_hDisk )
	{
		return E_FAIL;
	}

	vdisk_error vd_error = VDISK_ERROR_SUCCESS;
	if ( GetFileAttributes(lpszFilePath) == INVALID_FILE_ATTRIBUTES)
	{
		if ( bCreateNoExist && disksize)
		{
			

			USES_CONVERSION;
			vd_error = vdisk_create(
				W2A(lpszFilePath), 
				disksize, 
				RSDISK_CRYPT_NONE,
				NULL,
				true);

			if ( vd_error == VDISK_ERROR_SUCCESS )
			{
				vd_error = vdisk_open(W2A(lpszFilePath), NULL, &m_hDisk);
				if ( vd_error != VDISK_ERROR_SUCCESS )
				{
					DeleteFile(lpszFilePath);
					return E_FAIL;
				}

				vd_error = vdisk_format(m_hDisk, fmt_type_fat32);
				if ( vd_error != VDISK_ERROR_SUCCESS )
				{
					CloseStore();
					DeleteFile(lpszFilePath);
					return E_FAIL;
				}

				return S_OK;
			}
		}
	}

	if ( vd_error == VDISK_ERROR_SUCCESS)
	{
		USES_CONVERSION;
		vd_error = vdisk_open(W2A(lpszFilePath), NULL, &m_hDisk);
	}

	return vd_error == VDISK_ERROR_SUCCESS ? S_OK : E_FAIL;
}

VOID CFileStore::CloseStore()
{
	if ( m_hDisk )
	{
		vdisk_close(m_hDisk);
		m_hDisk = NULL;
	}
	return;
}

HANDLE CFileStore::OpenFile(LPCWSTR lpszFilePath, FILE_STORE_OPEN_MODE mode)
{
	RASSERT(m_hDisk, INVALID_HANDLE_VALUE);

	unsigned char om = mode == FILE_STORE_OPEN_EXISTING ? VDISK_OPEN_EXISTING : VDISK_CREATE_NEW;
	USES_CONVERSION;
	return (HANDLE)vdisk_fopen(m_hDisk, W2A(lpszFilePath), om);
}

VOID CFileStore::CloseFile(HANDLE hFile)
{
	if (m_hDisk && hFile && hFile != INVALID_HANDLE_VALUE )
	{
		vdisk_fclose((void*)hFile);
	}
	return;
}

BOOL CFileStore::ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	if (m_hDisk && hFile && hFile != INVALID_HANDLE_VALUE && lpBuffer && nNumberOfBytesToRead )
	{
		int nRead = 0;
		nRead = vdisk_fread((void*)hFile, lpBuffer, nNumberOfBytesToRead);
		if ( lpNumberOfBytesRead )
		{
			*lpNumberOfBytesRead = nRead;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CFileStore::WriteFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
{
	if (m_hDisk && hFile && hFile != INVALID_HANDLE_VALUE && lpBuffer && nNumberOfBytesToWrite )
	{
		int nWrite = vdisk_fwrite((void*)hFile, lpBuffer, nNumberOfBytesToWrite);
		if ( lpNumberOfBytesWritten )
		{
			*lpNumberOfBytesWritten = nWrite;
		}

		return TRUE;
	}
	
	return FALSE;
}

DWORD CFileStore::GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
	return 0;
}

HRESULT CFileStore::DeleteFile(LPCWSTR lpszFilePath)
{
	if ( m_hDisk && lpszFilePath && wcslen(lpszFilePath))
	{
		USES_CONVERSION;
		return vdisk_remove(m_hDisk,W2A(lpszFilePath)) == VDISK_ERROR_SUCCESS;
	}
	return E_FAIL;
}

HRESULT CFileStore::MoveFile(LPCWSTR lpszFilePath, LPCWSTR lpszNewFilePath)
{
	if ( m_hDisk && lpszFilePath && wcslen(lpszFilePath) && lpszNewFilePath && wcslen(lpszNewFilePath))
	{
		USES_CONVERSION;
		return vdisk_rename(m_hDisk,W2A(lpszFilePath), W2A(lpszNewFilePath)) == VDISK_ERROR_SUCCESS;
	}
	return E_FAIL;
}

CFileStore::CFileStore() : m_hDisk(NULL)
{

}

CFileStore::~CFileStore()
{
	if ( m_hDisk )
	{
		vdisk_close(m_hDisk);
		m_hDisk = NULL;
	}
}

BOOL CFileStore::CreateDirectory(LPCWSTR lpszDir)
{
	if ( m_hDisk && lpszDir && wcslen(lpszDir))
	{
		USES_CONVERSION;
		return vdisk_mkdir(m_hDisk, W2A(lpszDir)) == VDISK_ERROR_SUCCESS;
	}

	return FALSE;
}
