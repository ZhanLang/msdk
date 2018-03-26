// rscenter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <time.h>

#include "commx/commx_error.h"
#include "call_mgr.h"
#include "rscenter.h"
//#include <mscom/rscom_ex.h>
//#include "RpcThreadItem.h"
#include <util/utility_ex.h>

typedef UTIL::sentry<HMODULE, UTILEX::hmodule_sentry> CRSHModule;

static int GetCurrentSessionId()
{
	CRSHModule  hKernel32 = LoadLibrary("kernel32.dll");
	if(hKernel32 == NULL)
		return 0;

	typedef BOOL (WINAPI *FNPTS )(DWORD , DWORD*);

	DWORD dwPid	;
	DWORD dwSid ;
	FNPTS fnPts = NULL;
	fnPts = (FNPTS)GetProcAddress(hKernel32 , "ProcessIdToSessionId");
	if(fnPts)
	{
		dwPid = GetCurrentProcessId();
		dwSid = 0;
		fnPts(dwPid , &dwSid);
	}
	else
	{
		dwSid = 0;
	}	
	return dwSid;
}

static int GetCurrentHostId()
{
	return 0;
}

//消息实体对象
template<class _T>
class IMessageImpl : public _T
{
public:
	CLIENTINFO m_souci;//源对象信息
	CLIENTINFO m_tagci;//目标对象信息

	MESSAGEINFO m_msginfo;

	UTIL::com_ptr<IClientObject> m_pTargetClient;

	IMessageImpl()
	{
		ZeroMemory(&m_souci, sizeof(CLIENTINFO));
		m_souci.dwSize = sizeof(CLIENTINFO);

		ZeroMemory(&m_tagci, sizeof(CLIENTINFO));
		m_tagci.dwSize = sizeof(CLIENTINFO);

		ZeroMemory( &m_msginfo ,sizeof(MESSAGEINFO) );
		m_msginfo.dwSize = sizeof(MESSAGEINFO);
	}
public:	
	virtual HRESULT GetCallType(DWORD* pdwCallType)
	{
		*pdwCallType = m_msginfo.dwCallType;
		return S_OK;
	}
	virtual HRESULT PutCallType(DWORD dwCallType)
	{
		m_msginfo.dwCallType = dwCallType;
		return S_OK;
	}

	virtual HRESULT GetSource(CLIENTINFO* pci)
	{
		CopyMemory(pci, &m_souci, sizeof(CLIENTINFO));
		return S_OK;
	}
	virtual HRESULT PutSource(CLIENTINFO* pci)
	{
		CopyMemory(&m_souci, pci, sizeof(CLIENTINFO));
		m_msginfo.source_defid = pci->defid;
		m_msginfo.source_maskid = pci->maskid;
		m_msginfo.source_objid = pci->objid;
		return S_OK;
	}
	virtual HRESULT GetTarget(CLIENTINFO* pci)
	{
		CopyMemory(pci, &m_tagci, sizeof(CLIENTINFO));
		return S_OK;
	}
	virtual HRESULT PutTarget(CLIENTINFO* pci)
	{
		CopyMemory(&m_tagci, pci, sizeof(CLIENTINFO));
		m_msginfo.defid = pci->defid;
		m_msginfo.maskid = pci->maskid;
		m_msginfo.objid = pci->objid;
		return S_OK;
	}

	virtual HRESULT GetTargetObjid(OBJID &objid)
	{
		objid = m_tagci.objid;
		return S_OK;
	}

	virtual HRESULT PutTargetObjid(OBJID objid)
	{
		m_tagci.objid = objid;
		m_msginfo.objid = objid;
		return S_OK;
	}

	virtual IClientObject* GetTargetObject()
	{
		return m_pTargetClient;
	}
	virtual HRESULT PutTargetObject(IClientObject* pTargetClient)
	{
		m_pTargetClient = pTargetClient;
		return S_OK;
	}

	virtual HRESULT GetTargetMask(DWORD* pdwMask)
	{
		*pdwMask = m_tagci.dwMask;
		return S_OK;
	}
	virtual HRESULT PutTargetMask(DWORD dwMask)
	{
		m_tagci.dwMask = dwMask;
		return S_OK;
	}

	virtual HRESULT GetMessageInfo(MESSAGEINFO **ppMsgInfo)
	{
		*ppMsgInfo = &m_msginfo;
		return S_OK;
	}

	virtual HRESULT PutOutputInfo(byte* pOutData, int iOutCch)
	{
		if(m_msginfo.ppOutData)
			*m_msginfo.ppOutData = pOutData;
		if(m_msginfo.pOutCch)
			*m_msginfo.pOutCch = iOutCch;
		return S_OK;
	}
};


class CSendMessage : public IMessageImpl<ISendMessage>
				   , public CUnknownImp
{
public:
	UNKNOWN_IMP2(ISendMessage, IMessage);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	virtual HRESULT Init(IClientObject *pClientObject, MESSAGEINFO* pMsg)
	{
		pClientObject->GetClientInfo(&m_souci);

		m_souci.defid = pMsg->source_defid;
		m_souci.maskid = pMsg->source_maskid;
		m_souci.objid = pMsg->source_objid;
		m_souci.subsysid = pMsg->subsysid;

		m_tagci.defid = pMsg->defid;
		m_tagci.maskid = pMsg->maskid;
		m_tagci.objid = pMsg->objid;
		m_tagci.subsysid = pMsg->subsysid;

		CopyMemory(&m_msginfo, pMsg, sizeof(m_msginfo));

		if(CALL_TYPE_SYNC & pMsg->dwCallType)
		{
		}
		else
		{			
			m_msginfo.pInData = new byte[pMsg->nInCch+1];
			CopyMemory(m_msginfo.pInData, pMsg->pInData, pMsg->nInCch);
			m_msginfo.pInData[pMsg->nInCch] = 0;
			m_msginfo.nInCch = pMsg->nInCch;
			m_msginfo.ppOutData = NULL;
			m_msginfo.pOutCch = 0;
		}

		return S_OK;
	}

	virtual HRESULT Uninit()
	{
		if(CALL_TYPE_SYNC & m_msginfo.dwCallType)
		{
		}
		else
		{	
			delete []m_msginfo.pInData;
		}
		ZeroMemory(&m_msginfo, sizeof(m_msginfo));
		return S_OK;
	}
};

class CReceMessage : public IMessageImpl<IReceMessage>
				   , public CUnknownImp
{
public:
	UNKNOWN_IMP2(IReceMessage, IMessage);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	virtual HRESULT Init(MESSAGEINFO* pMsg)
	{
		m_souci.defid = pMsg->source_defid;
		m_souci.maskid = pMsg->source_maskid;
		m_souci.objid = pMsg->source_objid;
		m_souci.subsysid = pMsg->subsysid;

		m_tagci.defid = pMsg->defid;
		m_tagci.maskid = pMsg->maskid;
		m_tagci.objid = pMsg->objid;
		m_tagci.subsysid = pMsg->subsysid;

		CopyMemory(&m_msginfo, pMsg, sizeof(m_msginfo));
		if(CALL_TYPE_SYNC & pMsg->dwCallType)
		{
		}
		else
		{			
			m_msginfo.pInData = new byte[pMsg->nInCch+1];
			CopyMemory(m_msginfo.pInData, pMsg->pInData, pMsg->nInCch);
			m_msginfo.pInData[pMsg->nInCch] = 0;
			m_msginfo.nInCch = pMsg->nInCch;
			m_msginfo.ppOutData = NULL;
			m_msginfo.pOutCch = 0;
		}

		return S_OK;
	}

	virtual HRESULT Uninit()
	{
		if(CALL_TYPE_SYNC & m_msginfo.dwCallType)
		{
		}
		else
		{	
			delete []m_msginfo.pInData;
		}
		ZeroMemory(&m_msginfo, sizeof(m_msginfo));
		return S_OK;
	}
};

//接收数据队列的处理服务对象：处理收到的数据，并进入队列
class CMessageQuene : public IMessageQuene
{
public:
	virtual HRESULT Append(IReceMessage* pReceMessage);	
};

//线程（池）管理对象：对队列数据通过线程回调给通讯客户端
class CThreadPools : public IThreadPools
{
public:
	virtual HRESULT Run();
	virtual HRESULT OnEvent(IReceMessage *pReceMessage);
};


///////////////////////////////////////////////////////////////////////////////////////////
// static CHAR g_szServerName[SERVER_RPC_NAME_MAX] = {0};
// static LPCSTR SetCurrentServerName(OBJID pobjid)
// {
// 	//服务器名字只和接口版本、进程id相关
// 	wsprintf(g_szServerName, "%d_%d_%d", RPC_CALL_INTERFACE_VER, GetCurrentProcessId(), pobjid);
// 	//wsprintf(g_szServerName, "%d_%d", RPC_CALL_INTERFACE_VER, GetCurrentProcessId());
// 	return g_szServerName;
// }
inline int msdk::GetCurrentSubsysId(LPCSTR szName)
{
	//强制把asc字符串前2字节+后2字节转化为一个WORD，然后放入高16位中
	return ((*(short int*)(szName) + *(short int*)(szName+2))&0x0000ffff)<<16;
}


//服务器信息对象：保存服务器信息
class CServer : public IServer
			  , public CUnknownImp
{
	SERVERINFO m_si;//记录当前服务器信息对象的相关信息
public:
	UNKNOWN_IMP1(IServer);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	virtual HRESULT PutServer(SERVERINFO* psi)
	{
		CopyMemory(&m_si, psi, sizeof(SERVERINFO));
		return S_OK;
	}

	virtual HRESULT GetServer(SERVERINFO* psi)
	{
		CopyMemory(psi, &m_si, sizeof(SERVERINFO));
		return S_OK;
	}
};

//进程服务通讯对象管理：负责管理本机所有进程的服务通讯对象的名字
class CCCenterNamesManager : public ICCenterNamesManager
						   , public CUnknownImp
{
	UTIL::com_ptr<IServer> m_pServer;
	UTIL::com_ptr<ICCenterNames> m_pCCenterNames;

public:	
	UNKNOWN_IMP1(ICCenterNamesManager);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	CCCenterNamesManager()
	{		
		DllQuickCreateInstance(CLSID_CCenterNames, __uuidof(ICCenterNames), m_pCCenterNames,NULL);		
		Assert(m_pCCenterNames);
		Msg("CCCenterNamesManager::CCCenterNamesManager m_pCCenterNames = %x \n", (ICCenterNames*)m_pCCenterNames);
	}

	~CCCenterNamesManager()
	{
		Msg("CCCenterNamesManager::~CCCenterNamesManager m_pCCenterNames = %x begin\n", (ICCenterNames*)m_pCCenterNames);
		m_pCCenterNames = INULL;
		Msg("CCCenterNamesManager::~CCCenterNamesManager m_pServer = %x \n", (IServer*)m_pServer);
		m_pServer = INULL;
		Msg("CCCenterNamesManager::~CCCenterNamesManager end\n");
	}

	virtual HRESULT GetServer(SERVERINFO* pServer)
	{
		return m_pServer->GetServer(pServer);
	}
	
	virtual HRESULT GetServerObjectID(OBJID &pobjid)
	{
		SERVERINFO si = {0};
		m_pServer->GetServer(&si);
		pobjid = si.pobjid;
		return S_OK;
	}

	virtual HRESULT GetServerType(DWORD &dwType)
	{
		SERVERINFO si = {0};
		m_pServer->GetServer(&si);
		dwType = si.dwType;
		return S_OK;
	}

	virtual IServer* FindServer(CLIENTINFO* pci)
	{		
		//确认目标defid是否存在，并获得其进程
		HRESULT hr;
		SERVERINFO si = {
			sizeof(SERVERINFO),
			CI_MASK_PID|CI_MASK_POBJID,//通过pid和objid查找
			0,
			pci->pid,
			0,
			0,
			pci->pobjid,
			"",
		};
		hr = m_pCCenterNames->Find(&si);
		if(S_OK==hr)
		{			
			UTIL::com_ptr<IServer> pServer;
			DllQuickCreateInstance(CLSID_CServer, __uuidof(IServer), pServer,NULL);
			hr = pServer->PutServer(&si);
			if(S_OK==hr)
			{
				pServer->AddRef();
				return pServer;
			}
			else
			{
				pServer = INULL;
			}			
		}
		return NULL;;
	}

	virtual HRESULT Register(LPCSTR szName, LPCSTR szWorkPath, DWORD dwType)
	{
		HRESULT hr;
		SERVERINFO si = {
			sizeof(SERVERINFO),
			CI_MASK_PID,
			dwType,
			GetCurrentProcessId(),
			msdk::GetCurrentSubsysId(szName),
			RPC_CALL_INTERFACE_VER,
			0,
			"",
		};
		
		//子系统服务名字表只和接口版本、子系统名字相关
		char ServerTableName[MAX_PATH+32];
		wsprintf(ServerTableName, "%s%s_s%d.dat", szWorkPath, szName, RPC_CALL_INTERFACE_VER);		
		Assert(m_pCCenterNames);
		hr = m_pCCenterNames->Open(ServerTableName);
		if(S_OK==hr)
		{
			//加入此进程到服务名字表中来
			hr = m_pCCenterNames->Add(&si, TRUE);
			if(S_OK==hr)
			{				
				DllQuickCreateInstance(CLSID_CServer, __uuidof(IServer), m_pServer,NULL);				
				Assert(m_pServer);
				hr = m_pServer->PutServer(&si);	
				//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] < CCCenterNamesManager::Register Process > [ subsys = %s , pid = %ld ] successful ") , szName ,  GetCurrentProcessId() );
			}
			else
			{
				m_pCCenterNames->Close();
			}
		}
		return hr;
	}

	virtual HRESULT Unregister()
	{
		Assert(m_pCCenterNames);
		Assert(m_pServer);
		
		HRESULT hr = S_OK;
		SERVERINFO si;
		if(m_pServer)
		{
			hr = m_pServer->GetServer(&si);
			if(S_OK==hr)
			{			
				m_pCCenterNames->Delete(&si);
			}
			m_pServer = INULL;
		}		
		hr = m_pCCenterNames->Close();		
		return hr;
	}

	virtual HRESULT GetServerName(LPTSTR szServerName, DWORD dwSize)
	{
		HRESULT hr;
		SERVERINFO si;
		hr = m_pServer->GetServer(&si);
		if(S_OK==hr)
		{
			lstrcpyn(szServerName, si.ServerName, min(dwSize, SERVER_RPC_NAME_MAX));
		}
		return hr;
	}	
};

//客户通讯对象：抽象的通讯逻辑对象，直接提供给用户用于进程间的通讯使用
class CClientObject : public IClientObject	//CSingleInstance<IClientObject>
					, public CUnknownImp
{
	DWORD m_dwTimeout;
	CLIENTINFO	m_clientinfo;
	int m_iMode;

	UTIL::com_ptr<IMessageCallBack> m_pCallBack;
	UTIL::com_ptr<IClientObjectManager> m_pClientObjectManager;
	UTIL::com_ptr<ICCenterObject> m_pCCenterObject;
	
public:
	UNKNOWN_IMP1(IClientObject);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	CClientObject():m_dwTimeout(1000)
	{
	}
	virtual ~CClientObject()
	{
	}
	virtual HRESULT InitClientInfo(CLIENTINFO* pci, IClientObjectManager* pClientObjectManager)
	{		
		CopyMemory(&m_clientinfo, pci, sizeof(CLIENTINFO));
		m_pClientObjectManager = pClientObjectManager;
		Assert(m_pClientObjectManager);
		m_pCCenterObject = m_pClientObjectManager->GetCCenter();
		Assert(m_pCCenterObject);
		return S_OK;
	}
	virtual HRESULT GetClientInfo(CLIENTINFO* pci)
	{
		CopyMemory(pci, &m_clientinfo, sizeof(CLIENTINFO));		
		return S_OK;
	}
	virtual HRESULT SetCallbackMode(int imode, IMessageCallBack *pMessageCallBack)
	{
		m_iMode = imode;
		m_pCallBack = pMessageCallBack;
		Assert( m_pCallBack );
		//	m_pCallBack->AddRef(); //del by ysb 莫名的加了一个引用计数
		return S_OK;
	}
	virtual IMessageCallBack* GetCallBack()
	{
		return m_pCallBack;
	}
	virtual HRESULT SendMessage(ISendMessage* pSendMessage)
	{		
		return m_pClientObjectManager->FormatNeedSend(pSendMessage, CI_MASK_NORMALID);
	}
	virtual HRESULT BroadcastMessage(ISendMessage* pSendMessage)
	{		
		return m_pClientObjectManager->FormatNeedSend(pSendMessage, CI_MASK_GROUPID);
	}
	virtual HRESULT SendMessage(DEFID defid, MASKID maskid, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout)
	{
		UTIL::com_ptr<ISendMessage> pSendMessage;
		DllQuickCreateInstance(CLSID_CSendMessage, __uuidof(ISendMessage), pSendMessage,NULL);
		Assert(pSendMessage);
		MESSAGEINFO callmsg = 
		{
			sizeof(MESSAGEINFO),
			CALL_TYPE_SYNC,
			dwTimeout,
			m_clientinfo.defid,
			m_clientinfo.maskid,
			m_clientinfo.objid,
			defid,
			maskid,
			0,
			m_clientinfo.subsysid,
			msgid,
			(byte*)pInData,
			nInCch,
			NULL,
			0,
		};
		pSendMessage->Init(this, &callmsg);
		return SendMessage(pSendMessage);
	}
	virtual HRESULT SendMessageEx(DEFID defid, MASKID maskid, MSGID msgid, void* pInData, int nInCch, void** ppOutData, int* pOutCch, DWORD dwTimeout)
	{
		UTIL::com_ptr<ISendMessage> pSendMessage;
		DllQuickCreateInstance(CLSID_CSendMessage, __uuidof(ISendMessage), pSendMessage,NULL);
		MESSAGEINFO callmsg = 
		{
			sizeof(MESSAGEINFO),
			CALL_TYPE_SYNC,
			dwTimeout,
			m_clientinfo.defid,
			m_clientinfo.maskid,
			m_clientinfo.objid,
			defid,
			maskid,
			0,
			m_clientinfo.subsysid,
			msgid,
			(byte*)pInData,
			nInCch,
			(byte**)ppOutData,
			pOutCch,
		};
		pSendMessage->Init(this, &callmsg);
		return SendMessage(pSendMessage);
	}
	virtual HRESULT BroadcastMessage(MASKID maskid, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout)
	{		
		UTIL::com_ptr<ISendMessage> pSendMessage;
		DllQuickCreateInstance(CLSID_CSendMessage, __uuidof(ISendMessage), pSendMessage,NULL);
		MESSAGEINFO callmsg = 
		{
			sizeof(MESSAGEINFO),
			CALL_TYPE_SYNC,
			dwTimeout,
			m_clientinfo.defid,
			m_clientinfo.maskid,
			m_clientinfo.objid,
			0,
			maskid,
			0,
			m_clientinfo.subsysid,
			msgid,
			(byte*)pInData,
			nInCch,
			NULL,
			0,
		};
		pSendMessage->Init(this, &callmsg);
		return BroadcastMessage(pSendMessage);
	}

	virtual HRESULT BroadcastMessageEx(CLIENTINFO* pTargetInfo, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout)
	{
		UTIL::com_ptr<ISendMessage> pSendMessage;
		DllQuickCreateInstance(CLSID_CSendMessage, __uuidof(ISendMessage), pSendMessage,NULL);
		MESSAGEINFO callmsg = 
		{
			sizeof(MESSAGEINFO),
				CALL_TYPE_SYNC,
				dwTimeout,
				m_clientinfo.defid,
				m_clientinfo.maskid,
				m_clientinfo.objid,
				0,
				0,
				0,
				m_clientinfo.subsysid,
				msgid,
				(byte*)pInData,
				nInCch,
				NULL,
				0,
		};
		pSendMessage->Init(this, &callmsg);
		CLIENTINFO ci;
		pSendMessage->GetTarget(&ci);
		ci.dwMask = pTargetInfo->dwMask;
		ci.defid = pTargetInfo->defid;
		ci.maskid = pTargetInfo->maskid;
		ci.objid = pTargetInfo->objid;
		ci.tid = pTargetInfo->tid;
		ci.pid = pTargetInfo->pid;
		ci.sid = pTargetInfo->sid;
		ci.hid = pTargetInfo->hid;		
		pSendMessage->PutTarget(&ci);
		return m_pClientObjectManager->FormatNeedSend(pSendMessage, pTargetInfo->dwMask);
	}

	virtual HRESULT SendMessageTo(OBJID objid, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout)
	{
		UTIL::com_ptr<ISendMessage> pSendMessage;
		DllQuickCreateInstance(CLSID_CSendMessage, __uuidof(ISendMessage), pSendMessage,NULL);
		Assert(pSendMessage);
		MESSAGEINFO callmsg = 
		{
			sizeof(MESSAGEINFO),
				CALL_TYPE_SYNC,
				dwTimeout,
				m_clientinfo.defid,
				m_clientinfo.maskid,
				m_clientinfo.objid,
				0,
				0,
				objid,
				m_clientinfo.subsysid,
				msgid,
				(byte*)pInData,
				nInCch,
				NULL,
				0,
		};
		pSendMessage->Init(this, &callmsg);
		return m_pClientObjectManager->FormatNeedSend(pSendMessage, CI_MASK_EXTENDID);
	}
	virtual HRESULT SendMessageToEx(OBJID objid, MSGID msgid, void* pInData, int nInCch, void** ppOutData, int* pOutCch, DWORD dwTimeout)
	{
		UTIL::com_ptr<ISendMessage> pSendMessage;
		DllQuickCreateInstance(CLSID_CSendMessage, __uuidof(ISendMessage), pSendMessage,NULL);
		MESSAGEINFO callmsg = 
		{
			sizeof(MESSAGEINFO),
				CALL_TYPE_SYNC,
				dwTimeout,
				m_clientinfo.defid,
				m_clientinfo.maskid,
				m_clientinfo.objid,
				0,
				0,
				objid,
				m_clientinfo.subsysid,
				msgid,
				(byte*)pInData,
				nInCch,
				(byte**)ppOutData,
				pOutCch,
		};
		pSendMessage->Init(this, &callmsg);
		return m_pClientObjectManager->FormatNeedSend(pSendMessage, CI_MASK_EXTENDID);
	}
};


//客户通讯对象管理器：负责管理客户通讯对象的名字的管理器对象。
class CClientObjectManager : public IClientObjectManager
						   , public CUnknownImp					
{
	UTIL::com_ptr<IClientObjectNames> m_pClientObjectNames;
	UTIL::com_ptr<ICCenterObject> m_pCCenterObject;
	
	int m_iSubsysID;
protected:
	HRESULT SendOne(int sourcepid, int targetpid, int targetobjid, ISendMessage* pSendMessage)
	{
		DWORD dwCallType, dwOrgType;
		pSendMessage->GetCallType(&dwOrgType);
		dwCallType = dwOrgType;
		//确认是否是在同一个进程中
		if(sourcepid==targetpid)
		{
			dwCallType |= CALL_TYPE_INPROCESS;			
			
			pSendMessage->PutTargetObject(m_objlist[targetobjid]);
		}
		else
		{
			dwCallType |= CALL_TYPE_INHOST;				
		}
		pSendMessage->PutCallType(dwCallType);
		pSendMessage->PutTargetObjid(targetobjid);

		//通过通讯中心对象来发送数据
		HRESULT hr = m_pCCenterObject->SendMessage(pSendMessage);
		//因为这个消息还要重用一下发给其他对象，所以应该把它清空
		pSendMessage->PutCallType(dwOrgType);
		return hr;
	}
public:
	UNKNOWN_IMP1(IClientObjectManager);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	CClientObjectManager():m_iSubsysID(0)
	{		
		DllQuickCreateInstance(CLSID_CClientObjectNames, __uuidof(IClientObjectNames), m_pClientObjectNames,NULL);		
		Assert(m_pClientObjectNames);
	}
	virtual ~CClientObjectManager()
	{
		m_pClientObjectNames = INULL;
	}
	virtual HRESULT Open(LPCSTR szName, LPCSTR szWorkPath)
	{
		Assert(m_pClientObjectNames);
		m_iSubsysID = msdk::GetCurrentSubsysId(szName);

		//客户通讯对象名字表只和接口版本、子系统名字相关
		char ClientTableName[MAX_PATH+32];
		wsprintf(ClientTableName, "%s%s_c%d.dat", szWorkPath, szName, RPC_CALL_INTERFACE_VER);
		return m_pClientObjectNames->Open(ClientTableName);
	}
	virtual HRESULT Close()
	{
		Assert(m_pClientObjectNames);
		return m_pClientObjectNames->Close();
	}
	virtual IClientObjectNames* GetClientObjectNames()
	{
		return m_pClientObjectNames;
	}
	virtual ICCenterObject* GetCCenter()
	{
		return m_pCCenterObject;
	}
	virtual HRESULT PutCCenter(ICCenterObject* pCCenterObject)
	{
		m_pCCenterObject = pCCenterObject;
		return S_OK;
	}
	virtual IClientObject* Register(DEFID defid, MASKID maskid, DWORD dwStyle, OBJID pobjid, DWORD dwType, MSGID* enablemsgs, MSGID* disablemsgs)
	{
		HRESULT hr;
		int i;
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <IClientObject* Register> [ defid = %d , maskid = %d , dwStyle = %ld , pobjid = %d , dwType = %d ] ") , defid , maskid , dwStyle , pobjid , dwType );

		CLIENTINFO ci =	{
			sizeof(CLIENTINFO),
			CREATE_COMM_OBJECT_SINGLE==dwStyle?CI_MASK_NORMALID:CI_MASK_EXTENDID,
			dwType,
			defid,
			maskid,
			m_iSubsysID,
			0,
			pobjid,
			GetCurrentThreadId(),
			GetCurrentProcessId(),
			GetCurrentSessionId(),
			GetCurrentHostId(),
			NULL,
			NULL,
			NULL,
		};
		i=0;
		while(enablemsgs && enablemsgs[i] && i<CLIENT_INFO_MSGS_MAX)
			ci.enablemsgs[i] = enablemsgs[i];
		i=0;
		while(disablemsgs && disablemsgs[i] && i<CLIENT_INFO_MSGS_MAX)
			ci.disablemsgs[i] = disablemsgs[i];

		UTIL::com_ptr<IClientObject> pClientObject;

		Assert(m_pClientObjectNames);
		hr = m_pClientObjectNames->Add(&ci, FALSE);
		if(S_OK==hr)
		{			
			DllQuickCreateInstance(CLSID_CClientObject, __uuidof(IClientObject), pClientObject,NULL);			
			Assert(pClientObject);
			hr = pClientObject->InitClientInfo(&ci, this);
			if(S_OK==hr)
			{
				pClientObject->AddRef();
				m_objlist[ci.objid] = pClientObject;				
			}
			else
			{
				pClientObject = INULL;
			}
		}
		return pClientObject;
	}

	virtual HRESULT UnRegister(IClientObject *pClientObject)
	{
		HRESULT hr;
		Assert(pClientObject);

		CLIENTINFO ci;
		hr = pClientObject->GetClientInfo(&ci);
		if(S_OK==hr)
		{
			m_objlist.erase(ci.objid);
			pClientObject->Release();
		}

		Assert(m_pClientObjectNames);
		m_pClientObjectNames->Delete(&ci);
		return hr;
	}
	
	//查找指定对象发送消息
	virtual HRESULT FormatNeedSend(ISendMessage* pSendMsg, DWORD dwMatch)
	{	
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CClientObjectManager::FormatNeedSend>  Entering ......") );
		Assert(pSendMsg);
		UTIL::com_ptr<ISendMessage> pSendMessage(pSendMsg);


		byte *pLastBuf = NULL;
		HRESULT hr=S_FALSE;
		//只要发送成功一次，就认为发送成功
		BOOL    bSomeOneOk = FALSE;
		CLIENTINFO sci, tci;
		pSendMessage->GetSource(&sci);
		pSendMessage->GetTarget(&tci);

		//必须检查目标是否可以接收消息
		tci.dwMask = dwMatch | CI_MASK_TYPEID;
		//确认目标defid是否存在，是否关心此消息数据			
		Assert(m_pClientObjectNames);
		if(CI_MASK_NORMALID==dwMatch||CI_MASK_EXTENDID==dwMatch)
		{
			//Send到多个进程的多个对象（因为现在允许defineid 和 maskid重复）
			HANDLE32 hFind;
			for( hr=m_pClientObjectNames->QueryFirstObject(hFind);S_OK==hr;hr=m_pClientObjectNames->QueryNextObject(hFind))
			{
				CLIENTINFO ci;
				if(S_OK==m_pClientObjectNames->QueryObject(hFind, &ci))
				{
					Msg("Pre Send to\tobjid=%d, [%d, %X], pojb=%d, susys=%08X\n", ci.objid, ci.defid, ci.maskid, ci.pobjid, ci.subsysid);
					if(tci==ci)//按照tci要求进程匹配
					{						
						pSendMessage->PutTarget(&ci);						
						pSendMessage->PutOutputInfo(NULL, 0);
						if(S_OK!=SendOne(sci.pid, ci.pid, ci.objid, pSendMessage))
						{
							//Warning("Result: send to (%d,[%d, %X], %d %08X) failed!\n", ci.objid, ci.defid, ci.maskid, ci.pobjid,ci.subsysid);
							m_pClientObjectNames->Delete(&ci);
						}
						else
						{
							MESSAGEINFO *pMsgInfo = NULL;
							pSendMessage->GetMessageInfo(&pMsgInfo);
							if ( pMsgInfo->ppOutData != NULL )
							{
								if ( *(pMsgInfo->ppOutData) != NULL )
								{
									if ( pLastBuf != NULL )
									{
										MS_FreeCallCenter( pLastBuf );
										pLastBuf = NULL;
									}
									pLastBuf =*(pMsgInfo->ppOutData);
								}
							}
							//只要发送成功一次，就认为发送成功
							bSomeOneOk = TRUE;
							//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CClientObjectManager::FormatNeedSend> sendmsg successful  ......") );
							Msg("------Send ok!\n");
						}
					}
				}
			} 
		}
		else
		{
			//Send到多个进程的多个对象
			HANDLE32 hFind;
			for(hr=m_pClientObjectNames->QueryFirstObject(hFind);S_OK==hr;hr=m_pClientObjectNames->QueryNextObject(hFind))
			{
				CLIENTINFO ci;
				if(S_OK==m_pClientObjectNames->QueryObject(hFind, &ci))
				{
					Msg("Pre Send to\tobjid=%d, [%d, %X], pojb=%d, susys=%08X\n", ci.objid, ci.defid, ci.maskid, ci.pobjid, ci.subsysid);
					if(tci==ci)//按照tci要求进程匹配
					{
						pSendMessage->PutTarget(&ci);
						pSendMessage->PutOutputInfo(NULL, 0);
						if(S_OK!=SendOne(sci.pid, ci.pid, ci.objid, pSendMessage))
						{
							//Warning("Result: send to (%d,[%d, %X], %d %08X) failed!\n", ci.objid, ci.defid, ci.maskid, ci.pobjid,ci.subsysid);
							m_pClientObjectNames->Delete(&ci);
						}
						else
						{
							MESSAGEINFO *pMsgInfo = NULL;
							pSendMessage->GetMessageInfo(&pMsgInfo);
							if ( pMsgInfo->ppOutData != NULL )
							{
								if ( *(pMsgInfo->ppOutData) != NULL )
								{
									if ( pLastBuf != NULL )
									{
										MS_FreeCallCenter( pLastBuf );
										pLastBuf = NULL;
									}
									pLastBuf =*(pMsgInfo->ppOutData);
								}
							}
							//只要发送成功一次，就认为发送成功
							bSomeOneOk = TRUE;
							//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CClientObjectManager::FormatNeedSend> {Send到多个进程的多个对象} sendmsg successful  ......") );
							Msg("------Send ok!\n");
						}
					}
				}
			} 
		}
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CClientObjectManager::FormatNeedSend> { bSomeOneOk = %d } sendmsg successful  ......") , bSomeOneOk );
		return ( hr = ( bSomeOneOk ? S_OK : hr ) );
	}

	//接受到消息查找指定对象
	virtual HRESULT FormatNeedRecv(IReceMessage *pReceMsg)
	{
		Assert(pReceMsg);
		UTIL::com_ptr<IReceMessage> pReceMessage(pReceMsg);

		HRESULT hr;
		//////////////////////////////////////////////////////////////////////////
		CLIENTINFO sci;
		hr = pReceMessage->GetSource(&sci);
		sci.dwMask = CI_MASK_EXTENDID;
		Assert(m_pClientObjectNames);
		hr = m_pClientObjectNames->Find(&sci);
		if(S_OK==hr)
		{
			//得到详细的目标defid信息
			pReceMessage->PutSource(&sci);
		}
		//////////////////////////////////////////////////////////////////////////
		CLIENTINFO tci;
		hr = pReceMessage->GetTarget(&tci);

		//确认目标defid是否存在
		tci.dwMask = CI_MASK_EXTENDID;
		Assert(m_pClientObjectNames);
		hr = m_pClientObjectNames->Find(&tci);
		if(S_OK==hr)
		{
			//得到详细的目标defid信息
			pReceMessage->PutTarget(&tci);
			UTIL::com_ptr<IClientObject> pClient = m_objlist[tci.objid];
			if(pClient==NULL)
			{
				//Log("m_objlist no found objid=%d\n", tci.objid);
				return E_COMMX_NO_FOUND_OBJECT;
			}
			hr = pReceMessage->PutTargetObject(pClient);			

			//转化进程外调用为进程内部的直接调用
			DWORD dwCallType;
			pReceMessage->GetCallType(&dwCallType);		
			dwCallType &= (~CALL_TYPE_INHOST);
			dwCallType |= CALL_TYPE_INPROCESS;
			pReceMessage->PutCallType(dwCallType);
		}

		return hr;
	}
public:
	map<OBJID, UTIL::com_ptr<IClientObject> > m_objlist;//对象列表
};

//通讯中心主模块运行对象：主模块运行的实现
class CCCenter : public ICCenter	//CSingleInstance<ICCenter>
			   , public CUnknownImp					
{
	UTIL::com_ptr<ICCenterObject> m_pCCenterObject;//RPC通讯对象
	UTIL::com_ptr<ICCenterNamesManager> m_pCCenterNamesManager;//负责管理本机所有进程的服务通讯对象的名称
	UTIL::com_ptr<IClientObjectManager> m_pClientObjectManager;//管理客户通讯对象的名字
	UTIL::com_ptr<IClientObjectNames> m_pClientObjectNames;//名字的增删操作

	int m_iSubsysID;
	CAutoCriticalSection m_lockref;

	TCHAR m_szServerName[SERVER_RPC_NAME_MAX];
	TCHAR m_szSubsysName[SUB_SYSTEM_NAME_MAX];	
	TCHAR m_szWorkPath[MAX_PATH];
	DWORD m_dwType;
protected:	

public:
	UNKNOWN_IMP1(ICCenter);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	CCCenter()
	{
		m_dwType = 0;
		ZeroMemory(m_szWorkPath, sizeof(m_szWorkPath));
		ZeroMemory(m_szServerName, sizeof(m_szServerName));
		ZeroMemory(m_szSubsysName, sizeof(m_szSubsysName));

		m_iSubsysID = 0;
	}
	virtual ~CCCenter()
	{
	}	
	virtual HRESULT GetSubsysID(int *pSubsysID)
	{
		*pSubsysID = m_iSubsysID;
		return S_OK;
	}
	virtual ICCenterNamesManager* GetCCenterNamesManager()
	{
		return m_pCCenterNamesManager;
	}
	virtual IClientObjectManager* GetClientObjectManager()
	{
		return m_pClientObjectManager;
	}
	virtual HRESULT Initilize(LPCSTR szSubsysName, LPCSTR szWorkPath = NULL)
	{
		srand( (unsigned)time( NULL ) );

		if(NULL==szWorkPath || NULL==szWorkPath[0])
		{
			//modify by ysb
			/*::GetCurrentDirectory(sizeof(m_szWorkPath), m_szWorkPath);
			lstrcat(m_szWorkPath, "\\");*/
			lstrcpyn(m_szWorkPath, "\\", 2);
		}
		else
		{
			lstrcpyn(m_szWorkPath, szWorkPath, min(MAX_PATH, lstrlen(szWorkPath)+1));
			if('\\'!=m_szWorkPath[lstrlen(m_szWorkPath)-1])			
				lstrcat(m_szWorkPath, "\\");
		}
		lstrcpyn(m_szSubsysName, szSubsysName, min(SUB_SYSTEM_NAME_MAX, lstrlen(szSubsysName)+1));
		m_iSubsysID = msdk::GetCurrentSubsysId(m_szSubsysName);
		
		DllQuickCreateInstance(CLSID_CCenterNamesManager, __uuidof(ICCenterNamesManager), m_pCCenterNamesManager,NULL);
		Assert(m_pCCenterNamesManager);

		DllQuickCreateInstance(CLSID_ClientObjectManager, __uuidof(IClientObjectManager), m_pClientObjectManager,NULL);
		Assert(m_pClientObjectManager);
		
		DllQuickCreateInstance(CLSID_CCenterObject, __uuidof(ICCenterObject), m_pCCenterObject,NULL);		
		Assert(m_pCCenterObject);
		return S_OK;
	}
	virtual HRESULT Uninitlize()
	{
		Msg("ICCenter->Uninitlize %s in %s %x\n", m_szSubsysName, m_szWorkPath?m_szWorkPath:"", (ICCenterObject*)m_pCCenterObject);
		m_pCCenterObject = INULL;
		Msg("ICCenter->Uninitlize m_pCCenterObject %x\n", (IClientObjectManager*)m_pClientObjectManager);
		m_pClientObjectManager = INULL;
		Msg("ICCenter->Uninitlize m_pClientObjectManager %x\n", (ICCenterNamesManager*)m_pCCenterNamesManager);
		m_pCCenterNamesManager = INULL;
		Msg("ICCenter->Uninitlize m_pCCenterNamesManager\n");

		ZeroMemory(m_szServerName, sizeof(m_szServerName));
		return S_OK;
	}
	virtual HRESULT Start(DWORD dwType/* = CCENTER_START_BOTH*/)
	{
		Msg("CCenterNamesManager->Register(%x) %s in %s\n", dwType, m_szSubsysName, m_szWorkPath?m_szWorkPath:"");
		//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenter::Start> CCenterNamesManager->Register(%x) %s in %s " ) , dwType, m_szSubsysName, m_szWorkPath?m_szWorkPath:"");

		m_dwType = dwType;

		HRESULT hr;
		CStackLockWrapper<CAutoCriticalSection> lock(m_lockref);

		//进程服务通讯对象管理器注册本进程的名字
		hr = m_pCCenterNamesManager->Register(m_szSubsysName, m_szWorkPath, dwType);

		if(S_OK == hr)
		{
			Msg("ClientObjectManager->Open %s\n", m_szSubsysName);
			//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenter::Start> ClientObjectManager->Open %s " ) , m_szSubsysName);
			hr = m_pClientObjectManager->Open(m_szSubsysName, m_szWorkPath);
			if(S_OK == hr)
			{
				m_pClientObjectNames = m_pClientObjectManager->GetClientObjectNames();
				Assert(m_pClientObjectNames);

				m_pCCenterNamesManager->GetServerName(m_szServerName, SERVER_RPC_NAME_MAX);

				Msg("CCenterObject->Start %s\n", m_szServerName);
				//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenter::Start> CCenterObject->Start %s" ) , m_szServerName);

				//启动通讯中心对象
				hr = m_pCCenterObject->Start(m_szServerName, m_pCCenterNamesManager, dwType);
				if(S_OK == hr)
				{
					Msg("ClientObjectManager->PutCCenter\n");
					//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <CCCenter::Start> ClientObjectManager->PutCCenter " ) );

					hr = m_pClientObjectManager->PutCCenter(m_pCCenterObject);
					if(S_OK == hr)
					{						
						return hr;
					}
				}
			}
		}

	
		return hr;

	}

	virtual HRESULT Stop()
	{
		//增加和OnCallEvent的同步
		g_rpcCall.CancelRun();

		CStackLockWrapper<CAutoCriticalSection> lock(m_lockref);

		HRESULT hr = S_OK;

		//Msg("ICCenter->Stop m_pCCenterObject->Stop\n");
		if(m_pCCenterObject!=NULL)
		{
			hr = m_pCCenterObject->Stop();
		}	

		//Msg("ICCenter->Stop m_pClientObjectManager!=NULL\n");
		if(m_pClientObjectManager!=NULL)
		{
			m_pClientObjectManager->PutCCenter(NULL);
			m_pClientObjectManager->Close();
		}
		if(m_pClientObjectNames!=NULL)
			m_pClientObjectNames = INULL;

		//Msg("ICCenter->Stop m_pCCenterNamesManager!=NULL\n");
		if(m_pCCenterNamesManager!=NULL)
		{
			m_pCCenterNamesManager->Unregister();
		}

		return hr;
	}

	virtual IClientObject* Logon(DEFID defid, MASKID maskid, IMessageCallBack* pCallback, DWORD dwStyle)
	{		
		return LogonEx(defid, maskid, pCallback, NULL, NULL, 0, dwStyle);
	}

	virtual IClientObject* LogonEx(DEFID defid, MASKID maskid, IMessageCallBack* pCallback, MSGID* enablemsgs, MSGID* disablemsgs, int imode, DWORD dwStyle)
	{
		CStackLockWrapper<CAutoCriticalSection> lock(m_lockref);

		Assert(m_pCCenterNamesManager);
		OBJID pobjid;
		m_pCCenterNamesManager->GetServerObjectID(pobjid);

		DWORD dwType;
		m_pCCenterNamesManager->GetServerType(dwType);

		Assert(m_pClientObjectManager);
		UTIL::com_ptr<IClientObject> pClient = m_pClientObjectManager->Register(defid, maskid, dwStyle, pobjid, dwType, enablemsgs, disablemsgs);
		if(pClient!=NULL && m_dwType&CCENTER_START_SERVER)
		{
			pClient->SetCallbackMode(imode, pCallback);			
		}
		return pClient;
	}

	virtual HRESULT Logoff(IClientObject* pClient)
	{
		CStackLockWrapper<CAutoCriticalSection> lock(m_lockref);

		HRESULT hr;		
		Assert(m_pClientObjectManager);
		hr = m_pClientObjectManager->UnRegister(pClient);		
		return hr;
	}

	virtual HRESULT QueryFirstObject(HANDLE32 &handle)
	{		
		Assert(m_pClientObjectNames);
		return m_pClientObjectNames->QueryFirstObject(handle);
	}
	virtual HRESULT QueryNextObject(HANDLE32 &handle)
	{
		Assert(m_pClientObjectNames);
		return m_pClientObjectNames->QueryNextObject(handle);
	}
	virtual HRESULT QueryObject(HANDLE32 handle, CLIENTINFO* pClientInfo)
	{
		Assert(m_pClientObjectNames);
		return m_pClientObjectNames->QueryObject(handle, pClientInfo);
	}

	virtual HRESULT QueryFirstObjectEx(HANDLE32 &handle, const CLIENTINFO* pClientInfo)
	{		
		Assert(m_pClientObjectNames);

		CLIENTINFO ci;
		if(S_OK==m_pClientObjectNames->QueryFirstObject(handle))
		{
			m_pClientObjectNames->QueryObject(handle, &ci);
			if(*pClientInfo == ci)
				return S_OK;
		}
		return S_FALSE;
	}
	virtual HRESULT QueryNextObjectEx(HANDLE32 &handle, const CLIENTINFO* pClientInfo)
	{
		Assert(m_pClientObjectNames);

		CLIENTINFO ci;
		if(S_OK==m_pClientObjectNames->QueryNextObject(handle))
		{
			m_pClientObjectNames->QueryObject(handle, &ci);
			if(*pClientInfo == ci)
				return S_OK;
		}
		return S_FALSE;
	}

	virtual void* MS_Allocate(size_t size)
	{
		return MS_AllocateCallCenter(size);
	}
	virtual void MS_Free(void *pointer)
	{
		MS_FreeCallCenter(pointer);
	}
};

class CComX3 : public IComX3
	, public CUnknownImp	
{
public:
	UNKNOWN_IMP1(IComX3);

	STDMETHOD(init_class)(IMSBase* prot, IMSBase* punkOuter){return S_OK;}

	ICCenter* Initialize(LPCSTR szSubsysName, LPCSTR szWorkPath = NULL, DWORD dwType = CCENTER_START_BOTH)
	{
		return MS_InitializeCallCenter(szSubsysName, szWorkPath, dwType);
	}
	void Uninitialize(ICCenter *pCCenter)
	{
		MS_UninitializeCallCenter(pCCenter);
	}
	void* Allocate(size_t size)
	{
		return MS_AllocateCallCenter(size);
	}
	void FreeCallCenter(void *pointer)
	{
		MS_FreeCallCenter(pointer);
	}
};
