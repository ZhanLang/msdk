#include "stdafx.h"
#include <process.h>
#include "SyncObject/CriticalSection.h"

#include "commx/commx_error.h"
#include "call_mgr.h"
//#include <mscom/rscom_ex.h>

//进程内通讯中心：实现进程内的通讯

HRESULT CCommInProcess::SendMessage(ISendMessage* pSendMessage)
{
	HRESULT hr = E_FAIL;
	UTIL::com_ptr<IClientObject> pTargetClient = pSendMessage->GetTargetObject();
	if(pTargetClient!=NULL)
	{
		UTIL::com_ptr<IMessageCallBack> pMessageCallBack = pTargetClient->GetCallBack();
		if(pMessageCallBack!=NULL)
		{
			hr = pMessageCallBack->Dispatch(pSendMessage);				
		}
		pTargetClient = INULL;
	}
	return hr;
}


//进程间通讯中心：实现进程间的通讯
CCommInHost::CCommInHost()
{
}

CCommInHost::~CCommInHost()
{
	for (CMapClientConnect::iterator it = m_cc.begin() ; it != m_cc.end() ; it++)
	{
		delete (it->second);
	}

	m_cc.clear();
}

HRESULT CCommInHost::SetCCenterNamesManager(ICCenterNamesManager *pCCenterNamesManager)
{
	m_pCCenterNamesManager = pCCenterNamesManager;
	return S_OK;
}

HRESULT CCommInHost::RPC_SendMessage(int sourcedefid, int sourcemaskid, int sourceobjid, IServer *pServer, ISendMessage* pSendMessage)
{
	HRESULT hr;
	error_status_t RetVal;
	
	SERVERINFO si;
	hr = pServer->GetServer(&si);
	Assert(S_OK==hr);

	LPCTSTR szEndpoint = si.ServerName;
	if(NULL==szEndpoint[0])
		return E_COMMX_ERROR_SERVERNAME;

	Msg("ClientConnect: begin %s\n", szEndpoint);
	m_lockConn.Lock();
	LPCONNECTED_HANDLE lpch = m_cc[si.pobjid];
	if(NULL==lpch)
	{
		lpch = new CONNECTED_HANDLE;
		lpch->m_Binding = NULL;
		lpch->m_stringBinding = NULL;
		CopyMemory(&lpch->m_si, &si, sizeof(lpch->m_si));
		m_cc[si.pobjid] = lpch;
	}		
	if(NULL==lpch->m_Binding)
	{
		lpch->m_Binding = ClientConnect(szEndpoint, lpch->m_stringBinding);			
		if(NULL!=lpch->m_Binding)
		{
			RpcTryExcept 
			{
				RetVal = RPC_Ping(lpch->m_Binding);
				if(RPC_S_OK!=RetVal)
					hr = E_COMMX_CALL_RPC_SERVICE;
			}
			RpcExcept(1) 
			{
				//RSLOG( RSLOG_ALERT , _T("[-COMX3.dll-] <RPC_Ping> maybe some except Leaving......") );
				hr = E_COMMX_CALL_RPC_SERVICE;
			}
			RpcEndExcept
		}
		else
			hr = E_COMMX_CALL_RPC_SERVICE;
	}
	m_lockConn.Unlock();
	Msg("ClientConnect: end %s, hr = %d\n", szEndpoint, hr);
	//进程间通讯中心调RPC到客户通讯对象B所在进程进程间通讯中心
	MESSAGEINFO* pMsginfo = NULL;
	pSendMessage->GetMessageInfo(&pMsginfo);
	int iCount = 2;
	while(NULL!=lpch->m_Binding && iCount>0)
	{
		RpcTryExcept 
		{
			if(pMsginfo->ppOutData)
			{
				RetVal = RPC_ServerCall2(lpch->m_Binding, sourcedefid, sourcemaskid, sourceobjid, pMsginfo->defid, pMsginfo->maskid, pMsginfo->objid,
											pMsginfo->dwCallType|pMsginfo->subsysid, pMsginfo->dwTimeout, pMsginfo->msgid,
											NULL==pMsginfo->pInData?(byte*)"":pMsginfo->pInData, pMsginfo->nInCch, pMsginfo->ppOutData, pMsginfo->pOutCch);
			}
			else
			{
				if(pMsginfo->pInData)
				{
					RetVal = RPC_ServerCall1(lpch->m_Binding, sourcedefid, sourcemaskid, sourceobjid, pMsginfo->defid, pMsginfo->maskid, pMsginfo->objid,
											pMsginfo->dwCallType|pMsginfo->subsysid, pMsginfo->dwTimeout, pMsginfo->msgid,
											pMsginfo->pInData, pMsginfo->nInCch);
				}
				else
					RetVal = RPC_ServerCall(lpch->m_Binding, sourcedefid, sourcemaskid, sourceobjid, pMsginfo->defid, pMsginfo->maskid, pMsginfo->objid,
											pMsginfo->dwCallType|pMsginfo->subsysid, pMsginfo->dwTimeout, pMsginfo->msgid);
			}
		}
		RpcExcept(1) 
		{
			RetVal = RpcExceptionCode();
			//RSLOG( RSLOG_ALERT , _T("[-COMX3.dll-] <RPC_ServerCallX=%d> maybe some except Leaving......"), RetVal);
		}
		RpcEndExcept

		if(RPC_S_OK==RetVal)
		{
			hr = S_OK;
			break;
		}
		else
		{
			hr = 0x80000000|RetVal;

			Msg("2	ClientConnect: begin %s\n", szEndpoint);
			m_lockConn.Lock();
			DisConnect(lpch->m_Binding, lpch->m_stringBinding);
			lpch->m_Binding = NULL;
			lpch->m_stringBinding = NULL;
			iCount--;

			lpch->m_Binding = ClientConnect(szEndpoint, lpch->m_stringBinding);
			m_lockConn.Unlock();
			Msg("2	ClientConnect: end %s, hr = %d\n", szEndpoint, hr);
		}
	}
	return hr;
}

HRESULT CCommInHost::SendMessage(ISendMessage* pSendMessage)
{
	HRESULT hr;

	CLIENTINFO ci;
	pSendMessage->GetTarget(&ci);

	CLIENTINFO sci;
	hr = pSendMessage->GetSource(&sci);
	
	Assert(m_pCCenterNamesManager);
	//查询进程服务通讯对象管理器，得到所有的目标通讯对象所在进程的服务通讯对象名字
	UTIL::com_ptr<IServer> pServer = m_pCCenterNamesManager->FindServer( &ci);
	if(pServer!=NULL)
	{
		hr = RPC_SendMessage(sci.defid, sci.maskid, sci.objid, pServer, pSendMessage);
		pServer->Release();
	}
	return hr;
}

// 判断RPC是否有效。
HRESULT	CCommInHost::CheckRPCState(OBJID objid, BOOL bTryConnected/* = FALSE*/)
{
	HRESULT hr = E_COMMX_ERROR_SERVERNAME;
	LPCONNECTED_HANDLE lpch = m_cc[objid];
	if(NULL==lpch)
	{
		return hr;
	}
	if(bTryConnected)
	{
		if(NULL!=lpch->m_Binding)
			DisConnect(lpch->m_Binding, lpch->m_stringBinding);	
		
        LPCTSTR szEndpoint = lpch->m_si.ServerName;
		lpch->m_Binding = ClientConnect(szEndpoint, lpch->m_stringBinding);
		if(NULL!=lpch->m_Binding)
			hr = S_OK;
	}
	return hr;
}

//通讯中心对象：负责数据的真正的通讯的实现
BOOL CCCenterObject::CheckListenOk(LPCTSTR szEndpoint, DWORD dwTimeout/* = 200*/)
{
	BOOL bRet = TRUE;
	DWORD dwTime = 0;		
	while(dwTime<dwTimeout)
	{
		Msg(TEXT("Check RPC Listen...\n"));
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenterObject::CheckListenOk> Check RPC Listen... ") );
		unsigned char* stringBinding = NULL;
		//建立RPC服务端
		RPC_BINDING_HANDLE Binding = ClientConnect(szEndpoint, stringBinding);
		if(Binding)
		{
			RpcTryExcept 
			{
				if(RPC_S_OK!=RPC_Ping(Binding))
					bRet = FALSE;
			}
			RpcExcept(1) 
			{
				//RSLOG( RSLOG_ALERT , _T("[-COMX3.dll-] <RPC_Ping> maybe some except Leaving......") );
				bRet = FALSE;
			}
			RpcEndExcept
			DisConnect(Binding, stringBinding);
			//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenterObject::CheckListenOk> RPC is %s ") , bRet ? _T("OK") : _T("NOT OK") );
			return bRet;
		}
		else
			bRet = FALSE;
		Sleep(100);
		dwTime+=100;
	}
	//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenterObject::CheckListenOk> RPC is %s ") , bRet ? _T("OK") : _T("NOT OK") );
	return bRet;
}
BOOL CCCenterObject::CheckStartOk(DWORD dwTimeout/* = 200*/)
{
	//一定会返回的
	if (WAIT_OBJECT_0==::WaitForSingleObject(m_hNotify, INFINITE))
	{
		//等一个经验值，200毫秒足够检查RpcMgmtWaitServerListen运行是否成功了
		//Sleep(dwTimeout);
	}
	return m_status == RPC_S_OK;//s_hThread && (m_status == RPC_S_OK);
}

void CCCenterObject::Run()
{
#ifdef _DEBUG
	//Sys_RegisterThread("rising_rpc_listen");
#endif
	m_status = RPC_S_OK;
	m_status = ServiceStart(m_szEndpoint, m_hNotify);
	if(RPC_S_OK!=m_status)
	{
		Msg(TEXT("RPC service start failed. ecode=%d\n"), m_status);
	}
}
CCCenterObject::CCCenterObject()
{		
}

CCCenterObject::~CCCenterObject()
{
}

HRESULT CCCenterObject::Start(LPCTSTR szEndpoint, ICCenterNamesManager *pCCenterNamesManager, DWORD dwType)
{
	HRESULT hr = S_OK;
	m_pCCenterNamesManager = pCCenterNamesManager;				
	DllQuickCreateInstance(CLSID_CCommInProcess, __uuidof(ICommInProcess), m_pCommInProcess,NULL);
	Assert(m_pCommInProcess);			
	DllQuickCreateInstance(CLSID_CCommInHost, __uuidof(ICommInHost), m_pCommInHost,NULL);
	Assert(m_pCommInHost);	
	m_pCommInHost->SetCCenterNamesManager(pCCenterNamesManager);		

	if(0==(dwType&CCENTER_START_SERVER))	//不需要启动服务
	{
		m_lockref.AddRef();
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenterObject::Start> { 0==(dwType&CCENTER_START_SERVER) } 不需要启动服务 ") );
		return hr;
	}

	//只能让一个线程进入
	m_lockStart.Lock();		
	lstrcpyn(m_szEndpoint, szEndpoint, min(SERVER_RPC_NAME_MAX, lstrlen(szEndpoint)+1));
	if(CheckListenOk(szEndpoint))
	{
		//只会增加引用计数了
		m_lockref.AddRef();
	}
	else
	{
		//s_hThread = (HANDLE)_beginthread(CCCenterObject::ThreadFun, 0, this);
		//
		Run();
		if(!CheckStartOk())
		{
			hr = E_COMMX_START_RPC_SERVICE;
		}
		else
		{
			//以后只会增加引用计数了
			m_lockref.AddRef();
		}
	}
	m_lockStart.Unlock();
	return hr;
}
HRESULT CCCenterObject::Stop()
{
	int iref = m_lockref.ReleaseRef();
	if(iref==1)
	{
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <ICCenterObject::Stop>"));
		ServiceStop();
		
		//等200×3毫秒
		//int iCount = 3;
		//while(CheckThreadRuning(200) && iCount-->0);

		//只好强制中止
		//if(iCount==0)
		//{
		//	//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <ICCenterObject::Stop  TerminateThread!!!>"));
		//	Msg(TEXT("Force stop RPC service!\n"));
		//	//::TerminateThread(s_hThread, -1);
		//}
		//s_hThread = NULL;
		//ZeroMemory(m_szEndpoint, sizeof(m_szEndpoint));
	}

	if(m_pCommInProcess)
	{
		m_pCommInProcess = INULL;
	}
	if(m_pCommInHost)
	{
		m_pCommInHost->SetCCenterNamesManager(NULL);
		m_pCommInHost = INULL;
	}
	return S_OK;
}

HRESULT CCCenterObject::SendMessage(ISendMessage* pSendMessage)
{
	HRESULT hr;
	DWORD dwInProcess;
	hr = pSendMessage->GetCallType(&dwInProcess);
	if(CALL_TYPE_INPROCESS & dwInProcess)
	{
		if(m_pCommInProcess)
			hr = m_pCommInProcess->SendMessage(pSendMessage);
	}
	else if (CALL_TYPE_INHOST & dwInProcess)
	{
		if(m_pCommInHost)
			hr = m_pCommInHost->SendMessage(pSendMessage);
	}
	return hr;
}

//HANDLE CCCenterObject::s_hThread = NULL;
RPC_STATUS CCCenterObject::m_status = 0;	
CRSHandle CCCenterObject::m_hNotify = ::CreateEvent(NULL, FALSE, FALSE, NULL);
TCHAR CCCenterObject::m_szEndpoint[SERVER_RPC_NAME_MAX] = {0};
CAutoCriticalSection CCCenterObject::m_lockStart;
CSingleAutoCriticalSection CCCenterObject::m_lockref;

//EXPOSE_SINGLE_INTERFACE(CCCenterObject, ICCenterObject, CCENTEROBJECT_INTERFACE_VERSION);

