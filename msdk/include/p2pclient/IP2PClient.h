#ifndef __IP2PClient_H__
#define __IP2PClient_H__


#include <mscom/msplugin.h>

typedef ::msdk::mscom::IMsPlugin    IMsP2PPlugin;
#define P2PString                   wchar_t*
#define P2P_HASH_SIZE               20
#ifndef AnyUrlHash
static unsigned char g_anyUrlHash[20] = { \
										0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
										0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
									};
static unsigned char g_nullUrlHash[20] = {0};
#define AnyUrlHash					g_anyUrlHash
#define NullUrlHash					g_nullUrlHash
#define HttpUrlHash                 AnyUrlHash
#endif



enum P2POption
{
	// P2P 下载相关
    Opt_P2PMaxUpSpeedInBps = 0, // 最大的上传速度，数据类型为 DWORD，默认为 0，不限制
    Opt_P2PMaxDownSpeedInBps,   // 最大的下载速度，数据类型为 DWORD，默认为 0，不限制
    Opt_P2PListenPort,          // 监听端口，      数据类型为 USHORT，默认为 (11935 ~ 15000) 中任意一个
    Opt_P2PMaxConnectNum,       // 最大连接数，    数据类型为 DWORD，默认为 0，不限制
    Opt_P2PTracker,	            // 单 tracker 地址， 数据类型为 P2PString，例如："udp://192.168.150.2:7369"
	Opt_P2PTrackerList,         // tracker 地址链表，数据类型为 vector<P2PString>*
	Opt_P2PSavePath,            // 下载文件保存路径，数据类型为 P2PString

    Opt_P2PTimeOut,             // P2P 连接的超时时间，    数据类型为 DWORD，单位为秒，默认为 60 秒， -1 为不限制.
	Opt_P2PTolerate,            // P2P 下载最小速度限制,   数据类型为 DWORD, 单位为字节, 默认为 1024, -1 为不限制.
	Opt_P2PMsgInterval,         // P2P 下载和上传事件间隔，数据类型为 DWORD，单位为豪秒，默认为 1000 豪秒
	Opt_P2PRidUrl,              // 上报 UrlHash 获取 Rid， 数据类型为 P2PString，默认为 "http://s.rising.cn/p2p/searchrid.php"

	Opt_P2PUpsLimit,            // 上传数限制， 数据类型为 DWORD，默认为 -1，不限制

	// 产品相关
	Opt_P2PHostName = 0x100,    // P2P 使用者标示， 数据类型 P2PString, 必要早于 init
	Opt_P2PAutoUpload,			// 模块加载模式, 0, 自动上传下载; 1, 自动下载; 2, 自动上传; 3, 不做任何事. 必要早于 init
	Opt_P2PDataReport,			// 0x1 任务信息上报 0x2 下载信息上报  0x4 上传信息上报  0x8 .....
	Opt_P2PReportUrl            // 上报 URL，数据类型 P2PString，默认为 "http://u.rising.cn/p2p/upinfo.php"
};

//完成状态
enum FileCompleteState
{
	//失败
	FileCompleteState_Fiald   = 0x0,

	//成功
	FileCompleteState_Success = 0x1,

	//用户取消
	FileCompleteState_Cancel  = 0x2,

	//用户暂停
	FileCompleteState_Pause   = 0x3,

	//停止
	FileCompleteState_Stop    = 0x4,
};

//开始状态
enum StartDownloadState
{
	//下载开始
	StartDownloadState_Start     = 0x0,

	//下载继续，断点上传
	StartDownloadState_continue  = 0x1,
};

enum DownloadMode
{
	Mod_Auto = 0, 
	Mod_P2P, 
	Mod_Http, 
	Mod_P2SP, 
	Mod_Unknown
};

enum NotifyMessageID
{
    Msg_FileLength = 0,	        // 文件长度消息，消息参数 wp 为文件长度高字节，类型为 DWORD；lp 为文件长度低字节，类型为 DWORD
    Msg_DownloadInfo,           // 下载信息消息，消息参数 wp 为下载总字节，    类型为 DWORD；lp 为下载总时间，    类型为 DWORD
    Msg_UploadInfo,             // 上传信息消息，消息参数 wp 为上传总字节，    类型为 DWORD；lp 为上传总时间，    类型为 DWORD
	Msg_FileComplete,           // 下载结束消息，消息参数 wp 为下载结果，      类型为 DWORD, (0x0: 失败; 0x1: 成功; 0x2: 用户取消; 0x3: 用户暂停, 0x4; 上传停止)；lp 保留

	Msg_ModeChange,             // 下载模式改变消息，消息参数 wp 为下载模式，  类型为 DWORD，lp 保留
	Msg_StartDownload,          // 开始下载消息，消息参数 wp 为下载类型，      类型为 DWORD，(0x0: 开始; 0x1: 继续(暂停后))；lp 保留
};

enum StateMessageID
{
	Stat_Info = 0,              // Info: (int) 下载状态 0 = ? 1 = Downloading 2 = Pause 3 = Stop .....
	Stat_Mode,                  // Mode: (int) 下载模式 1 = P2P 2 = HTTP 3 = P2SP

	Stat_FileSavePath,          // FileSavePath: (wchar_t*) xxx 下载文件保存路径
	Stat_FileName,              // FileName: (wchar_t*) xxx 下载文件名
	Stat_FileLength,            // FileLength:    (int) xxx 下载文件长度
	Stat_DownResult,            // DownResult:    (int) xxx 下载结果 0 ~ 3 = ? 4 = finish 8 = error ...

	Stat_DownloadSize,          // DownloadSize:  (int) xxx 下载总字节
	Stat_DownloadTimes,         // DownloadTimes: (int) xxx 下载时间
	Stat_DownloadRate,          // DownloadRate:  (int) xxx 下载速度
	Stat_UploadSize,            // UploadSize:    (int) xxx 上传总字节
	Stat_UploadTimes,           // UploadTimes:   (int) xxx 上传时间
	Stat_UploadRate,            // UploadRate:    (int) xxx 上传速度
	Stat_Seeds, 
	Stat_Peers
};


interface IMSP2PClientNotify : public IUnknown
{
	STDMETHOD(OnNotify)(const unsigned char* urlhash, NotifyMessageID atype, WPARAM wp, LPARAM lp) = 0;
};

MS_DEFINE_IID(IMSP2PClientNotify, "{15010CF0-6392-438c-983D-84F5A16A597A}");


// urlhash 任务标示, 20字节的二进制串
interface IMSP2PClient : public IMsP2PPlugin
{
	// P2P 操作
	STDMETHOD(StartWithURL)(const P2PString url, const P2PString strSaveFilePathName, OUT unsigned char* urlhash) = 0;
	STDMETHOD(Pause)       (const unsigned char* urlhash) = 0;      // urlhash = 0xfffffff....ff   === pause all task
	STDMETHOD(Resume)      (const unsigned char* urlhash) = 0;		
	STDMETHOD(Stop)        (const unsigned char* urlhash, BOOL remove_files = FALSE) = 0;

	// 任务状态配置
	// nlen: 统一传递缓冲区大小，以字节计数. 
	STDMETHOD(GetStat)     (const unsigned char* urlhash, StateMessageID statid, void* data, int nlen) = 0;
	STDMETHOD(SetStat)     (const unsigned char* urlhash, StateMessageID statid, void* data, int nlen) = 0;

	// P2P 配置
	STDMETHOD(SetOption)(P2POption option, void * data, int nlen) = 0;
	STDMETHOD(GetOption)(P2POption option, void * data, int nlen) = 0;

	// P2P 下载事件通知
	STDMETHOD(SetNotify)(IMSP2PClientNotify * data) = 0;
};

MS_DEFINE_IID(IMSP2PClient, "{A6023D5C-E39A-4e85-829B-7E4602BED8B5}");

// {5065C70F-0F00-4b66-8323-CDB22AC08FEE}
MS_DEFINE_GUID(CLSID_MSP2PClient, 0x5065c70f, 0xf00, 0x4b66, 0x83, 0x23, 0xcd, 0xb2, 0x2a, 0xc0, 0x8f, 0xee);


// ---------------------------------------------------------------------
// 备注：

// 下载模式
// 首先使用P2P下载
// 如果下载超时，即在限定时间内没有下载到任何内容， 切换到 http 下载方式 

// 超时
// 超时分两部分控制, 连接超时限制 (Opt_P2PTimeOut) 和下载速度过小限制 (Opt_P2PTolerate). 
// 超时时，还要进行重试，如果重试还超时，才真正切换下载方式

// 设置
// 全局设置由  SetOption 设置
// 单任务设置由 SetStat 设置

// 字符串编码采用 UNICODE 方式

// 通知消息
// 1 完成消息：
//   当下载完成时，会通知使用者。但是任务并没有停止，当用户停止时，发送完成停止消息。
// 

// 注意：
// 1 调用停止任务接口时
//   如果一个一个任务调用，有可能造成停止慢的现象。
//   这是因为 P2P 内部有两个队列：下载队列 和 等待队列。当下载队列中的任务下载结束或者
//   停止，则从等待队列中获取下一个任务下载.
//   因此推荐停止调用接口：
//   IMSP2PClient->Stop(AnyUrlHash).
//   或者先调用未开始下载的任务，再调用正在下载的任务
// 

// 模块使用方式指南:
// 1 新任务下载
//  IMSP2PClient->SetOption(Opt_P2PHostName, _T("RAV"));
//  IMSP2PClient->SetOption(Opt_P2PAutoUpload, 1);
//  IMSP2PClient->Init()
//. IMSP2PClient->StartWithURL()
//
// 2 重新启动后继续下载或上传
//  IMSP2PClient->SetOption(Opt_P2PAutoUpload, 0, 4);
//  IMSP2PClient->Init();
// 
// 3 增加纯 HTTP 下载接口. 它不会进行 p2p 种子获取. 
//   P2P 模式调用方式不变：
//  unsigned char urlhash[20];
//  memcpy(urlhash, NullUrlHash, P2P_HASH_SIZE);
//  IMSP2PClient->StartWithURL();
//   HTTP 模式调用方式：
//  unsigned char urlhash[20];
//  memcpy(urlhash, HttpUrlHash, P2P_HASH_SIZE);
//  IMSP2PClient->StartWithURL();
// 


#endif
