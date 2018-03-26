// rscenter.h
//


#pragma once
//#pragma warning(push)
//#pragma warning(disable: 4786)


#include "util/globalevent.h"
#include "SyncObject/criticalsection.h"
#include "util/memmapfile.h"


#include <vector>
#include <map>
using namespace std;

#ifdef X64
	#include "x64/icenter.h"
#else
	#include "win32/icenter.h"
#endif


#include "icenter_p.h"

#include "commx/srvclientinfo.h"
#include "commx/namespacetables.h"
#include "commx/commx3.h"

namespace msdk {

//实现一个回调
error_status_t OnCallEvent(MESSAGEINFO* pMsg);


//接收数据队列的处理服务对象：处理收到的数据，并进入队列
struct IMessageQuene : public IMSBase
{
public:
	virtual HRESULT Append(IReceMessage* pReceMessage);	
};

//线程（池）管理对象：对队列数据通过线程回调给通讯客户端
struct IThreadPools : public IMSBase
{
public:
	virtual HRESULT Run();
	virtual HRESULT OnEvent(IReceMessage *pReceMessage);
};


//服务器信息对象：保存服务器信息
struct IServer : public IMSBase
{
public:
	virtual HRESULT PutServer(SERVERINFO* psi) = 0;

	virtual HRESULT GetServer(SERVERINFO* psi) = 0;
};
MS_DEFINE_IID(IServer, "{19386124-A28F-43d6-9B8F-1BD4B0BA445A}");
// {1A20474D-0A87-4e43-B368-AE85B77CBAD9}
MS_DEFINE_GUID(CLSID_CServer, 
			0x1a20474d, 0xa87, 0x4e43, 0xb3, 0x68, 0xae, 0x85, 0xb7, 0x7c, 0xba, 0xd9);



//进程服务通讯对象管理：负责管理本机所有进程的服务通讯对象的名字
struct ICCenterNamesManager : public IMSBase
{
public:	
	virtual IServer* FindServer(CLIENTINFO* pci) = 0;
	virtual HRESULT Register(LPCSTR szName, LPCSTR szWorkPath, DWORD dwType) = 0;
	virtual HRESULT Unregister() = 0;

	virtual HRESULT GetServerName(LPSTR szServerName, DWORD dwSize) = 0;
	virtual HRESULT GetServer(SERVERINFO* pServer) = 0;
	virtual HRESULT GetServerObjectID(OBJID &pobjid) = 0;
	virtual HRESULT GetServerType(DWORD &dwType) = 0;
};

MS_DEFINE_IID(ICCenterNamesManager, "{E4271AFF-8BEC-4de9-B35B-6469DD68B482}");
// {A0D75FD6-E1EE-418d-9840-6E1A6B6F7237}
MS_DEFINE_GUID(CLSID_CCenterNamesManager, 
			0xa0d75fd6, 0xe1ee, 0x418d, 0x98, 0x40, 0x6e, 0x1a, 0x6b, 0x6f, 0x72, 0x37);



//客户通讯对象管理器：负责管理客户通讯对象的名字的管理器对象。
struct IClientObjectManager : public IMSBase
{
public:
	virtual HRESULT Open(LPCSTR szName, LPCSTR szWorkPath) = 0;
	virtual HRESULT Close() = 0;
	virtual IClientObjectNames* GetClientObjectNames() = 0;
	virtual ICCenterObject* GetCCenter() = 0;
	virtual HRESULT PutCCenter(ICCenterObject* pCCenterObject) = 0;
	virtual IClientObject* Register(DEFID defid, MASKID maskid, DWORD dwStyle, OBJID pobjid, DWORD dwType, MSGID* enablemsgs, MSGID* disablemsgs) = 0;
	virtual HRESULT UnRegister(IClientObject *pClientObject) = 0;
	//查找指定对象发送消息
	virtual HRESULT FormatNeedSend(ISendMessage* pSendMsg, DWORD dwMatch) = 0;
	//接受到消息查找指定对象
	virtual HRESULT FormatNeedRecv(IReceMessage *pReceMsg) = 0;
};

MS_DEFINE_IID(IClientObjectManager, "{66D03339-40B5-4eb8-AEBA-01D3F7EAD8B3}");
// {7AB23F2D-6162-4cd9-91DC-AE45FF212CF0}
MS_DEFINE_GUID(CLSID_ClientObjectManager, 
			0x7ab23f2d, 0x6162, 0x4cd9, 0x91, 0xdc, 0xae, 0x45, 0xff, 0x21, 0x2c, 0xf0);

//////////////////////////////////////////////////////////////////////////
int GetCurrentSubsysId(LPCSTR szName);

//#pragma warning(pop)	// 4786

ICCenter* MS_InitializeCallCenter(LPCSTR szSubsysName, LPCSTR szWorkPath/* = NULL*/, DWORD dwType/* = CCENTER_START_BOTH*/);
void MS_UninitializeCallCenter(ICCenter *pCCenter);
void MS_ShutDown(DWORD dwParam);
void *MS_AllocateCallCenter(size_t size);
void MS_FreeCallCenter(void *pointer);
error_status_t OnCallEvent(MESSAGEINFO* pMsg);

} //namespace msdk