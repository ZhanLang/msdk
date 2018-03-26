// prodinfo.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "prodinfo\ProdInfoImp.h"
#include "ProdInfo.h"




CProdInfo::CProdInfo(void)
{
}


CProdInfo::~CProdInfo(void)
{
}

STDMETHODIMP CProdInfo::SetProdInfo(LPCWSTR lpszKey, LPCWSTR lpszValue)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::SetProdInfo::GetProdId Failed."));
		return E_FAIL;
	}

	return ::SetProdInfoW(lpszProdId, lpszKey, lpszValue);
}

STDMETHODIMP CProdInfo::SetProdInfo(LPCWSTR lpszProdId, LPCWSTR lpszKey, LPCWSTR lpszValue)
{
	return ::SetProdInfoW(lpszProdId, lpszKey, lpszValue);
}


STDMETHODIMP CProdInfo::GetProdInfo(LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::SetProdInfo::GetProdInfo Failed."));
		return NULL;
	}
	return ::GetProdInfoW(lpszProdId, lpszKey, lpszValue, cch);
}

STDMETHODIMP CProdInfo::GetProdInfo(LPCWSTR lpszProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch)
{
	return ::GetProdInfoW(lpszProdId, lpszKey, lpszValue, cch);
}

STDMETHODIMP CProdInfo::SetSubProdInfo(LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPCWSTR lpszValue)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::SetSubProdInfo::GetProdInfo Failed."));
		return NULL;
	}

	return ::SetSubProdInfoW(lpszProdId, lpszSubProdId, lpszKey, lpszValue);
}

STDMETHODIMP CProdInfo::SetSubProdInfo(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPCWSTR lpszValue)
{
	return ::SetSubProdInfoW(lpszProdId, lpszSubProdId, lpszKey, lpszValue);
}

STDMETHODIMP CProdInfo::GetSubProdInfo(LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::GetSubProdInfo::GetProdInfo Failed."));
		return NULL;
	}

	return ::GetSubProdInfoW(lpszProdId, lpszSubProdId, lpszKey, lpszValue, cch);
}

STDMETHODIMP CProdInfo::GetSubProdInfo(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch)
{
	return ::GetSubProdInfoW(lpszProdId, lpszSubProdId, lpszKey, lpszValue, cch);
}

LPCWSTR CProdInfo::GetProdId()
{
	return GetEnvParamString("prouctid");
}

LPCWSTR CProdInfo::GetSubProdId()
{
	return GetEnvParamString("subprodid");
}

STDMETHODIMP CProdInfo::DelProd()
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::DelProd::GetProdInfo Failed."));
		return NULL;
	}


	return ::DelProdW(lpszProdId);
}

STDMETHODIMP CProdInfo::DelProd(LPCWSTR lpszProdId)
{
	return ::DelProdW(lpszProdId);
}

STDMETHODIMP CProdInfo::DelProdInfo(LPCWSTR lpszKey)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::DelProdInfo::GetProdInfo Failed."));
		return NULL;
	}

	return ::DelProdInfoW(lpszProdId, lpszKey);
}

STDMETHODIMP CProdInfo::DelProdInfo(LPCWSTR lpszProdId, LPCWSTR lpszKey)
{
	return ::DelProdInfoW(lpszProdId, lpszKey);
}

STDMETHODIMP CProdInfo::DelSubProd(LPCWSTR lpszSubProdId)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::DelSubProd::GetProdInfo Failed."));
		return NULL;
	}
	return ::DelSubProdW(lpszProdId, lpszSubProdId);
}

STDMETHODIMP CProdInfo::DelSubProd(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId)
{
	return ::DelSubProdW(lpszProdId, lpszSubProdId);
}

STDMETHODIMP CProdInfo::DelSubProdInfo(LPCWSTR lpszSubProdId, LPCWSTR lpszKey)
{
	LPCWSTR lpszProdId = GetProdId();
	if (!( lpszProdId && wcslen(lpszProdId)))
	{
		GrpError(GroupName, MsgLevel_Error, _T("CProdInfo::DelSubProdInfo::GetProdInfo Failed."));
		return NULL;
	}

	return ::DelSubProdW(lpszProdId, lpszSubProdId);
}

STDMETHODIMP CProdInfo::DelSubProdInfo(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey)
{
	return DelSubProdW(lpszProdId, lpszSubProdId);
}

