// dome.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\include\use_inetfile.h"

LPCTSTR lpUrl = _T("http://qianqian.baidu.com/spec/download703/ttpsetup-95024068.exe");
int _tmain(int argc, _TCHAR* argv[])
{
	fsInternetSession::InitializeWinInet();

	fsInternetSession internetSession;
	fsInternetResult iret = internetSession.Create(_T("magj"), IAT_NOPROXY,NULL);
	
	fsHttpConnection httpConnection;
	httpConnection.Initialize(&internetSession);
	
	fsURL url;
	url.Crack(lpUrl,TRUE);

	iret = httpConnection.Connect(url.GetHostName(),url.GetUserName(),url.GetPassword(),url.GetPort());
	
	fsHttpFile httpFile;
	httpFile.Initialize(&httpConnection);
	httpFile.UseSecure(FALSE);

	httpFile.QuerySize(url.GetPath());
	UINT64 uFileSize = httpFile.GetFileSize();

	iret = httpFile.Open(url.GetPath(),0);

	HANDLE hFile = CreateFile(_T("c:\\EDACoreDll.dll") , GENERIC_READ|GENERIC_WRITE,NULL,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD dwError = GetLastError();
	while (TRUE)
	{
		BYTE bRead[MAX_PATH] = {0};
		DWORD dwRead = 0;
		httpFile.Read(bRead , MAX_PATH , &dwRead);
		if (dwRead == 0)
		{
			break;
		}
		DWORD dwWrite = 0;
		WriteFile(hFile,bRead,dwRead,&dwWrite,NULL);
		DWORD dwError = GetLastError();
		DWORD a = dwError;
	}
	CloseHandle(hFile);

	return 0;
}

