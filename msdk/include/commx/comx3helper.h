#pragma once
#include "commx\commx3.h"


namespace msdk{;
namespace mscom{;

#define BEGIN_COMX3_MSG_MAP(_theclass)\
	virtual HRESULT Dispatch(IMessage* pMsg)\
{\
	RASSERT(pMsg, E_FAIL);\
	MESSAGEINFO* pMsgInfo = NULL;\
	pMsg->GetMessageInfo(&pMsgInfo);\
	RASSERT(pMsgInfo, E_FAIL);\
	CLIENTINFO pClientInfo = {NULL};\
	RFAILED(pMsg->GetSource(&pClientInfo))\


#define END_COMX3_MSG_MAP return S_OK;}


//在使用过程中必须要注意先后顺序

//定义COMX3消息映射
#define DEFINE_COMX3_MSG(_msgid, func)\
	if (pMsgInfo->msgid == _msgid)\
	{\
		return func(pClientInfo, pMsgInfo, pMsg);\
	}\


//指定DEFID
#define COMX3_DEFID_BEGIN(_defid)\
		if (pMsgInfo->defid == _defid){\

#define COMX3_DEFID_END }

//指定MASKID
#define COMX3_MASKID_BEGIN(_maskid)\
	if (pMsgInfo->maskid == _maskid){\

#define COMX3_MASKID_END }

#define COMX3_DEFID_MADKID_BEGIN(_defid,_maskid)\
	if (pMsgInfo->source_defid == _defid && pMsgInfo->source_maskid == _maskid){\

#define COMX3_DEFID_MADKID_END }


template<class T>
class CComx3Helper : public IMessageCallBack
{
public:
	CComx3Helper()
	{
		 m_CCenter = NULL;
		 m_Client  = NULL;
	};
	
	//封装发消息，自动处理Session问题和自动处理广播问题
	//defid=0使用广播
	//bSendByCurrentSession : TRUE 只发送到当前session下
	HRESULT SendMsg(DEFID defid, MASKID maskid, MSGID msgid, 
					void* pInData, int nInCch
					)
	{
		if(m_Client)
		{
			CLIENTINFO clientinfo;
			ZeroMemory(&clientinfo, sizeof(clientinfo));
			clientinfo.dwSize = sizeof(clientinfo);	

			if(maskid != 0)
			{
				clientinfo.dwMask |= CI_MASK_MASKID;
				clientinfo.maskid = maskid;
			}

			if(defid != 0)
			{
				clientinfo.dwMask |= CI_MASK_DEFID;
				clientinfo.defid = defid;
			}

// 			if(bSendByCurrentSession)
// 			{
// 				clientinfo.dwMask |= CI_MASK_SID;
// 			}

			//return m_Client->BroadcastMessage(maskid, msgid, pInData, nInCch, 0);
			return m_Client->BroadcastMessageEx(&clientinfo, msgid, pInData, nInCch, 0);
		}

		return E_FAIL;
	}

	HRESULT SendMsgTo( OBJID objid, MSGID msgid, void* pInData, int nInCch)
	{
		if (m_Client)
		{
			return m_Client->SendMessageTo(objid,msgid,pInData, nInCch,0);
		}
		return E_FAIL;
	}

	HRESULT SendMsgToEx(OBJID objid, MSGID msgid, 
		void* pInData, int nInCch, 
		void** ppOutData, int* pOutCch)
	{
		if (m_Client)
		{
			return m_Client->SendMessageToEx(objid, msgid, pInData, nInCch, ppOutData, pOutCch, 0);
		}

		return E_FAIL;
	}

	HRESULT SendMsgEx(DEFID defid, MASKID maskid, MSGID msgid, 
						void* pInData, int nInCch, 
						void** ppOutData, int* pOutCch)
	{
		if(m_Client)
		{
			return m_Client->SendMessageEx(defid, maskid, msgid, pInData, nInCch, ppOutData, pOutCch, 0);
		}

		return E_FAIL;
	}

	void* Allocate(size_t size)
	{
		if(m_ComX3)
		{
			return m_ComX3->Allocate(size);
		}

		return NULL;
	}

	

	void FreeCallCenter(void *pointer)
	{
		if(m_ComX3)
		{
			m_ComX3->FreeCallCenter(pointer);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	HRESULT CreateComx3(IMSBase* pBaseRot, DEFID defid, MASKID maskid,LPCSTR szSubsysName, LPCSTR szWorkPath = NULL, DWORD dwType = CCENTER_START_BOTH)
	{
		HRESULT hr = E_FAIL;

		UTIL::com_ptr<IMscomRunningObjectTable> pRot = pBaseRot;

		if(pRot && szSubsysName)
		{
			hr = pRot->CreateInstance(CLSID_CComX3, NULL, __uuidof(IComX3), (void**)&m_ComX3);
			if(SUCCEEDED(hr) && m_ComX3)
			{
				
				m_CCenter = m_ComX3->Initialize(szSubsysName, szWorkPath);
				if(!m_CCenter)
				{
					hr = E_FAIL;
				}
				else
				{
					UTIL::com_ptr<IMessageCallBack> pCallBack = this;
					m_Client = m_CCenter->Logon(defid, maskid, pCallBack);

					if(!m_Client)
						hr = E_FAIL;
				}
			}
		}

		if(FAILED(hr))
		{
			ReleaseComx3();
		}

		return hr;
	}

	HRESULT ReleaseComx3()
	{
		if(m_ComX3)
		{
			if (m_CCenter)
			{
				if (m_Client)
				{
					m_CCenter->Logoff(m_Client);
					m_Client = NULL;
				}
				m_ComX3->Uninitialize(m_CCenter);
				m_CCenter = NULL;
			}

			m_ComX3 = INULL;
		}
		return S_OK;
	}


	virtual HRESULT Dispatch(IMessage* pMsg)
	{
		return S_OK;
	}


	IClientObject* GetClient(){return m_Client;}

private:
	UTIL::com_ptr<IComX3>			m_ComX3;
	ICCenter*						m_CCenter;
	IClientObject*					m_Client;
};


};
};