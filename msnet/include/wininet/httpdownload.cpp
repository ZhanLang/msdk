// HttpDownload.cpp: implementation of the CHttpDownload class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpDownload.h"
#include "url.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")

#ifdef __ATLMISC_H__
#define _ATL_USE_CSTRING_FLOAT
#endif


#define __HTTP_VERB_GET					_T("GET")
#define __HTTP_VERB_POST                _T("POST")
#define __HTTP_ACCEPT_TYPE				_T("*/*")
#define __HTTP_POST_HEAD_FLAGS          _T("InPost=")
#define	__HTTP_POST_CONTENT_TYPE        _T("Content-Type: application/x-www-form-urlencoded")

#define __HTTP_HEADER_BUF_SIZE			64
#define __HTTP_LOCATION_BUF_SIZE		1024
#define __HTTP_LASTMODIFIED_BUF_SIZE	64
#define __HTTP_READ_BUF_SIZE			(4*1024)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL GetIEVersion(LPTSTR lpszVersion, UINT uSize)
{
	//////////////////////////////////////////////////////////////////////////
	TCHAR szFileName[MAX_PATH] = { 0 };
	HMODULE hModule = ::GetModuleHandle(_T("WININET.DLL"));
	DWORD dwHandle = 0;
	if (hModule == 0)
		return FALSE;
	DWORD dwLen = GetModuleFileName(hModule, szFileName, sizeof(szFileName) / sizeof(szFileName[0]));
	if (dwLen <= 0)
		return FALSE;

	dwLen = GetFileVersionInfoSize(szFileName, &dwHandle);
	if (dwLen <= 0)
		return FALSE;
	BYTE *pVersionInfo = new BYTE[dwLen];
	if (!::GetFileVersionInfo(szFileName, dwHandle, dwLen, pVersionInfo))
	{
		delete pVersionInfo;
		return FALSE;
	}

	VS_FIXEDFILEINFO *pFixed = NULL;
	UINT uLen = 0;
	if (!VerQueryValue(pVersionInfo, _T("\\"), (LPVOID*)&pFixed, &uLen))
	{
		delete pVersionInfo;

		return FALSE;
	}

	//wsprintf(lpszVersion, _T("MSIE %d.%d"), verInfo.dwMajorVersion, verInfo.dwMinorVersion);
	wsprintf(lpszVersion,
		_T("MSIE %d.%d"), 
		HIWORD(pFixed->dwFileVersionMS/*dwProductVersionMS*/),
		LOWORD(pFixed->dwFileVersionMS/*dwProductVersionMS*/));	

	delete pVersionInfo;

	return TRUE;
}

BOOL GetOSVersion(LPTSTR lpszVersion, UINT uSize)
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
		{
			*lpszVersion = _T('\0');
			return FALSE;
		}
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:
		{
			wsprintf(lpszVersion, _T("Windows NT %d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion);
		}

		break;

		// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			wsprintf(lpszVersion, _T("Windows 95"));
			
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			wsprintf(lpszVersion, _T("Windows 98"));
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			wsprintf(lpszVersion, _T("Windows Me"));
		} 
		break;

	case VER_PLATFORM_WIN32s:

		wsprintf(lpszVersion, _T("Windows"));
		break;
	}
	return TRUE; 

}
CHttpDownload::CHttpDownload()
{
	m_hInternet				= NULL;
	m_hConnect				= NULL;
	m_hRequest				= NULL;

	m_bServerAlter			= FALSE;
	m_bAuthorization		= FALSE;
	m_bProxyAuthorization	= FALSE;
	m_bUseProxy				= FALSE;
	m_bStop					= FALSE;
	
	m_nServerPort			= INTERNET_DEFAULT_HTTP_PORT;
	m_nProxyPort			= 0;
	m_dwLastError			= S_OK;
	m_hNotifyWnd			= NULL;
	m_nConnectType          = CONNECT_TYPE_IE;
	m_dwFileSize			= 0;

	// _T("Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)")
	TCHAR szIEVer[MAX_PATH] = { 0 };
	GetIEVersion(szIEVer, MAX_PATH);
	TCHAR szOSVer[MAX_PATH] = { 0 };
	GetOSVersion(szOSVer, MAX_PATH);
	m_strUserAgent.Format(_T("Mozilla/4.0 (compatible; %s; %s; Margin)"), szIEVer, szOSVer);
 }

CHttpDownload::~CHttpDownload()
{
	CloseHandles();
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Connect()
//	用  途：初始化，建立session、Http链接（内部使用）
//	对全局变量的影响：无
//	参  数：无
//	返回值：成功－TRUE；失败FALSE
/////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::Connect()
{
	DWORD dwTimeOut = 30*1000;

	if (!m_hInternet)
	{
		if (m_bUseProxy)
		{
			// 若pszProxyName、lpszProxyBypass 为空IE7会返回失败
			m_hInternet = InternetOpen(m_strUserAgent, INTERNET_OPEN_TYPE_PROXY, TEXT("proxy"), TEXT("<local>"), 0L);
		}
		else
			m_hInternet = InternetOpen(m_strUserAgent, m_nConnectType, NULL,	NULL, 0L);
		
		if (NULL == m_hInternet)
		{
			m_dwLastError = ::GetLastError();
			return FALSE;
		}
		
		INTERNET_PROXY_INFO proxyInfo;
		ZeroMemory(&proxyInfo, sizeof(INTERNET_PROXY_INFO));
		TCHAR szProxy[MAX_PATH] = { 0 };
		if (m_bUseProxy)
		{				
			proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
			wsprintf(szProxy, _T("%s:%d"), m_strProxyServer, m_nProxyPort );
			proxyInfo.lpszProxy = szProxy;
			
			if (!InternetSetOption(m_hInternet, INTERNET_OPTION_PROXY, &proxyInfo, sizeof(INTERNET_PROXY_INFO)))
			{
				m_dwLastError = ::GetLastError();
				CloseHandles();
				return FALSE;
			}
		}
		
		if (!InternetSetOption(m_hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD)))
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return FALSE;
		}
	}
	
	if (!m_hConnect)
	{
		if (!m_bAuthorization)
		{
			m_hConnect = InternetConnect(m_hInternet,
				m_strServerName,
				m_nServerPort,
				NULL,
				NULL,
				INTERNET_SERVICE_HTTP,
				0L,
				NULL);
		}
		else
		{
			m_hConnect = InternetConnect(m_hInternet,
				m_strServerName,
				m_nServerPort,
				m_strUsername,
				m_strPassword,
				INTERNET_SERVICE_HTTP,
				0L,
				NULL);
		}

		if (NULL == m_hConnect)
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return FALSE;
		}

		if (ERROR_SUCCESS != ::InternetAttemptConnect(NULL))
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return FALSE;
		}
		
		if (!InternetSetOption(m_hConnect, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD)))
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return FALSE;
		}
		
		if (!InternetSetOption(m_hConnect, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD)))
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return FALSE;
		}

		if (m_bProxyAuthorization)
		{
			if (!InternetSetOption(m_hConnect, 
						INTERNET_OPTION_PROXY_USERNAME,
						(LPVOID)(LPCTSTR)m_strProxyUsername,
						m_strProxyUsername.GetLength() + 1))
			{
				m_dwLastError = ::GetLastError();
				CloseHandles();
				return FALSE;
			}
				
			if (!InternetSetOption(m_hConnect, 
						INTERNET_OPTION_PROXY_PASSWORD,
						(LPVOID)(LPCTSTR)m_strProxyPassword,
						m_strProxyPassword.GetLength() + 1))
			{
				m_dwLastError = ::GetLastError();
				CloseHandles();
				return FALSE;
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL _CreateDirectory(LPCTSTR lpFileName,
//					   BOOL    bIsFile,
//					   LPSECURITY_ATTRIBUTES lpSecurityAttributes)
//	用  途：创建多级路径(内部使用)
//	对全局变量的影响：无
//	参  数：
//		lpFileName		: 待创建的目录
//		bIsFile			: lpFileName是否为目录还是文件－TRUE为文件名；FALSE为目录名
//      lpSecurityAttributes : NULL
//	返回值：成功－TRUE；失败FALSE
/////////////////////////////////////////////////////////////////////////////////
static BOOL _CreateDirectory(LPCTSTR lpFileName,
					   BOOL    bIsFile,
					   LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	
	BOOL	fRet = TRUE;
	TCHAR	szFileName[MAX_PATH] = { 0 };
	TCHAR*	lpChr = NULL;
	TCHAR*	lpHead = NULL;

	_ASSERT(lpFileName != NULL && *lpFileName != _T('\0'));
	if (lpFileName == NULL || lstrlen(lpFileName) <= 0)
		return FALSE;
		
	if (lstrlen(lpFileName) > sizeof(szFileName))
	{
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	
	lstrcpyn(szFileName, lpFileName, sizeof(szFileName)-1);
	if (bIsFile)
		PathRemoveFileSpec(szFileName);
	lpHead = PathSkipRoot(szFileName);
	if (lstrlen(lpHead) == 0)
	{
		CreateDirectory(szFileName, lpSecurityAttributes);
		return TRUE;
	}

	while (true)
	{
		lpChr = _tcschr(lpHead, _T('\\'));

		// zcg+, 增加对路径包含 '/' 的支持. 
		if (NULL == lpChr)
		{
			lpChr = _tcschr(lpHead, _T('/'));
		}

		if (NULL == lpChr)
		{
			fRet &= CreateDirectory(szFileName, lpSecurityAttributes);
			if (!fRet && (GetLastError() == ERROR_ALREADY_EXISTS))
				fRet = TRUE;
			break;
		}
		else
		{
			*lpChr = NULL;
			fRet &= CreateDirectory(szFileName, lpSecurityAttributes);
			if (!fRet && (GetLastError() == ERROR_ALREADY_EXISTS))
				fRet = TRUE;
			*lpChr = _T('\\');
			lpHead = lpChr+1;
		}
	}
	return fRet;
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：HRESULT Download(LPCTSTR lpszURL,
//				LPCTSTR lpszSaveFileName	/*= NULL*/,
//				BOOL bAutoRedirect /*= FALSE*/)
//	用  途：下载入口，调用此函数来开始下载过程
//	对全局变量的影响：无
//	参  数：
//		lpszDownloadUrl   : 待下载的网络URL
//		lpszSaveFileName  : 本地保存文件名(当要得到重定向URL时可以为NULL)	
//      bAutoRedirect     : 是否自动重定向
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
HRESULT CHttpDownload::Download(LPCTSTR lpszURL, LPCTSTR lpszSaveFileName /* = NULL */, 
								BOOL bAutoRedirect /*= FALSE*/, LPCTSTR lpszResetHost/* = NULL*/)
{
	_ASSERT(lpszURL);
	m_strRedirectURL.Empty();		// 清空重定向地址

	// 得到下载的临时文件名，在原有文件的基础上加.rs
	CString strTmpFile = lpszSaveFileName;
	if (!strTmpFile.IsEmpty())
	{
		strTmpFile += _T(".rs");
	}
	
	// 分析URL是否合法
	if (!ParseUrl(lpszURL))
	{
		return ERR_HTTP_INVALID_URL;
	}

	if (m_bServerAlter)
		CloseHandles();
	
	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}
	
	// 初始化消息结构
	HTTPDOWNLOADSTATUS httpDown;
	httpDown.dwFileSize = 0;
	httpDown.dwFileDownloadedSize = 0;
	httpDown.strFileName = m_strObjectName.Mid(m_strObjectName.ReverseFind(_T('/')) + 1);

	httpDown.nStatusType = DOWNLOAD_STATUS_FILENAME;
	// 向调用窗口发送下载文件名消息
	if (!strTmpFile.IsEmpty())
	{
		Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);
	}
	
	// 初始化WinINet句柄，建立连接
	if (!Connect())
	{
		return ERR_HTTP_FAIL;
	}

	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	static LPCTSTR szAcceptType[]= { __HTTP_ACCEPT_TYPE, 0 };
	DWORD dwOpenFlags = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION;
	
	if (!bAutoRedirect)
		dwOpenFlags |= INTERNET_FLAG_NO_AUTO_REDIRECT;

	// Open Request
	m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
					m_strObjectName, HTTP_VERSION, NULL,
					szAcceptType, dwOpenFlags, NULL);
	// ERROR return
	if (NULL == m_hRequest)
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		
		return ERR_HTTP_FAIL;
	}

	TCHAR szHost[272] = { _T('\0') };
	if (lpszResetHost != NULL) 
	{
		// 替换Http头中Host值
		lstrcpy(szHost, _T("Host: "));
		lstrcat(szHost, lpszResetHost);
		//lstrcat(szHost, "\r\n");
		HttpAddRequestHeaders(m_hRequest, szHost, -1, HTTP_ADDREQ_FLAG_ADD );
	}

	TCHAR szHeader[__HTTP_HEADER_BUF_SIZE+1] = { 0 };
	BOOL bSendRange = FALSE;
	
	SYSTEMTIME stLocalFile;						// 本地存在待下载文件时间
	SYSTEMTIME stNetFile;						// 服务器待下载文件时间

	// 默认取当前时间
	GetLocalTime(&stNetFile);

	// 若本地存在待下载临时文件名则得到文件大小写入Htpp头的Range中
	HANDLE hFileDown = NULL;
	if (!strTmpFile.IsEmpty())
	{
		WIN32_FIND_DATA finder = {0};
		HANDLE hFileDown = FindFirstFile(strTmpFile, &finder);
		if (hFileDown != INVALID_HANDLE_VALUE)
		{
			FindClose(hFileDown);
			hFileDown = NULL;
			FileTimeToSystemTime(&finder.ftLastWriteTime, &stLocalFile);

			httpDown.dwFileDownloadedSize  = finder.nFileSizeLow;
			
            if (httpDown.dwFileDownloadedSize > 0 )
            {
                wsprintf(szHeader, _T("Range: bytes=%d-"), finder.nFileSizeLow);
			    bSendRange = TRUE;
            }
            else
            {
                SetFileAttributes(strTmpFile, FILE_ATTRIBUTE_NORMAL);
                DeleteFile(strTmpFile);
            }
		}	
	}

	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	// SEND REQUEST
	if (!::HttpSendRequest(m_hRequest, szHeader, lstrlen(szHeader),	NULL, 0))
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();	
		
		switch (m_dwLastError)
		{
		case ERROR_INTERNET_CANNOT_CONNECT:
		case ERROR_INTERNET_NAME_NOT_RESOLVED:
			return ERR_HTTP_NET_ERROR;
			break;
		case ERROR_INTERNET_TIMEOUT:
			return ERR_HTTP_SEND_TIMEOUT;
		default:
			{
			}
			return ERR_HTTP_FAIL;;
		}
	}	
	
	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	DWORD dwCode = 0;
	DWORD dwSize = sizeof(DWORD) ;
	if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		return ERR_HTTP_FAIL;
	}
	
	BOOL bSendAgain = FALSE;
	switch (dwCode)
	{
	// 重定向
	case HTTP_STATUS_REDIRECT:
	case HTTP_STATUS_MOVED:
		{
			TCHAR* pszLocation = new TCHAR[__HTTP_LOCATION_BUF_SIZE + 1];
			ZeroMemory(pszLocation, __HTTP_LOCATION_BUF_SIZE +1 );
			dwSize = __HTTP_LOCATION_BUF_SIZE;
			if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_LOCATION, pszLocation, &dwSize, NULL))
			{
				delete pszLocation;
				m_dwLastError = ::GetLastError();
				CloseHandles();

				return ERR_HTTP_FAIL;
			}

			m_strRedirectURL = pszLocation;

			CloseHandles();
			delete pszLocation;
			
			return ERR_HTTP_REDIRECT;
		}
	// 200 OK
	case HTTP_STATUS_OK:
		{
			if (strTmpFile.IsEmpty())
			{
				CloseHandles();
				return ERR_HTTP_DEST_FILENAME_EMPTY;
			}
			else
			{
				// 尝试删除上次下载中断的文件
				DeleteFile(strTmpFile);
			}
		}
		break;
	case HTTP_STATUS_NOT_FOUND:
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return ERR_HTTP_NOT_FIND;
		}
		break;

	case HTTP_STATUS_FORBIDDEN:
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return ERR_HTTP_FORBIDDEN_ACCESS;
		}
	// 断点请求返回
	case HTTP_STATUS_PARTIAL:
	case HTTP_STATUS_PARTIAL_CONTENT:
		{
			TCHAR szLastModified[__HTTP_LASTMODIFIED_BUF_SIZE + 1] = { 0 };
			dwSize = __HTTP_LASTMODIFIED_BUF_SIZE;
			if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_LAST_MODIFIED, szLastModified, &dwSize, NULL))
			{
				m_dwLastError = ::GetLastError();
                DeleteFile(strTmpFile);
                bSendAgain = TRUE;
                httpDown.dwFileDownloadedSize = 0;
				CloseHandles();
				return ERR_HTTP_FAIL;
			}
			HttpDateToSystemTime(szLastModified, stNetFile);

			if (stNetFile.wSecond != stLocalFile.wSecond
				|| stNetFile.wMinute != stLocalFile.wMinute
				|| stNetFile.wHour != stLocalFile.wHour
				|| stNetFile.wDay != stLocalFile.wDay
				|| stNetFile.wMonth != stLocalFile.wMonth
				|| stNetFile.wYear != stLocalFile.wYear)
			{
				// 删除上次下载中断的文件
				DeleteFile(strTmpFile);
				bSendAgain = TRUE;
				httpDown.dwFileDownloadedSize = 0;
			}
		}
		break;
	case HTTP_STATUS_SERVER_ERROR:
		{
			m_dwLastError = ::GetLastError();
			return ERR_HTTP_SERVER_ERROR;
		}		
		break;
	default:
		if (bSendRange)
		{
			bSendAgain = TRUE;
			break;
		}
		else
		{
			m_dwLastError = ::GetLastError();
			return ERR_HTTP_FAIL;
		}
	}
	
	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	// 重新发送请求
	if (bSendAgain)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
						m_strObjectName, HTTP_VERSION, NULL,
						szAcceptType, dwOpenFlags, NULL);		
		if (NULL == m_hRequest)
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return ERR_HTTP_FAIL;
		}

		if (lpszResetHost != NULL)
			HttpAddRequestHeaders(m_hRequest, szHost, -1, HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON);

		if (m_bStop)
		{
			CloseHandles();
			return ERR_HTTP_USER_STOP;
		}
		// SEND REQUEST
		if (!::HttpSendRequest(m_hRequest, NULL, 0L, NULL,0L))
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return ERR_HTTP_FAIL;
		}

		if (m_bStop)
		{
			CloseHandles();
			return ERR_HTTP_USER_STOP;
		}
		
		dwSize = sizeof(DWORD);
		if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return ERR_HTTP_FAIL;
		}

		if (dwCode != HTTP_STATUS_OK)
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			return ERR_HTTP_FAIL;
		}
	}

	TCHAR szLastModified[__HTTP_LASTMODIFIED_BUF_SIZE + 1] = { 0 };
	dwSize = __HTTP_LASTMODIFIED_BUF_SIZE;
	// 得到服务器文件的最后修改时间,可能会得不到
	if (HttpQueryInfo(m_hRequest, HTTP_QUERY_LAST_MODIFIED, szLastModified, &dwSize, NULL))
	{
		HttpDateToSystemTime(szLastModified, stNetFile);
	}

	DWORD dwContentLength = 0;
	dwSize = sizeof(DWORD);
	// 得到服务器文件的大小
	if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLength, &dwSize, NULL))
	{
		// 有些及其个别机器，返回错误，但可以下载文件，此时默认200K
		dwContentLength = 200 * 1024;
		httpDown.dwFileDownloadedSize = 0;
	}

	//httpDown.dwFileSize = dwContentLength;
	// 短点续传得到的大小为剩下的大小
	httpDown.dwFileSize = dwContentLength + httpDown.dwFileDownloadedSize;
	m_dwFileSize = httpDown.dwFileSize;
	
	httpDown.nStatusType = DOWNLOAD_STATUS_FILESIZE;
	// 向下载窗口发送下载文件大小消息
	if (!strTmpFile.IsEmpty())
	{
		Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);
	}
	
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}
	// 创建下载目录
	if (FALSE == _CreateDirectory(strTmpFile, TRUE, NULL))
	{
		m_dwLastError = GetLastError();
		CloseHandles();
		return ERR_HTTP_CREATE_FILE_FAIL;
	}
	// 创建下载临时文件
	hFileDown = CreateFile(strTmpFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileDown == INVALID_HANDLE_VALUE)
	{
		m_dwLastError = GetLastError();
		CloseHandles();
		return ERR_HTTP_CREATE_FILE_FAIL;
	}

	FILETIME ftWrite;			
	SystemTimeToFileTime(&stNetFile, &ftWrite);
	SetFileTime(hFileDown, NULL, NULL, &ftWrite);

	if (bSendAgain)
	{
		SetFilePointer(hFileDown, 0L, 0L, FILE_BEGIN);
		SetEndOfFile(hFileDown);
	}
	else
	{
		SetFilePointer(hFileDown, 0L, 0L, FILE_END);
	}
		
							// 本地文件写时间
	CHAR* pszData = new CHAR[__HTTP_READ_BUF_SIZE + 1]; // 读文件缓冲区
	DWORD dwRead = 0;

	httpDown.nStatusType = DOWNLOAD_STATUS_DOWNLOADEDSIZE;
	do 
	{
		ZeroMemory(pszData, __HTTP_READ_BUF_SIZE + 1);
		if (m_bStop)
		{
			// 调用者中止了下载
			delete pszData;
			pszData = NULL;
			CloseHandles();

			FlushFileBuffers(hFileDown);
			SetEndOfFile(hFileDown);
			
			// 将本地下载的文件的修改时间修改为与服务器文件时间一致
			SystemTimeToFileTime(&stNetFile, &ftWrite);
			SetFileTime(hFileDown, NULL, NULL, &ftWrite);
			CloseHandle(hFileDown);
			hFileDown = NULL;

			return ERR_HTTP_USER_STOP;
		}

		// 超时或其他错误
		if (!InternetReadFile(m_hRequest, pszData, __HTTP_READ_BUF_SIZE, &dwRead))
		{
			m_dwLastError = ::GetLastError();
			// 调用者中止了下载
			delete pszData;
			pszData = NULL;

			m_dwLastError = ::GetLastError();
			CloseHandles();

			FlushFileBuffers(hFileDown);
			SetEndOfFile(hFileDown);
			
			// 将本地下载的文件的修改时间修改为与服务器文件时间一致
			SystemTimeToFileTime(&stNetFile, &ftWrite);
			SetFileTime(hFileDown, NULL, NULL, &ftWrite);
			CloseHandle(hFileDown);
			hFileDown = NULL;

			return ERR_HTTP_RECEIVE_TIMEOUT;
		}

		httpDown.dwFileDownloadedSize += dwRead;
		
		// 向调用者窗口发送下载大小消息
		Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);

		if (dwRead > 0)
		{
			DWORD dwWrited = 0;
			WriteFile(hFileDown, pszData, dwRead, &dwWrited, NULL);
		}
	}while (dwRead > 0);

	httpDown.nStatusType = DOWNLOAD_STATUS_DOWNLOADCOMPLETE;
	Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);
	
	// 没有下载完成
	if ( httpDown.dwFileDownloadedSize < httpDown.dwFileSize )
	{
		CloseHandles();

		delete pszData;
		pszData = NULL;

		FlushFileBuffers(hFileDown);
		SetEndOfFile(hFileDown);

		// 将本地下载的文件的修改时间修改为与服务器文件时间一致
		SystemTimeToFileTime(&stNetFile, &ftWrite);
		SetFileTime(hFileDown, NULL, NULL, &ftWrite);
		CloseHandle(hFileDown);	
		hFileDown = NULL;

		return ERR_HTTP_FAIL;
	}
	
	// httpDown.dwFileDownloadedSize = httpDown.dwFileSize;
	//Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);

	TCHAR szConnection[__HTTP_HEADER_BUF_SIZE + 1] = { 0 };
	dwSize = __HTTP_HEADER_BUF_SIZE;
	if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONNECTION, szConnection, &dwSize, NULL))
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
	}
	else
	{
		if ( 0 ==lstrcmpi(szConnection, _T("close")) )
		{
			CloseHandles();
		}
		else
		{
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
	}	

	delete pszData;
	pszData = NULL;

	FlushFileBuffers(hFileDown);
	SetEndOfFile(hFileDown);

	// 将本地下载的文件的修改时间修改为与服务器文件时间一致
	SystemTimeToFileTime(&stNetFile, &ftWrite);
	SetFileTime(hFileDown, NULL, NULL, &ftWrite);
	CloseHandle(hFileDown);	
	hFileDown = NULL;
	
	// 若指定下载存储文件的文件存在，删除此文件，将下载文件由临时文件为指定存储文件
	if (PathFileExists(lpszSaveFileName))
	{
		SetFileAttributes(lpszSaveFileName, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(lpszSaveFileName);
	}
	if (!MoveFile(strTmpFile, lpszSaveFileName))
	{
		return ERR_HTTP_RENAME_FILE_FAIL;
	};

	return ERR_HTTP_SUCCESS;
}

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
HRESULT CHttpDownload::PostData(LPCTSTR lpszURL, LPVOID lpBuffer, DWORD cbBuffer,
				LPBYTE lpData, LPDWORD lpcbData, BOOL bAddPostHeadFlag)
{
	m_strRedirectURL.Empty();
	_ASSERT(lpszURL);
	if (!ParseUrl(lpszURL))
		return ERR_HTTP_INVALID_URL;

	// 初始化WinINet句柄，建立连接
	if (!Connect())
	{
		return ERR_HTTP_FAIL;
	}

	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	static LPCTSTR szAcceptType[]= { __HTTP_ACCEPT_TYPE, 0 };
	DWORD dwOpenFlags = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION;
	//if (!bAutoRedirect)
	dwOpenFlags |= INTERNET_FLAG_NO_AUTO_REDIRECT;

	m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_POST, m_strObjectName, 
			NULL, NULL, szAcceptType, dwOpenFlags, 0);

	// ERROR return
	if (NULL == m_hRequest)
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		return ERR_HTTP_FAIL;
	}
	
	DWORD dwSendBufferCount = cbBuffer;
	if (bAddPostHeadFlag)
		dwSendBufferCount += lstrlen(__HTTP_POST_HEAD_FLAGS);
	LPVOID lpSendBuffer = new BYTE[dwSendBufferCount];

	ZeroMemory(lpSendBuffer, dwSendBufferCount);
	if (bAddPostHeadFlag)
	{
		memcpy(lpSendBuffer, __HTTP_POST_HEAD_FLAGS, lstrlen(__HTTP_POST_HEAD_FLAGS));
		memcpy( (LPBYTE)lpSendBuffer+lstrlen(__HTTP_POST_HEAD_FLAGS), lpBuffer, cbBuffer);
	}
	else
	{
		memcpy( (LPBYTE)lpSendBuffer, lpBuffer, cbBuffer);
	}
	
    if	(!HttpSendRequest( m_hRequest, __HTTP_POST_CONTENT_TYPE, lstrlen(__HTTP_POST_CONTENT_TYPE), lpSendBuffer, dwSendBufferCount))
    {
		delete lpSendBuffer;
        m_dwLastError = ::GetLastError();
		CloseHandles();
		if (m_dwLastError == ERROR_INTERNET_TIMEOUT)
		{
			return ERR_HTTP_SEND_TIMEOUT;
		}
		else
			return ERR_HTTP_FAIL;
    }
	delete lpSendBuffer; 
	lpSendBuffer = NULL;

	DWORD dwCode = 0;
	DWORD dwSize = sizeof(DWORD) ;
	if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		return ERR_HTTP_FAIL;
	}
	
	switch (dwCode)
	{
	// 重定向
	case HTTP_STATUS_REDIRECT:
	case HTTP_STATUS_MOVED:
		{
			TCHAR szLocation[__HTTP_LOCATION_BUF_SIZE + 1] = { 0 };
			dwSize = __HTTP_LOCATION_BUF_SIZE;
			if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_LOCATION, szLocation, &dwSize, NULL))
			{
				m_dwLastError = ::GetLastError();
				CloseHandles();
				return ERR_HTTP_FAIL;
			}

			m_strRedirectURL = szLocation;
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
			return ERR_HTTP_REDIRECT;
		}
	// 200 OK
	case HTTP_STATUS_OK:
		break;
	default:
		{
			CloseHandles();
			return ERR_HTTP_FAIL;
		}
	}
	
	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	// 读取放回数据到指定BUFFER中
	CHAR pcReadBuffer[__HTTP_READ_BUF_SIZE];
	ZeroMemory(pcReadBuffer, sizeof( pcReadBuffer ));
	DWORD dwBytesRead = 0;

	LPBYTE pByte = (LPBYTE)lpData;			// 记录读取位置指针
	DWORD dwDataMaxCount = *lpcbData;       // 保存传入BUFFER大小
	*lpcbData = 0L;
	do
	{	
		dwBytesRead = 0;
		// 调用者中止
		if (m_bStop)
		{
			CloseHandles();
			return ERR_HTTP_USER_STOP;
		}
		if (InternetReadFile(m_hRequest, pcReadBuffer, __HTTP_READ_BUF_SIZE-1, &dwBytesRead))
		{
			if (dwBytesRead && ((dwBytesRead + *lpcbData) <= dwDataMaxCount))
			{
				memcpy(pByte, pcReadBuffer, dwBytesRead);
				
				pByte += dwBytesRead;
				*lpcbData += dwBytesRead;
			}
			else
			{
				*lpcbData += dwBytesRead;
			}
		}
		else
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			
			return ERR_HTTP_FAIL;
		}
		
	}while (dwBytesRead>0);
		
	*(++pByte) = _T('\0');
	InternetCloseHandle(m_hRequest);
	m_hRequest = NULL;

	if ( *lpcbData > dwDataMaxCount )
		return ERR_HTTP_BUFFER_TOO_SMALL;

	return ERR_HTTP_SUCCESS;
}

HRESULT CHttpDownload::GetContent(LPCTSTR lpszURL, LPVOID lpContent, 
								  LPDWORD lpcbContent, BOOL bAutoRedirect)
{
	m_strRedirectURL.Empty();
	_ASSERT(lpszURL);
	if (!ParseUrl(lpszURL))
		return ERR_HTTP_INVALID_URL;

	// 初始化WinINet句柄，建立连接
	if (!Connect())
	{
		return ERR_HTTP_FAIL;
	}

	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	static LPCTSTR szAcceptType[]= { __HTTP_ACCEPT_TYPE, 0 };
	DWORD dwOpenFlags = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION;
	
	if (!bAutoRedirect)
		dwOpenFlags |= INTERNET_FLAG_NO_AUTO_REDIRECT;

	// Open Request
	m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
					m_strObjectName, HTTP_VERSION, NULL,
					szAcceptType, dwOpenFlags, NULL);
	// ERROR return
	if (NULL == m_hRequest)
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		return ERR_HTTP_FAIL;
	}
	
	// SEND REQUEST
	if (!::HttpSendRequest(m_hRequest, NULL, 0,	NULL, 0))
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		
		switch (m_dwLastError)
		{
		case ERROR_INTERNET_CANNOT_CONNECT:
		case ERROR_INTERNET_NAME_NOT_RESOLVED:
			return ERR_HTTP_NET_ERROR;
		case ERROR_INTERNET_TIMEOUT:
			return ERR_HTTP_SEND_TIMEOUT;
		default:
			return ERR_HTTP_FAIL;;
		}
	}

	DWORD dwCode = 0;
	DWORD dwSize = sizeof(DWORD) ;
	if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
	{
		m_dwLastError = ::GetLastError();
		CloseHandles();
		return ERR_HTTP_FAIL;
	}
	
	switch (dwCode)
	{
	// 重定向
	case HTTP_STATUS_REDIRECT:
	case HTTP_STATUS_MOVED:
		{
			TCHAR szLocation[__HTTP_LOCATION_BUF_SIZE + 1] = { 0 };
			dwSize = __HTTP_LOCATION_BUF_SIZE;
			if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_LOCATION, szLocation, &dwSize, NULL))
			{
				m_dwLastError = ::GetLastError();
				CloseHandles();
				return ERR_HTTP_FAIL;
			}

			m_strRedirectURL = szLocation;
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
			return ERR_HTTP_REDIRECT;
		}
	// 200 OK
	case HTTP_STATUS_OK:
		break;
	default:
		{
			CloseHandles();
			return ERR_HTTP_FAIL;
		}
	}
	
	// 调用者中止
	if (m_bStop)
	{
		CloseHandles();
		return ERR_HTTP_USER_STOP;
	}

	// 读取放回数据到指定BUFFER中
	CHAR pcReadBuffer[__HTTP_READ_BUF_SIZE];
	ZeroMemory(pcReadBuffer, sizeof( pcReadBuffer ));
	DWORD dwBytesRead = 0;

	LPBYTE pByte = (LPBYTE)lpContent;			// 记录读取位置指针
	DWORD dwDataMaxCount = *lpcbContent;       // 保存传入BUFFER大小
	*lpcbContent = 0L;
	do
	{	
		dwBytesRead = 0;
		// 调用者中止
		if (m_bStop)
		{
			CloseHandles();
			return ERR_HTTP_USER_STOP;
		}

		if (InternetReadFile(m_hRequest, pcReadBuffer, __HTTP_READ_BUF_SIZE-1, &dwBytesRead))
		{
			if (dwBytesRead > 0) 
			{
				if ((dwBytesRead + *lpcbContent) <= dwDataMaxCount)
				{
					memcpy(pByte, pcReadBuffer, dwBytesRead);
					
					pByte += dwBytesRead;
					*lpcbContent += dwBytesRead;
				}
				else	// 缓冲区太小，读满缓冲区为止
				{
					DWORD dwRemain = dwDataMaxCount - *lpcbContent;
					memcpy(pByte, pcReadBuffer, dwRemain);
					*lpcbContent += dwRemain;
					break;
				}
			}
		}
		else
		{
			m_dwLastError = ::GetLastError();
			CloseHandles();
			
			return ERR_HTTP_FAIL;
		}
		
	}while (dwBytesRead>0);

	InternetCloseHandle(m_hRequest);
	m_hRequest = NULL;

	if (dwBytesRead > 0)	// 因为缓冲区太小而停止读文件
		return ERR_HTTP_BUFFER_TOO_SMALL;

	return ERR_HTTP_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL ParseUrl(LPCTSTR lpszUrl)
//	用  途：分析输入URL的合法性及拆分URL(内部使用)
//	对全局变量的影响：无
//	参  数：
//		lpszUrl		: 待拆分的URL
//	返回值：成功－TRUE；失败FALSE
/////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::ParseUrl(LPCTSTR lpszUrl)
{
	if (NULL == lpszUrl || lstrlen(lpszUrl) <=0)
		return FALSE;

	CSplitURL url;
	
	if (!url.Split(lpszUrl))
	{
		return FALSE;
	}

	CString strHostName;
	strHostName = url.GetHostName();

	// 查看服务器地址是否改变
	if (m_strServerName.CompareNoCase(strHostName) != 0)
	{
		m_bServerAlter = TRUE;
	}
	else
	{
		m_bServerAlter = FALSE;
	}
	m_strServerName = strHostName;
	
	/*
	CString strUserName, strPassword
	strUserName = url.GetUserName();
	strPassword = url.GetPassword();
	if (!strUserName.IsEmpty())
	{
		SetAuthorization(strUserName, strPassword);
	}

	//*/
	m_nServerPort = url.GetPort() ? url.GetPort() :INTERNET_DEFAULT_HTTP_PORT;

	m_strObjectName.Format(_T("%s%s"), url.GetURLPath(), url.GetExtraInfo()); 

	return TRUE;
}

// 字符传转化为数字串(内部使用)
static int IntVal(CString strVal)
{
	int nVal = 0;
	strVal.TrimLeft();
	for(int ndx = 0; ndx < strVal.GetLength(); ++ndx)
		nVal = nVal*10 + strVal.GetAt(ndx) - _T('0');

	return nVal;
}

// 转化月份为数字(内部使用)
static int MonthFromStr(const CString& str)
{
	LPCTSTR aMonths[] = {
		_T("xxx"), _T("jan"), _T("feb"), _T("mar"), _T("apr"), _T("may"), _T("jun"),
		_T("jul"), _T("aug"), _T("sep"), _T("oct"), _T("nov"), _T("dec") };
	int nMonth = 1;
	for(nMonth = 1; nMonth <= 12; ++nMonth)
	{
		if (str.CompareNoCase(aMonths[nMonth]) == 0)
			break;
	}

	return nMonth;
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL HttpDateToSystemTime(LPCTSTR lpszTime, SYSTEMTIME &st)
//	用  途：转换网络时间到系统时间(内部使用)
//	对全局变量的影响：无
//	参  数：
//		lpszTime		: HTTP格式系统时间
//      st			    : 被转化的系统时间
//	返回值：成功－TRUE；失败FALSE
//  说明：// Dow, dd Mon year hh:mm:ss GMT
/////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::HttpDateToSystemTime(LPCTSTR lpszTime, SYSTEMTIME &st)
{
	ZeroMemory(&st, sizeof(SYSTEMTIME));
		
	CString strHttp = lpszTime;
	// assume we couldn't get a good time conversion....
	BOOL bOk = FALSE;

	int ndx = 0;
	switch(strHttp.GetAt(3))
	{
	case ',':
		// read RFC-1123 (preferred)....
		st.wDay = IntVal(strHttp.Mid(5,2));
		st.wMonth = MonthFromStr(strHttp.Mid(8,3));
		st.wYear = IntVal(strHttp.Mid(12,4));
		st.wHour = IntVal(strHttp.Mid(17,2));
		st.wMinute = IntVal(strHttp.Mid(20,2));
		st.wSecond = IntVal(strHttp.Mid(23,2));
		break;
	case ' ':
		// read ANSI-C time format....
		st.wDay = IntVal(strHttp.Mid(8,2));
		st.wMonth = MonthFromStr(strHttp.Mid(4,3));
		st.wYear = IntVal(strHttp.Mid(20,4));
		st.wHour = IntVal(strHttp.Mid(11,2));
		st.wMinute = IntVal(strHttp.Mid(14,2));
		st.wSecond = IntVal(strHttp.Mid(17,2));
		break;
	default:
		if ((ndx = strHttp.Find(_T(", "))) != -1)
		{
			st.wDay = IntVal(strHttp.Mid(ndx+2,2));
			st.wMonth = MonthFromStr(strHttp.Mid(ndx+5,3));
			st.wYear = IntVal(strHttp.Mid(ndx+9,2));
			st.wHour = IntVal(strHttp.Mid(ndx+12,2));
			st.wMinute = IntVal(strHttp.Mid(ndx+15,2));
			st.wSecond = IntVal(strHttp.Mid(ndx+18,2));
			// add the correct century....
			st.wYear += (st.wYear > 50)?1900:2000;
		}
		break;
	}
	// if year not zero....
	if (st.wYear != 0)
	{
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SetProxy(LPCTSTR lpszProxyServer,
//					USHORT nProxyPort, 
//					BOOL bUseProxy /* =TRUE */,
//					BOOL bProxyAuthorization /* =FALSE */,
//					LPCTSTR lpszProxyUsername /* =NULL */,
//					LPCTSTR lpszProxyPassword /* =NULL */)
//	用  途：设置代理服务器访问参数
//	对全局变量的影响：无
//	参  数：
//		lpszProxyServer		: 代理服务器名
//      nProxyPort			: 代理服务器端口
//		bUseProxy			: 是否使用代理(此值为FALSE时，前两个参数将被忽略)
//		bProxyAuthorization : 代理服务器是否需要身份验证
//		lpszProxyUsername   : 代理服务器身份验证用户名
//		lpszProxyUsername   : 代理服务器身份验证密码
//	返回值：成功－TRUE；失败FALSE
/////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::SetProxy(LPCTSTR lpszProxyServer,
							USHORT nProxyPort, 
							BOOL bUseProxy /* =TRUE */,
							BOOL bProxyAuthorization /* =FALSE */,
							LPCTSTR lpszProxyUsername /* =NULL */,
							LPCTSTR lpszProxyPassword /* =NULL */)
{
	m_bUseProxy				= bUseProxy;
	m_strProxyServer		= lpszProxyServer;
	m_nProxyPort			= nProxyPort;
	m_bProxyAuthorization	= bProxyAuthorization;
	m_strProxyUsername		= lpszProxyUsername;
	m_strProxyPassword		= lpszProxyPassword;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：void SetAuthorization(
//				LPCTSTR lpszUsername,
//				LPCTSTR lpszPassword,
//				BOOL bAuthorization/* = TRUE*/) 
//	用  途：设置WWW认证信息(访问被保护的页面时需要)
//	对全局变量的影响：无
//	参  数：
//		lpszUsername   : 访问页面的用户名
//		lpszPassword   : 口令
//		bAuthorization : 是否需要验证(此值为FALSE时，前两个参数将被忽略)
//	返回值：void
////////////////////////////////////////////////////////////////////////////////
void CHttpDownload::SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization/* = TRUE*/)
{
	if(bAuthorization && lpszUsername != NULL)
	{
		m_bAuthorization = TRUE;
		m_strUsername	 = lpszUsername;
		m_strPassword	 = lpszPassword;
	}
	else
	{
		m_bAuthorization = FALSE;
		m_strUsername	 = _T("");
		m_strPassword	 = _T("");
	}
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL CloseHandles(void)
//	用  途：关闭下载所需所有的INTERNET句柄（内部使用）
//	对全局变量的影响：无
//	参  数：无
//	返回值：成功-TRUE，失败-FALSE
/////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::CloseHandles()
{
	if (m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}

	if (m_hConnect)
	{
		InternetCloseHandle(m_hConnect);
		m_hConnect = NULL;
	}

	if (m_hInternet)
	{
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SetNotifyHwnd(HWND hNotifyWnd)
//	用  途：设置接收下载消息的窗口句柄
//	对全局变量的影响：无
//	参  数：hNotifyWnd : 接收下载消息的窗口句柄
//	返回值：合法窗口为-TRUE，否则-FALSE
/////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::SetNotifyHwnd(HWND hNotifyWnd)
{
	// hNotifyWnd 是有效窗口句柄
	if (hNotifyWnd && IsWindow(hNotifyWnd))
	{
		m_hNotifyWnd = hNotifyWnd;
		return TRUE;
	}
	else
	{
		m_hNotifyWnd = NULL;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：void StopDownload()
//	用  途：停止下载
//	对全局变量的影响：无
//	参  数：无
//	返回值：无
/////////////////////////////////////////////////////////////////////////////////
void CHttpDownload::StopDownload()
{
	m_bStop = TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：LPCTSTR GetRedirectURL()
//	用  途：得到重定向后的ＵＲＬ
//	对全局变量的影响：无
//	参  数：无
//	返回值：URL字符串
//  说明  : 当调用Download后返回值为ERR_HTTP_REDIRECT，再调用此函数得到重定向URL
/////////////////////////////////////////////////////////////////////////////////
LPCTSTR CHttpDownload::GetRedirectURL()
{
	return m_strRedirectURL;
}

//////////////////////////////////////////////////////////////////////////////////
// 
// 
BOOL CHttpDownload::Notify(int msg, WPARAM wp, LPARAM lp)
{
	if (m_hNotifyWnd && IsWindow(m_hNotifyWnd))
	{
		return SendMessage(m_hNotifyWnd, msg, wp, lp) != NULL ? TRUE : FALSE;
	}

	//switch(msg)
	//{
	//case WM_HTTPDOWN_NOTIFY_MSG:
	//	if(lp != NULL)
	//	{
	//		HTTPDOWNLOADSTATUS* ds = (HTTPDOWNLOADSTATUS*)lp;
	//		switch(ds->nStatusType)
	//		{
	//		case DOWNLOAD_STATUS_FILENAME:
	//			break;

	//		case DOWNLOAD_STATUS_FILESIZE:
	//			// here, may get 'filename' and 'filesize'
	//			break;
	//		}
	//	}
	//	break;
	//}

	return TRUE;
}
