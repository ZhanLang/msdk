
#pragma once
#include "filestore/IFileStore.h"


class CFileStore : 
	public IFileStore , 
	public CUnknownImp
{
public:
	CFileStore();
	~CFileStore();

	UNKNOWN_IMP1(IFileStore);
	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter){return S_OK;}

	virtual HRESULT OpenStore(LPCWSTR lpszFilePath, ULONGLONG disksize = 2097152 /*д╛хо1G*/, BOOL bCreateNoExist = TRUE);
	virtual VOID	CloseStore();
	virtual HANDLE  OpenFile(LPCWSTR lpszFilePath, FILE_STORE_OPEN_MODE mode = FILE_STORE_OPEN_EXISTING);
	virtual VOID	CloseFile(HANDLE hFile);
	virtual BOOL	ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	virtual BOOL	WriteFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten);
	virtual DWORD	GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
	virtual HRESULT DeleteFile(LPCWSTR lpszFilePath);
	virtual HRESULT MoveFile(LPCWSTR lpszFilePath, LPCWSTR lpszNewFilePath);
	virtual BOOL   CreateDirectory(LPCWSTR lpszDir);

private:
	void* m_hDisk;
};