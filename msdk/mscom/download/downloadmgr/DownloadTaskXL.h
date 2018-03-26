#pragma once
#include <SyncObject/criticalsection.h>
#include <download/IDownload.h>
#include <thread/SubThreadImp.h>
#include "IDownloadTaskEx.h"
#include "SpeedMeter.h"
#include "xbuffer.hpp"
#include "XlModule.h"


//使用迅雷下载
class CDownloadTaskXL:
	public IDownloadTask,
	public IDownloadTaskEx,
	public CMsComBase<CDownloadTaskXL>,
	public CSubThread
{
public:
	UNKNOWN_IMP2_(IDownloadTask, IDownloadTaskEx);
	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pOuter);
	CDownloadTaskXL(void);
	~CDownloadTaskXL(void);

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

	//开始下载
	STDMETHOD(Start)();
	STDMETHOD(Stop)();

protected:
	virtual HRESULT	Run() ;

private:
	VOID SendNotify( DL_NotifyEvnet event );

private:
	UTIL::com_ptr<IDownloadNotify>	m_dlNotify;
	CString m_strUrl;
	CString m_strSavePath;
	CString m_strProxyService;
	CString m_strProxyUser;
	CString m_strProxyPwd;
	DWORD	m_dwTaskID;
	DWORD  m_dwDownloadThreadID;

	DL_TaskState m_dlTaskState;
	DL_Error	 m_dlError;
	fsURL		m_fsUrl;

	FILETIME	m_CreateFileTime;

	BOOL m_bAddToStore;

	LONGLONG	m_llTotalSize;	
	LONGLONG    m_llCurSize;
	CSpeedMeter						m_speedMeter;
private:

	CString				m_strTempSafePath;
	
	HANDLE m_hDLTask;
public:
	static CXlModule			m_xlModule;
};

