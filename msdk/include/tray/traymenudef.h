
#ifndef _MS_TRAYMENUDEF_H_
#define _MS_TRAYMENUDEF_H_

#include <mscom/msbase.h>

namespace msdk {

// {FCA0E62A-5DD4-46fb-AFB2-BDC74EA7DB36} //提供了一个默认的托盘消息一次处理器
MS_DEFINE_GUID(ClSID_CTrayMenuBase,
			   0xfca0e62a, 0x5dd4, 0x46fb, 0xaf, 0xb2, 0xbd, 0xc7, 0x4e, 0xa7, 0xdb, 0x36);

typedef struct _MENUCMD_INFO	//参考 CMINVOKECOMMANDINFO
{
	DWORD	cbSize;
	HMENU	hMenu;
	UINT	uMenuItem;
	UINT	wMenuID;
}MENUCMD_INFO;
typedef MENUCMD_INFO *LPMENUCMD_INFO;

enum
{
	MenuType_RightButton = 0,
	MenuType_LeftButton,
};

enum
{
	MenuFlag_Normal = 0,	//普通菜单项
	MenuFlag_DefaultOnly,	//默认菜单项
};

//实现ITrayMsg的组件支持的连接点
//菜单处理
interface ITrayMenuBaseConnectPoint : public IMSBase	//响应菜单
{
	STDMETHOD_(UINT, QueryMenu)(	//返回占用菜单ID的区间数
		HMENU hMenu,				//菜单
		UINT indexMenu,				//当前可添加菜单的位置
		UINT idCmdFirst,			//当前可用菜单ID数
		UINT uFlags,				//添加的菜单项标识 MenuFlag_Normal | MenuFlag_DefaultOnly
		UINT uMenuType) = 0;		//菜单类型（左键还是右键）	 MenuType_RightButton | MenuType_LeftButton

	STDMETHOD(InvokeMenuCmd)(
		LPMENUCMD_INFO lpmci) = 0;	//响应菜单消息
};
MS_DEFINE_IID(ITrayMenuBaseConnectPoint, "{A450CD33-A022-4a2f-B815-A7A2080C7FEA}");

} //namespace msdk

#endif	//_MS_TRAYMENUDEF_H_