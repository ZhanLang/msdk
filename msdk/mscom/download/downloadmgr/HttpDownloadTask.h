#pragma once
#include "download/IDownload.h"
#include "IDownloadTaskEx.h"
#include "SpeedMeter.h"
#include "xbuffer.hpp"
#include "SyncObject/criticalsection.h"

#define Max_TempBufSize (1024*1024)
class CHttpDownloadTask:
	public IDownloadTask,
	public IDownloadTaskEx,
	public CMsComBase<CHttpDownloadTask>

{
public:
	UNKNOWN_IMP2_(IDownloadTask, IDownloadTaskEx);
	CHttpDownloadTask(void);
	~CHttpDownloadTask(void);

protected:
	STDMETHOD_(LPCWSTR		, GetUrl)();
	STDMETHOD_(LPCWSTR		, GetSavePath)();
	STDMETHOD_(DL_TaskType	, GetTaskType)();
	STDMETHOD_(LONGLONG		, GetTotalFileSize)();
	STDMETHOD_(LONGLONG		, GetCurrentFileSize)();
	STDMETHOD_(LPCWSTR		, GetProxyService)();
	STDMETHOD_(LPCWSTR		, GetProxyUser)();
	STDMETHOD_(LPCWSTR		, GetProxyPwd)();
	STDMETHOD_(DWORD		, GetTaskId)();
	STDMETHOD_(DL_Error		, GetTaskError)();
	STDMETHOD_(DL_TaskState , GetTaskState)();
	STDMETHOD_(FILETIME		, GetCreateTime)();
	STDMETHOD_(DWORD, GetSpeed)();
	STDMETHOD_(DWORD, GetSpeed2)();
protected:
	STDMETHOD(SetCookie)(LPCWSTR lpszCookie);
	STDMETHOD(SetProxyService)(LPCWSTR lpszService);
	STDMETHOD(SetProxyUser)(LPCWSTR lpszUsr);
	STDMETHOD(SetProxyPwd)(LPCWSTR lpszPwd);

protected:
	STDMETHOD(SetCreateTime)(const FILETIME& time);
	STDMETHOD(SetTaskState)(DL_TaskState state);
	STDMETHOD(SetTaskError)(DL_Error error);

	STDMETHOD(set_AddToStore)(BOOL bAdd);
	STDMETHOD_(BOOL, get_AddToStore)();

	STDMETHOD(SetTotalSize)(LONGLONG llTotalSize);
	STDMETHOD(SetCurSize)(LONGLONG llCurSize);
	STDMETHOD_(DWORD, GetEngine)();
protected:
	STDMETHOD(CreateTask)(LPCWSTR lpszUrl, LPCWSTR lpszSavePath, IDownloadNotify* pNotify, DWORD dwTaskId);
	STDMETHOD(DestroyTask)();

	//¿ªÊ¼ÏÂÔØ
	STDMETHOD(Start)();
	STDMETHOD(Stop)();



private:
	static DWORD WINAPI _DownloadThead(LPVOID lpThread);
	DWORD DownloadThead();

private:
	BOOL CreateInternetSession();
	BOOL ConnectToService();
	BOOL DwonloadToFile();
	BOOL CreateCurrentFile();
	BOOL WriteFile(BOOL bPush = FALSE);

	VOID	SendNotify(DL_NotifyEvnet event);

	VOID CloseDownloadSession();
private:
	fsInternetResult HttpOpenPath(LPCTSTR pszPath, fsHttpConnection *pServer, 
		fsHttpFile *pFile, LPTSTR* ppRedirectedUrl, BOOL *pbRedirInner, UINT64 uPos/* =0*/);
	fsInternetResult HttpOpenUrl(LPCTSTR pszUrl, LPCTSTR pszUser, 
		LPCTSTR pszPassword, fsHttpConnection *pServer, fsHttpFile *pFile, 
		LPTSTR* ppRedirectedUrl, BOOL *pbRedirInner, UINT64 uPos/* =0*/);
private:
	UTIL::com_ptr<IDownloadNotify>	m_dlNotify;

	CString m_strUrl;
	CString m_strSavePath;


	CString m_strProxyService;
	CString m_strProxyUser;
	CString m_strProxyPwd;

	DWORD	m_dwTaskID;

	HANDLE m_hDownloadExitEvent;
	HANDLE m_hDownloadThread;
	DWORD  m_dwDownloadThreadID;

	DL_TaskState m_dlTaskState;
	DL_Error	 m_dlError;
	fsURL		m_fsUrl;

	FILETIME	m_CreateFileTime;

	BOOL m_bAddToStore;

	LONGLONG	m_llTotalSize;	
	LONGLONG    m_llCurSize;
private:
	fsInternetSession*    m_pSession;
	fsHttpConnection*	  m_pHttpConnection;
	fsHttpFile*			  m_pHttpFile;
	CString				  m_strCookes;

	HANDLE				m_hSaveFile;

	CString				m_strTempSafePath;

	CSpeedMeter m_speedMeter;

	DECLARE_AUTOLOCK_CS(m_TempBuf);
	XBuffer m_TempBuf;
};

