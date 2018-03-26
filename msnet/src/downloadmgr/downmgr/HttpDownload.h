#pragma once
#include "DwonloadSetting.h"
#include "download/download_i.h"
#include "util/xbuffer.hpp"
#include "SpeedMeter.h"


class CHttpDownload:
	 public IDownLoader
	,public ISpeed
	,public IDownLoadEventNotify
	, public CUnknownImp
{
public:
	CHttpDownload();
	~CHttpDownload(void);
	UNKNOWN_IMP2_(ISpeed,IDownLoader)

	STDMETHOD(Open)(IDownLoadEventNotify * pEvent, 
		LPCWSTR lpstrUrl,
		LPCWSTR lpstrSavePath,
		DWORD dwStartPos= 0,
		DWORD dwEndPos = -1);

	STDMETHOD(Close)();
	STDMETHOD(DeleteConfigFile)();
	STDMETHOD(Abort)();


	STDMETHOD(Download)();
	STDMETHOD_(UINT64,GetFileSize)();
	STDMETHOD_(UINT64,GetDownloadSize)();	

	STDMETHOD_(DownloadType,GetDownloadType)();
	STDMETHOD_(BOOL,IsAbort)();


	STDMETHOD(SetOption)(DWORD option, LPVOID lpData, DWORD dwLen, BOOL bSave = FALSE);
	STDMETHOD(GetOption)(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen);

	//ISpeed
	STDMETHOD_(DWORD,GetSpeed)();

	//IDownLoadEventNotify
	virtual HRESULT OnDownLoadEventNotify(DownloaderEvent dlEvent, WPARAM wp, LPARAM lp);

private:
	fsInternetResult HttpOpenPath(LPCTSTR pszPath, fsHttpConnection *pServer, 
		fsHttpFile *pFile, LPTSTR* ppRedirectedUrl, BOOL *pbRedirInner, UINT64 uPos/* =0*/);
	fsInternetResult HttpOpenUrl(LPCTSTR pszUrl, LPCTSTR pszUser, 
		LPCTSTR pszPassword, fsHttpConnection *pServer, fsHttpFile *pFile, 
		LPTSTR* ppRedirectedUrl, BOOL *pbRedirInner, UINT64 uPos/* =0*/);

};
