#include "StdAfx.h"
#include "prodinfo/prodinfoimp.h"
#include "util/utility.h"

#include<string>
#include "arith/encrypt/RC4.h"
#include "msapi/mswinapi.h"
#include "mslog/logtool/logtool.h"
#include "vistafunc/AccessPermission.h"
using namespace msdk;

#ifndef fstring
#	ifdef UNICODE
#		define fstring std::wstring
#	else
#		define fstring std::string
#	endif//ifdef UNICODE
#endif//#ifndef fstring


static LONG GetValue(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPBYTE lpValue, DWORD& dwSize, DWORD& dwTpye){

	REGSAM type = KEY_QUERY_VALUE;

	HKEY hSubKey = NULL;
	LONG  dwRet = RegOpenKeyEx(hRoot, lpszSubKey, 0, type,  &hSubKey);
	if ( dwRet != ERROR_SUCCESS)
		return dwRet;

	dwRet = RegQueryValueEx(hSubKey, lpszValueName, 0, &dwTpye, lpValue, &dwSize);
	RegCloseKey(hSubKey);
	return dwRet;
}


static LONG SetValue(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwValueType, LPBYTE lpValue, DWORD dwSize)
{
	DWORD dwRet = 0;
	HKEY hKey = NULL;

	REGSAM type = KEY_WRITE;

	dwRet = RegOpenKeyEx(hRoot, lpszSubKey, 0, type,  &hKey);
	if (dwRet != ERROR_SUCCESS)
	{
		dwRet = RegCreateKeyEx(HKEY_CLASSES_ROOT, lpszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, type, NULL, &hKey, NULL);
		if (dwRet != ERROR_SUCCESS)
		{
			ZM1_GrpDbgOutput(GroupName, _T("´´½¨×¢²á±íÊ§°Ü¡¾%s¡¿Ê§°Ü[%d]"), lpszSubKey, dwRet);
			return dwRet;
		}

		CAccessPermission AccessPermission;
		if ((dwRet = AccessPermission.Reg_GrantAccess(hRoot, _T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}"),  _T("Everyone"))) != ERROR_SUCCESS)
		{
			ZM1_GrpDbgOutput(GroupName, _T("ÐÞ¸Ä×¢²á±íÈ¨ÏÞ¡¾%s¡¿Ê§°Ü[%d]"), lpszSubKey,dwRet);
		}
	}

	dwRet = RegSetValueEx(hKey, lpszValueName, 0,  dwValueType ,  lpValue, dwSize);
	RegCloseKey(hKey);
	return dwRet;
}


static fstring GetString(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LONG* lRet = NULL)
{
	fstring sRet;
	DWORD dwSize = 0;
	DWORD dwType;

	LONG lTemp = GetValue(hRoot, lpszSubKey, lpszValueName, NULL, dwSize, dwType);
	if ( !(dwSize && lTemp == ERROR_SUCCESS))
	{
		if (lRet)
			*lRet = lTemp;

		return sRet;
	}



	LPBYTE pData = new BYTE[dwSize];
	while( (lTemp = GetValue(hRoot, lpszSubKey, lpszValueName, pData, dwSize, dwType)) == ERROR_MORE_DATA)
	{
		if (pData)
			delete[] pData;

		pData = new BYTE[dwSize];
	}

	if (lTemp == ERROR_SUCCESS)
	{
		if (dwType == REG_SZ)
			sRet = (LPCTSTR)pData;
		else if(dwType == REG_DWORD)
		{
			TCHAR temp[MAX_PATH] = { 0 };
			_stprintf_s(temp, MAX_PATH,_T("%d"), *((DWORD*)pData));
			sRet = temp;
		}
	}

	if (pData)
		delete[] pData;

	if (lRet)
		*lRet = lTemp;

	return sRet;
}

static LONG SetString(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	return SetValue(hRoot, lpszSubKey, lpszValueName, REG_SZ, (LPBYTE)lpszValue, (_tcslen(lpszValue) + 1) * sizeof(TCHAR));
}


static LONG DeleteKey(HKEY hRoot, LPCTSTR lpszSubKey)
{
	return RegDeleteKey(hRoot, lpszSubKey);
}

static LONG DeleteValue(HKEY hRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueNale)
{
	DWORD dwRet = 0;
	HKEY hKey = NULL;
	dwRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, lpszSubKey, 0, KEY_ALL_ACCESS,  &hKey);

	if ( dwRet != ERROR_SUCCESS)
	{
		return dwRet;
	}


	dwRet = RegDeleteValue(hKey, lpszValueNale);
	RegCloseKey(hKey);

	return dwRet;
}

char lpszKey[] = "B3D694195AA0";
std::wstring EncryptKey(LPCWSTR lpszContext)
{
	CRC4EncryptSafeBase64 Encrypt((unsigned char*)lpszKey, _countof(lpszKey));
	Encrypt.Encrypt((unsigned char*)lpszContext,  (wcslen(lpszContext) +1) * sizeof(WCHAR));
	USES_CONVERSION;
	return A2W(Encrypt.GetResult());
}

std::wstring Encrypt(LPCWSTR lpszContext)
{
	CRC4EncryptSafeBase64 Encrypt((unsigned char*)lpszKey, _countof(lpszKey));
	Encrypt.Encrypt((unsigned char*)lpszContext,  (wcslen(lpszContext) +1) * sizeof(WCHAR));
	USES_CONVERSION;
	return A2W(Encrypt.GetResult());
}

std::wstring Decrypt(LPCWSTR lpszContext)
{
	USES_CONVERSION;
	CRC4DecryptSafeBase64 Decrypt((unsigned char*)lpszKey, _countof(lpszKey));
	Decrypt.Decrypt(W2A(lpszContext));
	return (WCHAR*)Decrypt.GetResult();
}

HRESULT SetProdInfoW(LPCWSTR lpszProdId, LPCWSTR lpszKey, LPCWSTR lpszValue)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s"), lpszProdId);

	return SetString(HKEY_CLASSES_ROOT, strSubKey, EncryptKey(lpszKey).c_str(), Encrypt(lpszValue).c_str());	
}

HRESULT SetSubProdInfoW(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPCWSTR lpszValue)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s\\%s"), lpszProdId, lpszSubProdId);

	return SetString(HKEY_CLASSES_ROOT, strSubKey, EncryptKey(lpszKey).c_str(), Encrypt(lpszValue).c_str());
}

HRESULT GetProdInfoW(LPCWSTR lpszProdId,  LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s"), lpszProdId);

	LONG dwRet = 0;
	std::wstring szValue = GetString(HKEY_CLASSES_ROOT, strSubKey,  EncryptKey(lpszKey).c_str(), &dwRet);
	if (dwRet == ERROR_SUCCESS && szValue.length())
	{
		wcscpy_s(lpszValue, cch, Decrypt(szValue.c_str()).c_str());
	}
	
	return dwRet;
}


HRESULT GetSubProdInfoW(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s\\%s"), lpszProdId, lpszSubProdId);
	LONG dwRet = 0;
	std::wstring szValue = GetString(HKEY_CLASSES_ROOT, strSubKey,  EncryptKey(lpszKey).c_str(), &dwRet);
	if (dwRet == ERROR_SUCCESS && szValue.length())
	{
		wcscpy_s(lpszValue, cch, Decrypt(szValue.c_str()).c_str());
	}
	return dwRet;
}


HRESULT DelProdW(LPCWSTR lpszProdId)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s\\"), lpszProdId);
	return DeleteKey(HKEY_CLASSES_ROOT,strSubKey);
}

HRESULT DelProdInfoW(LPCWSTR lpszProdId, LPCWSTR lpszKey)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s\\"), lpszProdId);
	return DeleteValue(HKEY_CLASSES_ROOT, strSubKey, EncryptKey(lpszKey).c_str());
}


HRESULT DelSubProdW(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s\\%s\\"), lpszProdId, lpszSubProdId);
	return RegDeleteKey(HKEY_CLASSES_ROOT,strSubKey);
}

HRESULT DelSubProdInfoW(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey)
{
	CString strSubKey;
	strSubKey.Format(_T("{BFF93DC2-2658-4d82-8718-687CBD8065B5}\\%s\\%s\\"), lpszProdId, lpszSubProdId);

	return DeleteValue(HKEY_CLASSES_ROOT, strSubKey, EncryptKey(lpszKey).c_str());
}



//////////////////////////////////////////////////////////////////////////
HRESULT SetProdInfoA(LPCSTR lpszProdId, LPCSTR lpszKey, LPCSTR lpszValue)
{
	USES_CONVERSION;
	return SetProdInfoW(A2W(lpszProdId), A2W(lpszKey),A2W(lpszValue));
}

HRESULT SetSubProdInfoA(LPCSTR lpszProdId, LPCSTR lpszSubProdId, LPCSTR lpszKey, LPCSTR lpszValue)
{
	USES_CONVERSION;
	return SetSubProdInfoW(A2W(lpszProdId),A2W(lpszSubProdId),A2W(lpszKey),A2W(lpszValue));
}

HRESULT GetProdInfoA(LPCSTR lpszProdId,  LPCSTR lpszKey, LPSTR lpszValue, DWORD cch)
{
	USES_CONVERSION;
	WCHAR szValue[2048] = {0};
	HRESULT hRet = GetProdInfoW(A2W(lpszProdId),A2W(lpszKey),szValue, _countof(szValue));
	strcpy_s(lpszValue, cch, W2A(szValue));
	return hRet;
}


HRESULT GetSubProdInfoA(LPCSTR lpszProdId, LPCSTR lpszSubProdId, LPCSTR lpszKey, LPSTR lpszValue, DWORD cch)
{
	USES_CONVERSION;
	WCHAR szValue[2048] = {0};
	HRESULT hRet = GetSubProdInfoW(A2W(lpszProdId),A2W(lpszSubProdId),A2W(lpszKey),szValue, _countof(szValue));
	strcpy_s(lpszValue, cch, W2A(szValue));
	return S_OK;
}


HRESULT DelProdA(LPCSTR lpszProdId)
{
	USES_CONVERSION;
	return DelProdW(A2W(lpszProdId));
}

HRESULT DelProdInfoA(LPCSTR lpszProdId, LPCSTR lpszKey)
{
	USES_CONVERSION;
	return DelProdInfoW(A2W(lpszProdId), A2W(lpszKey));
}


HRESULT DelSubProdA(LPCSTR lpszProdId, LPCSTR lpszSubProdId)
{
	USES_CONVERSION;
	return DelSubProdW(A2W(lpszProdId), A2W(lpszSubProdId));
}

HRESULT DelSubProdInfoA(LPCSTR lpszProdId, LPCSTR lpszSubProdId, LPCSTR lpszKey)
{
	USES_CONVERSION;
	return DelSubProdInfoW(A2W(lpszProdId),A2W(lpszSubProdId),A2W(lpszKey));
}