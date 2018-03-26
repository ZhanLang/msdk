
#ifndef _MSPLUGIN_TRAYSRV_H_
#define _MSPLUGIN_TRAYSRV_H_

#include <mscom/msbase.h>

namespace msdk {

//TraySrv提供的连接点服务
interface ITrayMsgConnectPoint : public IMSBase	//响应托盘消息
{
	/*lParam:
		WM_RBUTTONDOWN
		WM_LBUTTONDBLCLK
		WM_RBUTTONDBLCLK
		WM_MOUSEMOVE
		WM_LBUTTONDOWN
		WM_MOUSELEAVE
		WM_MOUSEHOVER
	*/
	STDMETHOD_(LRESULT, OnTrayIconMsg)(LPARAM lParam, INT x, INT y, BOOL& bHandle) = 0;	//托盘图标消息
	STDMETHOD_(LRESULT, OnBalloonClicked)(UINT nActionID, BOOL& bHandle)=0;				//气球被点击
	STDMETHOD_(LRESULT, OnOtherMsg)(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle) = 0; //其他消息
};
MS_DEFINE_IID(ITrayMsgConnectPoint, "{CA2DFAFD-8E48-4588-B6FC-1300BC2A4AEA}");

interface ITrayMenuConnectPoint : public IMSBase	//响应托盘消息
{
	STDMETHOD_(LRESULT, OnShowLeftButtonMenu)(INT x, INT y, BOOL& bHandle) = 0;			//左键菜单
	STDMETHOD_(LRESULT, OnShowRightButtonMenu)(INT x, INT y, BOOL& bHandle) = 0;		//右键菜单
	STDMETHOD_(LRESULT, OnMenuCommand)(WORD nMenuId, BOOL &bHandle) = 0;
	STDMETHOD_(LRESULT, OnMenuCommand)(UINT nMenuIndex, HMENU hMenu, BOOL &bHandle) = 0;
	STDMETHOD_(LRESULT, OnDefaultMenu)(BOOL &bHandle) = 0;	//左键双击响应默认菜单
};
MS_DEFINE_IID(ITrayMenuConnectPoint, "{8C649637-5E05-4c06-87F9-D1778BC7B239}");

//////////////////////////////////////////////////////////////////////////
interface ITraySrv;
interface ITraySrvPlugin : public IMSBase
{
	STDMETHOD(SetTraySrv)(ITraySrv* pTraySrv) = 0;  //这个方法仅仅给TraySrv组件用
	STDMETHOD(GetTraySrv)(ITraySrv** pTraySrv) = 0; //其被ITraySrvPlugin组件使用的组件通过这个方法拿到ITraySrv
};
MS_DEFINE_IID(ITraySrvPlugin, "{087EA29A-7559-42e6-8D32-14317E650AF8}");

//****************************************************************************************
// 气泡提示的图标样式，供函数PopupTip的第三个参数使用
#define	TRAYSRV_POPTIPSTYLE_INFORMATION 0x00000001	// 叹号
#define	TRAYSRV_POPTIPSTYLE_WARNING     0x00000002	// 警告(黄)
#define	TRAYSRV_POPTIPSTYLE_ERROR       0x00000003	// 错误(红)

interface ITraySrv : public IMSBase	//托盘服务
{
	//InitTray的clsOnTrayMsg组件应该是要实现ITrayMsg方法
	STDMETHOD(CreateTray)(UINT nMsgID, UINT nUID, CLSID clsOnTrayMsg/* = ClSID_CTrayMenuBase*/) = 0;
	STDMETHOD(CreateTray)(UINT nMsgID, UINT nUID, ITraySrvPlugin* pTrayMsg = NULL) = 0;
	STDMETHOD(DestroyTray)() = 0;
	STDMETHOD(GetTraySrvPlugin)(ITraySrvPlugin** pTrayMsg) = 0;
	
	//bOfficial:表明是否是正式状态，ResetTray就是会回到最后一个标识bOfficial的状态
	STDMETHOD(UpdateTray)(LPCTSTR lpcszIcon, LPCTSTR lpcszTips, BOOL bOfficial = TRUE) = 0;
	STDMETHOD(UpdateTray)(HICON hIcon, LPCTSTR lpcszTips, BOOL bOfficial = TRUE) = 0;

	STDMETHOD(UpdateTray)(UINT nIconResId, LPCTSTR lpcszTips, BOOL bOfficial, IN HINSTANCE hInstance/* = NULL*/) = 0;
	STDMETHOD(ShowTray)(bool bShow = TRUE) = 0;

	// 显示Tray气球信息
	STDMETHOD(ShowBalloon)(LPCTSTR lpcszTips, LPCTSTR lpcszTitle = NULL, BOOL bSynchronous = FALSE
		, UINT uTimeout = 5000, UINT uStyle=TRAYSRV_POPTIPSTYLE_INFORMATION, UINT nActionID=0) = 0;

	//托盘闪动，nTimerElapse闪动间隔，0：停止闪动
	STDMETHOD(FlickerTray)(UINT nTimerElapse = 0) = 0;
	STDMETHOD_(BOOL, IsFlicker)() = 0;

	//托盘复位，回到之前最后设定的稳定状态
	STDMETHOD(ResetTray)() = 0;
};
MS_DEFINE_IID(ITraySrv, "{0C5CEB28-9317-416d-A4EC-DF7C0ADA62C8}");

// {4FCE6281-8849-4fc6-A764-95C793EB8A48}
MS_DEFINE_GUID(ClSID_CTraySrv, 
			0x4fce6281, 0x8849, 0x4fc6, 0xa7, 0x64, 0x95, 0xc7, 0x93, 0xeb, 0x8a, 0x48);

} //namespace msdk

#endif	//_MSPLUGIN_TRAYSRV_H_