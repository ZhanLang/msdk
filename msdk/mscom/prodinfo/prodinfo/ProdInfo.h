#pragma once

#include "prodinfo/IPordInfo.h"
class CProdInfo : 
	public IMsProdInfo,
	public CMsComBase<CProdInfo>
{
public:
	UNKNOWN_IMP1(IMsProdInfo);
	CProdInfo(void);
	~CProdInfo(void);

protected://IMsProdInfo
	STDMETHOD(SetProdInfo)(LPCWSTR lpszKey, LPCWSTR lpszValue);
	STDMETHOD(GetProdInfo)(LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch);

	STDMETHOD(SetProdInfo)(LPCWSTR lpszProdId, LPCWSTR lpszKey, LPCWSTR lpszValue);
	STDMETHOD(GetProdInfo)(LPCWSTR lpszProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch);

	STDMETHOD(SetSubProdInfo)(LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPCWSTR lpszValue );
	STDMETHOD(GetSubProdInfo)(LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch);

	STDMETHOD(SetSubProdInfo)(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPCWSTR lpszValue);
	STDMETHOD(GetSubProdInfo)(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey, LPWSTR lpszValue, DWORD cch);

	//
	STDMETHOD(DelProd)();
	STDMETHOD(DelProdInfo)(LPCWSTR lpszKey);

	STDMETHOD(DelProd)(LPCWSTR lpszProdId);
	STDMETHOD(DelProdInfo)(LPCWSTR lpszProdId, LPCWSTR lpszKey);

	STDMETHOD(DelSubProd)(LPCWSTR lpszSubProdId);
	STDMETHOD(DelSubProdInfo)(LPCWSTR lpszSubProdId, LPCWSTR lpszKey);

	STDMETHOD(DelSubProd)(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId);
	STDMETHOD(DelSubProdInfo)(LPCWSTR lpszProdId, LPCWSTR lpszSubProdId, LPCWSTR lpszKey);

private:
	LPCWSTR GetProdId();
	LPCWSTR GetSubProdId();
};

