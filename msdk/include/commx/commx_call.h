#pragma once

#include "commx/bufstream.h"

#define COMX3_ARG_MAP(arg) 

#define BEGIN_COMX3_MSG_MAP(msg, func)
	
#define END_COMX3_MSG_MAP


class CComX3CallBackImpl
	:public IMessageCallBack
{
public:
	virtual HRESULT Dispatch(IMessage* pMsg)
	{
		CLIENTINFO si = { 0 }, ti = { 0 };
		pMsg->GetSource(&si);
		pMsg->GetTarget(&ti);

		MESSAGEINFO* pMsgInfo = { 0 };
		pMsg->GetMessageInfo(&pMsgInfo);

		PBYTE pInData = pMsgInfo->pInData;
		INT nInCch = pMsgInfo->nInCch;


		if (pMsgInfo->msgid == 100)
		{

		}
		

	  // pMsg->PutOutputInfo()
	}
};



enum _ValueInfo
{
	VALUEINFO_UNKNOWN,
	VALUEINFO_IN,
	VALUEINFO_OUT,
	VALUEINFO_INOUT,
};

template<typename _Type>
class CValue
{
public:
	CValue(_Type value) : m_value(value){}
	const static _ValueInfo ValueInfo = VALUEINFO_UNKNOWN;
	virtual HRESULT serialize(CBasicStream* pStream)
	{
		CBuffStream* pBufStream = static_cast<CBuffStream*>(pStream);
		return pBufStream->Serialize(this->m_value);
	}

	virtual HRESULT unserialize(CBasicStream* pStream)
	{
		CBuffStream* pBufStream = static_cast<CBuffStream*>(pStream);
		return pBufStream->Unserialize(&this->m_value);
	}
public:
	_Type m_value;
};

__declspec(selectany) CValue<byte> s_nullValue('\0');

/*
/*传入参数类型
/*
******************/
template<typename _Type = byte>
class CInValue
{
public:
	const static _ValueInfo ValueInfo = VALUEINFO_IN;
};


template<typename _Type>
class COutValue
{
public:
	const static _ValueInfo ValueInfo = VALUEINFO_OUT;
};


template<typename _Type>
class CInOutValue
	:public CValue<_Type&>
{
public:
	const static _ValueInfo ValueInfo = VALUEINFO_INOUT;

public:
	CInOutValue(_Type& value) :CValue(value){}
	CInOutValue& operator = (_Type value)
	{
		m_value = value;
		return *this;
	}

	operator _Type () const
	{
		return m_value;
	}
};

class CComX3CallImpl
{
public:
	typedef  CValue<BYTE> _D;


	HRESULT Init(IClientObject* pClientObject)
	{
		RASSERT(pClientObject, E_FAIL);
		m_pClientObject = pClientObject;
		return S_OK;
	}

	HRESULT UnInit()
	{
		m_pClientObject = NULL;
		return S_OK;
	}

	template<class _R, class _A1 = _D, class _A2 = _D, class _A3 = _D, class 
		_A4 = _D, class _A5 = _D, class _A6 = _D, class _A7 = _D, class _A8 = _D, class _A9 = _D>
	_R SendMessage(_R defAultResult, DEFID defid, MASKID maskid, MSGID msgid, 
		_A1 a1 = s_nullValue, _A2 a2 = s_nullValue, _A3 a3 = s_nullValue, 
		_A4 a4 = s_nullValue, _A5 a5 = s_nullValue, _A6 a6 = s_nullValue,
		_A7 a7 = s_nullValue, _A8 a8 = s_nullValue, _A9 a9 = s_nullValue)
	{
		_R ret = defAultResult;
		
		//包装入参


		//m_pClientObject->SendMessage(defid, maskid, msgid, NULL, NULL);

		return ret;
	}


// 	template<class _R, class _A1>
// 	_R SendMessage(DEFID defid, MASKID maskid, MSGID msgid, _A1 arg_1)
// 	{
// 		_R ret;
// 		return ret;
// 	}
// 
// 	template<class _R, class _A1, class _A2>
// 	_R SendMessage(DEFID defid, MASKID maskid, MSGID msgid, _A1 arg_1, _A2 arg_2)
// 	{
// 		_R ret;
// 		return ret;
// 	}


private:
	IClientObject* m_pClientObject;
};
