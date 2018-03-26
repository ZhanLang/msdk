// HttpDownload.h: interface for the CHttpDownload class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_HTTPDOWNLOAD_H__)
#define _HTTPDOWNLOAD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Wininet.h>
#pragma comment(lib, "wininet.lib")

////////////////////////////////////////////////////////////////////////////////
// 自定义消息
//#define WM_HTTPDOWN_NOTIFY_MSG (WM_USER + 2005) // 2005与界面库有冲突
#define WM_HTTPDOWN_NOTIFY_MSG (WM_USER + 2008)

// 结构定义, 接受下载消息
typedef struct _tagHttpDownloadStatus
{
	UINT	nStatusType;					// 状态类型: 1－ 文件名; 2－文件大小；3－文件已经下载大小
	DWORD	dwFileSize;					    // 文件总的大小
	DWORD	dwFileDownloadedSize;		    // 已经下载大小
	CString	strFileName;				    // 文件名
}HTTPDOWNLOADSTATUS,*PHTTPDOWNLOADSTATUS;
// 下载自定义消息结构状态类型,对应HTTPDOWNLOADSTATUS.nStatusType
enum
{
	DOWNLOAD_STATUS_FILENAME			= 1, // 文件名
	DOWNLOAD_STATUS_FILESIZE			= 2,	// 文件大小
	DOWNLOAD_STATUS_DOWNLOADEDSIZE		= 3,	// 文件已经下载大小
	DOWNLOAD_STATUS_DOWNLOADCOMPLETE    = 4, // 下载完成
};
//////////////////////////////////////////////////////////////////////////
//LRESULT OnHttpdownloadNotify(WPARAM /*wParam*/, LPARAM lParam,)
//{
//	PHTTPDOWNLOADSTATUS pHttpStatus = (PHTTPDOWNLOADSTATUS)lParam;
//
//	CString strFileName;
//	LONG lFileSize =0, lDownloadedSize = 0
//	switch ( pHttpStatus->nStatusType )
//	{
//	case DOWNLOAD_STATUS_FILENAME:
//		strFileName = pHttpStatus->strFileName;
//		break;
//	case DOWNLOAD_STATUS_FILESIZE:
//		lFileSize = pHttpStatus->dwFileSize;
//		break;
//	case DOWNLOAD_STATUS_DOWNLOADEDSIZE:
//		lDownloadedSize = pHttpStatus->dwFileDownloadedSize;
//		break;
//	default:
//		break;
//		}
//	return 0;
//}
//////////////////////////////////////////////////////////////////////////

// Download 函数返回值
enum
{
	ERR_HTTP_SUCCESS				=	0x00000000,			// 下载成功
	ERR_HTTP_FAIL					=	0xFFFFFFFF,			// 下载失败
	ERR_HTTP_REDIRECT				=   0x00000001,			// 重定向HTTP
	ERR_HTTP_USER_STOP              =   0x00000002,			// 用户停止下载(调用了StopDownload函数)
	ERR_HTTP_INVALID_URL			=   0x80000001,         // 无效URL
	ERR_HTTP_DEST_FILENAME_EMPTY	=   0x80000002,			// 本地存储文件为空
	ERR_HTTP_CREATE_FILE_FAIL		=   0x80000003,			// 创建下载文件失败
	ERR_HTTP_RENAME_FILE_FAIL       =   0x80000004,			// 重命名下载文件失败
	ERR_HTTP_SEND_TIMEOUT           =   0x80000005,			// 请求超时
	ERR_HTTP_BUFFER_TOO_SMALL       =   0x80000006,			// buffer too samll
	ERR_HTTP_NOT_FIND				=   0x80000007,			// 请求不存在
	ERR_HTTP_NET_ERROR              =   0x80000008,         // 网络错误
	//HTTP_RESULT_CONNECT_FAIL    =   0x80000002,         // CONNECT Error 
	ERR_HTTP_RECEIVE_TIMEOUT        =   0x80000009,			// 请求超时
	ERR_HTTP_FORBIDDEN_ACCESS       =   0x80000010,			// 拒绝访问
	ERR_HTTP_SERVER_ERROR           =   0x80000011,			// 服务器错误（internal server error）：500
};

// SetConnectType参数
enum
{
	CONNECT_TYPE_IE                = 0,	// 使用ＩＥ设置
	CONNECT_TYPE_DIRECT            = 1, // 直接连接
	CONNECT_TYPE_PROXY             = 3  // 使用代理
};
class CHttpDownload  
{
public:
	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：HRESULT Download(LPCTSTR lpszURL,
	//				LPCTSTR lpszSaveFileName	/*= NULL*/，
	//				BOOL    bAutoRedirect /*= FALSE*/)
	//	用  途：下载入口，调用此函数来开始下载过程
	//	对全局变量的影响：无
	//	参  数：
	//		lpszURL			  : [in]待下载的网络URL
	//		lpszSaveFileName  : [in]本地保存文件名(当要得到重定向URL时可以为NULL)
	//      bAutoRedirect     : [in]是否自动重定向
	//		lpszResetHost	  : [in]如果需要重置HTTP协议头的Host时，此参数有效，
	//			为NULL表示不需要重置HTTP协议头的Host，如果lpszURL里指定的是服务器
	//			是IP地址时会用到这个功能
	//	返回值：HRESULT
	//		ERR_HTTP_USER_STOP		: 用户停止下载(调用了StopDownload函数)
	//		ERR_HTTP_FAIL			: 下载失败
	//		ERR_HTTP_SUCCESS		: 下载成功
	//		ERR_HTTP_REDIRECT		: 重定向到HTTP
	//		ERR_HTTP_INVALID_URL	: 无效URL
	//      ERR_HTTP_DEST_FILENAME_EMPTY : 本地存储文件为空
	//      ERR_HTTP_CREATE_FILE_FAIL : 创建下载文件失败
	//      ERR_HTTP_RENAME_FILE_FAIL : 重命名下载文件失败
	//      ERR_HTTP_SEND_TIMEOUT     : 请求超时
	/////////////////////////////////////////////////////////////////////////////////
	HRESULT Download(LPCTSTR lpszURL, LPCTSTR lpszFileName = NULL, 
		BOOL bAutoRedirect = FALSE, LPCTSTR lpszResetHost = NULL);
	
	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：HRESULT PostData(LPCTSTR lpszURL,LPVOID lpBuffer, DWORD cbBuffer,
	//				LPBYTE lpData, LPDWORD lpcbData );
	//	用  途：向服务器发送数据
	//	对全局变量的影响：无
	//	参  数：
	//		lpszURL			  : [in]结束数据服务器ＵＲＬ
	//		lpBuffer          : [in]要发送的数据
	//      cbBuffer          : [in]要发送的数据的字节长度
	//      lpData            : [out]接收数据缓冲区
	//      lpcbData          : [in,out]输入时为lpData缓冲区字节数,返回时为接收数据字节数
	//      bAddPostHeadFlag  : [in]是否在Post数据前自动加InPost=标志
	//	返回值：HRESULT
	//		ERR_HTTP_USER_STOP		: 用户停止下载(调用了StopDownload函数)
	//		ERR_HTTP_FAIL			: 发送失败
	//		ERR_HTTP_SUCCESS		: 发送成功
	//		ERR_HTTP_REDIRECT		: 重定向到HTTP
	//		ERR_HTTP_INVALID_URL	: 无效URL
	//      ERR_HTTP_BUFFER_TOO_SMALL: 接收缓冲区太小
	/////////////////////////////////////////////////////////////////////////////////
	HRESULT PostData(LPCTSTR lpszURL, LPVOID lpBuffer, DWORD cbBuffer,
				LPBYTE lpData, LPDWORD lpcbData , BOOL bAddPostHeadFlag = FALSE);

	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：GetContent
	//	用  途：读Internet文件内容
	//	对全局变量的影响：无
	//	参  数：
	//		lpszURL			  : [in]结束数据服务器ＵＲＬ
	//		lpContent         : [in]接受文件内容的缓冲区
	//      lpcbContent       : [in/out]接受文件内容的缓冲区的字节长度
	//      bAutoRedirect     : [in]是否自动重定向
	//	返回值：HRESULT
	//		ERR_HTTP_USER_STOP		: 用户停止下载(调用了StopDownload函数)
	//		ERR_HTTP_FAIL			: 失败
	//		ERR_HTTP_SUCCESS		: 成功
	//		ERR_HTTP_REDIRECT		: 重定向到HTTP
	//		ERR_HTTP_INVALID_URL	: 无效URL
	//      ERR_HTTP_BUFFER_TOO_SMALL: 接收缓冲区太小
	/////////////////////////////////////////////////////////////////////////////////
	HRESULT GetContent(LPCTSTR lpszURL, LPVOID lpContent, LPDWORD lpcbContent, BOOL bAutoRedirect);

	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：void StopDownload()
	//	用  途：停止下载
	//	对全局变量的影响：无
	//	参  数：无
	//	返回值：无
	/////////////////////////////////////////////////////////////////////////////////
	void    StopDownload();

	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：BOOL SetNotifyHwnd( HWND hNotifyWnd )
	//	用  途：设置接收下载消息的窗口句柄
	//	对全局变量的影响：无
	//	参  数：hNotifyWnd : [in]接收下载消息的窗口句柄
	//	返回值：合法窗口为-TRUE，否则-FALSE
	/////////////////////////////////////////////////////////////////////////////////
	BOOL	SetNotifyHwnd(HWND hNotifyWnd);

	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：LPCTSTR GetRedirectURL()
	//	用  途：得到重定向后的ＵＲＬ
	//	对全局变量的影响：无
	//	参  数：无
	//	返回值：URL字符串
	//  说明  : 当调用Download后返回值为HTTP_RES_REDIRECT，再调用此函数得到重定向URL
	/////////////////////////////////////////////////////////////////////////////////
	LPCTSTR  GetRedirectURL();
	
	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：BOOL SetProxy( LPCTSTR lpszProxyServer,
	//					USHORT nProxyPort, 
	//					BOOL bUseProxy /* =TRUE */,
	//					BOOL bProxyAuthorization /* =FALSE */,
	//					LPCTSTR lpszProxyUsername /* =NULL */,
	//					LPCTSTR lpszProxyPassword /* =NULL */ )
	//	用  途：设置代理服务器访问参数
	//	对全局变量的影响：无
	//	参  数：
	//		lpszProxyServer		: [in]代理服务器名
	//      nProxyPort			: [in]代理服务器端口
	//		bUseProxy			: [in]是否使用代理(此值为FALSE时，前两个参数将被忽略)
	//		bProxyAuthorization : [in]代理服务器是否需要身份验证
	//		lpszProxyUsername   : [in]代理服务器身份验证用户名
	//		lpszProxyUsername   : [in]代理服务器身份验证密码
	//	返回值：成功－TRUE；失败FALSE
	/////////////////////////////////////////////////////////////////////////////////
	BOOL SetProxy(LPCTSTR lpszProxyServer,
					USHORT nProxyPort, 
					BOOL bUseProxy = TRUE,
					BOOL bProxyAuthorization = FALSE,
					LPCTSTR lpszProxyUsername = NULL,
					LPCTSTR lpszProxyPassword = NULL);

	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：void SetAuthorization(
	//				LPCTSTR lpszUsername,
	//				LPCTSTR lpszPassword,
	//				BOOL bAuthorization/* = TRUE*/ ) 
	//	用  途：设置WWW认证信息(访问被保护的页面时需要)
	//	对全局变量的影响：无
	//	参  数：
	//		lpszUsername   : [in]访问页面的用户名
	//		lpszPassword   : [in]口令
	//		bAuthorization : [in]是否需要验证(此值为FALSE时，前两个参数将被忽略)
	//	返回值：void
	////////////////////////////////////////////////////////////////////////////////
	void SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization = TRUE);

	//////////////////////////////////////////////////////////////////////////////////
	//	函数名：void SetConnectType(INT nType /*= CONNECT_TYPE_IE*/) 
	//	用  途：设置连接类型
	//	对全局变量的影响：无
	//	参  数：
	//		nType   : [in]类型（CONNECT_TYPE_IE/* 0 - IE; 1 - 直接连接; 3 - Proxy*/）
	//	返回值：void
	////////////////////////////////////////////////////////////////////////////////
	void SetConnectType(INT nType = CONNECT_TYPE_IE/* 0 - IE; 1 - 直接连接; 3 - Proxy*/) { m_nConnectType = nType; }

	DWORD GetLastError(){return m_dwLastError;}

	DWORD GetFileSize() {return m_dwFileSize;}

public:
	CHttpDownload();
	virtual ~CHttpDownload();

protected:
	BOOL ParseUrl(LPCTSTR lpszUrl);
	BOOL Connect();
	BOOL HttpDateToSystemTime(LPCTSTR lpszTime, SYSTEMTIME& st);
	BOOL CloseHandles();
	virtual BOOL Notify(int msg, WPARAM wp, LPARAM lp);

protected:
	HINTERNET m_hInternet;					// internet open handle
	HINTERNET m_hConnect;					// internet connection handle
	HINTERNET m_hRequest;					// internet request handle

	// 针对请求服务器端
	CString         m_strUserAgent;         // UserAgent (IE version;os version)
	CString			m_strServerName;		// 服务器名(hostname,例如：www.Margin.com.cn)
	INTERNET_PORT   m_nServerPort;			// 服务器端口
	BOOL			m_bAuthorization;		// 服务器名是否需要身份验证
	CString			m_strUsername;			// 服务器名登陆用户名
	CString			m_strPassword;			// 服务器名登陆密码
	CString			m_strObjectName;		// 相对URL路径(例如：/register/pcver/AutoUpgradePad/ver2005/MSAGENT/NewVer/MsAgent_exe.zip)

	// 针对代理服务器端
	CString			m_strProxyServer;		// 代理服务器名
	INTERNET_PORT	m_nProxyPort;			// 代理服务器端口
	BOOL			m_bUseProxy;			// 是否使用代理服务器
	BOOL			m_bProxyAuthorization;	// 代理服务器是否需要身份验证
	CString			m_strProxyUsername;     // 代理服务器名登陆用户名
	CString			m_strProxyPassword;		// 代理服务器名登陆密码

	BOOL			m_bStop;				// 用户中止了下载连接
	BOOL			m_bServerAlter;         // 多次请求时，第二次URL地址的HostName是否改变
	
	CString         m_strRedirectURL;       // 重定向地址
	HWND			m_hNotifyWnd;           // 接受下载消息的窗口句柄

	DWORD			m_dwLastError;          // 存储内部错误码
	DWORD           m_nConnectType;
	DWORD			m_dwFileSize;
};

#endif // !defined(_HTTPDOWNLOAD_H__)
