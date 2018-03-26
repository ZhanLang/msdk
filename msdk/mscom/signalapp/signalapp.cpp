// signalapp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "signalapp.h"
#include "xmlvalue/XmlValueW.h"
#define SIGNAL_DEFID  999
#define SIGNAL_MASKID 10101
#define SIGNAL_MSGID  10102
STDMETHODIMP CSignalAppliction::Init(void*)
{
	
	RFAILED(GetGlobalMutexName());
	RFAILED(CreateComX3());
	

	BOOL bRet = m_globalMutex.OpenMutex(MUTEX_ALL_ACCESS , TRUE , m_strGlobalMutexName);
	//进程已经存在
	if (bRet)
	{
		RFAILED(SendMessage(ReadProcessInfo()));
		NotifyExit();
		//刻意返回失败
		return E_FAIL;
	}
	return m_globalMutex.CreateMutex(NULL , TRUE , m_strGlobalMutexName) ? S_OK : E_FAIL;
}

STDMETHODIMP CSignalAppliction::Uninit()
{
	DelConnectionPoint(re_uuidof(ISingleAppMsgConnectPoint) , m_SignalAppConnectPoint);
	m_SignalAppConnectPoint.EmptyConnection();
	ReleaseComX3();
	m_globalMutex.CloseHandle();

	SAFE_RELEASE(m_pRot);
	return S_OK;
}

HRESULT CSignalAppliction::GetGlobalMutexName()
{
	RASSERT(m_pRot , E_FAIL);
	UTIL::com_ptr<IMsEnv> pIMsEnv;
	RFAILED(m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IMsEnv) , (void**)&pIMsEnv));
	RASSERT(pIMsEnv , E_FAIL);

	UTIL::com_ptr<IPropertyStr> pPropStr;
	pIMsEnv->GetEnvParam((IMSBase**)&pPropStr.m_p);
	RASSERT(pPropStr, E_FAIL);

	CPropStrSet propSet(pPropStr);
	m_strGlobalMutexName = (LPWSTR)propSet["mutex"];
	RASSERT(m_strGlobalMutexName.GetLength(),E_FAIL);
	
	m_ComX3NameSpace = (LPWSTR)propSet["comx3"];
	RASSERT(m_ComX3NameSpace.GetLength(), E_FAIL);
	return S_OK;
}


/*
<signalapp>
	<command>
	</command>
	<pid>
	</pid>
</signalapp>
*/

CString CSignalAppliction::ReadProcessInfo()
{
	CXmlValueW xmlValue;
	xmlValue[L"command"] = GetCommandLine();
	xmlValue[L"pid"] =  (int)GetCurrentProcessId();
	xmlValue[L"mutex"] = m_strGlobalMutexName;

	return xmlValue.ToXml().c_str();
	
}

HRESULT CSignalAppliction::SendMessage(CString str)
{
	RASSERT(str.GetLength() , E_INVALIDARG);
	
	RASSERT(m_pClientObject , E_FAIL);


	LPVOID lpBuf = (LPVOID)str.GetBuffer();
	int nLen = (str.GetLength() + 1) * sizeof(WCHAR);
	RFAILED(m_pClientObject->SendMessage(SIGNAL_DEFID , SIGNAL_MASKID , SIGNAL_MSGID , lpBuf , nLen,0));
	return S_OK;
}

HRESULT CSignalAppliction::CreateComX3()
{
	RASSERT(m_pRot ,E_FAIL);
	RFAILED(m_pRot->CreateInstance(CLSID_CComX3 , NULL , re_uuidof(IComX3) , (void**)&m_pComX3));
	RASSERT(m_pComX3 ,E_FAIL);

	
	USES_CONVERSION;
	m_pCenter = m_pComX3->Initialize(W2A(m_ComX3NameSpace));
	RASSERT(m_pCenter, E_FAIL);

	m_pClientObject = m_pCenter->Logon(SIGNAL_DEFID , SIGNAL_MASKID , this , CREATE_COMM_OBJECT_MULTI);
	RASSERT(m_pClientObject , E_FAIL);

	return S_OK;
}

HRESULT CSignalAppliction::ReleaseComX3()
{
	if (m_pClientObject)
	{
		m_pCenter->Logoff(m_pClientObject);
	}

	if (m_pCenter)
	{
		m_pComX3->Uninitialize(m_pCenter);
		m_pCenter = NULL;
	}

	SAFE_RELEASE(m_pComX3);
	

	return S_OK;
}

HRESULT CSignalAppliction::Dispatch(IMessage* pMsg)
{
	RASSERT(pMsg , E_FAIL);

	CLIENTINFO targetInfo;
	CLIENTINFO sourceInfo;
	RFAILED(pMsg->GetTarget(&targetInfo));
	RFAILED(pMsg->GetSource(&sourceInfo));

	if (sourceInfo.pid == GetCurrentProcessId())
	{
		return S_OK; 
	}


	MESSAGEINFO* msgInfo = NULL;
	RFAILED(pMsg->GetMessageInfo(&msgInfo));
	RASSERT(msgInfo , E_FAIL);

	
	
	CXmlValueW xmlValue;
	int nOffiset = 0;
	if (xmlValue.FromXml((LPWSTR)msgInfo->pInData,&nOffiset))
	{
		std::wstring sGlobalName = xmlValue[L"mutex"];
		int nCompare = wcsicmp(m_strGlobalMutexName, sGlobalName.c_str());
		if (nCompare == 0)
		{
			CallConnectPoint((LPWSTR)msgInfo->pInData);
		}
	}
	
	return S_OK;
}

HRESULT CSignalAppliction::CallConnectPoint(LPCWSTR pXml)
{
	UTIL::com_ptr<IMsEnumConnections> pEnum;
	if(SUCCEEDED(m_SignalAppConnectPoint.EnumConnections((IMsEnumConnections**)&pEnum)) && pEnum)
	{
		while(1)
		{
			MSCONNECTDATA data[1];
			/*一个一个遍历的，S_FALSE不行	*/
			if(S_OK == pEnum->Next(1, data, NULL))
			{
				UTIL::com_ptr<ISingleAppMsgConnectPoint> p = data[0].pUnk;
				if(p)
				{
					BOOL bHandle = FALSE;
					p->OnSingleAppMsg(pXml , bHandle);
					if (bHandle)
					{
						break;
					}
				}
			}
			else
				break;
		}
	}	

	return S_OK;
}

HRESULT CSignalAppliction::NotifyExit()
{
	RASSERT(m_pRot , E_FAIL);
	UTIL::com_ptr<IExit> pExit;
	m_pRot->GetObject(CLSID_MsEnv , re_uuidof(IExit) , (void**)&pExit);
	RASSERT(pExit ,E_FAIL);
	return pExit->NotifyExit();
}