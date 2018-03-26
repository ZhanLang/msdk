// filestoretest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <mscom/mscominc.h>
using namespace mscom;

#include <filestore/IFileStore.h>

int _tmain(int argc, _TCHAR* argv[])
{
	CMSComLoader FileStoreLib;
	FileStoreLib.LoadMSCom(_T("filestore.dll"));

	UTIL::com_ptr<IFileStore> pFileStore;
	FileStoreLib.CreateInstance(CLSID_FileStore, NULL, NULL, re_uuidof(IFileStore), (VOID**)&pFileStore);
	RASSERT(pFileStore, E_FAIL);

	pFileStore->OpenStore(_T("d:\\test.ft"));

	HANDLE hFile = pFileStore->OpenFile(L"\\a.txt", FILE_STORE_CREATE_NEW);

	DWORD dwWrite = 0;

	BYTE Buf[MAX_PATH] = { 0 };
	pFileStore->ReadFile(hFile, Buf, MAX_PATH, &dwWrite);
	pFileStore->WriteFile(hFile, "madfefadfeadf", sizeof("madfefadfeadf"), &dwWrite);

	pFileStore->CloseFile(hFile);

	pFileStore->DeleteFile(L"\\a.txt");
	return 0;
}

