#pragma once
#include "download/IDownload.h"
#include "SyncObject/criticalsection.h"
#include <map>

#include "database/genericdb.h"
#include "thread/SubThreadImp.h"
#include "SpeedMeter.h"
class CDownloadMgr : 
	public IDownloadMgr,
	public IDownloadNotify,
	public CMsComBase<CDownloadMgr>,
	public CSubThread
{
public:
	UNKNOWN_IMP2_(IDownloadMgr, IDownloadNotify);
	CDownloadMgr();
	~CDownloadMgr();

protected:
	STDMETHOD(OpenDownloadMgr)(IDownloadNotify* pNotify, LPCWSTR lpszNameSpace);
	STDMETHOD(CloseDownloadMgr)();
	STDMETHOD_(DWORD, CreateDownloadTask)(LPCWSTR lpszUrl, LPCWSTR lpszSavePath, BOOL bForce = FALSE, DWORD dwEng = DL_EngineHttp);
	STDMETHOD_(DWORD, GetUrlHash)(LPCWSTR lpszUrl, BOOL bRandom = FALSE);
	STDMETHOD_(DL_TaskType, GetDownloadType)(LPCWSTR lspzUrl);
	STDMETHOD_(DWORD, QueryTask)(IMSBase** pTask);
	STDMETHOD_(IDownloadTask*, GetDownloadTask)(DWORD dwTaskId);
	STDMETHOD(StartTask)(DWORD dwTaskId);
	STDMETHOD(StopTask)(DWORD dwTaskId);
	STDMETHOD(RemoveTask)(DWORD dwTaskId);

	STDMETHOD_(DWORD, GetSpeed)();
	STDMETHOD_(BOOL, IsTaskExist)(DWORD dwTaskID);
protected:
	STDMETHOD(OnDownloadNotify)(DL_NotifyEvnet notifyType, DWORD dwTaskId);

protected://CSubThread
	virtual HRESULT	Run();

private:
	HRESULT CreateTask(LPCWSTR lpszUrl, LPCWSTR lpszPath, DWORD dwID, DWORD dwDlEng);

private:
	typedef std::map<DWORD, UTIL::com_ptr<IDownloadTask>> CTaskMap;

	DECLARE_AUTOLOCK_CS(m_taskMap);
	CTaskMap m_taskMap;

	CString m_strNameSpace;

	UTIL::com_ptr<IDownloadNotify>	m_dlNotify;
	CSpeedMeter						m_speedMeter;
};