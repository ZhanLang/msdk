#pragma once
//下载中心

#include "download/usedownloadmgr.h"

struct IDownLoadCentre
	:  public IMSBase
	,  public IDownLoadMgr
{
	
};

MS_DEFINE_IID(IDownLoadCentre,"{264FD003-8200-435e-BE94-93E4D11809B4}");

#define Msg_CreateTask 100		//添加一个下载任务 hash 任务ID
#define Msg_RemoveTask 101	//删除一个任务，hash 任务ID,(AnyUrlHash)删除全部
struct IDownLoadNotifyConnectPoint
	:public IMSBase
	,public IDownLoadNotify
{
	//扩展出来的通知接口
	virtual VOID OnDownLaodNotifyEx(const sha1_hash& hash , DWORD msgid ,WPARAM wp, LPARAM lp) = 0;
};

MS_DEFINE_IID(IDownLoadNotifyConnectPoint,"{F29DCD9C-1E15-41d0-A26B-B45FBC43B2BF}");


// {A7D960C5-DD61-4342-A53B-C5D394A94A6F}
MS_DEFINE_GUID(CLSID_DownLoadCentre, 
			0xa7d960c5, 0xdd61, 0x4342, 0xa5, 0x3b, 0xc5, 0xd3, 0x94, 0xa9, 0x4a, 0x6f);
