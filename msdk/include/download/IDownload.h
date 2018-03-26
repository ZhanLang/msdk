#pragma once

enum DL_Engine
{
	DL_EngineHttp,	//使用自身HTTP下载
	DL_EngineXL,	//使用迅雷下载
};

enum DL_NotifyEvnet
{
	DL_NotifyEvnet_Connecting,		//链接通知
	DL_NotifyEvnet_Downloading,		//正在下载通知
	DL_NotifyEvnet_DownloadStart,	//开始下载
	DL_NotifyEvnet_Complete,		//下载完成
	DL_NotifyEvnet_Stop,
	DL_NotifyEvent_Error,			//下载失败

};


enum DL_TaskType
{
	DL_TaskType_UnKnown,
	DL_TaskType_Http,
	DL_TaskType_Https,
	DL_TaskType_Ftp,
};

enum DL_Error
{
	DLE_SUCCESS,				
	DLE_S_FALSE,				
	DLE_ALREADYCONNECTED,	
	DLE_WININETUNKERROR,		
	DLE_NOTINITIALIZED,		
	DLE_LOGINFAILURE,		
	DLE_FILENOTOPENED,		
	DLE_BADURL,				
	DLE_INVALIDPARAM,		
	DLE_ERROR,				
	DLE_TIMEOUT,				
	DLE_CANTCONNECT,			
	DLE_FILENOTFOUND,		
	DLE_LOSTCONNECTION,		
	DLE_NAMENOTRESOLVED,		
	DLE_RANGESNOTAVAIL,		
	DLE_PROXYAUTHREQ,		
	DLE_WINERROR,			
	DLE_NEEDREDIRECT,		
	DLE_EXTERROR,			
	DLE_SERVERBADREQUEST,	
	DLE_SERVERUNKERROR,		
	DLE_CONNECTIONABORTED,	
	DLE_OUTOFMEMORY,			
	DLE_S_REDIRECTED,		
	DLE_INVALIDPASSWORD,		
	DLE_INVALIDUSERNAME,		
	DLE_NODIRECTACCESS,		
	DLE_NOINTERNETCONNECTION,	
	DLE_HTTPVERNOTSUP,		
	DLE_BADFILESIZE,

	DLE_DOUBTFUL_RANGESRESPONSE,	

	DLE_E_NOTIMPL,

	DLE_E_WININET_UNSUPP_RESOURCE,
};

enum DL_TaskState
{
	DL_TaskState_UnKnown,
	DL_TaskState_Connecting,	//正在连接
	DL_TaskState_Downloading,	//正在下载
	DL_TaskState_Stoped,		//已停止下载
	DL_TaskState_Error,			//下载错误
	DL_TaskState_Done,			//下载已完成
};

//通知回调
struct IDownloadNotify : public IMSBase
{
	STDMETHOD(OnDownloadNotify)(DL_NotifyEvnet notifyType, DWORD dwTaskId) = 0;
};
MS_DEFINE_IID(IDownloadNotify, "{DC744FA0-8874-4252-B1D6-5700BB98CF65}");

//下载任务获取属性接口
struct IDownloadTask : public IMSBase
{
	//获取属性
	STDMETHOD_(LPCWSTR		, GetUrl)()				 = 0;
	STDMETHOD_(LPCWSTR		, GetSavePath)()		 = 0;
	STDMETHOD_(DL_TaskType	, GetTaskType)()		 = 0;
	STDMETHOD_(LONGLONG		, GetTotalFileSize)()	 = 0;
	STDMETHOD_(LONGLONG		, GetCurrentFileSize)()	 = 0;
	STDMETHOD_(FILETIME		, GetCreateTime)()		 = 0;
	STDMETHOD_(DWORD, GetSpeed)() = 0;
	//获取代理属性
	STDMETHOD_(LPCWSTR, GetProxyService)()			 = 0;
	STDMETHOD_(LPCWSTR, GetProxyUser)()				 = 0;
	STDMETHOD_(LPCWSTR, GetProxyPwd)()				 = 0;

	STDMETHOD_(DWORD, GetTaskId)()					 = 0;

	//获取错误信息
	STDMETHOD_(DL_Error, GetTaskError)()			 = 0;

	STDMETHOD_(DL_TaskState, GetTaskState)()		= 0;

	STDMETHOD(SetCookie)(LPCWSTR lpszCookie)		 = 0;

	//设置代理
	STDMETHOD(SetProxyService)(LPCWSTR lpszService)  = 0;
	STDMETHOD(SetProxyUser)(LPCWSTR lpszUsr)		 = 0;
	STDMETHOD(SetProxyPwd)(LPCWSTR lpszPwd)			 = 0;

	STDMETHOD(CreateTask)(LPCWSTR lpszUrl, LPCWSTR lpszSavePath, IDownloadNotify* pNotify, DWORD dwTaskId) = 0;
	STDMETHOD(DestroyTask)() = 0;

	STDMETHOD_(DWORD, GetEngine)() = 0;

	//开始下载
	STDMETHOD(Start)() = 0;
	STDMETHOD(Stop)() = 0;
};
MS_DEFINE_IID(IDownloadTask, "{8A17155B-6487-4D52-B97F-7886D3405878}");







// {D4E72BA0-6ABC-4E08-B172-AE74F418B1B3}
MS_DEFINE_GUID(CLSID_HttpDownload, 
	0xd4e72ba0, 0x6abc, 0x4e08, 0xb1, 0x72, 0xae, 0x74, 0xf4, 0x18, 0xb1, 0xb3);



// {5E791BAF-4715-4781-A045-256CE472AED1}
MS_DEFINE_GUID(CLSID_XlDownload, 
	0x5e791baf, 0x4715, 0x4781, 0xa0, 0x45, 0x25, 0x6c, 0xe4, 0x72, 0xae, 0xd1);


struct IDownloadMgr : public IMSBase
{
	STDMETHOD(OpenDownloadMgr)(IDownloadNotify* pNotify, LPCWSTR lpszNameSpace) = 0;

	STDMETHOD(CloseDownloadMgr)() = 0;

	//返回任务ID
	/*
		bForce 强制下载，即使重复也下载
	*/
	STDMETHOD_(DWORD, CreateDownloadTask)(LPCWSTR lpszUrl, LPCWSTR lpszSavePath, BOOL bForce = FALSE, DWORD dwEng = DL_EngineHttp) = 0;
	STDMETHOD_(DL_TaskType, GetDownloadType)(LPCWSTR lspzUrl) = 0;

	STDMETHOD_(DWORD, QueryTask)(/*IProperty2*/IMSBase** pTask) = 0;

	STDMETHOD_(IDownloadTask*, GetDownloadTask)(DWORD dwTaskId) = 0;

	STDMETHOD(StartTask)(DWORD dwTaskId)	 = 0;
	STDMETHOD(StopTask)(DWORD dwTaskId)		 = 0;
	STDMETHOD(RemoveTask)(DWORD dwTaskId)	 = 0;

	//获取全局的下载速度
	STDMETHOD_(DWORD, GetSpeed)() = 0;


	//返回值实际上就是任务ID
	STDMETHOD_(DWORD, GetUrlHash)(LPCWSTR lpszUrl, BOOL bRandom = FALSE) = 0;
	STDMETHOD_(BOOL, IsTaskExist)(DWORD dwTaskID) = 0;
};

MS_DEFINE_IID(IDownloadMgr, "{7DC6B170-10CE-4802-8B71-8A4A8D56A6D8}");



	// {A9708DBA-31AE-4DEB-9444-31B384FE2A36}
MS_DEFINE_GUID(CLSID_DownloadMgr, 
	0xa9708dba, 0x31ae, 0x4deb, 0x94, 0x44, 0x31, 0xb3, 0x84, 0xfe, 0x2a, 0x36);




struct IHttpSyncRequest : public IMSBase
{
	STDMETHOD(HttpPost)(LPCWSTR lpszUrl,  LPCWSTR lpszPath, LPCWSTR lpszParam,/*IMsBuffer*/IMSBase** pBuffer ) = 0;
	STDMETHOD(HttpRequest)(LPCWSTR lpszUrl, /*IMsBuffer*/IMSBase** pBuffer) = 0;
	STDMETHOD(HttpDownload)(LPCWSTR lpszUrl, LPCWSTR lpszSafeFile) = 0;
};

MS_DEFINE_IID(IHttpSyncRequest,"{7C0BB5F8-D7DC-4194-8FCD-7DF8677B1EB6}");

// {FB9F9739-9BF4-4451-BFA8-1DF630016D0B}
MS_DEFINE_GUID(CLSID_HttpSyncRequest,
	0xfb9f9739, 0x9bf4, 0x4451, 0xbf, 0xa8, 0x1d, 0xf6, 0x30, 0x1, 0x6d, 0xb);



struct IHttpAsyncRequestNotify : public IMSBase
{
	STDMETHOD(OnHttpAsyncRequestNotify)(DWORD dwCookie, /*IMsBuffer*/IMSBase* pBuffer, DWORD dwResult) = 0;
};
MS_DEFINE_IID(IHttpAsyncRequestNotify,"{7C532166-D5DD-4784-9220-652D8B2BEC7A}");


struct IHttpAsyncRequest : public IMSBase
{
	STDMETHOD(Init)() = 0;
	STDMETHOD(UnInit)() = 0;
	STDMETHOD(HttpAsyncRequest)(LPCWSTR lpszUrl, DWORD dwCookie, IHttpAsyncRequestNotify* pNotify) = 0;
};
MS_DEFINE_IID(IHttpAsyncRequest,"{ED4A6E36-B3C9-460d-AF0F-9AF360A7D208}");

// {D6E387D2-EBE0-4955-A123-E088E054750E}
MS_DEFINE_GUID(CLSID_HttpAsyncRequst, 
	0xd6e387d2, 0xebe0, 0x4955, 0xa1, 0x23, 0xe0, 0x88, 0xe0, 0x54, 0x75, 0xe);



/*@
	http相关的lua扩展
*/
//{38647D80-E517-46BA-8067-01C95FCAF5C3}
MS_DEFINE_GUID(CLSID_Http_LuaEx, 
	0x38647d80, 0xe517, 0x46ba, 0x80, 0x67, 0x1, 0xc9, 0x5f, 0xca, 0xf5, 0xc3);