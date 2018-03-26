#include "StdAfx.h"
#include "HookContext.h"


#include "mpool/mpool.h"
#include "mhook/mhook.h"
#include "HookActions.h"
//////////////////////////////////////////////////////////////////////////

CHookContext::CHookContext()
{
	m_MemPool = new mpool;
	ZeroMemory(m_MemPool, sizeof(mpool));
	mpool_init_default(m_MemPool, sizeof(HOOK_CONTEXT) * 1024, sizeof(HOOK_CONTEXT));

	InitializeCriticalSection(&m_sec);
	m_dwTlsIndex = TlsAlloc();
}

CHookContext::~CHookContext()
{
	TlsFree(m_dwTlsIndex);
	m_dwTlsIndex = 0;
	if (m_MemPool)
	{
		mpool_destroy(m_MemPool);
		delete m_MemPool;
		m_MemPool = NULL;
	}

	DeleteCriticalSection(&m_sec);
}

CHookContext& CHookContext::GetInstance()
{
	static CHookContext g_hook_context;
	return g_hook_context;
}

//当前线程的hook上下文
LPHOOK_CONTEXT CHookContext::GetHookContext()
{
	LPHOOK_CONTEXT pContext = (LPHOOK_CONTEXT)TlsGetValue(m_dwTlsIndex);
	return pContext;
}

//获取当前是函数标记的上下文
LPHOOK_CONTEXT CHookContext::GetHookContext(int nTag)
{
	LPHOOK_CONTEXT pContext = GetHookContext();
	

	//如果跟节点为空
	if (!pContext)
	{
		pContext = CreateContext(nTag);
		pContext->dwEnterCount++;
		return pContext;
	}


	//跟节点不为空
	for (; pContext ; pContext = pContext->NextContext)
	{
		if (pContext->nHookTag == nTag)
		{
			pContext->dwEnterCount++;
			break;
		}
	}

	//没有找到，创建一个新的
	if (! pContext)
	{
		pContext = CreateContext(nTag);
		pContext->dwEnterCount++;
	}
	

	return pContext;
}


LPHOOK_CONTEXT CHookContext::CreateContext(int nTag)
{
	if (!m_MemPool)
	{
		return NULL;
	}

	LPHOOK_CONTEXT pContext= Alloc();
	if (!pContext)
	{
		return NULL;
	}

	ZeroMemory(pContext, sizeof(HOOK_CONTEXT));
	pContext->bHasCall = FALSE;
	pContext->dwThread = GetCurrentThreadId();
	pContext->nHookTag = nTag;

	LPHOOK_CONTEXT pContextParent = GetHookContext();
	if (!pContextParent)//没有根节点
	{
		TlsSetValue(m_dwTlsIndex, pContext);
		return pContext;
	}

	if (pContextParent)
	{
		//找到最后一个节点
		for (pContextParent ; pContextParent->NextContext ; pContextParent = pContextParent->NextContext);

		pContextParent->NextContext = pContext;
		pContext->PrevContext = pContextParent;
	}

	return pContext;
}

VOID CHookContext::DestoryContext(LPHOOK_CONTEXT pHookContext)
{
	LPHOOK_CONTEXT pRootContext = GetHookContext();
	_ASSERT(pRootContext);

	if (pRootContext == NULL)
	{
		return ;
	}


	//如果有引用计数
	if (pHookContext->dwEnterCount > 1)
	{
		pHookContext->dwEnterCount--;
		return;
	}

	LPHOOK_CONTEXT pDeleteConext = NULL;
	if (pRootContext == pHookContext)
	{
		TlsSetValue(m_dwTlsIndex, pRootContext->NextContext);
		pDeleteConext = pRootContext;
	}
	else
	{
		//找到要删除的节点
		for (pDeleteConext = pRootContext ; pDeleteConext && pDeleteConext != pHookContext ; pDeleteConext = pDeleteConext->NextContext);
	}

	if (pDeleteConext)
	{
		if(pDeleteConext->PrevContext)
		{
			pDeleteConext->PrevContext->NextContext = pDeleteConext->NextContext;
		}
		
		if (pDeleteConext->NextContext)
		{
			pDeleteConext->NextContext->PrevContext = pDeleteConext->PrevContext;
		}
		Free(pDeleteConext);
	}
}

LPHOOK_CONTEXT CHookContext::Alloc()
{
	if (m_MemPool)
	{
		EnterCriticalSection(&m_sec);
		LPHOOK_CONTEXT pContext = (LPHOOK_CONTEXT)mpool_salloc(m_MemPool, sizeof(HOOK_CONTEXT));
		LeaveCriticalSection(&m_sec);
		return pContext;
	}

	return NULL;
}

VOID CHookContext::Free(LPHOOK_CONTEXT pContext)
{
	if (pContext && m_MemPool)
	{
		EnterCriticalSection(&m_sec);
		mpool_sfree(m_MemPool, pContext, sizeof(HOOK_CONTEXT));
		LeaveCriticalSection(&m_sec);
	}
}

BOOL CHookContext::IsHookContext()
{
	LPHOOK_CONTEXT pContext = (LPHOOK_CONTEXT)TlsGetValue(m_dwTlsIndex);
	return pContext ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////

CHookManager& CHookManager::GetInstance()
{
	static CHookManager g__CHookManager;
	return g__CHookManager;
}

FARPROC CHookManager::GetHookRealAddress(int nHookTag)
{
	PHOOK_ENTRY pEntry = GetEntry(nHookTag);
	if (pEntry)
	{
		return pEntry->pRealFunctionAddr;
	}

	return NULL;
}

FARPROC CHookManager::GetHookNewAddress(int nHookTag)
{
	PHOOK_ENTRY pEntry = GetEntry(nHookTag);
	if (pEntry)
	{
		return pEntry->pNewFunctionAddr;
	}

	return NULL;
}

BOOL	CHookManager::IsHook(int nHookTag)
{
	PHOOK_ENTRY pEntry = GetEntry(nHookTag);
	if (pEntry)
	{
		return pEntry->bHook;
	}

	return FALSE;
}

PHOOK_ENTRY CHookManager::GetEntry(int nHookTag)
{
	CHookEntryMap::iterator it_entry = m_hookEntryMap.find(nHookTag);
	if (it_entry != m_hookEntryMap.end())
	{
		return &it_entry->second;
	}
	
	return NULL;
}

BOOL	CHookManager::SetRegeditHookEntry(pRegeditHookEntry pFunc)
{
	CRegeditHookEntrySet::iterator it = m_RegeditHookEntrySet.find(pFunc);
	if (it == m_RegeditHookEntrySet.end())
	{
		m_RegeditHookEntrySet.insert(it,pFunc);
		return TRUE;
	}

	return FALSE;
}

BOOL CHookManager::DoHook()
{
	ZeroMemory(m_s_ini_path, sizeof(m_s_ini_path));
	GetModulePath(g_hModule, m_s_ini_path, _countof(m_s_ini_path));
	_tcscat_s(m_s_ini_path, _countof(m_s_ini_path), _T("\\inject.ini"));

	for (CRegeditHookEntrySet::iterator it = m_RegeditHookEntrySet.begin() ; it != m_RegeditHookEntrySet.end() ; it++)
	{
		if (*it)
		{
			PHOOK_ENTRY pEntry = NULL;
			DWORD dwCount = (*it)(&pEntry);
			for (DWORD dwLoop = 0 ; dwLoop < dwCount ; dwLoop++)
			{
				m_hookEntryMap[pEntry[dwLoop].nTag] = pEntry[dwLoop];
			}
		}
	}


	BOOL bDefault = GetPrivateProfileInt(_T("default"), _T("hook"),1, m_s_ini_path);
	
	for (CHookEntryMap::iterator it_entry = m_hookEntryMap.begin() ; it_entry != m_hookEntryMap.end() ; it_entry++)
	{
		if (it_entry->second.pRealFunctionAddr && it_entry->second.pNewFunctionAddr)
		{
			//读取INI文件
			CString strKey ; strKey.Format(_T("%d"), it_entry->first);
			INT nHook = GetPrivateProfileInt(_T("inject"), strKey,bDefault, m_s_ini_path);
			if (nHook)
			{
				BOOL bRet = Mhook_SetHook((PVOID*)&it_entry->second.pRealFunctionAddr, it_entry->second.pNewFunctionAddr);
				if (!bRet)
				{
					ZM1_GrpDbgOutput(GroupName, _T("Hook: module[%s] name:[%s] real[%x] new:[%d] ret:[%d]"),
						it_entry->second.strfModuleName, it_entry->second.strFuncName, it_entry->second.pRealFunctionAddr,it_entry->second.pNewFunctionAddr, bRet);
				}
			}
			
		}
	}
	

	return TRUE;
}

BOOL CHookManager::DoUnHook()
{
	//m_AsynFunc.CancelRun(TRUE);
	for (CHookEntryMap::iterator it_entry = m_hookEntryMap.begin() ; it_entry != m_hookEntryMap.end() ; it_entry++)
	{
		BOOL bRet = Mhook_Unhook((PVOID*)&it_entry->second.pRealFunctionAddr);
	}
	return TRUE;
}


// BOOL CHookManager::HookCanRun()
// {
// 	return m_AsynFunc.CanRun();
// }
//////////////////////////////////////////////////////////////////////////
//

CHookContextCheck::CHookContextCheck(int nTag)
{
	m_pContext = CHookContext::GetInstance().GetHookContext(nTag);
}

CHookContextCheck::~CHookContextCheck()
{
	if (m_pContext)
	{
		CHookContext::GetInstance().DestoryContext(m_pContext);
		m_pContext = NULL;
	}
}

LPHOOK_CONTEXT CHookContextCheck::GetContext()
{
	return m_pContext;
}

BOOL CHookContextCheck::IsReEnter()
{

	//检查重入
	if (m_pContext && m_pContext->dwEnterCount > 1)
	{
		return TRUE;
	}


	if (CommunicationManager::GetInstance().IsNpcThreadContext())
	{
		return TRUE;
	}


	//检查特殊的重入，，npc_init_tag

	if (m_pContext->PrevContext)
	{
		for (LPHOOK_CONTEXT pContext = m_pContext->PrevContext;pContext ; pContext = pContext->PrevContext)
		{
			if (pContext->nHookTag == npc_init_tag)
			{
				return TRUE;
			}
		}
	}

	
	if (m_pContext->NextContext)
	{
		for (LPHOOK_CONTEXT pContext = m_pContext->NextContext ; pContext ; pContext = pContext->NextContext)
		{
			if (pContext->nHookTag == npc_init_tag)
			{
				return TRUE;
			}
		}
	}
	

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//

// CSetLastErrorHelp::CSetLastErrorHelp(DWORD dwLastError /* = GetLastError() */)
// {
// 	m_dwError = dwLastError;
// }
// CSetLastErrorHelp::~CSetLastErrorHelp()
// {
// 	SetLastError(m_dwError)
// }

BOOL IsEntryHooked(int nTag)
{
	return CHookManager::GetInstance().IsHook(nTag);
}

CRegeditHookEntryHelp::CRegeditHookEntryHelp(pRegeditHookEntry pFunc)
{
	if (pFunc)
	{
		CHookManager::GetInstance().SetRegeditHookEntry(pFunc);
	}
}