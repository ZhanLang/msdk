
#ifndef _DWONLOAD_I_H_
#define _DWONLOAD_I_H_

#include "download_def.h"
enum DownloaderEvent
{
	DLEvent_Connecting,
	DLEvent_Downloading,    //wp (DWORD)  长度 ,lp (BYTE*)内容
	DLEvent_DownloadStart,  // wp (UINT64*)已下载大小 , lp(UINT64*)文件总大小
	DLEvent_Done,           //完成
	DLEvent_Abort,			//取消
	DLEvent_Error,			//发生错误
	DLEvent_Speed,			//通知下载速度
};


interface IDownLoadEventNotify
{
	virtual HRESULT OnDownLoadEventNotify(DownloaderEvent dlEvent, WPARAM wp, LPARAM lp) = 0;
};

typedef DWORD DLError;

//下载任务属性
struct IDownloadInfo
{

};


interface IDownLoader :public IMSBase
{
	STDMETHOD(Open)(IDownLoadEventNotify * pEvent, 
		LPCWSTR lpstrUrl, 
		LPCWSTR lpstrSavePath,
		DWORD dwStartPos= 0 , 
		DWORD dwEndPos = -1) = 0;
	
	STDMETHOD(Close)() = 0;
	STDMETHOD(DeleteConfigFile)() = 0;
	STDMETHOD(Abort)() = 0;

	
	STDMETHOD(Download)() = 0;
	STDMETHOD_(UINT64,GetFileSize)() = 0;
	STDMETHOD_(UINT64,GetDownloadSize)() = 0;	

	STDMETHOD_(DownloadType,GetDownloadType)() = 0;
	STDMETHOD_(BOOL,IsAbort)()     = 0;
	

	STDMETHOD(SetOption)(DWORD option, LPVOID lpData, DWORD dwLen, BOOL bSave = FALSE) = 0;
	STDMETHOD(GetOption)(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen) = 0;
};
MS_DEFINE_IID(IDownLoader,"{72369712-1319-473b-B75F-4517AD167D11}");


// {3543934A-FCCC-42ce-A9DF-4624949A3B3D}
MS_DEFINE_GUID(CLSID_HttpDownlod, 
			0x3543934a, 0xfccc, 0x42ce, 0xa9, 0xdf, 0x46, 0x24, 0x94, 0x9a, 0x3b, 0x3d);

// {A71FDA21-265B-4e89-84AB-B7F6FE1BC78C}
MS_DEFINE_GUID(CLSID_HttpDwonloadTask, 
			   0xa71fda21, 0x265b, 0x4e89, 0x84, 0xab, 0xb7, 0xf6, 0xfe, 0x1b, 0xc7, 0x8c);
#endif //_DWONLOAD_I_H_