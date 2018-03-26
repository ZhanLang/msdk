
#pragma once
#ifndef _MSGETNETADAPTERINFO_H
#define _MSGETNETADAPTERINFO_H
#include <Iphlpapi.h>
namespace msdk {
	

	#define RSMAC_SIZE		6
	#define RSMACSTR_SIZE	18

#pragma warning( push)
#pragma warning( disable:4996 )
	class CGetNetAdapterInfo
	{
	public:

		static bool GetMac(TCHAR macAddr[RSMACSTR_SIZE] , bool bLine = true)
		{
			BYTE memMac[RSMAC_SIZE] = { 0 };
			if( GetMac(memMac) )
			{
				_stprintf_s(macAddr, RSMACSTR_SIZE, 
					bLine ? _T("%02X-%02X-%02X-%02X-%02X-%02X") : _T("%02X%02X%02X%02X%02X%02X"),
					memMac[0], memMac[1], memMac[2], memMac[3], memMac[4], memMac[5]);
				return true;
			}
			return false;
		}

		static std::basic_string<TCHAR> GetMac(bool bLine = true)
		{
			TCHAR macAddr[ MAX_PATH ] = { 0 };
			GetMac(macAddr, bLine);
			return macAddr;
		}

		static bool GetMac(BYTE macAddress[RSMAC_SIZE])
		{
			bool bGet = false;
			HMODULE m_hDll = NULL;
			PIP_ADAPTER_INFO pAdapterInfo;
			PIP_ADAPTER_INFO pAdapter = NULL;
		
			DWORD (WINAPI *m_pGetAdaptersInfo)(
				PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen
				);
			
			m_hDll = ::LoadLibrary(_T("Iphlpapi.dll"));
			if(NULL == m_hDll)
				goto pend;

			(FARPROC&)m_pGetAdaptersInfo = ::GetProcAddress(m_hDll, "GetAdaptersInfo");
			if(NULL == m_pGetAdaptersInfo)
				goto pend;

			pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
			if(NULL == pAdapterInfo)
				goto pend;

			ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
			// Make an initial call to GetAdaptersInfo to get
			// the necessary size into the ulOutBufLen variable
			if (m_pGetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
				free(pAdapterInfo);
				pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
				if(NULL == pAdapterInfo)
					goto pend;
			}

			if(m_pGetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != NO_ERROR)
				goto pend;

			PIP_ADAPTER_INFO pAdapInfo = pAdapterInfo; //咱们只需要第一个

			do{
				if (IsLocalAdapter(pAdapInfo->AdapterName))
				{
					memcpy(macAddress, pAdapInfo->Address, RSMAC_SIZE);
					bGet = true;

					break;
				}

				pAdapInfo = pAdapInfo->Next;

			}while (pAdapInfo);

pend:
			if (pAdapterInfo)
			{
				free(pAdapterInfo);
				pAdapterInfo = NULL;
			}

			if(m_hDll)
			{
				FreeLibrary(m_hDll);
				m_hDll = NULL;
			}

			return bGet;
		}

	protected:
		static BOOL IsLocalAdapter(char *pAdapterName)
		{
			BOOL ret_value = FALSE;

			#define NET_CARD_KEY _T("System\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
			char szDataBuf[MAX_PATH+1];
			DWORD dwDataLen = MAX_PATH;
			DWORD dwType = REG_SZ;
			HKEY hNetKey = NULL;
			HKEY hLocalNet = NULL;

			if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, NET_CARD_KEY, 0, KEY_READ, &hNetKey))
				return FALSE;
			sprintf(szDataBuf, "%s\\Connection", pAdapterName);
			if(ERROR_SUCCESS != RegOpenKeyExA(hNetKey ,szDataBuf ,0 ,KEY_READ, &hLocalNet))
			{
				RegCloseKey(hNetKey);
				return FALSE;
			}
			/*if (ERROR_SUCCESS != RegQueryValueExA(hLocalNet, "MediaSubType", 0, &dwType, (BYTE *)szDataBuf, &dwDataLen))
			{
				goto ret;
			}
			if (*((DWORD *)szDataBuf)!=0x01)
				goto ret;*/
			dwDataLen = MAX_PATH;
			if (ERROR_SUCCESS != RegQueryValueExA(hLocalNet, "PnpInstanceID", 0, &dwType, (BYTE *)szDataBuf, &dwDataLen))
			{
				goto ret;
			}
			if (_strnicmp(szDataBuf, "PCI", strlen("PCI")))
				goto ret;

			ret_value = TRUE;

ret:
			RegCloseKey(hLocalNet);
			RegCloseKey(hNetKey);

			return ret_value;
		}
	};
#pragma warning( pop )

} //namespace msdk

#endif	//_MSGETNETADAPTERINFO_H