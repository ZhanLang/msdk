#pragma once

#include "combase/IMsString.h"
#include "SyncObject/criticalsection.h"

template<typename LockType = CNullCriticalSection>
class MStringW:
	public IStringW,
	public ATL::CStringW,
	private CUnknownImp
{
public:
	typedef CStackLockWrapper<LockType> SRVATUOLOCK;
	UNKNOWN_IMP1(IStringW);
	STDMETHOD(init_class)(IMSBase* pRot,IMSBase* pOuter)
	{
		return S_OK;
	}
public:
	MStringW():CStringW()
	{

	}

	STDMETHOD(Clone)(IStringW** pString)
	{
		SRVATUOLOCK lock(m_lock);

		UTIL::com_ptr<IStringW> pStrW;
		RFAILED(DllQuickCreateInstance(CLSID_StringW , re_uuidof(IStringA),pStrW,NULL));
		RASSERT(pStrW , E_FAIL);
		RFAILED(pStrW->Append(this->GetBuffer()));
		return pStrW->QueryInterface(re_uuidof(IStringW) , (void**)pString);
	}

	STDMETHOD_(VOID,Clear)()
	{
		SRVATUOLOCK lock(m_lock);
		return CStringW::Empty();
	}

	STDMETHOD_(LPCWSTR,GetBuffer)()
	{
		SRVATUOLOCK lock(m_lock);

		return CStringW::GetBuffer();
	}

	STDMETHOD_(LPWSTR,GetBufferSetLength)(INT dwLen)
	{
		SRVATUOLOCK lock(m_lock);

		return CStringW::GetBufferSetLength(dwLen);
	}
	STDMETHOD_(VOID , ReleaseBuffer)(int nNewLength = -1)
	{
		SRVATUOLOCK lock(m_lock);

		CStringW::ReleaseBuffer(nNewLength);
	}

	STDMETHOD_(INT,GetLength)()
	{
		SRVATUOLOCK lock(m_lock);
		return CStringW::GetLength();
	}

	STDMETHOD(Append)(LPCWSTR lpStr)
	{
		RASSERT(lpStr , S_FALSE);

		SRVATUOLOCK lock(m_lock);
		RASSERT(lpStr , E_INVALIDARG);
		CStringW::Append(lpStr);
		return S_OK;

	}
	STDMETHOD_(INT,Compare)(LPCWSTR lpStr)
	{
		return CStringW::Compare(lpStr);
	}

	STDMETHOD_(INT,Compare)(LPCSTR lpStr)
	{
		RASSERT(lpStr,1);
		USES_CONVERSION;
		LPCWSTR lpData = A2W(lpStr);
		RASSERT(lpData , 1);

		return CStringW::Compare(lpData);
	}

	STDMETHOD_(INT,Compare)(IStringA* lpStr)
	{
		RASSERT(lpStr,1);
		USES_CONVERSION;
		LPCWSTR lpData = A2W(lpStr->GetBuffer());
		RASSERT(lpData , 1);

		return CStringW::Compare(lpData);
	}

	STDMETHOD_(INT,Compare)(IStringW* lpStr)
	{
		RASSERT(lpStr , 1);
		return CStringW::Compare(lpStr->GetBuffer());
	}

	STDMETHOD_(INT,CompareNoCase)(LPCWSTR lpStr)
	{
		RASSERT(lpStr , 1);
		return CStringW::CompareNoCase(lpStr);
	}

	STDMETHOD_(INT,CompareNoCase)(LPCSTR lpStr)
	{
		RASSERT(lpStr,1);
		USES_CONVERSION;
		LPCWSTR lpData = A2W(lpStr);
		return CStringW::CompareNoCase(lpData);
	}

	STDMETHOD_(INT,CompareNoCase)(IStringA* lpStr)
	{
		RASSERT(lpStr,1);
		USES_CONVERSION;
		LPCWSTR lpData = A2W(lpStr->GetBuffer());
		return CStringW::CompareNoCase(lpData);
	}

	STDMETHOD_(INT,CompareNoCase)(IStringW* lpStr)
	{
		USES_CONVERSION;
		RASSERT(lpStr , 1);
		return CStringW::CompareNoCase(lpStr->GetBuffer());
	}

	virtual void Format(LPCWSTR lpFormat , ...)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(lpFormat , );

		va_list args;
		va_start( args, lpFormat );
		CStringW::FormatV(lpFormat , args);
		va_end( args );
	}

	virtual void AppendFormat(LPCWSTR lpFormat,...)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(lpFormat , );
		va_list args;
		va_start( args, lpFormat );
		CStringW::AppendFormatV(lpFormat , args);
		va_end( args );
	}

	STDMETHOD(ToStringA)(IStringA** pString)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(pString , E_INVALIDARG);
		USES_CONVERSION;
		LPCSTR lpData = W2A(CStringW::GetBuffer());
		
		UTIL::com_ptr<IStringA> pStrA;
		RFAILED(DllQuickCreateInstance(CLSID_StringA , re_uuidof(IStringA),pStrA,NULL));
		RASSERT(pStrA , E_FAIL);
		RFAILED(pStrA->Append(lpData));
		return pStrA->QueryInterface(re_uuidof(IStringA) , (void**)pString);
	}

	LockType m_lock;
};

