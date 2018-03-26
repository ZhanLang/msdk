// namespacetables.h

#pragma once
#include "srvclientinfo.h"

namespace msdk {

#define FIND_INVALID_INDEX				(__int32(-1))

typedef __int32 HANDLE32;

//本机进程名字管理器：真正的名字的增删查改
struct ICCenterNames : public IMSBase
{
public:
	virtual HRESULT Open(LPCSTR szName) = 0;
	virtual HRESULT Close() = 0;
	virtual HRESULT FlushData() = 0;

	virtual HRESULT BeginTrans() = 0;
	virtual HRESULT EndTrans() = 0;
	//通过pid查找存在的ServerName并返回S_OK，没有则返回S_FALSE
	virtual HRESULT Find(SERVERINFO* psi) = 0;
	//查找(通过pid)存在的ServerName，重复了根据是否强制覆盖决定立即返回E_ADD_EXIST_ONE，或者覆盖老的记录
	virtual HRESULT Add(SERVERINFO* psi, BOOL bForce) = 0;
	//
	virtual HRESULT Delete(SERVERINFO* psi) = 0;
};

MS_DEFINE_IID(ICCenterNames, "{503FFDE3-C990-4d66-B5EC-716AA57194EE}");
// {99B11F35-D7E4-46af-B1C3-90CE5ED5E871}
MS_DEFINE_GUID(CLSID_CCenterNames, 
			0x99b11f35, 0xd7e4, 0x46af, 0xb1, 0xc3, 0x90, 0xce, 0x5e, 0xd5, 0xe8, 0x71);



//本机通讯对象名字管理器：真正的名字的增删查改
struct IClientObjectNames : public IMSBase
{
public:
	virtual HRESULT Open(LPCSTR szName) = 0;
	virtual HRESULT Close() = 0;
	virtual HRESULT FlushData() = 0;
	
	virtual HRESULT BeginTrans() = 0;
	virtual HRESULT EndTrans() = 0;
	//查询是否存在，如果已经有一个则返回存在的objid并返回S_OK，没有则返回S_FALSE
	virtual HRESULT Find(CLIENTINFO* pci) = 0;
	//查询是否存在，如果已经有一个则返回存在的objid并返回E_ADDEXISTONE，没有则添加一个新的名字并返回新的objid返回成功
	virtual HRESULT Add(CLIENTINFO* pci, BOOL bForce) = 0;
	//删除指定的对象
	virtual HRESULT Delete(CLIENTINFO* pci) = 0;

	//遍历
	virtual HRESULT QueryFirstObject(HANDLE32 &handle) = 0;
	virtual HRESULT QueryNextObject(HANDLE32 &handle) = 0;
	virtual HRESULT QueryObject(HANDLE32 handle, CLIENTINFO* pClientInfo) = 0;
};

MS_DEFINE_IID(IClientObjectNames, "{A1E0780D-BF48-44ed-9065-27BC15DB6608}");
// {09923979-75D7-406c-96D7-59BBEA638B3A}
MS_DEFINE_GUID(CLSID_CClientObjectNames, 
			0x9923979, 0x75d7, 0x406c, 0x96, 0xd7, 0x59, 0xbb, 0xea, 0x63, 0x8b, 0x3a);

} //namespace msdk

