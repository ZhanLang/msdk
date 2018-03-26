#pragma once
#include <string>

namespace msdk{;



// UNICODE转UTF8
static std::string W2UTF8(LPCWSTR szBuffer)
{
	std::string strBuffer;
	if( !(szBuffer && wcslen(szBuffer) ) )
		return strBuffer;


	int nBytes = WideCharToMultiByte( CP_UTF8, 0, szBuffer, -1, NULL, 0, NULL, NULL);
	char* pBuffer = new char[nBytes+1];
	ZeroMemory(pBuffer, nBytes+1);
	WideCharToMultiByte( CP_UTF8, 0, szBuffer, -1, pBuffer, nBytes, NULL, NULL);
	strBuffer = pBuffer;
	delete[] pBuffer;
	return strBuffer;
}


//UTF8转UNICODE
static std::wstring UTF82W(LPCSTR sBuffer)
{
	std::wstring strBuffer;
	if( !(sBuffer && strlen(sBuffer)) )
		return strBuffer;

	int nBytes = MultiByteToWideChar( CP_UTF8, 0, sBuffer, -1, NULL, 0);
	WCHAR* pBuffer = new WCHAR[nBytes+1];
	ZeroMemory(pBuffer, nBytes+1);
	MultiByteToWideChar( CP_UTF8, 0, sBuffer, -1, pBuffer, nBytes);
	strBuffer = pBuffer;
	delete[] pBuffer;
	return strBuffer;
}

//UNICODE转为多字节
static std::string W2MBACP(LPCWSTR szBuffer)
{
	std::string strBuffer;
	if(!(szBuffer && wcslen(szBuffer) ))
		return strBuffer;


	int nBytes = WideCharToMultiByte( CP_ACP, 0, szBuffer, -1, NULL, 0, NULL, NULL);
	char* pBuffer = new char[nBytes+1];
	ZeroMemory(pBuffer, nBytes+1);
	WideCharToMultiByte( CP_ACP, 0, szBuffer, -1, pBuffer, nBytes, NULL, NULL);
	strBuffer = pBuffer;
	delete[] pBuffer;
	return strBuffer;
}

//多字节转UNICODE
static std::wstring MB2WACP(LPCSTR sBuffer)
{
	std::wstring strBuffer;
	if( !(sBuffer && strlen(sBuffer)) )
		return strBuffer;

	int nBytes = MultiByteToWideChar( CP_ACP, 0, sBuffer, -1, NULL, 0);
	WCHAR* pBuffer = new WCHAR[nBytes+1];
	ZeroMemory(pBuffer, nBytes+1);
	MultiByteToWideChar( CP_ACP, 0, sBuffer, -1, pBuffer, nBytes);
	strBuffer = pBuffer;
	delete[] pBuffer;
	return strBuffer;
}

static std::string  UTF82CAP(std::string strUtf8)
{ 
	std::string amp ;
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8.c_str(), -1, NULL,0); 
	unsigned short * wszGBK = new unsigned short[len+1]; 
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8.c_str(), -1, (LPWSTR)wszGBK, len); 
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL,NULL); 
	//strUtf8 = szGBK; 
	amp=szGBK;
	delete[] szGBK; 
	delete[] wszGBK; 

	return amp;
}
};
