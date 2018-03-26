#pragma once
#include "download_def.h"


#ifndef DOWNLOAD_DLL_EXPORT
#	define DOWNLOAD_API __declspec(dllimport)
#else
#	define DOWNLOAD_API __declspec(dllexport)
#endif


//////////////////////////////////////////////////////////////////////////
static UCHAR g_anyUrlHash[20] = 
{
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
};



static UCHAR g_nullUrlHash[20] = {0};

typedef struct sha1_hash
{
	sha1_hash(){
		memset(hash ,0 ,sizeof(hash));
	}

	sha1_hash(CONST UCHAR* szHash){
		memset(hash ,0 ,sizeof(hash));
		memcpy(hash,szHash,sizeof(hash));
	}

	unsigned char hash[20];
}sha1_hash;


//使用在map中
struct hashless{
	bool operator() (const sha1_hash&left, const sha1_hash&right) const{
		return memcmp(left.hash, right.hash, sizeof(right.hash)) < 0;
	}
};


inline bool operator == (const sha1_hash& _hash_l , const sha1_hash& _hash_r)
{
	return memcmp(_hash_l.hash,_hash_r.hash , sizeof(_hash_r.hash)) == 0;
}

inline bool operator != (const sha1_hash& _hash_l , const sha1_hash& _hash_r)
{
	return memcmp(_hash_l.hash,_hash_r.hash , sizeof(_hash_r.hash)) != 0;
}

#define AnyUrlHash  sha1_hash(g_anyUrlHash)
#define NullUrlHash sha1_hash(g_nullUrlHash)



//////////////////////////////////////////////////////////////////////////
//完成状态
enum FileCompleteState
{
	FCS_Fiald       = 0x0,//失败
	FCS_Success     = 0x1,//成功
	FCS_Stop        = 0x2,//停止
	FCS_Wait        = 0x4, //等待下载

	//LPARAM 下载长度
	FCS_Downloading = 0x8, //正在下载

	FCS_Unknow     = -1,
};


//开始状态
enum StartDownloadState
{
	SDS_Start     = 0x0,//下载开始
	SDS_Continue  = 0x1,//断点续下
};

//下载类型,类型间可以组合




enum NotifyMessageID
{
	//正在请求信息
	Msg_QueryInfo			= (1<<1),	

	//下载速度，消息参数 wp 为下载速度
	//AnyUrlHash 时为全局速度
	Msg_DownloadSpeed		= (1<<2),
	//FileCompleteState  
	//下载结束消息，消息参数 wp 为下载结果FileCompleteState ；lp 保留
	Msg_FileComplete		= (1<<3),

	//StartDownloadState 
	//开始下载消息，消息参数 wp 为下载类型，类型为 DWORD，(0x0: 开始; 0x1: 继续(暂停后))；lp 保留
	Msg_StartDownload		= (1<<4),  

	//下载中状态，wp(DWORD) 下载字节长度；lp 保留
	Msg_Downloading			= (1<<5),

	//下载类型变更，wp(DownloadType) 下载字节长度；lp 保留
	Msg_DownloadTypeChange	= (1<<6),
};

#define DefaultNotifyMsgMask (Msg_QueryInfo|Msg_DownloadSpeed|Msg_FileComplete|Msg_StartDownload)




enum DownLaodOption
{
	//todo:
	//下载任务个数，参数：PDWORD , SIZEOF(DWORD)
	//default:5
	DLO_MaxTaskNum_Option,

	//代理设置 IAT_ProxyInfo
	DLO_IAT_Option,

	//设置需要接收的消息ID，目的是过滤一些消息ID提高效率
	//default: DefaultNotifyMsgMask
	DLO_NotifyMsgMask,
	DLO_GetDownloadType,  //查询下载类型
};

enum DLTaskOption
{
	////////////////
	//SetTaskOption
	DLTO_Cookies,      //wchar_t*  //设置Cookie，保证重定向文件能下载成功
	DLTO_Referer,      //wchar_t*	//设置Referer，保证导航的文件能下载成功
	DLTO_NoCache,	   //BOOL (TRUE)无缓存模式下载，绕过服务器缓存，保证下载的文件是最新的，用于升级等
	
	//设置需要接收的消息ID，目的是过滤一些消息ID提高效率
	//default: DefaultNotifyMsgMask
	DLTO_NotifyMsgMask,
};

struct DownLoadTaskInfo
{
	DownLoadTaskInfo()
	{
		task_hash = NullUrlHash;
		lpUrl= NULL;
		lpSavePath= NULL;
		Fsc = FCS_Unknow;
		ZeroMemory(&DLSize , sizeof(DLSize));
		ZeroMemory(&DLTime,sizeof(DLTime));
		Speed = 0;
	}

	//紧缓存一下，避免重复计算
	sha1_hash task_hash;  

	LPCWSTR   lpUrl;      //url
	LPCWSTR   lpSavePath; //保存地址

	FileCompleteState  Fsc; //完成状态

	struct DownLoadSize
	{
		UINT64 totalSize;    //文件总大小
		UINT64 currentSize; //已下载大小
	} DLSize;

	struct DownLoadTime//可用这个作为排序
	{
		UINT64   createTime; //任务创建时间 
		UINT64   finishTime; //完成时间
		DWORD    userTime;   //毫秒
		UINT64   needTime;    //预计索要花的时间，-1无效,单位(秒)
	} DLTime;

	DWORD  Speed;		//当前速度, 单位(byte/s)
	DownloadType  DT;	//下载类型，可以是组合关系(DT_HTTP|DT_P2P),如果没有开始下载(DT_UNKNOWN)
};

struct IEnumDownLoadTask
{
	/*
	功能：枚举到的下载任务
	使用方法：建议与IDownLoadMgr::QueryTaskInfo一起使用
	*/
	virtual VOID OnEnumDownLoadTask(const sha1_hash& hash) = 0;
};

//回调接口
struct IDownLoadNotify
{
	/*
	功能：下载状态实时通知
	参数：
		hash 任务id ，特别注意 AnyUrlHash 与 NullUrlHash
		msgid 消息id
	*/
	virtual VOID OnDownLaodNotify(const sha1_hash& hash , NotifyMessageID msgid ,WPARAM wp, LPARAM lp) = 0;
};


//下载管理接口
struct IDownLoadMgr
	: public IMSBase
{
	/*
	功能:
		初始化任务管理器。
	参数:
		pNotify 任务事件回调接口。
		lpNameSpace 任务命名空间
		bCache 是否缓存任务状态
	返回:
		S_OK，成功
	*/
	virtual HRESULT InitDownloadMgr(IDownLoadNotify* pNotify, LPCWSTR lpNameSpace = NULL,BOOL bCache = TRUE) = 0;
	
	/*
	功能:
	参数:
	返回:
	*/
	virtual HRESULT UninitDownloadMgr() = 0;


	/*
	功能：枚举下载任务，该函数同步返回
	参数：枚举回调函数
	返回：SUCCESSEDD(hr)
	*/
	virtual HRESULT EnumDownloadTask(IEnumDownLoadTask* pCallBack) = 0;

	/*
	功能：查询下载任务信息
	参数：任务di
	返回：null 失败
	使用方法：为了避免下载任务销毁，不要存储该结构
	*/
	virtual const DownLoadTaskInfo* QueryTaskInfo(const sha1_hash& hash) = 0; 
	
	/*
	功能：创建下载任务
	参数：
		lpUrl：		下载URL
		lpSavePath：保存地址
		hash：该任务id
	返回：SUCCESSED(HR)
	*/
	virtual HRESULT CreateDownloadTask(LPCWSTR lpUrl,LPCWSTR lpSavePath, OUT sha1_hash& hash) = 0;

	/*
	功能：开始一个任务
	参数：urlhash 下载任务hash，注：AnyUrlHash 开始所有
	返回：SUCCESSED(hr)
	*/
	virtual HRESULT Start(const sha1_hash& urlhash) = 0;
	
	/*
	功能：停止一个下载任务
	参数：urlhash 下载任务hash，注：AnyUrlHash 暂停所有
	返回：SUCCESSED(hr)
	*/
	virtual HRESULT Stop(const sha1_hash& urlhash, BOOL remove_files = FALSE) = 0;

	/*
	功能：删除一个下载任务，从任务列表里面删除
	参数：urlhash 下载任务hash，注：AnyUrlHash 删除所有
	返回：SUCCESSED(hr)
	*/
	virtual HRESULT Delete(const sha1_hash& urlhash,BOOL remove_files=FALSE)  = 0;
	
	/*
	功能：设置
	参数：
		option 设置项,参照DownLaodOption，自定义的使用 DWORD 值
		lpdata 设置内容
		dwLen  长度
	返回：SUCCESSED(HR)
	*/
	virtual HRESULT SetOption(/*DownLaodOption*/DWORD option,LPVOID lpdata,DWORD dwLen) = 0;
	
	/*
	功能：获取设置状态
	参数：option 设置项，参照 DownLaodOption，自定义的使用 DWORD。
		  lpData 获取到的设置内容
		  dwLen  内容长度
	注意：如果返回的结果需要长期使用，最好做一下缓存，以免过了生存周期，导致崩溃.
	返回：S_OK,成功获取到了设置的结果。否则，没有设置该项
	*/
	virtual HRESULT GetOption(/*DownLaodOption*/DWORD option, OUT LPVOID* lpData, OUT DWORD&dwLen) = 0;

	
	/*
	功能：设置任务属性
	参数：
		hash   任务ID
		option 设置项,参照DLTaskOption，自定义的使用 DWORD 值
		lpdata 设置内容
		dwLen  长度
	返回：SUCCESSED(HR)
	*/
	virtual HRESULT SetTaskOption(const sha1_hash& hash, /*DLTaskOption*/DWORD option,LPVOID lpData,DWORD dwLen) = 0;

	/*
	功能：获取任务属性
	参数：
		  hash   任务ID
		  option 设置项，参照 DLTaskOption，自定义的使用 DWORD。
		  lpData 获取到的设置内容
		  dwLen  内容长度
	注意：如果返回的结果需要长期使用，最好做一下缓存，以免过了生存周期，导致崩溃.
	返回：S_OK,成功获取到了设置的结果。否则，没有设置该项
	*/
	virtual HRESULT GetTaskOption(const sha1_hash& hash, /*DLTaskOption*/DWORD option,OUT LPVOID* lpData,OUT DWORD&dwLen) = 0;
};

MS_DEFINE_IID(IDownLoadMgr,"{3DD441EA-9537-4785-8CC3-830AE9FD8F8F}");


EXTERN_C
{
	/*
	功能：创建下载管理类
	参数：
		pNotify 下载通知回调接口
		lpNameSpace 命名空间隔离,如果为空，则以当前进程名称作为命名空间
		bCache 是否存储当前状态
	返回：IDownLoadMgr接口指针，返回NULL失败
	*/
	DOWNLOAD_API IDownLoadMgr*  CreateDownLoadMgr(IDownLoadNotify* pNotify, LPCWSTR lpNameSpace = NULL,BOOL bCache = TRUE);
	DOWNLOAD_API VOID DistoryDownLoadMgr(IDownLoadMgr** pMgr);

	//DOWNLOAD_API VOID Get
};
