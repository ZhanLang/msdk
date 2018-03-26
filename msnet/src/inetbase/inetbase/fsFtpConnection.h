/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSFTPCONNECTION_H__7A93846D_5A0C_4B2F_B07E_765B21315FB7__INCLUDED_)
#define AFX_FSFTPCONNECTION_H__7A93846D_5A0C_4B2F_B07E_765B21315FB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "fsServerConnection.h"
#include "fsinet.h"	
#include "fsFtpFile_i.h"

class fsFtpConnection : public fsFtpConnection_i,public fsInternetServerConnection  , private CUnknownImp
{
public:
	UNKNOWN_IMP2_(fsInternetServerConnection_i,fsFtpConnection_i);
	fsInternetResult Connect (LPCSTR pszServer, LPCSTR pszUser, LPCSTR pszPassword, INTERNET_PORT nPort = INTERNET_DEFAULT_FTP_PORT);
	
	void UsePassiveMode (BOOL bUse);
	
	BOOL IsPassiveMode();
	
	fsInternetResult SetCurrentDirectory (LPCSTR pszDir);
	fsFtpConnection();
	virtual ~fsFtpConnection();

private:

protected:
	
	
	
	void ReceiveExtError();
	BOOL m_bPassiveMode;	
};

#endif 
