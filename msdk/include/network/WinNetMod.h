
#pragma once
#include <Wininet.h>
#include <util/callapi.h>
namespace msdk{;
namespace network{;
class CWinNet : public tImpModuleMid<CWinNet>
{
public:

	BOOL (__stdcall* DeleteUrlCacheEntryA)(__in LPCSTR lpszUrlName);
	BOOL (__stdcall* DeleteUrlCacheEntryW)(__in LPCWSTR lpszUrlName);
	HINTERNET(__stdcall*HttpOpenRequestA)(__in HINTERNET hConnect,__in_opt LPCSTR lpszVerb,__in_opt LPCSTR lpszObjectName,__in_opt LPCSTR lpszVersion,__in_opt LPCSTR lpszReferrer,__in_z_opt LPCSTR FAR * lplpszAcceptTypes,__in DWORD dwFlags,__in_opt DWORD_PTR dwContext);
	BOOL (__stdcall* HttpSendRequestA)(
		__in HINTERNET hRequest,
		__in_ecount_opt(dwHeadersLength) LPCSTR lpszHeaders,
		__in DWORD dwHeadersLength,
		__in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
		__in DWORD dwOptionalLength
		);

	BOOL (__stdcall* InternetCloseHandle)(
		__in HINTERNET hInternet
		);

	HINTERNET(__stdcall*InternetConnectA)(
		__in HINTERNET hInternet,
		__in LPCSTR lpszServerName,
		__in INTERNET_PORT nServerPort,
		__in_opt LPCSTR lpszUserName,
		__in_opt LPCSTR lpszPassword,
		__in DWORD dwService,
		__in DWORD dwFlags,
		__in_opt DWORD_PTR dwContext
		);

	BOOL (__stdcall* InternetGetConnectedState)(
		__out  LPDWORD  lpdwFlags,
		__reserved DWORD    dwReserved);

	HINTERNET(__stdcall* InternetOpenA)(
		__in_opt LPCSTR lpszAgent,
		__in DWORD dwAccessType,
		__in_opt LPCSTR lpszProxy,
		__in_opt LPCSTR lpszProxyBypass,
		__in DWORD dwFlags
		);

	HINTERNET(__stdcall* InternetOpenUrlW)(
		__in HINTERNET hInternet,
		__in LPCWSTR lpszUrl,
		__in_ecount_opt(dwHeadersLength) LPCWSTR lpszHeaders,
		__in DWORD dwHeadersLength,
		__in DWORD dwFlags,
		__in_opt DWORD_PTR dwContext
		);

	HINTERNET(__stdcall* InternetOpenW)(
		__in_opt LPCWSTR lpszAgent,
		__in DWORD dwAccessType,
		__in_opt LPCWSTR lpszProxy,
		__in_opt LPCWSTR lpszProxyBypass,
		__in DWORD dwFlags
		);

	BOOL (__stdcall* InternetQueryDataAvailable)(
		__in HINTERNET hFile,
		__out_opt LPDWORD __out_data_source(NETWORK) lpdwNumberOfBytesAvailable,
		__in DWORD dwFlags,
		__in_opt DWORD_PTR dwContext
		);

	BOOL (__stdcall* InternetReadFile)(
		__in HINTERNET hFile,
		__out_bcount(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
		__in DWORD dwNumberOfBytesToRead,
		__out LPDWORD lpdwNumberOfBytesRead
		);

	BOOL (__stdcall* InternetSetOptionW)(
		__in_opt HINTERNET hInternet,
		__in       DWORD dwOption,
		__in_opt LPVOID lpBuffer,
		__in       DWORD dwBufferLength
		);



	DECLARE_FUN_BEGIN(CWinNet, "WININET.DLL")
		DECLARE_FUN(DeleteUrlCacheEntryA)
		DECLARE_FUN(DeleteUrlCacheEntryW)
		DECLARE_FUN(HttpOpenRequestA)
		DECLARE_FUN(HttpSendRequestA)
		DECLARE_FUN(InternetCloseHandle)
		DECLARE_FUN(InternetConnectA)
		DECLARE_FUN(InternetGetConnectedState)
		DECLARE_FUN(InternetOpenA)
		DECLARE_FUN(InternetOpenUrlW)
		DECLARE_FUN(InternetOpenW)
		DECLARE_FUN(InternetQueryDataAvailable)
		DECLARE_FUN(InternetReadFile)
		DECLARE_FUN(InternetSetOptionW)
	DECLARE_FUN_END()
};
};};