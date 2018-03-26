#/************************************************************************/
/* 
Author:

lourking. .All rights reserved.

Create Time:

	4,1th,2014

Module Name:

	dsHftpInterface.h  

Abstract: 内部网络任务


*/
/************************************************************************/
#include <wininet.h>
#include <string>
#include <iostream>
using namespace std;

#pragma comment(lib, "wininet.lib")

//下载
#define  DOWNHELPER_AGENTNAME         _T("ds_lourking")
#define  LEN_OF_BUFFER_FOR_QUERYINFO  128 

class dsHftpInterface;

typedef LRESULT (CALLBACK *SPEEDPROC)(dsHftpInterface* /*phi*/, BOOL /*bRetPerAPI*/, DWORD /*dwDownBytesCount*/, LPVOID /*lpParam*/, DWORD /*dwLastError*/);


typedef struct dsLittlePieceInfo
{
	BOOL bRet;
	DWORD dwDownBytesCount;
	DWORD dwLastError;

	inline void SetInfo(BOOL _bRet, DWORD _dwDownBytesCount, DWORD _dwLastError){
		bRet = _bRet;
		dwDownBytesCount = _dwDownBytesCount;
		dwLastError = _dwLastError;
	}

}DSLPI,*PDSLPI;

class dsHftpInterface
{
public:
	DWORD m_dwDownloadSizePerRead;
	DWORD m_dwMaxDownloadSize;
	UINT m_uRequestTryTimes;
	UINT m_uTimeOut; 
	HINTERNET m_hRequestGet;//HTTP Request
	HINTERNET m_hRequestHead;//HTTP Request
	HINTERNET m_hConnect;//HTTP连接
	HINTERNET m_hInet;//打开internet连接handle


public:
	dsHftpInterface():
		m_dwDownloadSizePerRead(1024),
		m_dwMaxDownloadSize(1000*1024*1024),
		m_uRequestTryTimes(20),
		m_hInet(NULL),
		m_hConnect(NULL),
		m_hRequestGet(NULL),
		m_hRequestHead(NULL),
		m_uTimeOut(100)
	{

	}

	~dsHftpInterface(){

	}
public:

	BOOL Init();
	void Close(){
		if (m_hRequestGet)
			InternetCloseHandle(m_hRequestGet);
		if (m_hRequestHead)
			InternetCloseHandle(m_hRequestHead);
		if (m_hConnect)
			InternetCloseHandle(m_hConnect);
		if (m_hInet)
			InternetCloseHandle(m_hInet);

	}

public:
	//HTTP下载函数，通过先请求HEAD的方式然后GET，可以通过HEAD对下载的文件类型和大小做限制
	BOOL Init(LPCTSTR lpszUrl, __out DWORD *pdwBufSize, DWORD dwOffset = 0)//, byte *pbufForRecv, UINT uBufSize)
	{
		BOOL bRet = FALSE;

		if(NULL == lpszUrl || 0 == _tcslen(lpszUrl) /*|| NULL == pbufForRecv || uBufSize <= 0*/)
			return FALSE;

		char* pBuf = NULL; //缓冲区
		DWORD dwDownBytes = 0; //每次下载的大小
		char bufQueryInfo[LEN_OF_BUFFER_FOR_QUERYINFO] = {0}; //用来查询信息的buffer
		DWORD dwBufQueryInfoSize = sizeof(bufQueryInfo);
		DWORD dwStatusCode = 0;
		DWORD dwContentLen = 0;
		DWORD dwSizeDW = sizeof(DWORD);

		//分割URL
		TCHAR pszHostName[INTERNET_MAX_HOST_NAME_LENGTH] = {0};
		TCHAR pszUserName[INTERNET_MAX_USER_NAME_LENGTH] = {0};
		TCHAR pszPassword[INTERNET_MAX_PASSWORD_LENGTH] = {0};
		TCHAR pszURLPath[INTERNET_MAX_URL_LENGTH] = {0};
		TCHAR szURL[INTERNET_MAX_URL_LENGTH] = {0};

		URL_COMPONENTS urlComponents = {0};
		urlComponents.dwStructSize = sizeof(URL_COMPONENTSA);
		urlComponents.lpszHostName = pszHostName;
		urlComponents.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
		urlComponents.lpszUserName = pszUserName;
		urlComponents.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
		urlComponents.lpszPassword = pszPassword;
		urlComponents.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
		urlComponents.lpszUrlPath = pszURLPath;
		urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;

		bRet = InternetCrackUrl(lpszUrl, 0, NULL, &urlComponents);
		bRet = (bRet && urlComponents.nScheme == INTERNET_SERVICE_HTTP);
		if (!bRet)
		{
			cout<<"解析域名失败"<<endl;
			goto _END_OF_DOWNLOADURL;
		}

		//打开一个internet连接
		m_hInet = InternetOpen(DOWNHELPER_AGENTNAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
		if (!m_hInet)
		{
			cout<<"打开internet失败"<<endl;
			bRet = FALSE;
			goto _END_OF_DOWNLOADURL;
		}

		//打开HTTP连接
		m_hConnect = InternetConnect(m_hInet, pszHostName, urlComponents.nPort, pszUserName, pszPassword, INTERNET_SERVICE_HTTP, 0, NULL);
		if (!m_hConnect)
		{
			cout<<"打开连接失败"<<endl;
			bRet = FALSE;
			goto _END_OF_DOWNLOADURL;
		}

		//创建HTTP request句柄
		if (urlComponents.dwUrlPathLength !=  0)
			_tcscpy(szURL, urlComponents.lpszUrlPath);
		else
			_tcscpy(szURL, _T("/"));

		//请求HEAD，通过HEAD获得文件大小及类型进行校验
		m_hRequestHead = HttpOpenRequest(m_hConnect, _T("HEAD"), szURL, _T("HTTP/1.1"), _T(""), NULL, INTERNET_FLAG_RELOAD, 0);
		bRet = _TryHttpSendRequest(m_hRequestHead, m_uRequestTryTimes);
		if (!bRet)
		{
			cout<<"请求HEAD失败"<<endl;
			goto _END_OF_DOWNLOADURL; //请求HEAD失败
		}

		//查询content-length大小
		dwContentLen = 0;
		dwSizeDW = sizeof(DWORD);
		bRet = HttpQueryInfo(m_hRequestHead, HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_CONTENT_LENGTH, &dwContentLen, &dwSizeDW, NULL);
		if (bRet)
		{

			if(NULL != pdwBufSize)
				*pdwBufSize = dwContentLen;

			//检查是否文件过大
			if (dwContentLen > m_dwMaxDownloadSize)
			{
				cout<<"文件过大"<<endl;
				bRet = FALSE;
				goto _END_OF_DOWNLOADURL;
			}
		}

		//校验完成后再请求GET，下载文件
		m_hRequestGet = HttpOpenRequest(m_hConnect, _T("GET"), szURL, _T("HTTP/1.1"), _T(""), NULL, INTERNET_FLAG_DONT_CACHE, 2);
		

		if(0 != dwOffset)
		{
			WCHAR szOffset[MAX_PATH];
			wsprintf (szOffset, L"Range:bytes=%d-\r\n",dwOffset);
			HttpAddRequestHeaders(m_hRequestGet,szOffset,-1,HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);
		}
		

		bRet = HttpSendRequest(m_hRequestGet, NULL, 0, 0, 0);

		char szBuf[10] = {0};

		DWORD dwRecv = 0;
		

		//bRet = _TryHttpSendRequest(m_hRequestGet, m_uRequestTryTimes);
		//bRet = InternetReadFile(m_hRequestGet, szBuf, 10, &dwRecv);
		if (!bRet)
		{
			cout<<"请求HEAD失败"<<endl;
			goto _END_OF_DOWNLOADURL; //请求HEAD失败
		} 

		
		ATLASSERT(bRet);


		//清理
_END_OF_DOWNLOADURL:
		//Close();

		return bRet;
	}

	inline BOOL SetOption(__in DWORD dwOption, __in_opt LPVOID lpBuffer, __in DWORD dwBufferLength ){

		return InternetSetOption(m_hRequestGet, dwOption, lpBuffer, dwBufferLength);
	}

	inline DWORD SetFileOffset(LONG lOffset){
		return ::InternetSetFilePointer(m_hRequestGet, lOffset, NULL, FILE_BEGIN, 0);
	}

	BOOL DownloadPieceOnce(byte *pbufForRecv, UINT uDownSize, DWORD &dwDownBytesCount){
		//memset(pbufForRecv, 0, uDownSize*sizeof(char));
		return InternetReadFile(m_hRequestGet, pbufForRecv, uDownSize, &dwDownBytesCount);
	}

	BOOL EnterDownloadLoop(byte *pbufForRecv, UINT uBufSize, DWORD &dwDownBytesCount, SPEEDPROC proc = NULL, LPVOID lpParam = NULL)
	{
		//分配缓冲
		
		dwDownBytesCount = 0;

		int nRetryCount = 0;
		BOOL bRet = FALSE;
		DWORD dwTempCount;
		DWORD dwReadCount = 0;
		while (TRUE)
		{
			dwTempCount = 0;
			//memset(pbufForRecv + dwTempCount, 0, m_dwDownloadSizePerRead*sizeof(char));

			dwReadCount = min(m_dwDownloadSizePerRead, uBufSize - dwDownBytesCount);

			bRet = InternetReadFile(m_hRequestGet, pbufForRecv + dwDownBytesCount, dwReadCount, &dwTempCount);
			DWORD dwError = WSAGetLastError(); 

			if(NULL != proc)
				if(-1 == proc(this, bRet, dwTempCount, lpParam, dwError)) //回调，可用于计算下载速度
				{

					break;
				}

			if (bRet)
			{
				if (dwTempCount > 0)
				{
					dwDownBytesCount += dwTempCount;
				}
				else if (0 == dwTempCount)
				{
					break; //下载到文件最末尾
				}

				if(dwDownBytesCount == uBufSize)
					break; //切片下载完成
			}
			else if(ERROR_INTERNET_TIMEOUT == dwError && nRetryCount++ < m_uRequestTryTimes){
				cout<<"请求超时，继续重试下载，已累计次数："<<nRetryCount<<endl;

				if (dwTempCount > 0)
				{
					dwDownBytesCount += dwTempCount;
				}
				else if (0 == dwTempCount)
				{
					break; //下载到文件最末尾
				}

				if(dwDownBytesCount == uBufSize)
					break; //切片下载完成

			}
			else{
				cout<<"接收数据失败"<<"错误码："<<dwError<<endl;
				bRet = FALSE;
				break;
			}
		}

		return bRet;
	}

	//多次发送请求函数
	BOOL _TryHttpSendRequest(LPVOID hRequest, int nMaxTryTimes)
	{
		BOOL bRet = FALSE;
		DWORD dwStatusCode = 0;
		DWORD dwSizeDW = sizeof(DWORD);
		while (hRequest && (nMaxTryTimes-- > 0)) //多次尝试发送请求
		{
			//发送请求
			bRet = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
			if (!bRet)
			{
				continue;
			}
			else
			{
				//判断HTTP返回的状态码
				dwStatusCode = 0;
				dwSizeDW = sizeof(DWORD);
				bRet = HttpQueryInfo(hRequest, HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_STATUS_CODE, &dwStatusCode, &dwSizeDW, NULL);
				if (bRet)
				{
					//检查状态码
					if (HTTP_STATUS_OK == dwStatusCode) //200 OK
					{
						break;
					}
					else
					{
						bRet = FALSE;
						continue;
					}
				}
			}
		}

		return bRet;
	}



public:



};





