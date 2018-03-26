#include "stdafx.h"

#include "http.h"
#include "download\IDownload.h"
namespace http{;
std::string http_request(const char* url)
{
	std::string sResult;
	UTIL::com_ptr<IHttpSyncRequest> pHttp;
	DllQuickCreateInstance(CLSID_HttpSyncRequest, re_uuidof(IHttpSyncRequest), pHttp, NULL);
	RASSERT(pHttp, sResult);

	UTIL::com_ptr<IMsBuffer> pBuffer;
	USES_CONVERSION;
	RFAILEDP(pHttp->HttpRequest(A2W(url), (IMSBase**)&pBuffer), sResult);

	sResult.append((char*)pBuffer->GetData());

	return sResult;
}

char Dec2HexChar(short int n) 
{
	if ( 0 <= n && n <= 9 ) {
		return char( short('0') + n );
	} else if ( 10 <= n && n <= 15 ) {
		return char( short('A') + n - 10 );
	} else {
		return char(0);
	}
}

short int HexChar2Dec(char c)
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

std::string encode_url(const char* URL)
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

std::string decode_url(const char* URL)
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

bool http_download( const char* url, const char* file )
{
	UTIL::com_ptr<IHttpSyncRequest> pHttp;
	DllQuickCreateInstance(CLSID_HttpSyncRequest, re_uuidof(IHttpSyncRequest), pHttp, NULL);
	RASSERT(pHttp, false);

	USES_CONVERSION;
	RFAILEDP(pHttp->HttpDownload(A2W(url), A2W(file)), false);

	return true;
}

std::wstring utf8_string_to_wstring(const char* SourceStr)
{
	std::wstring str =_T("");
	int  nSize = MultiByteToWideChar(CP_UTF8, 0,SourceStr, -1, NULL, 0);
	WCHAR* buff = new WCHAR[nSize+1];
	if(NULL == buff) return str;
	MultiByteToWideChar(CP_UTF8, 0,SourceStr, -1, buff, nSize+1);
	str = buff;
	delete[] buff;
	return str;
}

std::string http_post(const char* url ,const char* lpszPath, const char* lpszParam)
{
	std::string sResult;
	UTIL::com_ptr<IHttpSyncRequest> pHttp;
	DllQuickCreateInstance(CLSID_HttpSyncRequest, re_uuidof(IHttpSyncRequest), pHttp, NULL);
	RASSERT(pHttp, sResult);

	UTIL::com_ptr<IMsBuffer> pBuffer;
	USES_CONVERSION;
	RFAILEDP(pHttp->HttpPost(A2W(url),A2W(lpszPath),A2W(lpszParam), (IMSBase**)&pBuffer), sResult);

	LPBYTE pByte = pBuffer->GetData();
	DWORD dwSize = pBuffer->GetBufferSize();

	sResult.resize(dwSize + 1);
	for (DWORD nLoop = 0 ; nLoop < dwSize ; nLoop++)
	{
		sResult[nLoop] = ((char)pByte[nLoop]);
	}
	
	char * p = (char *)pByte;
	OutputDebugString(utf8_string_to_wstring(p).c_str());
	return sResult;
}

};