
#pragma once

#include <winsock.h>
#include "fsInternetFile.h"
#include "fsinet.h"	

enum fsFtpTransferType
{
	FTT_BINARY,		
	FTT_ASCII,		
	FTT_UNKNOWN		
};

class fsFtpFile : public fsInternetFile  
{
public:
	fsInternetResult Write (LPBYTE pBuffer, DWORD dwToWrite, DWORD *pdwWritten);
	fsInternetResult OpenEx (LPCTSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize = _UI64_MAX, UINT64 uUploadTotalSize = _UI64_MAX);
	fsFtpTransferType GetTransferType();
	VOID SetDontUseLIST (BOOL b);
	virtual fsInternetResult QuerySize (LPCTSTR pszFilePath);
	virtual VOID CloseHandle();
	fsInternetResult SetTransferType (fsFtpTransferType enType);
	fsInternetResult Read (LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead);
	fsInternetResult Open (LPCTSTR pszFilePath, UINT64 uStartPos);
	VOID DeleteLatter();
	fsFtpFile();
	virtual ~fsFtpFile();

protected:
	static BOOL FtpCommand (HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags, LPCTSTR pszCommand, DWORD_PTR dwContext, HINTERNET* phFtpCommand);
	BOOL Send_LIST (LPTSTR pszCmd, DWORD dwCount,LPCTSTR pszFile);
	BOOL m_bDontUseLIST;
	BOOL IsDigit (TCHAR c);
	fsInternetResult PASV_ConnectSocket();
	BOOL ParseSIZE();
	fsInternetResult FtpError ();
	SHORT OpenSocket();
	fsString m_strPORT;	
	SOCKET m_sFile;		
	SOCKET m_sRcv;		
	VOID ReceiveExtError ();	
	DWORD m_dwTransferType;	
};

