/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSFTPFILE_H__12AF861F_17DB_4F84_8679_7B8EBDCD4D75__INCLUDED_)
#define AFX_FSFTPFILE_H__12AF861F_17DB_4F84_8679_7B8EBDCD4D75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <winsock.h>
#include "fsInternetFile.h"
#include "fsinet.h"	
#include "fsFtpFile_i.h"



class fsFtpFile : public fsFtpFile_i,public fsInternetFile ,CUnknownImp
{
public:
	UNKNOWN_IMP2_(fsInternetFile_i,fsFtpFile_i)
	fsInternetResult Write (LPBYTE pBuffer, DWORD dwToWrite, DWORD *pdwWritten);
	
	
	
	fsInternetResult OpenEx (LPCSTR pszFilePath, UINT64 uStartPos, UINT64 uUploadPartSize = _UI64_MAX, UINT64 uUploadTotalSize = _UI64_MAX);
	fsFtpTransferType GetTransferType();
	void SetDontUseLIST (BOOL b);
	
	virtual fsInternetResult QuerySize (LPCSTR pszFilePath);
	
	virtual void CloseHandle();
	
	fsInternetResult SetTransferType (fsFtpTransferType enType);
	
	fsInternetResult Read (LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead);
	
	fsInternetResult Open (LPCSTR pszFilePath, UINT64 uStartPos);
	fsFtpFile();
	virtual ~fsFtpFile();

protected:
	static BOOL FtpCommand (HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags, LPCSTR pszCommand, DWORD_PTR dwContext, HINTERNET* phFtpCommand);
	BOOL Send_LIST (LPSTR pszCmd, LPCSTR pszFile);
	BOOL m_bDontUseLIST;
	
	BOOL IsDigit (char c);
	
	
	fsInternetResult PASV_ConnectSocket();
	
	BOOL ParseSIZE();
	
	fsInternetResult FtpError ();
	
	
	
	SHORT OpenSocket();

	fsString m_strPORT;	
	SOCKET m_sFile;		
	SOCKET m_sRcv;		
	void ReceiveExtError ();	
	DWORD m_dwTransferType;	
};

#endif 
