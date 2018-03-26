#pragma once
namespace msdk{;
namespace mscom{;

enum FILE_STORE_OPEN_MODE
{
	FILE_STORE_OPEN_EXISTING,
	FILE_STORE_CREATE_NEW,
};

struct IFileStore : public IMSBase
{
	//对仓库操作
	virtual HRESULT	OpenStore(LPCWSTR lpszFilePath, ULONGLONG disksize = 2097152 /*默认1G*/, BOOL bCreateNoExist = TRUE) = 0;
	virtual VOID	CloseStore() = 0;


	//对仓库内的文件进行操作
	virtual HANDLE  OpenFile(LPCWSTR lpszFilePath, FILE_STORE_OPEN_MODE mode = FILE_STORE_OPEN_EXISTING) = 0;
	virtual VOID	CloseFile(HANDLE hFile) = 0;

	virtual BOOL	ReadFile(HANDLE hFile,  LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead) = 0;
	virtual BOOL	WriteFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten) = 0;

	virtual DWORD	GetFileSize( HANDLE hFile,  LPDWORD lpFileSizeHigh) = 0; 

	virtual HRESULT DeleteFile(LPCWSTR lpszFilePath) = 0;
	virtual HRESULT MoveFile(LPCWSTR lpszFilePath, LPCWSTR lpszNewFilePath) = 0;

	virtual BOOL   CreateDirectory(LPCWSTR lpszDir) = 0;
};


MS_DEFINE_IID(IFileStore, "{84664822-DED0-4B8A-920E-E88CAE8CA134}");

// {134D8209-4773-4953-933C-3D2F85375ACD}
MS_DEFINE_GUID(CLSID_FileStore, 
	0x134d8209, 0x4773, 0x4953, 0x93, 0x3c, 0x3d, 0x2f, 0x85, 0x37, 0x5a, 0xcd);


};};


