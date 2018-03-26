#include "stdafx.h"
#include "util/globalevent.h"
#include "SyncObject/criticalsection.h"
#include "util/memmapfile.h"

#include "icenter_p.h"
#include "commx/commx_error.h"
#include "commx/namespacetables.h"
#include "namespacetabledef.h"

inline unsigned __int32 GetCycleCount() 
{
	srand(GetTickCount());
	return rand();
} 

BOOL IsRunServer(__int32 pid)
{
	BOOL bRun = FALSE;
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if(hProcess!=NULL)
	{
		//if(WAIT_TIMEOUT==::WaitForSingleObject(hProcess, 0))
		//{
			bRun = TRUE;
		//}
		CloseHandle(hProcess);
	}
	else
	{
		if(GetLastError()==5)
			bRun = TRUE;
	}
	return bRun;
}

//本机进程服务器名字管理器：真正的名字的增删查改
void CCCenterNames::RandName(DWORD dwParam)
{
	//通过进程ID和进程创建的时间来表示一个唯一的进程。
	//通过这种命名方式避免创建过多的endpoint名，过多的endpoint会使rpc运行时库创建等量的线程
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
	if(hProc)
	{
		//取得时间信息；
		FILETIME tCreate, tExit, tKernel, tUser;
		if(GetProcessTimes(hProc,
			&tCreate,
			&tExit,
			&tKernel,
			&tUser)  
			)
		{
			wsprintf(s_szServerName, "%d_%d_%d_%d", RPC_CALL_INTERFACE_VER, GetCurrentProcessId(), tCreate.dwHighDateTime, tCreate.dwLowDateTime);
		}
		CloseHandle(hProc);
	}

	if(strlen(s_szServerName) == 0) //不成功的话使用老流程
	{
		if(0==s_iProcess)//确认这个只会调用一次！
		{
			s_iProcess = GetCycleCount();
			if(0==s_iProcess) s_iProcess = GetCycleCount();//太背了吧

			//服务器EndPoint名字只和接口版本、进程id相关
			wsprintf(s_szServerName, "%d_%d_%d", RPC_CALL_INTERFACE_VER, GetCurrentProcessId(), s_iProcess);
		}
	}
}

HRESULT CCCenterNames::Open(LPCSTR szName)
{	
	RandName(0);
	return OpenMap("ms_server_table_", szName);
}
HRESULT CCCenterNames::Close()
{		
	return CloseMap();
}
//查找(通过pid)存在的ServerName，重复了根据是否强制覆盖决定立即返回E_COMMX_ADD_EXIST_ONE，或者覆盖老的记录
HRESULT CCCenterNames::Add(SERVERINFO* psi, BOOL bForce)
{
	HRESULT hr = S_OK;
	CStackAutoMutexLock ssl(m_tablelock);

	for(__int32 i=0;i<m_iMaxCount;i++)
	{
		//按位检查，得到是否为空闲的块
		if( 0!=CheckAllocBlock(i) )
		{
			//检查是否为匹配
			SERVERINFO* pCur = GetArray(i);
			if(*psi==*pCur)
			{
				if(!bForce)
				{
					Assert(pCur->pid==psi->pid);
					Assert(pCur->interid==psi->interid);
					CopyMemory(psi, pCur, sizeof(SERVERINFO));
					hr = E_COMMX_ADD_EXIST_ONE;
					break;
				}
				else
				{
					//强制覆盖
					psi->pobjid = GetCycleCount();
					if(0==psi->pobjid) psi->pobjid = 1;//太背了吧
					lstrcpy(psi->ServerName, s_szServerName);//固定死的

					CopyMemory(pCur, psi, sizeof(SERVERINFO));

					Msg("ServerTable: Replace index = %d,  pid=%d, pobjid=%d in %d\n", i, psi->pid, psi->pobjid, GetCurrentProcessId());
					return hr;
				}
			}
		}
	}
	if(hr==S_OK)
	{
		//插入一条
		__int32 iFree = GetFreeIndex();
		if(FIND_INVALID_INDEX==iFree)
		{
			if(FreeUnused()==0)
			{
				Assert(!"进程太多了");
				hr = E_COMMX_OUTOF_PROCESS_BLOCK;
			}
			else
				iFree = GetFreeIndex();
		}
		if(FIND_INVALID_INDEX!=iFree)
		{
//#pragma	reminder("应该给个随机数，只要能够保证全系统唯一，这里只是随便写了一下")
			psi->pobjid = GetCycleCount();
			if(0==psi->pobjid) psi->pobjid = GetCycleCount();//太背了吧
			lstrcpy(psi->ServerName, s_szServerName);//固定死的

			SetAllocBlock(iFree);
			CopyMemory(GetArray(iFree), psi, sizeof(SERVERINFO));

			Msg("ServerTable: Add index = %d,  pid=%d, pobjid=%d in %d\n", iFree, psi->pid, psi->pobjid, GetCurrentProcessId());
		}
	}		
	return hr;
}

__int32 CCCenterNames::s_iProcess = 0;
CHAR CCCenterNames::s_szServerName[SERVER_RPC_NAME_MAX] = {0};


//本机通讯对象名字管理器：真正的名字的增删查改
HRESULT CClientObjectNames::Open(LPCSTR szName)
{
	return OpenMap("ms_client_table_", szName);
}
HRESULT CClientObjectNames::Close()
{		
	return CloseMap();
}
//查询是否存在，如果已经有一个则返回存在的objid并返回E_COMMX_ADD_EXIST_ONE，没有则添加一个新的名字并返回新的objid返回成功
HRESULT CClientObjectNames::Add(CLIENTINFO* pci, BOOL bForce)
{
	HRESULT hr = S_OK;		
	CStackAutoMutexLock ssl(m_tablelock);
	//先要清理一下错误的记录，比如某个进程崩溃遗留下的记录
	for(__int32 i=0;i<m_iMaxCount;i++)
	{
		//按位检查，得到是否为空闲的块
		if( 0!=CheckAllocBlock(i) )
		{
			//检查是否为匹配
			CLIENTINFO* pCur = GetArray(i);
			if(*pci==*pCur)
			{
				//发现同名对象了, 需要判定此进程是否真的运行着
				if(IsRunServer(pCur->pid) && !bForce)
				{						
					Assert(pCur->maskid==pci->maskid);
					CopyMemory(pci, pCur, sizeof(CLIENTINFO));
					hr = E_COMMX_ADD_EXIST_ONE;
					break;
				}
				else
				{
					//可以抹掉此条错误的记录
					SetFreeBlock(i);
					ZeroMemory(pCur, sizeof(CLIENTINFO));
				}
			}
		}
	}
	if(hr==S_OK)
	{
		//插入一条
		__int32 iFree = GetFreeIndex();
		if(FIND_INVALID_INDEX==iFree)
		{
			if(FreeUnused()==0)
			{
				Assert(!"对象太多了");
				hr = E_COMMX_OUTOF_OBJECTS_BLOCK;
			}
			else
				iFree = GetFreeIndex();
		}
		else
		{
//#pragma	reminder("应该给个随机数，只要能够保证全系统唯一，这里只是随便写了一下")
			pci->objid = GetCycleCount();
			if(0==pci->objid) pci->objid = GetCycleCount();//太背了吧

			SetAllocBlock(iFree);
			CopyMemory(GetArray(iFree), pci, sizeof(CLIENTINFO));
			hr = S_OK;

			Msg("Add [%d, %X] index = %d, objid=%d in %d\n", pci->defid, pci->maskid, iFree, pci->objid, GetCurrentProcessId());
			//RSLOG( RSLOG_DETAIL , _T("[-COMX3.DLL-] <IClientObject* Register> [ Add [%d, %X] index = %d, objid=%d in %d]"), pci->defid, pci->maskid, iFree, pci->objid, GetCurrentProcessId());
		}
	}		
	return hr;
}