#pragma once

/*
说明：
	注册进程内组件
作者:king.ma
日期:1013.10.14
*/
static HRESULT STD__RegisterServer(
					   REFGUID guidComponent,
					   LPCTSTR pDllPath,
					   LPCTSTR pVersion,
					   LPCTSTR pVerInd,
					   LPCTSTR pFriendlyName
					   )
{
	if (!pDllPath) return E_INVALIDARG;
	if (guidComponent == GUID_NULL) return E_FAIL;
	
	pVerInd = pVerInd ? pVerInd : (LPCTSTR)_T("");
	pFriendlyName = pFriendlyName ? pFriendlyName : (LPCTSTR)_T("");
	pVersion = pVersion ? pVersion : (LPCTSTR)_T("");

	HKEY hKeyClassRoot = NULL;
	HKEY hKeyClassCLSID = NULL;
	HKEY hKeyInprocServer = NULL;
	HKEY hKeyVersion = NULL;
	HKEY hKeyVerInd = NULL;

	DWORD dwCreateType = 0;
	
	TCHAR strGuid[MAX_PATH] = {0};
	WCHAR strGuidW[MAX_PATH] = {0};
	StringFromGUID2(guidComponent, strGuidW,MAX_PATH);
#ifdef UNICODE
	wcscpy_s(strGuid,MAX_PATH,strGuidW);
#else
	USES_CONVERSION;
	strcpy_s(strGuid,MAX_PATH,W2A(strGuidW));
#endif
	
	HRESULT hr = S_OK;
	do 
	{
		//打开HKEY_CLASSES_ROOT\CLSID
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("CLSID"), 0, KEY_ALL_ACCESS, &hKeyClassRoot))
		{
			hr = E_FAIL;
			break;
		}

		if (ERROR_SUCCESS != RegCreateKeyEx(hKeyClassRoot, strGuid, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			NULL, &hKeyClassCLSID, &dwCreateType))
		{
			hr = E_FAIL;
			break;
		}


		//设置CLSID的默认值为friendly name
		if (ERROR_SUCCESS != RegSetValueEx(hKeyClassCLSID, NULL, 0, 
			REG_SZ, (BYTE*)pFriendlyName, static_cast<DWORD>(sizeof(TCHAR)*_tcsclen(pFriendlyName))))
		{
			hr = E_FAIL;
			break;
		}

		//创建InprocServer32子键
		if (ERROR_SUCCESS != RegCreateKeyEx(hKeyClassCLSID,
			_T("InprocServer32"), 
			0, 
			NULL, 
			REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS,
			NULL, 
			&hKeyInprocServer, 
			&dwCreateType))
		{
			hr = E_FAIL;
			break;
		}

		//设置InprocServer32的值
		if (ERROR_SUCCESS != RegSetValueEx(hKeyInprocServer, NULL, 0, REG_SZ, 
			(BYTE*)pDllPath,static_cast<DWORD>(_tcsclen(pDllPath)*sizeof(TCHAR))))
		{
			hr = E_FAIL;
			break;
		}

		//创建ProgID子键
		if (ERROR_SUCCESS != RegCreateKeyEx(hKeyClassCLSID, _T("ProgID"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			NULL, &hKeyVersion, &dwCreateType))
		{
			hr = E_FAIL;
			break;
		}
		//设置ProgID的值
		if (ERROR_SUCCESS != RegSetValueEx(hKeyVersion, NULL, 0, REG_SZ, (BYTE*)pVersion, static_cast<DWORD>(sizeof(TCHAR)*_tcsclen(pVersion))))
		{
			hr = E_FAIL;
			break;
		}

		//创建VersionIndependentProgID子键
		if (ERROR_SUCCESS != RegCreateKeyEx(hKeyClassCLSID, _T("VersionIndependentProgID"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			NULL, &hKeyVerInd, &dwCreateType))
		{
			hr = E_FAIL;
			break;
		}

		//设置VersionIndependentProgID的值
		if (ERROR_SUCCESS != RegSetValueEx(hKeyVerInd, NULL, 0, REG_SZ, (BYTE*)pVerInd, static_cast<DWORD>(sizeof(TCHAR)*_tcsclen(pVerInd))))
		{
			hr = E_FAIL;
			break;
		}

	} while (false);

	if(hKeyClassRoot)
	{
		RegCloseKey(hKeyClassRoot);
		hKeyClassRoot = NULL;
	}

	if(hKeyClassCLSID)
	{
		RegCloseKey(hKeyClassCLSID);
		hKeyClassCLSID = NULL;
	}
	if(hKeyInprocServer)
	{
		RegCloseKey(hKeyInprocServer);
		hKeyInprocServer = NULL;
	}

	if(hKeyVersion)
	{
		RegCloseKey(hKeyVersion);
		hKeyVersion = NULL;
	}
	if(hKeyVerInd)
	{
		RegCloseKey(hKeyVerInd);
		hKeyVerInd = NULL;
	}

	return hr;
}

static HRESULT STD__UnregisterServer(
						 REFGUID guidComponent
						 )
{
	if (GUID_NULL == guidComponent) return E_FAIL;
	
	HKEY hKeyClassRoot = NULL;
	HKEY hKeyClassCLSID = NULL;


	TCHAR strGuid[MAX_PATH] = {0};
	WCHAR strGuidW[MAX_PATH] = {0};
	StringFromGUID2(guidComponent, strGuidW,MAX_PATH);
#ifdef UNICODE
	wcscpy_s(strGuid,MAX_PATH,strGuidW);
#else
	USES_CONVERSION;
	strcpy_s(strGuid,MAX_PATH,W2A(strGuidW));
#endif

	HRESULT hr = S_OK;

	do 
	{
		//打开HKEY_CLASSES_ROOT\CLSID
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("CLSID"), 0, KEY_ALL_ACCESS, &hKeyClassRoot))
		{
			hr = E_FAIL;
			break;
		}


		//创建CLSID子键
		if (ERROR_SUCCESS != RegOpenKeyEx(hKeyClassRoot, strGuid, 0, KEY_ALL_ACCESS,&hKeyClassCLSID))
		{
			hr = E_FAIL;
			break;
		}

		RegDeleteKey(hKeyClassCLSID , _T("InprocServer32"));
		RegDeleteKey(hKeyClassCLSID , _T("ProgID"));
		RegDeleteKey(hKeyClassCLSID , _T("VersionIndependentProgID"));
		RegDeleteKey(hKeyClassRoot , strGuid);

	} while (false);

	if(hKeyClassRoot)
	{
		RegCloseKey(hKeyClassRoot);
		hKeyClassRoot = NULL;
	}

	if(hKeyClassCLSID)
	{
		RegCloseKey(hKeyClassCLSID);
		hKeyClassCLSID = NULL;
	}

	return hr;
}