#pragma once


#include <string>
#include <map>
#include <vector>

#include <network/WinNetMod.h>
#include <Netlistmgr.h>
#include <SensAPI.h>
namespace msdk{;
namespace network{;

class CUrlParamValue
{

public:
	CUrlParamValue()
	{
		m_strValue = "";
	}

	CUrlParamValue(int value)
	{
		char buf[MAX_PATH] = {0};
		sprintf_s(buf, MAX_PATH, "%d", value);
		m_strValue = buf;
	}

	CUrlParamValue(unsigned long value)
	{
		char buf[MAX_PATH] = {0};
		sprintf_s(buf, MAX_PATH, "%u", value);
		m_strValue = buf;
	}

	CUrlParamValue(double value)
	{
		char buf[MAX_PATH] = {0};
		sprintf_s(buf, MAX_PATH, "%f", value);
		m_strValue = buf;
	}

	CUrlParamValue(std::string const& value)
	{
		m_strValue = value;
	}

	CUrlParamValue(std::wstring const& value)
	{
		if (value.length())
		{
			int  nSize = WideCharToMultiByte(CP_ACP, 0,value.c_str(), -1, NULL, 0,NULL,NULL);
			if ( nSize )
			{
				CHAR* buff = new CHAR[nSize+1];
				if ( buff )
				{
					WideCharToMultiByte(CP_UTF8, 0,value.c_str(), -1, buff, nSize+1,NULL,NULL);
					m_strValue = buff;
					delete[] buff;
				}
			}
		}
	}

	CUrlParamValue(const char* value)
	{
		if (value)
		{
			m_strValue = value;
		}
	}

	CUrlParamValue(const wchar_t* value)
	{
		if (value && wcslen(value))
		{
			int  nSize = WideCharToMultiByte(CP_ACP, 0,value, -1, NULL, 0,NULL,NULL);
			if ( nSize )
			{
				CHAR* buff = new CHAR[nSize+1];
				if ( buff )
				{
					WideCharToMultiByte(CP_UTF8, 0,value, -1, buff, nSize+1,NULL,NULL);
					m_strValue = buff;
					delete[] buff;
				}
			}
		}
	}
	std::string m_strValue;
};

typedef std::map<std::string, CUrlParamValue> CUrlParamValueMap;


class CHttpImplement
{
public:
	CHttpImplement(void)
	{
		m_retCode = 0;
		m_winNet.Load();
	}
	~CHttpImplement(void)
	{
		m_winNet.UnLoad();
	}

	//辅助函数
	static std::string GetUrl(const CUrlParamValueMap& param)
	{
		std::string strUrl;
		int n = param.size();
		for (CUrlParamValueMap::const_iterator it = param.begin() ; it != param.end() ; it++, n--)
		{
			strUrl += it->first;
			strUrl += '=';
			strUrl += encode_url(it->second.m_strValue.c_str());
			if(n > 1)
				strUrl += '&';
		}

		return strUrl;
	}

	static std::string GetUrl(LPCSTR lpszPath, const CUrlParamValueMap& param)
	{
		std::string strUrl = lpszPath;

		int n = param.size();
		if (n)
			strUrl += '?';

	
		return strUrl + GetUrl(param);
	}

	static std::string GetUrl(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszPath, const CUrlParamValueMap& param)
	{
		std::string strUrl = lpszHost;
		strUrl += ":";
		char szPort[20] = { 0 };
		_itoa_s(dwPort, szPort, sizeof(szPort), 10);
		strUrl += szPort;
		strUrl +=  "/";
		strUrl += GetUrl(lpszPath, param);
		return strUrl;
	}


	DWORD GetRequest(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszPath, const CUrlParamValueMap& param,  LPCSTR lpszParam = NULL)
	{
		return GetRequest(lpszHost, dwPort, GetUrl(lpszPath, param).c_str(), lpszParam);
	}




	//使用GET方式请求
	DWORD GetRequest(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszUrl, LPCSTR lpszParam)
	{
		return Request(lpszHost, dwPort, lpszUrl, "GET", "Content-Type: application/x-www-form-urlencoded", lpszParam);	
	}
	
	//使用POST方式请求
	DWORD PostRequest(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszPath, LPCSTR lpszParam)
	{
		return Request(lpszHost, dwPort, lpszPath, "POST","Content-Type: application/x-www-form-urlencoded", lpszParam);
	}

	DWORD PostRequest(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszPath, const CUrlParamValueMap& param)
	{
		return PostRequest(lpszHost, dwPort, lpszPath, GetUrl(param).c_str());
	}

	//请求JSON
	DWORD JSonRequest(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszUrl, LPCSTR lpszParam)
	{
		return Request(lpszHost, dwPort, lpszUrl, "POST","Content-Type:application/json", lpszParam);
	}



	//请求JSON
	DWORD TextRequest(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszUrl, LPCSTR lpszParam)
	{
		return Request(lpszHost, dwPort, lpszUrl, "POST","Content-Type:application/text", lpszParam);
	}

	//返回请求的结果
	DWORD GetRequestCode()
	{
		return m_retCode;
	}

	//获取请求的数据
	unsigned char* GetRequestData()
	{
		if ( m_data.size() )
			return &m_data[0];
		return NULL;
	}

	//获取请求到的数据长度
	unsigned long GetRequestLength()
	{
		return m_data.size();
	}




	static char Dec2HexChar(short int n) 
	{
		if ( 0 <= n && n <= 9 ) {
			return char( short('0') + n );
		} else if ( 10 <= n && n <= 15 ) {
			return char( short('A') + n - 10 );
		} else {
			return char(0);
		}
	}

	static short int HexChar2Dec(char c)
	{
		if ( '0'<=c && c<='9' ) {
			return short(c-'0');
		} else if ( 'a'<=c && c<='f' ) {
			return ( short(c-'a') + 10 );
		} else if ( 'A'<=c && c<='F' ) {
			return ( short(c-'A') + 10 );
		} else {
			return -1;
		}
	}

	static std::string encode_url(const char* URL)
	{
		std::string strResult = "";
		for ( unsigned int i=0; i< strlen(URL); i++ )
		{
			char c = URL[i];
			if (
				( '0'<=c && c<='9' ) ||
				( 'a'<=c && c<='z' ) ||
				( 'A'<=c && c<='Z' ) ||
				c=='/' || c=='.'
				) {
					strResult += c;
			} 
			else 
			{
				int j = (short int)c;
				if ( j < 0 )
				{
					j += 256;
				}
				int i1, i0;
				i1 = j / 16;
				i0 = j - i1*16;
				strResult += '%';
				strResult += Dec2HexChar(i1);
				strResult += Dec2HexChar(i0);
			}
		}

		return strResult;
	}

	static std::string decode_url(const char* URL)
	{
		std::string result = "";
		for ( unsigned int i=0; i< strlen(URL); i++ ) 
		{
			char c = URL[i];
			if ( c != '%' ) 
			{
				result += c;
			} 
			else 
			{
				char c1 = URL[++i];
				char c0 = URL[++i];
				int num = 0;
				num += HexChar2Dec(c1) * 16 + HexChar2Dec(c0);
				result += char(num);
			}
		}

		return result;
	}

public:
	DWORD Request(LPCSTR lpszHost, WORD dwPort, LPCSTR lpszUrl, LPCSTR lpszType, LPCSTR lpszContentType, LPCSTR lpszParam)
	{
		m_retCode = 0;
		m_data.clear();

		if ( !m_winNet.IsLoaded() )
			return ERROR_INTERNET_EXTENDED_ERROR;

		std::string strUrl = lpszHost;
		char buf[MAX_PATH] = {0};
		sprintf_s(buf, MAX_PATH, ":%d/", dwPort);
		strUrl += buf;
		strUrl += lpszUrl;

		//OutputDebugStringA(strUrl.c_str());


		//lpszParam=0;
		

		m_winNet.DeleteUrlCacheEntryA(strUrl.c_str());
		HINTERNET hSession = m_winNet.InternetOpenA( NULL,INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if(!hSession)
		{
			SetError();
			return m_retCode;
		}

		DWORD dwTimeout = 1000 * 10;
		BOOL bRet = m_winNet.InternetSetOption(hSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeout, sizeof(DWORD));
		bRet = m_winNet.InternetSetOption(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeout, sizeof(DWORD)); 
		HINTERNET hConnect = m_winNet.InternetConnectA(hSession, lpszHost, dwPort,	NULL, NULL, INTERNET_SERVICE_HTTP,0,0);
		if (!hConnect)
		{
			SetError();
			m_winNet.InternetCloseHandle(hSession);
			hSession = NULL;

			return m_retCode;
		}

		//CONST TCHAR tcsHdrs[] = _T("Content-Type: application/json");
		CONST CHAR *szAcceptType[2]={"Accept: */*",NULL};

		HINTERNET hRequest = m_winNet.HttpOpenRequestA( hConnect, lpszType, lpszUrl,  "HTTP/1.1", NULL, (const CHAR **)&szAcceptType,  0, 0);
		if (!hRequest)
		{
			SetError();
			m_winNet.InternetCloseHandle(hSession);
			hSession = NULL;

			m_winNet.InternetCloseHandle(hConnect);
			hConnect = NULL;

			return m_retCode;
		}

		if(!m_winNet.HttpSendRequestA(hRequest,  lpszContentType,  (DWORD)strlen(lpszContentType),  (LPVOID)lpszParam, lpszParam  ? (int)strlen(lpszParam) : 0))
		{
			SetError();
			m_winNet.InternetCloseHandle(hSession);
			hSession = NULL;

			m_winNet.InternetCloseHandle(hConnect);
			hConnect = NULL;

			m_winNet.InternetCloseHandle(hRequest);
			hRequest = NULL;
			return m_retCode;
		}

		DWORD dwSize = 0;     
		BYTE Temp[1024] = {0}; 
		if (m_winNet.InternetQueryDataAvailable(hRequest,&dwSize,0,0))     
		{      
			if (hRequest != NULL)  
			{       
				while( 1 )
				{
					BOOL bRet = m_winNet.InternetReadFile(hRequest, Temp, sizeof(Temp), &dwSize);
					if (!bRet)
					{
						SetError();
						m_data.clear();
						break;
					}

					if (dwSize)
						m_data.insert(m_data.end(), Temp, Temp+ dwSize);
					else
					{
						unsigned char cc = '\0';
						m_data.push_back(cc);
						break;
					}
				}	
			}    
		}

		SetError();
		m_winNet.InternetCloseHandle(hSession);
		hSession = NULL;

		m_winNet.InternetCloseHandle(hConnect);
		hConnect = NULL;

		m_winNet.InternetCloseHandle(hRequest);
		hRequest = NULL;

		return 0;
	}

	DWORD Download(LPCTSTR lpszUrl, std::vector<BYTE>& buf)
	{
		DWORD dwRetCode = 0;

		if ( !m_winNet.IsLoaded() )
			return ERROR_INTERNET_EXTENDED_ERROR;

		m_winNet.DeleteUrlCacheEntryW(lpszUrl);
		HINTERNET hSession = m_winNet.InternetOpenA(NULL,INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if(!hSession)
			return m_retCode;

		HINTERNET hRequest = m_winNet.InternetOpenUrl(hSession, lpszUrl, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (!hRequest)
		{
			dwRetCode = GetError();
			m_winNet.InternetCloseHandle(hSession);
			hSession = NULL;
			return dwRetCode;
		}

		DWORD dwSize = 0;     
		BYTE Temp [2048] = {0}; 
		if (m_winNet.InternetQueryDataAvailable(hRequest,&dwSize,0,0))     
		{      
			if (hRequest != NULL)  
			{       
				while( 1 )
				{
					BOOL bRet = m_winNet.InternetReadFile(hRequest, Temp, sizeof(Temp), &dwSize);
					if (!bRet)
					{
						dwRetCode = GetError();
						break;
					}

					if (dwSize)
						buf.insert(buf.end(),Temp, Temp+dwSize);
					else
						break;
				}	
			}    
		}

		m_winNet.InternetCloseHandle(hSession);
		hSession = NULL;
		m_winNet.InternetCloseHandle(hRequest);
		hRequest = NULL;

		return dwRetCode;
	}

	DWORD Download( LPCTSTR lpszUrl, LPCTSTR lpszFile)
	{
		DWORD dwRetCode = 0;

		if ( !m_winNet.IsLoaded() )
			return ERROR_INTERNET_EXTENDED_ERROR;

		m_winNet.DeleteUrlCacheEntry(lpszUrl);
		HINTERNET hSession = m_winNet.InternetOpen(NULL,INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if(!hSession)
			return GetError();

		HINTERNET hRequest = m_winNet.InternetOpenUrl(hSession, lpszUrl, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (!hRequest)
		{
			dwRetCode = GetError();
			m_winNet.InternetCloseHandle(hSession);
			hSession = NULL;
			return dwRetCode;
		}


		HANDLE hFile = CreateFile(lpszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if ( hFile == INVALID_HANDLE_VALUE )
		{
			dwRetCode = GetLastError();

			m_winNet.InternetCloseHandle(hRequest);
			hRequest = NULL;

			m_winNet.InternetCloseHandle(hSession);
			hSession = NULL;
			return dwRetCode;
		}


		DWORD dwSize = 0;     
		BYTE Temp [2048] = {0}; 
		if (m_winNet.InternetQueryDataAvailable(hRequest,&dwSize,0,0))     
		{      
			if (hRequest != NULL)  
			{       
				while( 1 )
				{
					BOOL bRet = m_winNet.InternetReadFile(hRequest, Temp, sizeof(Temp), &dwSize);
					if (!bRet)
					{
						dwRetCode = GetError();
						break;
					}

					if (dwSize)
						WriteFile(hFile, Temp, dwSize, &dwSize, NULL);
					else
						break;
				}	
			}    
		}

		FlushFileBuffers(hFile);
		CloseHandle(hFile);
		hFile = NULL;
		m_winNet.InternetCloseHandle(hSession);
		hSession = NULL;
		m_winNet.InternetCloseHandle(hRequest);
		hRequest = NULL;

		return dwRetCode;
	}

	VOID SetError()
	{
		DWORD dwLastErr = GetLastError();
		if  (dwLastErr  &  ERROR_INTERNET_EXTENDED_ERROR)
		{
			m_retCode = dwLastErr;
			/*
			TCHAR szError[MAX_PATH];
			DWORD dwLastErrorMsg = MAX_PATH;
			InternetGetLastResponseInfo(&m_retCode,szError,&dwLastErrorMsg);            
			OutputDebugString(szError);
			*/
		}
	}

	DWORD GetError()
	{
		DWORD dwLastErr = GetLastError();
		if  (dwLastErr  &  ERROR_INTERNET_EXTENDED_ERROR)
			return  dwLastErr;

		return 0;
	}

	BOOL IsHasInternetConnectionForVista(BOOL &bHandle)
	{
		bHandle = FALSE;
		BOOL bHasInternet = FALSE;
		CComPtr<INetworkListManager> spNetList;
		HRESULT hr = CoCreateInstance( CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_INetworkListManager, (void**) &spNetList );
		if( SUCCEEDED(hr) )
		{
			VARIANT_BOOL vb = VARIANT_FALSE;
			hr = spNetList->get_IsConnectedToInternet(&vb);
			if( SUCCEEDED(hr) )
			{
				bHandle = TRUE;
				if( vb == VARIANT_TRUE )
					bHasInternet = TRUE;
			}
		}

		return bHasInternet;
	}


	//判断网络是否存在
	//在一些特殊情况下我们需要检查当前的网络状态，
	//这个函数就能检查到当前网络是否处于连接状态
	BOOL WINAPI IsHasInternetConnection()
	{

		//////////////////////////////////////////////////////////////////////////
		//微软在WINDOWS VISTA之后提供了一个叫NLA(Network List Manager API)的接口，用于获取网络状态变化通知的一个接口。以COM技术实现
		//////////////////////////////////////////////////////////////////////////
		BOOL bLive = FALSE;
		OSVERSIONINFO	osver = {sizeof(OSVERSIONINFO)};
		GetVersionEx(&osver);
		if(osver.dwMajorVersion  >= 6)
		{
			//已知问题: 在跨进程互相Send WM_COPYDATA,在这里中处理COM会失败
			//例：A发送WM_COPYDATA到B,B在WM_COPYDATA消息中又给A发送WM_COPYDATA，此时，在A的WM_COPYDATA消息响应中，COM会失败
			//在有些情况下，系统网络图标显示无网，但实际是有网络的。对于这种情况，无网继续走下面判断
			BOOL bHandle = FALSE;
			bLive = IsHasInternetConnectionForVista(bHandle);
			if( bLive )
				return bLive;
		}




		DWORD dwFlag = 0;
		bLive = IsNetworkAlive(&dwFlag);
		DWORD dwError = GetLastError();
		if( dwError == 0 )
		{
			if( bLive )
				return bLive;
		}

		//这里有两种情形：1. 上面值不可用，2.System Event Notification 服务停止
		dwFlag = 0;
		if( m_winNet.InternetGetConnectedState(&dwFlag, 0) )
		{
			if(dwFlag&INTERNET_CONNECTION_OFFLINE)
				return FALSE;

			if( (dwFlag&INTERNET_CONNECTION_LAN) ||
				(dwFlag&INTERNET_CONNECTION_MODEM) ||
				(dwFlag&INTERNET_CONNECTION_PROXY) )
				return TRUE;
		}

		return FALSE;
	}
private:
	std::vector<unsigned char> m_data;
	DWORD	m_retCode;

public:
    CWinNet m_winNet;
};

};};
