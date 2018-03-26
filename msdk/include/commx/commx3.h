// rsct.h
//

#pragma once

#include ".\srvclientinfo.h"
#include ".\namespacetables.h"

namespace msdk {

//消息实体对象
struct IMessage : public IUnknown
{
public:
	virtual HRESULT GetCallType(DWORD* pdwCallType) = 0;
	virtual HRESULT PutCallType(DWORD dwCallType) = 0;

	virtual HRESULT GetSource(CLIENTINFO* pci) = 0;
	virtual HRESULT PutSource(CLIENTINFO* pci) = 0;
	virtual HRESULT GetTarget(CLIENTINFO* pci) = 0;
	virtual HRESULT PutTarget(CLIENTINFO* pci) = 0;

	virtual HRESULT GetTargetObjid(OBJID &objid) = 0;
	virtual HRESULT PutTargetObjid(OBJID objid) = 0;

	virtual IClientObject* GetTargetObject() = 0;
	virtual HRESULT PutTargetObject(IClientObject* pTargetClient) = 0;

	virtual HRESULT GetTargetMask(DWORD* pdwMask) = 0;
	virtual HRESULT PutTargetMask(DWORD dwMask) = 0;

	virtual HRESULT GetMessageInfo(MESSAGEINFO **ppMsgInfo) = 0;

	virtual HRESULT PutOutputInfo(byte* pOutData, int iOutCch) = 0;
};

MS_DEFINE_IID(IMessage, "{04E3C9FF-C7E0-4a14-A448-D3DDA6BC320C}");

struct ISendMessage : public IMessage
{
public:
	virtual HRESULT Init(IClientObject *pClientObject, MESSAGEINFO* pMsg) = 0;	
	virtual HRESULT Uninit() = 0;
};
// {AD8E7D84-4ABF-45b2-A16B-7F5B23344ACC}
MS_DEFINE_GUID(CLSID_CSendMessage,
			0xad8e7d84, 0x4abf, 0x45b2, 0xa1, 0x6b, 0x7f, 0x5b, 0x23, 0x34, 0x4a, 0xcc);


MS_DEFINE_IID(ISendMessage, "{41B19F13-8BFF-4f53-9BC8-7C6C737F86B6}");

struct IReceMessage : public IMessage
{
public:	
	virtual HRESULT Init(MESSAGEINFO* pMsg) = 0;
	virtual HRESULT Uninit() = 0;
};
// {1F26F496-502E-44be-8296-E0998CCEB3C4}
MS_DEFINE_GUID(CLSID_CReceMessage,
			0x1f26f496, 0x502e, 0x44be, 0x82, 0x96, 0xe0, 0x99, 0x8c, 0xce, 0xb3, 0xc4);


MS_DEFINE_IID(IReceMessage, "{854055C3-1CF3-432d-ACC4-834C52FF0DC9}");

//callback事件对象：通讯对象接收数据回调
struct IMessageCallBack : public IUnknown
{
public:
	virtual HRESULT Dispatch(IMessage* pMsg) = 0;
};

MS_DEFINE_IID(IMessageCallBack, "{45A31EDA-8B75-4621-9B49-8C2F134E166D}");


//客户通讯对象：抽象的通讯逻辑对象，直接提供给用户用于进程间的通讯使用
struct IClientObject : public IUnknown
{
public:
	//内部使用的
	virtual HRESULT InitClientInfo(CLIENTINFO* pci, IClientObjectManager* pClientObjectManager) = 0;
	virtual HRESULT GetClientInfo(CLIENTINFO* pci) = 0;
	virtual HRESULT SetCallbackMode(int imode, IMessageCallBack *pMessageCallBack) = 0;
	virtual IMessageCallBack* GetCallBack() = 0;

	//外部使用的
public:
	//目前所有的调用都是同步方式直接调用！因此 dwTimeout 目前没有使用，请大家注意了！
	// 注意返回值:
	//     （注意不是E_开头的返回值，E_开头的是一种错误）
	//     S_FALSE - 表示这次发送是尝试进行了，发送的动作进行了，只是没有发送成功（比如指定的对象不存在）
	//     S_OK    - 才是真正意义上的发送成功
	//     SendMessage时，只要有一个发送成功，就默认是发送成功了
	//注意：
	//		严禁用SendMessageEx给多个相同的defid与maskid对象发消息，因为ppOutData内容不确定
	virtual HRESULT SendMessage(DEFID defid, MASKID maskid, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout) = 0;
	virtual HRESULT SendMessageEx(DEFID defid, MASKID maskid, MSGID msgid, void* pInData, int nInCch, void** ppOutData, int* pOutCch, DWORD dwTimeout) = 0;
	
	virtual HRESULT BroadcastMessage(MASKID maskid, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout) = 0;
	virtual HRESULT BroadcastMessageEx(CLIENTINFO* pTargetInfo, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout) = 0;
	
	virtual HRESULT SendMessageTo(OBJID objid, MSGID msgid, void* pInData, int nInCch, DWORD dwTimeout) = 0;
	virtual HRESULT SendMessageToEx(OBJID objid, MSGID msgid, void* pInData, int nInCch, void** ppOutData, int* pOutCch, DWORD dwTimeout) = 0;
};
MS_DEFINE_IID(IClientObject, "{FBEB75A3-D0DC-4527-A61F-6895062BF36E}");
// {CC6554FE-5493-4ddd-A070-549E721B3496}
MS_DEFINE_GUID(CLSID_CClientObject,
			0xcc6554fe, 0x5493, 0x4ddd, 0xa0, 0x70, 0x54, 0x9e, 0x72, 0x1b, 0x34, 0x96);


//通讯中心主模块运行对象：主模块运行的实现
struct ICCenter : public IUnknown
{
public:
	//子系统初始化与启动
	virtual HRESULT Initilize(LPCSTR szSubsysName, LPCSTR szWorkPath = NULL) = 0;
	virtual HRESULT Uninitlize() = 0;
	virtual HRESULT Start(DWORD dwType = CCENTER_START_BOTH) = 0;
	virtual HRESULT Stop() = 0;

	//辅助功能接口
	virtual HRESULT GetSubsysID(int *pSubsysID) = 0;
	virtual ICCenterNamesManager* GetCCenterNamesManager() = 0;
	virtual IClientObjectManager* GetClientObjectManager() = 0;

	//注册、注销对象
	virtual IClientObject* Logon(DEFID defid, MASKID maskid, IMessageCallBack* pCallback, DWORD dwStyle = CREATE_COMM_OBJECT_MULTI) = 0;
	virtual IClientObject* LogonEx(DEFID defid, MASKID maskid, IMessageCallBack* pCallback, MSGID* enablemsgs, MSGID* disablemsgs, int imode, DWORD dwStyle = CREATE_COMM_OBJECT_MULTI) = 0;
	virtual HRESULT Logoff(IClientObject* pClient) = 0;

	//查询子系统内部的对象
	virtual HRESULT QueryFirstObject(HANDLE32 &handle) = 0;
	virtual HRESULT QueryNextObject(HANDLE32 &handle) = 0;
	virtual HRESULT QueryObject(HANDLE32 handle, CLIENTINFO* pClientInfo) = 0;

	virtual HRESULT QueryFirstObjectEx(HANDLE32 &handle, const CLIENTINFO* pClientInfo) = 0;
	virtual HRESULT QueryNextObjectEx(HANDLE32 &handle, const CLIENTINFO* pClientInfo) = 0;

	//commx3的内存管理
	virtual void* MS_Allocate(size_t size) = 0;
	virtual void MS_Free(void *pointer) = 0;
};

MS_DEFINE_IID(ICCenter, "{A3AFE7D3-4D43-47df-B493-BDA167DA7FD8}");
// {AFBB90B7-8C59-4a97-A022-3DA32D3BA8FF}
MS_DEFINE_GUID(CLSID_CCenter,
			0xafbb90b7, 0x8c59, 0x4a97, 0xa0, 0x22, 0x3d, 0xa3, 0x2d, 0x3b, 0xa8, 0xff);


//对象方式使用CommX3功能
struct IComX3 : public IUnknown
{
public:
	virtual ICCenter* Initialize(LPCSTR szSubsysName, LPCSTR szWorkPath = NULL, DWORD dwType = CCENTER_START_BOTH) = 0;
	virtual void Uninitialize(ICCenter *pCCenter) = 0;
	virtual void* Allocate(size_t size) = 0;
	virtual void FreeCallCenter(void *pointer) = 0;
};
MS_DEFINE_IID(IComX3, "{0DE3E95D-2DB6-4896-B1D7-F0CC5FB11282}");
// {A6B5E99A-6784-457e-A2F9-3E082BD76A64}
MS_DEFINE_GUID(CLSID_CComX3,
			0xa6b5e99a, 0x6784, 0x457e, 0xa2, 0xf9, 0x3e, 0x8, 0x2b, 0xd7, 0x6a, 0x64);


//API接口方式使用CommX3功能
extern "C" ICCenter* NPC_InitializeCallCenter(LPCSTR szSubsysName, LPCSTR szWorkPath = NULL, DWORD dwType = CCENTER_START_BOTH);
extern "C" void NPC_UninitializeCallCenter(ICCenter *pCCenter);
extern "C" void* NPC_AllocateCallCenter(size_t size);
extern "C" void NPC_FreeCallCenter(void *pointer);
extern "C" void NPC_ShutDown(DWORD dwParam = 0);

} //namespace msdk
