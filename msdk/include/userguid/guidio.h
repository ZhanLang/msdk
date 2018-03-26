#ifndef _MSGUIDIO_H
#define _MSGUIDIO_H

#include <util/str_encode_embed.h>

#ifdef UNICODE
#	define REGGUID_PATH ustr_encode_embed<30,'{','2','3','6','B','6','3','2','5','-','F','F','5','1','-','4','3','F','D','-','A','F','F','1','-','0','8','C','B','F','4','E','9','F','C','D','A','}'>().c_str()
#	define REGGUID_KEY  ustr_encode_embed<30,'P','r','o','c','I','D'>().c_str()
#	define REGGUID_MASK ustr_encode_embed<30,'G','u','i','d','M','a','s','k'>().c_str()
#else
#	define REGGUID_PATH str_encode_embed<30,'{','2','3','6','B','6','3','2','5','-','F','F','5','1','-','4','3','F','D','-','A','F','F','1','-','0','8','C','B','F','4','E','9','F','C','D','A','}'>().c_str()
#	define REGGUID_KEY  str_encode_embed<30,'P','r','o','c','I','D'>().c_str()
#	define REGGUID_MASK str_encode_embed<30,'G','u','i','d','M','a','s','k'>().c_str()
#endif

#include <mscom\msbase.h>
#include <AclAPI.h>

namespace msdk {

	interface IGuidIO
	{
		STDMETHOD(Get)(GUID& guid) = 0;
		STDMETHOD(Set)(const GUID& guid) = 0;

		STDMETHOD_(BYTE, GetMask)() = 0;
		STDMETHOD(SetMask)(BYTE byMask) = 0;
	};

	//默认的注册表实现
	class CRegGuidIO : public IGuidIO
	{
	public:
		CRegGuidIO(HKEY hRootKey = HKEY_CLASSES_ROOT,
				LPCTSTR szRegPath = REGGUID_PATH,
				LPCTSTR szRegKey = REGGUID_KEY,
				LPCTSTR szRegMask = REGGUID_MASK)
				:m_hRootKey(hRootKey),m_szRegPath(szRegPath),m_szRegKey(szRegKey),m_szRegMask(szRegMask)
		{}

		STDMETHOD(Get)(GUID& guid)
		{
			HRESULT hr = E_FAIL;
			guid = GUID_NULL;

			HKEY hkKey;
			long lResult = RegOpenKeyEx(m_hRootKey, m_szRegPath, 0, KEY_READ, &hkKey);
			if(ERROR_SUCCESS != lResult)
				return E_FAIL;

			TCHAR szValue[MAX_PATH]={0};
			DWORD dwRead=MAX_PATH;
			DWORD dwType = NULL;

			lResult = RegQueryValueEx(hkKey, m_szRegKey, NULL, &dwType, (LPBYTE)szValue, &dwRead);
			if(ERROR_SUCCESS == lResult)
			{
				guid = mscom::S2GUID(szValue);
				if(guid != GUID_NULL)
					hr = S_OK;
			}

			RegCloseKey(hkKey);
			return hr;
		}

		STDMETHOD(Set)(const GUID& guid)
		{
			USES_GUIDCONVERSION;
			LPCTSTR lpGuid = mscom::GUID2S(guid);
			if(!lpGuid)
				return E_FAIL;

			HRESULT hr = E_FAIL;

			HKEY hkKey;
			long lResult = RegOpenKey(m_hRootKey, m_szRegPath, &hkKey);
			if(ERROR_SUCCESS != lResult)
			{
				lResult = RegCreateKey(m_hRootKey, m_szRegPath, &hkKey);
				//SetRegPrivilege(_T("CLASSES_ROOT\\{236B6325-FF51-43FD-AFF1-08CBF4E9FCDA}"));
			}
			if(ERROR_SUCCESS != lResult)
				return E_FAIL;

			DWORD dwLength = (_tcslen(lpGuid) + 1)* (DWORD)sizeof(TCHAR);
			lResult = RegSetValueEx(hkKey, m_szRegKey , 0, REG_SZ, (LPBYTE)lpGuid, dwLength);

			if(ERROR_SUCCESS == lResult)
			{
				hr = S_OK;
			}

			RegCloseKey(hkKey);
			return hr;
		}

		STDMETHOD_(BYTE, GetMask)()
		{
			HRESULT hr = E_FAIL;
			BYTE byMask = 0;

			HKEY hkKey;
			long lResult = RegOpenKeyEx(m_hRootKey, m_szRegPath, 0, KEY_READ, &hkKey);
			if(ERROR_SUCCESS != lResult)
				return 0;

			DWORD dwValue = 0;
			DWORD dwRead  = sizeof(DWORD);
			DWORD dwType = REG_DWORD;

			lResult = RegQueryValueEx(hkKey, m_szRegMask, NULL, &dwType, (LPBYTE)&dwValue, &dwRead);
			if(ERROR_SUCCESS == lResult)
			{
				byMask = (BYTE)dwValue;
			}

			RegCloseKey(hkKey);
			return byMask;
		}

		STDMETHOD(SetMask)(BYTE byMask)
		{
			HRESULT hr = E_FAIL;

			HKEY hkKey;
			long lResult = RegOpenKey(m_hRootKey, m_szRegPath, &hkKey);
			if(ERROR_SUCCESS != lResult)
			{
				lResult = RegCreateKey(m_hRootKey, m_szRegPath, &hkKey);
			}
			if(ERROR_SUCCESS != lResult)
				return E_FAIL;

			DWORD dwValue = byMask;
			DWORD dwLength = sizeof(DWORD);
			lResult = RegSetValueEx(hkKey, m_szRegMask , 0, REG_DWORD, (LPBYTE)&dwValue, dwLength);

			if(ERROR_SUCCESS == lResult)
			{
				hr = S_OK;
			}

			RegCloseKey(hkKey);
			return hr;
		}

		private:
			
//修改注册表权限
//【MACHINE】对应【HKEY_LOCAL_MACHINE】
//【CLASSES_ROOT】 对应【HKEY_CLASSES_ROOT】
static DWORD SetRegPrivilege(LPTSTR lpszKey) 
{ 
	DWORD dwRet;

	LPTSTR SamName				= lpszKey; 
	PSECURITY_DESCRIPTOR pSD	= NULL; 
	PACL pOldDacl				= NULL; 
	PACL pNewDacl				= NULL; 
	HKEY hKey					= NULL;
	EXPLICIT_ACCESS ea			= {0}; 

	do 
	{
		// 获取SAM主键的DACL 
		dwRet = GetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDacl, NULL, &pSD);
		if (dwRet != ERROR_SUCCESS)
		{
			break;
		}

		// 创建一个ACE，允许Everyone完全控制对象，并允许子对象继承此权限 
		BuildExplicitAccessWithName(&ea, _T("Everyone"), KEY_ALL_ACCESS, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);

		// 将新的ACE加入DACL 
		dwRet = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl); 
		if (dwRet != ERROR_SUCCESS)
		{
			break;
		}

		// 更新SAM主键的DACL 
		dwRet = SetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL); 
		if (dwRet != ERROR_SUCCESS) 
		{ 
			break;
		}



	} while (FALSE);

	
	if (pNewDacl) 
		LocalFree(pNewDacl); 
	

	/* 还原注册表权限
	if (pOldDacl) 
		SetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pOldDacl, NULL); 
	if (pSD) LocalFree(pSD); 
	*/
	return dwRet; 
}
	private:
		HKEY m_hRootKey;
		LPCTSTR m_szRegPath;
		LPCTSTR m_szRegKey;
		LPCTSTR m_szRegMask;
	};

} //namespace msdk

#endif	//_MSGUIDIO_H