#pragma once
#include "download/download_i.h"
#include "use_inetfile.h"
#include "util/xbuffer.hpp"
class CFtpDownload:
	public IDownLoader
{
public:
	CFtpDownload(void);
	~CFtpDownload(void);

	HRESULT Open(IDownloadTask*pTask,IDownLoadEventNotify* pEvent , LPCTSTR lpstrUrl,LPCTSTR lpstrSavePath);
	HRESULT Close();
	HRESULT DeleteConfigFile();
	HRESULT Abort();
	UINT64  GetFileSize();
	VOID    DeleteThis();
	DLError GetLastError();
	HRESULT Download();
	BOOL    IsAbort();

private:
	HRESULT Init();
	HRESULT WriteFile(BOOL bPush = FALSE);
private:
	IDownLoadEventNotify* m_pNotify;
	CString m_strUrl;
	CString m_strSavePath;
	BOOL    m_bAbort;
	UINT64	m_uFileSize;
	UINT64	m_uDlSize;
	UINT64  m_uPos;
	HANDLE m_hSaveFile ;
	fsInternetSession*   m_pInternetSession;
	fsFtpConnection  *   m_pFtpConnection;
	fsFtpFile		 *   m_pFtpFile;
	IDwonloadSetting *	 m_pSetting;
	IDownloadTask    *   m_pTask;

	DECLARE_AUTOLOCK_CS(TempBuf);
	XBuffer m_TempBuf;
};
