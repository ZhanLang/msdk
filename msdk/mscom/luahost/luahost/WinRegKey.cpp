#include "StdAfx.h"
#include "WinRegKey.h"
#include "luahost/luahost.h"
#if defined(_MEM_CHECK_)

		#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
		#endif

#endif

namespace winfunc
{

bool CWinRegKey::Create(_In_ LPCTSTR szKeyParentName, _In_z_ LPCTSTR lpszKeyName,
						_In_opt_z_ LPCTSTR lpszClass, _In_ DWORD dwOptions, _In_ LPCTSTR samDesired) throw()
{
	try{

	
	return CRegKey::Create(GetHKeyByName(szKeyParentName), 
							lpszKeyName, GetClassByName(lpszClass), 
							dwOptions, GetREGSAMByName(samDesired), 
							(LPSECURITY_ATTRIBUTES)NULL, NULL) 
							== ERROR_SUCCESS
							? true : false;
	}catch(...)
	{
		return false;
	}
}
// Open an existing registry key.
bool CWinRegKey::Open(LPCTSTR szKeyParentName, LPCTSTR lpszKeyName, LPCTSTR samDesired) throw()
{
	return CRegKey::Open(GetHKeyByName(szKeyParentName), lpszKeyName, GetREGSAMByName(samDesired))
		== ERROR_SUCCESS ? true : false;
}

bool CWinRegKey::SetStringValue(_In_opt_z_ LPCTSTR pszValueName, _In_opt_z_ LPCTSTR pszValue, _In_ LPCTSTR szType) throw()
{
	return CRegKey::SetStringValue(pszValueName, pszValue, (DWORD)GetClassByName(szType))
		== ERROR_SUCCESS ? true : false;
}


TString CWinRegKey::QueryStringValue(LPCTSTR pszValueName) throw()
{
	TString strRet;	
	DWORD dwType = 0;
	ULONG nBytes = 0;	
	if(ERROR_SUCCESS==CRegKey::QueryValue(pszValueName, &dwType, NULL, &nBytes))
	{
		if(nBytes<=0) return strRet;

		strRet.resize(nBytes);
		if(ERROR_INVALID_DATA==CRegKey::QueryStringValue(pszValueName, &strRet[0], &nBytes))
			strRet.resize(0);			
	}	
	return strRet;
}

bool CWinRegKey::ModifyMultiStringValue(LPCTSTR pszValueName, LPCTSTR pszPattern) throw()
{
	RSLOG(RSLOG_ALERT,MsgLevel_Error, _T("CWinRegKey->ModifyMultiStringValue(%s, %s)\n"), pszValueName?pszValueName:_T("null"), pszPattern?pszPattern:_T("null"));
	if (pszValueName == NULL || pszPattern == NULL)
		return false;

	TString		strRet;
	DWORD dwType = 0;
	ULONG nBytes = 0;

	LONG  err = CRegKey::QueryValue(pszValueName, &dwType, NULL, &nBytes);
	if (err != ERROR_SUCCESS) {
		if (err == ERROR_FILE_NOT_FOUND) {
			RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("CWinRegKey->ModifyMultiStringValue RegValue not found"));
			return true;
		}
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("CWinRegKey->ModifyMultiStringValue Failed, ErrCode = %d\n"), err);
		return false;
	}
	
	if(nBytes<=0) return true;

	strRet.resize(nBytes);
	if(ERROR_INVALID_DATA == CRegKey::QueryMultiStringValue(pszValueName, &strRet[0], &nBytes)) {
		RSLOG(RSLOG_ALERT, MsgLevel_Error,_T("CWinRegKey->ModifyMultiStringValue Failed, ErrCode = %d\n"), ::GetLastError());
		return false;
	}

	TString::size_type e = 0,
					   p = strRet.find(pszPattern, e);
	while (p != TString::npos) {
		if ((e = strRet.find(_T('\0'), p)) != TString::npos) ++e;
		strRet.erase(p, e);
		p = strRet.find(pszPattern, p);
	}

	err = CRegKey::SetMultiStringValue(pszValueName, strRet.c_str());
	RSLOG(RSLOG_DETAIL,MsgLevel_Error, _T("CWinRegKey->ModifyMultiStringValue(%s, %s) Return: %d[ERROR_SUCCESS = 0]\n"),	pszValueName?pszValueName:_T("null"), 
																											pszPattern?pszPattern:_T("null"),
																											err);
	return err == ERROR_SUCCESS;
}

DWORD CWinRegKey::QueryDWORDValue(LPCTSTR pszValueName, DWORD dwDefault/* = 0*/) throw()
{
	CRegKey::QueryDWORDValue(pszValueName, dwDefault);	
	return dwDefault;
}

ULONGLONG CWinRegKey::QueryQWORDValue(LPCTSTR pszValueName, ULONGLONG qwDefault) throw()
{
	CRegKey::QueryQWORDValue(pszValueName, qwDefault);	
	return qwDefault;
}

bool CWinRegKey::DeleteSubKey(LPCTSTR pszKeyName) {
	LONG result = CRegKey::DeleteSubKey(pszKeyName);
	return (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) ? true : false;
}

bool CWinRegKey::RecurseDeleteKey(LPCTSTR pszKeyName) {
	LONG result = CRegKey::RecurseDeleteKey(pszKeyName);
	return (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) ? true : false;
}

bool CWinRegKey::DeleteValue(LPCTSTR pszValueName) {
	LONG result = CRegKey::DeleteValue(pszValueName);
	return (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) ? true : false;
}

bool CWinRegKey::Close() {
	return CRegKey::Close() == ERROR_SUCCESS ? true : false;
}

BOOL CWinRegKey::BindToLua(ILuaVM* pLuaVM)
{
	GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("winfunc")
		+mluabind::Class<winfunc::CWinRegKey>("WinRegKey")
		.Constructor()
		.Method("Create",					&winfunc::CWinRegKey::Create)
		.Method("Open",						&winfunc::CWinRegKey::Open)
		.Method("SetDWORDValue", (LONG (winfunc::CWinRegKey::*) (LPCTSTR, DWORD) )&winfunc::CWinRegKey::SetDWORDValue)
		.Method("SetQWORDValue", (LONG (winfunc::CWinRegKey::*) (LPCTSTR, ULONGLONG) )&winfunc::CWinRegKey::SetQWORDValue)
		.Method("SetStringValue",			&winfunc::CWinRegKey::SetStringValue)
		.Method("SetMultiStringValue", (LONG (winfunc::CWinRegKey::*) (LPCTSTR, LPCTSTR) )&winfunc::CWinRegKey::SetMultiStringValue)
		.Method("SetKeyValue", (LONG (winfunc::CWinRegKey::*) (LPCTSTR, LPCTSTR, LPCTSTR) )&winfunc::CWinRegKey::SetKeyValue)
		.Method("QueryStringValue",			&winfunc::CWinRegKey::QueryStringValue)
		.Method("ModifyMultiStringValue",	&winfunc::CWinRegKey::ModifyMultiStringValue)
		.Method("QueryDWORDValue", (DWORD (winfunc::CWinRegKey::*) (LPCTSTR, DWORD)throw() )&winfunc::CWinRegKey::QueryDWORDValue)
		.Method("QueryQWORDValue", (ULONGLONG (winfunc::CWinRegKey::*) (LPCTSTR, ULONGLONG)throw() )&winfunc::CWinRegKey::QueryQWORDValue)
		.Method("Close",					&winfunc::CWinRegKey::Close)
		.Method("Flush", (LONG (winfunc::CWinRegKey::*) () )&winfunc::CWinRegKey::Flush)
		.Method("DeleteSubKey",				&winfunc::CWinRegKey::DeleteSubKey)
		.Method("RecurseDeleteKey",			&winfunc::CWinRegKey::RecurseDeleteKey)
		.Method("DeleteValue",				&winfunc::CWinRegKey::DeleteValue)
		);

	return TRUE;
}

};//namespace winfunc