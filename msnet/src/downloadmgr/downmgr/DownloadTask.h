#pragma once
#include "thread/SubThreadImp.h"
#include "DwonloadSetting.h"
#include "SpeedMeter.h"
#include "httpdownload.h"
#include <map>
#include "util/xbuffer.hpp"
class CDownloadTask : 
	public CSubThread,
	public IDownLoadEventNotify,
	public IDownloadTask
{
public:
	typedef std::map<DWORD,XBuffer> CDLTaskOptionMap;
	CDownloadTask();
	~CDownloadTask(void);

	DWORD GetSpeed();
	BOOL  SetProp(const DownloadTaskProp* prop);
	const DownloadTaskProp* GetProp();
	HRESULT InitDownloadTask(IDownLoadNotify* pNotift,IDwonloadSetting* pSetting);
	HRESULT SetTaskOption(DWORD option, LPVOID lpData, DWORD dwLen);
	HRESULT GetTaskOption(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen);
	IDwonloadSetting * GetSetting();
	HRESULT CheckInit();

protected:
	HRESULT OnDownLoadEventNotify(DownloaderEvent dlEvent, WPARAM wp, LPARAM lp);
	
public:
	HRESULT Start();
	HRESULT Stop(BOOL remove_files = FALSE);
	DownLoadTaskInfo* QueryTaskInfo();
	
	//为任务管理器特别提供
	HRESULT SetFileCompleteState(FileCompleteState fsc);
protected:
	HRESULT	Run();

private:
	HRESULT InitDLTask();
	HRESULT UninitDLTask();
	
private:
	DownloadTaskProp m_prop;
	IDownLoadNotify* m_pNotify;
	IDwonloadSetting* m_pDownSetting;
	IDownLoader*		  m_pDownLoad;

	CString	m_strBackFile;
	CSpeedMeter m_speedMeter;
	BOOL		m_bRemoveFile;

	CDLTaskOptionMap m_dlTaskOptionMap;

	DWORD      m_dwNotifyIdMask;
};
