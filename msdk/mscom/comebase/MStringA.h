#pragma once

#include "combase/IMsString.h"
#include "SyncObject/criticalsection.h"

template<typename LockType = CNullCriticalSection>
class MStringA : 
	public IStringA,
	public ATL::CStringA,
	private CUnknownImp
{
public:
	typedef CStackLockWrapper<LockType> SRVATUOLOCK;
	UNKNOWN_IMP1(IStringA);
	STDMETHOD(init_class)(IMSBase* pRot,IMSBase* pOuter)
	{
		return S_OK;
	}
public:
	MStringA(): CStringA()
	{

	}

	STDMETHOD(Clone)(IStringA** pString)
	{
		SRVATUOLOCK lock(m_lock);

		UTIL::com_ptr<IStringA> pStrA;
		RFAILED(DllQuickCreateInstance(CLSID_StringW , re_uuidof(IStringA),pStrA,NULL));
		RASSERT(pStrA , E_FAIL);
		RFAILED(pStrA->Append(this->GetBuffer()));
		return pStrA->QueryInterface(re_uuidof(IStringA) , (void**)pString);
	}

	STDMETHOD_(VOID , Clear)()
	{
		SRVATUOLOCK lock(m_lock);
		return CStringA::Empty();
	}

	STDMETHOD_(LPCSTR,GetBuffer)()
	{
		SRVATUOLOCK lock(m_lock);

		return CStringA::GetBuffer();
	}

	STDMETHOD_(LPSTR,GetBufferSetLength)(INT dwLen)
	{
		SRVATUOLOCK lock(m_lock);

		return CStringA::GetBufferSetLength(dwLen);
	}
	STDMETHOD_(VOID , ReleaseBuffer)(int nNewLength = -1)
	{
		SRVATUOLOCK lock(m_lock);

		return CStringA::ReleaseBuffer(nNewLength);
	}

	STDMETHOD_(INT,GetLength)()
	{
		SRVATUOLOCK lock(m_lock);

		return CStringA::GetLength();
	}

	STDMETHOD(Append)(LPCSTR lpStr)
	{
		SRVATUOLOCK lock(m_lock);

		RASSERT(lpStr , E_INVALIDARG);
		CStringA::Append(lpStr);
		return S_OK;
	}

	STDMETHOD_(INT,Compare)(LPCWSTR lpStr)
	{
		USES_CONVERSION;
		LPCSTR lpData = W2A(lpStr);
		RASSERT(lpData , 1);
		return CStringA::Compare(lpData);
	}

	STDMETHOD_(INT,Compare)(LPCSTR lpStr)
	{
		RASSERT(lpStr,1);
		return CStringA::Compare(lpStr);
	}

	STDMETHOD_(INT,Compare)(IStringA* lpStr)
	{
		RASSERT(lpStr,1);
		return CStringA::Compare(lpStr->GetBuffer());
	}

	STDMETHOD_(INT,Compare)(IStringW* lpStr)
	{
		USES_CONVERSION;
		LPCSTR lpData = W2A(lpStr->GetBuffer());
		RASSERT(lpData , 1);
		return CStringA::Compare(lpData);
	}

	STDMETHOD_(INT,CompareNoCase)(LPCWSTR lpStr)
	{
		USES_CONVERSION;
		LPCSTR lpData = W2A(lpStr);
		RASSERT(lpData , 1);
		return CStringA::CompareNoCase(lpData);
	}

	STDMETHOD_(INT,CompareNoCase)(LPCSTR lpStr)
	{
		RASSERT(lpStr,1);
		return CStringA::CompareNoCase(lpStr);
	}

	STDMETHOD_(INT,CompareNoCase)(IStringA* lpStr)
	{
		RASSERT(lpStr,1);
		return CStringA::CompareNoCase(lpStr->GetBuffer());
	}

	STDMETHOD_(INT,CompareNoCase)(IStringW* lpStr)
	{
		USES_CONVERSION;
		LPCSTR lpData = W2A(lpStr->GetBuffer());
		RASSERT(lpData , 1);
		return CStringA::CompareNoCase(lpData);
	}

	virtual void Format(LPCSTR lpFormat , ...)
	{
		SRVATUOLOCK lock(m_lock);

		RASSERT(lpFormat , );

		va_list args;
		va_start( args, lpFormat );
		CStringA::FormatV(lpFormat , args);
		va_end( args );
	}

	virtual void AppendFormat(LPCSTR lpFormat,...)
	{
		SRVATUOLOCK lock(m_lock);

		RASSERT(lpFormat , );
		va_list args;
		va_start( args, lpFormat );
		CStringA::AppendFormatV(lpFormat , args);
		va_end( args );
	}

	STDMETHOD(ToStringW)(IStringW** pString)
	{
		SRVATUOLOCK lock(m_lock);

		RASSERT(pString , E_INVALIDARG);

		USES_CONVERSION;
		LPCWSTR lpData = A2W(CStringA::GetBuffer());
		RASSERT(lpData , E_FAIL);


		UTIL::com_ptr<IStringW> pStrW;
		RFAILED(DllQuickCreateInstance(CLSID_StringW , re_uuidof(IStringW),pStrW,NULL));
 		RASSERT(pStrW , E_FAIL);
		RFAILED(pStrW->Append(lpData));

 		return pStrW->QueryInterface(re_uuidof(IStringW) , (void**)pString);
	}
	LockType m_lock;
};

