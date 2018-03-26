// srvclientinfo.h

#if !defined(_INCLUDE_SRVCLIENTINFO_H__)
#define _INCLUDE_SRVCLIENTINFO_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

namespace msdk {

enum
{
	CLIENT_INFO_MSGS_MAX = 16,
	SERVER_RPC_NAME_MAX = 64,
	SUB_SYSTEM_NAME_MAX = 8,
};

//创建通讯对象时，是只允许创建单个不重名的对象，还是允许创建多个重名的对象
enum
{
	CREATE_COMM_OBJECT_MULTI = 0,
	CREATE_COMM_OBJECT_SINGLE,
};

//初始化对象的类型
enum
{	
	CCENTER_START_CLIENT = 0x1,
	CCENTER_START_SERVER = 0x2,
	CCENTER_START_BOTH = CCENTER_START_CLIENT|CCENTER_START_SERVER,
};

//对象定义说明：
//1、defid和maskid用来定义一个完整的对象，而objid是在对象成功创建时，立即自动生成的随机整数
//2、maskid为按位的集合id，也就是说相同的按位的集合的defid应该都是不同的
//3、相同的defid那么maskid不能相同，也就是他们不能同时存在一个子系统中，但是允许属于多个集合（因为是按位定义的）
enum
{
	CI_MASK_EMPTY =		0x0,
	CI_MASK_DEFID =		0x0001,
	CI_MASK_MASKID =	0x0002,
	CI_MASK_TYPEID =	0x0004,
	CI_MASK_OBJID =		0x0008,
	CI_MASK_POBJID =	0x0010,
	CI_MASK_TID =		0x0020,
	CI_MASK_PID =		0x0040,
	CI_MASK_SID =		0x0080,
	CI_MASK_HID =		0x0100,
	CI_MASK_PMCB =		0x0200,
	CI_MASK_EMSGID =	0x0400,
	CI_MASK_DMSGID =	0x0800,

	SI_MASK_SUBSYSID =	0x1000,
	SI_MASK_INTERID =	0x2000,
	SI_MASK_SERVERNAME =0x4000,

	CI_MASK_NORMALID = CI_MASK_DEFID|CI_MASK_MASKID|SI_MASK_SUBSYSID|CI_MASK_EMSGID|CI_MASK_DMSGID,
	CI_MASK_GROUPID =				 CI_MASK_MASKID|SI_MASK_SUBSYSID|CI_MASK_EMSGID|CI_MASK_DMSGID,
	CI_MASK_EXTENDID =				  CI_MASK_OBJID|SI_MASK_SUBSYSID|CI_MASK_EMSGID|CI_MASK_DMSGID,	

	CI_MASK_FULL =		0xffffffff,
};

enum
{
	CALL_TYPE_INPROCESS =	0x1,	//进程内
	CALL_TYPE_INHOST =		0x2,	//本机
	CALL_TYPE_INNET =		0x4,	//网络

	CALL_TYPE_SYNC =		0x10,	//同步
	CALL_TYPE_NSYS =		0x20,	//异步
};

enum
{
	MASK_ALL_GROUP_OBJECTS =			0xffffffff,

	DEFINE_ALL_OBJECTS =				-1L,					//属于内部定义的对象：代表所有对象（例如包括maskid=0）
};

typedef __int32 MSGID;
typedef __int32 DEFID;
typedef DEFID MASKID;
typedef __int32 OBJID;
typedef __int32 SUBSYSID;
typedef __int32 INTERID;

typedef struct st_Client_Info
{
	DWORD dwSize;
	DWORD dwMask;
	DWORD dwType;
	DEFID defid;
	MASKID maskid;
	SUBSYSID subsysid;
	OBJID objid;
	OBJID pobjid;	
	__int32 tid;
	__int32 pid;
	__int32 sid;
	__int32 hid;
	__int32 pMessageCallBack;//LPVOID
	MSGID enablemsgs[CLIENT_INFO_MSGS_MAX];
	MSGID disablemsgs[CLIENT_INFO_MSGS_MAX];	
}CLIENTINFO;
inline bool operator==(const CLIENTINFO& _X,	const CLIENTINFO& _Y)
{
	DWORD dwMask = _X.dwMask;

	bool rt = true;
	if(DEFINE_ALL_OBJECTS==_X.defid)//DEFINE_ALL_OBJECTS表示所有对象
	{
		rt &= true;
		if(0==_Y.maskid)//如果_Y为0，不属于任何组，这样就不能再比较组了。
		{
			dwMask &= ~CI_MASK_MASKID;
		}
	}
	else
	{
		if(CI_MASK_DEFID & dwMask)
			rt &= (_X.defid==_Y.defid);
	}	
	if(CI_MASK_MASKID & dwMask)
	{
		if(_X.maskid!=_Y.maskid)
			rt &= (0!=(_X.maskid&_Y.maskid));
	}
	/* 不同的子系统在不同的共享内存文件中，所以现在应该是都相同的
	if(SI_MASK_SUBSYSID & dwMask)
		rt &= (_X.subsysid==_Y.subsysid);
	*/
	if(CI_MASK_TYPEID & dwMask)//目标对象是否有服务启用
		rt &= (0!=(CCENTER_START_SERVER&_Y.dwType));

	if(CI_MASK_OBJID & dwMask)
		rt &= (_X.objid==_Y.objid);

	if(CI_MASK_POBJID & dwMask)
		rt &= (_X.pobjid==_Y.pobjid);

	if(CI_MASK_PID & dwMask)
		rt &= (_X.pid==_Y.pid);

	if(CI_MASK_SID & dwMask)
		rt &= (_X.sid==_Y.sid);

	if(CI_MASK_HID & dwMask)
		rt &= (_X.hid==_Y.hid);
	
	return rt;
}

typedef struct st_Server_Info
{
	DWORD dwSize;
	DWORD dwMask;
	DWORD dwType;
	__int32 pid;
	SUBSYSID subsysid;
	INTERID interid;
	OBJID pobjid;
	CHAR ServerName[SERVER_RPC_NAME_MAX];
}SERVERINFO;
inline bool operator==(const SERVERINFO& _X,	const SERVERINFO& _Y)
{
	DWORD dwMask = _X.dwMask;	

	bool rt = true;
	if(CI_MASK_TYPEID & dwMask)//目标对象是否有服务启用
		rt &= (0!=(CCENTER_START_SERVER&_Y.dwType));
	if(CI_MASK_PID & dwMask)
		rt &= (_X.pid==_Y.pid);
	if(CI_MASK_POBJID & dwMask)
		rt &= (_X.pobjid==_Y.pobjid);
	if(SI_MASK_SUBSYSID & dwMask)
		rt &= (_X.subsysid==_Y.subsysid);
	if(SI_MASK_INTERID & dwMask)
		rt &= (_X.interid==_Y.interid);
	return rt;
}

typedef struct st_Message_Info
{
	DWORD dwSize;
	DWORD dwCallType;
	DWORD dwTimeout;	

	DEFID source_defid;
	MASKID source_maskid;
	OBJID source_objid;

	DEFID defid;
	MASKID maskid;
	OBJID objid;

	SUBSYSID subsysid;

	MSGID msgid;
	byte* pInData;
	__int32 nInCch;
	byte** ppOutData;
	__int32* pOutCch;
}MESSAGEINFO;

struct ISendMessage;
struct IReceMessage;
struct IMessageCallBack;
struct IClientObjectManager;
struct IClientObjectNames;
struct IMessageQuene;
struct IThreadPools;
struct ICCenter;
struct IServer;
struct ICCenterObject;
struct ICCenterNamesManager;
struct ICommInProcess;
struct ICommInHost;
struct IClientObject;

}	//namespace msdk
#endif // !defined(_INCLUDE_SRVCLIENTINFO_H__)
