/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSFTPFILES_H__9903206C_EB0F_46C8_A778_FF0D15F795F5__INCLUDED_)
#define AFX_FSFTPFILES_H__9903206C_EB0F_46C8_A778_FF0D15F795F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 
#include "fsFtpFile_i.h"
#include "fsFtpConnection.h"	
#include "fsInternetFiles.h"
#include "fsinet.h"	



class fsFtpFiles :  public fsFtpFiles_i, public fsInternetFiles  ,CUnknownImp
{
public:
	UNKNOWN_IMP2_(fsFtpFiles_i,fsInternetFiles_i);

	fsInternetResult GetList (LPCSTR pszUrl);
	void Abort();
	LPCSTR GetLastError();
	

	
	void SetServer (fsFtpConnection_i* pServer);
	
	void Reload (BOOL bReload = TRUE);
	
	
	fsFtpFiles();
	virtual ~fsFtpFiles();

protected:
	UTIL::com_ptr<fsFtpConnection_i> m_pServer;	
	BOOL m_bReload;				
	BOOL m_bAbort;				
	
	fsInternetResult BuildList();
};

#endif 
