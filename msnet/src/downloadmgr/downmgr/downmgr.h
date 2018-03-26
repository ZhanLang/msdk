// downmgr.cpp : 定义 DLL 应用程序的导出函数。
//

#pragma once
#include "stdafx.h"

#include <map>
#include "DownloadTask.h"
#include "DwonloadSetting.h"
#include <algorithm>
#include <vector>
#include "safelist.h"
using namespace std;

enum TaskControlType
{
	Start_Control,
	Stop_Control,
	Delete_Control,
};

struct DownloadTaskItemInfo
{
	DownloadTaskItemInfo()
	{
		hash = NullUrlHash;
		pTask = NULL;
	}
	sha1_hash hash;
	CDownloadTask* pTask;
};

struct lessTaskItem
{
	bool operator()(const DownloadTaskItemInfo& r,const DownloadTaskItemInfo& l) const
	{
		return r.pTask->GetProp()->DLTime.createTime < l.pTask->GetProp()->DLTime.createTime;
	}
};


struct DownLoadMsgInfo
{
	sha1_hash hash;
	NotifyMessageID msgid;
	WPARAM wp;
	LPARAM lp;
};

class CDownloadMgr:
	public CSubThread,
	public IDownLoadMgr,
	public IDownLoadNotify,
	public CUnknownImp
{
public:
	typedef map<sha1_hash,CDownloadTask* ,hashless> CTaskMap;
	typedef std::vector<DownloadTaskItemInfo> CTaskList;

	CDownloadMgr();
	~CDownloadMgr();
	
	UNKNOWN_IMP1(IDownLoadMgr);
public:
	HRESULT InitDownloadMgr(IDownLoadNotify* pNotify ,LPCWSTR lpNameSpace = NULL,BOOL bCache =TRUE);
	HRESULT UninitDownloadMgr();

	VOID GetSpeed();
	LPCTSTR GetNameSpace();
protected:
	virtual HRESULT EnumDownloadTask(IEnumDownLoadTask* pCallBack);
	virtual const  DownLoadTaskInfo* QueryTaskInfo(const sha1_hash& hash); 
	virtual HRESULT CreateDownloadTask(LPCWSTR lpUrl,LPCWSTR lpSavePath, OUT sha1_hash& hash);

	virtual HRESULT Start(const sha1_hash& urlhash);
	virtual HRESULT Stop(const sha1_hash& urlhash, BOOL remove_files = FALSE);
	virtual HRESULT Delete(const sha1_hash& urlhash,BOOL remove_files=FALSE);
	virtual HRESULT SetOption(DWORD option,LPVOID lpdata,DWORD dwLen);
	virtual HRESULT GetOption(DWORD option, OUT LPVOID* lpData, OUT DWORD&dwLen);
	virtual HRESULT SetTaskOption(const sha1_hash& hash,DWORD option,LPVOID lpData,DWORD dwLen);
	virtual HRESULT GetTaskOption(const sha1_hash& hash,DWORD option,OUT LPVOID* lpData,OUT DWORD&dwLen);

protected:
	virtual HRESULT	Run();
private:
	//初始化下载任务

	BOOL InitDownloadTask();
	BOOL TaskControl(const sha1_hash& hash,TaskControlType control ,BOOL remove_files=FALSE);
	VOID OnDownLaodNotify(const sha1_hash& hash , NotifyMessageID msgid ,WPARAM wp, LPARAM lp);
	
	HRESULT NewTaskStart(const sha1_hash& hash = NullUrlHash); //开始一个新任务
	HRESULT AddTask(const sha1_hash& hash,CDownloadTask*pMgr);
	HRESULT RemoveTask(const sha1_hash& hash);
	

	//将下载任务信息同步到文件中
	HRESULT SyncConfigFile();
	HRESULT CreateTask(DownloadTaskProp* pProp);

	DWORD GetDownloadingTaskCount();
	CDownloadTask* GetDownloadTask(const sha1_hash& hash);
private:
	IDownLoadNotify* m_pDlNotify;
	CString			 m_strNameSpace;
	CTaskMap	     m_downloadTaskMap;
	CTaskList		 m_downloadTaskList;
	CString			 m_strDbPath;
	CDwonloadSetting m_dlSetting;

	DECLARE_AUTOLOCK_CS(taskmap);
	BOOL m_bStopAllTask;
	BOOL m_bCache;
	CSafeList<DownLoadMsgInfo> m_SafeList;
};