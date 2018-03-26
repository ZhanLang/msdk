// comx3.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <list>

extern "C" void MS_ShutDown(DWORD dwParam);

#ifndef EXPORT_STATICLIB
USE_DEFAULT_DLL_MAIN
#endif

#include "rscenter.cpp"
#include "call_mgr.h"
#include "namespacetabledef.h"

BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY(CLSID_CServer, CServer)
		CLIDMAPENTRY(CLSID_CSendMessage, CSendMessage)
		CLIDMAPENTRY(CLSID_CReceMessage, CReceMessage)
		CLIDMAPENTRY(CLSID_CClientObject, CClientObject)
		CLIDMAPENTRY(CLSID_CCenterNamesManager, CCCenterNamesManager)
		CLIDMAPENTRY(CLSID_ClientObjectManager, CClientObjectManager)
		CLIDMAPENTRY(CLSID_CComX3, CComX3)
		CLIDMAPENTRY(CLSID_CCenter, CCCenter)
		CLIDMAPENTRY(CLSID_CCommInProcess, CCommInProcess)
		CLIDMAPENTRY(CLSID_CCommInHost, CCommInHost)
		CLIDMAPENTRY(CLSID_CCenterObject, CCCenterObject)
		CLIDMAPENTRY(CLSID_CCenterNames, CCCenterNames)
		CLIDMAPENTRY(CLSID_CClientObjectNames, CClientObjectNames)
	CLIDMAPENTRY_END
END_CLIDMAP

#ifndef EXPORT_STATICLIB
DEFINE_ALL_EXPORTFUN
#else
// HRESULT MSDllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
// {
// 	for(size_t i = 0; i < sizeof(g_mapClassObject)/sizeof(g_mapClassObject[0]); ++i)
// 	{
// 		if(rclsid == g_mapClassObject[i].clsid)
// 		{
// 		return g_mapClassObject[i].pfnGetClassObject(riid, ppv);
// 		}
// 	}
// 	return E_INVALIDARG;
// }
#endif

/////////////////////////////////////////////////////////////////////////////
typedef map< int, UTIL::com_ptr<ICCenter> >	CCenterHostList;
static CCenterHostList g_cCenterList;
static CGlobalMutex g_cGlobalMutex(NULL, FALSE, NULL);
ICCenter* msdk::MS_InitializeCallCenter(LPCSTR szSubsysName, LPCSTR szWorkPath/* = NULL*/, DWORD dwType/* = CCENTER_START_BOTH*/)
{
#ifdef X64
	::srand( (unsigned)time( NULL ) );
#endif

	Assert(NULL!=HANDLE(g_cGlobalMutex));
	if(szSubsysName==NULL || *szSubsysName == NULL) 
	{
		//RSLOG( RSLOG_ALERT , _T("[-COMX3.DLL-] <MS_InitializeCallCenter> {(szSubsysName==NULL || *szSubsysName == NULL)} Leaving ......") );
		return NULL;
	}

	//做大写转换！
	CHAR szRealSubsysName[SUB_SYSTEM_NAME_MAX];
	int i = 0;
	ZeroMemory(szRealSubsysName, sizeof(szRealSubsysName));	
	LPCSTR pName=szSubsysName;
	while(*pName != NULL && i<SUB_SYSTEM_NAME_MAX)
	{
		szRealSubsysName[i] = UP_CHAR(*pName);
		pName++;
		i++;
	}

	UTIL::com_ptr<ICCenter> pCCenter;

	{
		CStackLockWrapper<CGlobalMutex> lock(g_cGlobalMutex);

		int isubsysid = msdk::GetCurrentSubsysId(szRealSubsysName);
		CCenterHostList::iterator iter = g_cCenterList.find(isubsysid);
		if(iter!=g_cCenterList.end())
		{
			pCCenter = (*iter).second;
			Assert(pCCenter);
			pCCenter->AddRef();
			//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <MS_InitializeCallCenter> { found one CCenter interface , subsysname = %s } Leaving ......") , szRealSubsysName );
			return pCCenter;
		}

		DllQuickCreateInstance(CLSID_CCenter, __uuidof(ICCenter), pCCenter,NULL);

		Assert(pCCenter);
		RASSERT(pCCenter, NULL);

		pCCenter->Initilize(szRealSubsysName, szWorkPath);
		if(S_OK==pCCenter->Start(dwType))
		{
			pCCenter->AddRef();
			g_cCenterList[isubsysid] = pCCenter;
			//Log("Initialize CallCenter:%X, %s, id=%X, %s\n", dwType, szRealSubsysName, isubsysid, szWorkPath);
			//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <MS_InitializeCallCenter> Initialize CallCenter:%X, %s, id=%X, %s" ) , dwType, szRealSubsysName, isubsysid, szWorkPath );
		}
		else
		{
			pCCenter->Uninitlize();
			pCCenter = INULL;
		}
	}

	return pCCenter;
}

void msdk::MS_UninitializeCallCenter(ICCenter *pCCenter)
{
	bool bIsWaitforthread = false;


	Assert(pCCenter);	
	if(pCCenter)
	{
		UTIL::com_ptr<ICCenter> pCC(pCCenter);
		if(/*3*/2==pCC->Release())
		{
			int iSubsysID = 0;
			pCC->GetSubsysID(&iSubsysID);
			if(0!=iSubsysID)
			{
				CStackLockWrapper<CGlobalMutex> lock(g_cGlobalMutex);
				g_cCenterList.erase(iSubsysID);
			}
			//Log("Uninitialize CallCenter: %X\n", iSubsysID);

			pCC->Stop();

			//Log("Uninitialize pCC->Stop\n");
			pCC->Uninitlize();

			bIsWaitforthread = true;
		}
	}


	//if (bIsWaitforthread)
	//{
	//	CRpcThreadItem::WaitforAllRpcThread();
	//}
}

void msdk::MS_ShutDown(DWORD dwParam)
{
	typedef std::list<UTIL::com_ptr<ICCenter> > Shut_Down_TempList;	
	Shut_Down_TempList ShutDown_tempList;

	{
		CStackLockWrapper<CGlobalMutex> lock(g_cGlobalMutex);
		for(CCenterHostList::iterator iter = g_cCenterList.begin();iter!=g_cCenterList.end();iter++)
		{
			ShutDown_tempList.push_back( (*iter).second);
		}
		g_cCenterList.clear();
	}

	Shut_Down_TempList::iterator it = ShutDown_tempList.begin();
	for(;it!=ShutDown_tempList.end();++it)
	{
		(*it)->Stop();
	}
}

void *msdk::MS_AllocateCallCenter(size_t size)
{
	return MIDL_user_allocate(size);
}
void msdk::MS_FreeCallCenter(void *pointer)
{
	MIDL_user_free(pointer);
}

//add by bob
//退出的时候因为反初始化没有和rpc回调做同步，可能造成崩溃
CAsynFunc g_rpcCall;
error_status_t msdk::OnCallEvent(MESSAGEINFO* pMsg)
{
	CAutoAsynFunc rpcCall(&g_rpcCall);
	if(!rpcCall.CanRun()) return 0;

	//CRpcThreadItem logThread;

	HRESULT hr;

	Msg("OnCallEvent:%d\t from (%d, %d) to (%d, %d, %d) in subsys=%08X\n", pMsg->msgid, pMsg->source_defid, pMsg->source_maskid, pMsg->defid, pMsg->maskid, pMsg->objid, pMsg->subsysid);


	//初始化	
	UTIL::com_ptr<IReceMessage> pReceMessage;
	DllQuickCreateInstance(CLSID_CReceMessage, __uuidof(IReceMessage), pReceMessage,NULL);
	Assert(pReceMessage);

	hr = pReceMessage->Init(pMsg);
	if(S_OK==hr)
	{		
		int icallsubsysid = pMsg->subsysid;

		//寻找正确的子系统
		UTIL::com_ptr<ICCenter> pCCenter;		
		{
			CStackLockWrapper<CGlobalMutex> lock(g_cGlobalMutex);
			try
			{
				CCenterHostList::iterator iter = g_cCenterList.find(icallsubsysid);
				if(iter!=g_cCenterList.end())
				{
					pCCenter = (*iter).second;
				}
			}
			catch (...)
			{
				Msg("OnCallEvent find exception!\n");
			}
		}		

		try
		{
			//如果找到了

			if(pCCenter!=INULL)
			{
				DBG_CODE
					(
					int iSubsysID = 0;
				pCCenter->GetSubsysID(&iSubsysID);
				Assert(iSubsysID==icallsubsysid);
				);

				UTIL::com_ptr<IClientObjectManager> pClientObjectManager = pCCenter->GetClientObjectManager();
				Assert(pClientObjectManager);

				DWORD dwCallType = 0;
				pReceMessage->GetCallType(&dwCallType);

				//获得正确的IClientObject目标pTargetClient
				hr = pClientObjectManager->FormatNeedRecv(pReceMessage);
				if( (S_OK==hr) && (CALL_TYPE_SYNC & dwCallType))
				{
					//同步调用
					UTIL::com_ptr<IClientObject> pTargetClient = pReceMessage->GetTargetObject();
					Assert(pTargetClient);
					if(pTargetClient!=NULL)
					{
						UTIL::com_ptr<IMessageCallBack> pMessageCallBack = pTargetClient->GetCallBack();
						Assert(pMessageCallBack);
						if(pMessageCallBack!=INULL)
							hr = pMessageCallBack->Dispatch(pReceMessage);
					}
				}
				else
				{
					// 				//Log("rpc no found this objid = %d, (hr=%d)\n", pMsg->objid, hr);				
					// 				//暂时没有做异步调用的逻辑
					// 				Assert(FALSE);
					// 
					// 				//异步调用，或者没有找到也试一下异步调用
					// 				IMessageQuene *pMessageQuene = NULL;
					// 				hr = pMessageQuene->Append(pReceMessage);
					// 				if(S_OK==hr)
					// 				{
					// 					IThreadPools *pThreadPools = NULL;
					// 					hr = pThreadPools->OnEvent(pReceMessage);
					// 				}
					Msg("No found target!\n");
				}
			}
		}
		catch (...)
		{
			Msg("OnCallEvent Dispatch exception!\n");
		}
	}

	return 0;
}


