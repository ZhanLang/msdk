#pragma once
#include "download\DownloadCentre_i.h"
#include "mscomhelper\ConnectionPointHelper.h"

//ÏÂÔØÖÐÐÄ
class CDownloadCentre
	:public IMsPlugin
	,public IDownLoadCentre
	,public CConnectionPointContainerHelper<CDownloadCentre>
	,public CMsComBase<CDownloadCentre>
	,public IDownLoadNotify
{
public:
	UNKNOWN_IMP3_(IMsPlugin, IDownLoadCentre, IMsConnectionPointContainer);
	CDownloadCentre(void);
	~CDownloadCentre(void);

protected:

	//IDownLoadNotify
	VOID OnDownLaodNotify(const sha1_hash& hash , NotifyMessageID msgid ,WPARAM wp, LPARAM lp);

	//IDownLoadCentre
	HRESULT EnumDownloadTask(IEnumDownLoadTask* pCallBack);
	const DownLoadTaskInfo* QueryTaskInfo(const sha1_hash& hash); 
	HRESULT CreateDownloadTask(LPCWSTR lpUrl,LPCWSTR lpSavePath, OUT sha1_hash& hash);
	HRESULT Start(const sha1_hash& urlhash);
	HRESULT Stop(const sha1_hash& urlhash, BOOL remove_files = FALSE);
	HRESULT Delete(const sha1_hash& urlhash,BOOL remove_files=FALSE);
	HRESULT SetOption(/*DownLaodOption*/DWORD option,LPVOID lpdata,DWORD dwLen);
	HRESULT GetOption(/*DownLaodOption*/DWORD option, OUT LPVOID* lpData, OUT DWORD&dwLen);
	HRESULT SetTaskOption(const sha1_hash& hash, /*DLTaskOption*/DWORD option,LPVOID lpData,DWORD dwLen);
	HRESULT GetTaskOption(const sha1_hash& hash, /*DLTaskOption*/DWORD option,OUT LPVOID* lpData,OUT DWORD&dwLen);

	//IMsPlugin
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

protected:
	IDownLoadMgr*		m_pdlMgr;

	CConnectionPointHelper m_NotifyPointContainer;
};
