#pragma once
namespace DuiKit{;


struct IDuiControlCtrl;
struct IDuiAnime;
struct IDuiCore;
struct IDuiSkin;
struct IDuiWindowCtrl;

//纵向布局方式
enum Valign
{
	ValignNull,
	ValignTop,		
	ValignCenter,
	ValignBottom,
};
enum Halign
{
	HalignNull,
	HalignLeft,
	HalignCenter,
	HalignRight,
};


struct IDuiControlCtrl : public IDuiObject
{
	virtual VOID SetDuiCore(IDuiCore* pCore) = 0;
	virtual IDuiCore* GetDuiCore() = 0;

	virtual VOID SetDuiSkin(IDuiSkin* pSkin) = 0;
	virtual IDuiSkin* GetDuiSkin() = 0;

	//virtual VOID DoInition

	virtual VOID AddPreMessageFilter(IDuiPreMessageFilter* pFilter) = 0;
	virtual VOID RemovePreMessageFilter(IDuiPreMessageFilter* pFilter) = 0;

	virtual VOID AddProMessageFilter(IDuiProMessageFilter* pFilter) = 0;
	virtual VOID RemoveProMessageFilter(IDuiProMessageFilter* pFilter) = 0;


	virtual LRESULT SendMessage(IDuiControlCtrl* CtrlTo,INT nMsg,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL) = 0;
	virtual BOOL PostMessage(IDuiControlCtrl* CtrlTo,INT nMsg,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL) = 0;


	virtual VOID AddRoutedEventHandler(IDuiRoutedEventHandler* RoutedEventHandler) = 0;
	virtual VOID RemoveRoutedEventHandler(IDuiRoutedEventHandler* RoutedEventHandler) = 0;
	
	//获取父控件
	virtual IDuiControlCtrl* GetParentCtrl() = 0;
	virtual VOID	 SetParentCtrl(IDuiControlCtrl* pCtrl) = 0;

	//获取拥有该控件的窗口
	virtual IDuiWindowCtrl* GetOwnerWindowCtrl() = 0;

	virtual BOOL SetTimer(UINT nTimerID, UINT uElapse) = 0;
	virtual BOOL KillTimer(UINT nTimerID) = 0;

	virtual VOID SetUserData(LPCWSTR lpszKey, LPCWSTR lpszValue) = 0;
	virtual LPCWSTR GetUserData(LPCWSTR lpszKey) = 0;

	virtual BOOL AddAnimation(IDuiAnime* pAnime) = 0;
	virtual BOOL RemoveAnimation(IDuiAnime* pAnime) = 0;

	virtual BOOL Invalidate() = 0;
	virtual BOOL IsInvalidate() = 0;
	virtual IDuiWindowCtrl* GetRootCtrl() = 0;

	//
	virtual DWORD GetWidth() = 0;
	virtual VOID  SetWidth(DWORD dwWidth) = 0;

	virtual DWORD GetHeight() = 0;
	virtual VOID SetHeight(DWORD dwHeight) = 0;

	virtual DWORD GetFixedWidth() = 0;
	virtual VOID SetFixedWidth(DWORD dwWidth) = 0;

	virtual DWORD GetFixedHeight() = 0;
	virtual VOID SetFixedHeight(DWORD dwHeight) = 0;

	virtual DWORD GetMaxWidth() = 0;
	virtual VOID  SetMaxWidth(DWORD dwWidth) = 0;

	virtual DWORD GetMaxHeight() = 0;
	virtual VOID SetMaxHeight(DWORD dwHeight) = 0;

	virtual DWORD GetMinWidth() = 0;
	virtual VOID  SetMinWidth(DWORD dwWidth) = 0;

	virtual DWORD GetMinHeight() = 0;
	virtual VOID SetMinHeight(DWORD dwHeight) = 0;

	virtual CDuiRect GetBorderSize() = 0;
	virtual VOID SetBorderSize(const CDuiRect& BorderSize) = 0;

	virtual VOID SetBorderRound(const CDuiSize& cxyRound) = 0;
	virtual CDuiSize GetBorderRound() = 0;

	virtual CDuiRect GetPadding() = 0;
	virtual VOID SetPadding(const CDuiRect& Padding) = 0;

	virtual DWORD GetControlFlags() = 0;
	//坐标
	virtual DWORD GetAxisX() = 0;
	virtual VOID  SetAxisX(DWORD dwAxis) = 0;
	virtual DWORD GetAxisY() = 0;
	virtual VOID  SetAxisY(DWORD dwAxis) = 0;
	virtual DWORD GetAxisZ() const = 0 ;
	virtual VOID  SetAxisZ(DWORD dwAxis) = 0;

	virtual Valign  GetValign() = 0;
	virtual VOID  SetValign(Valign pos) = 0;

	virtual Halign  GetHalign() = 0;
	virtual VOID  SetHalign(Halign pos) = 0;

	//绘制
	virtual VOID  SetBkImage(LPCWSTR lpszBkImage) = 0;
	virtual LPCWSTR GetBkImage() = 0;

	virtual VOID SetBkColor(DWORD dwBkColor) = 0;
	virtual DWORD GetBkColor() = 0;

	virtual LPCWSTR GetName() = 0;
	virtual VOID SetName(LPCWSTR lpszName) = 0;


	virtual VOID SetVisable(BOOL bVisable) = 0;
	virtual BOOL GetVisable() = 0;

	virtual LPCWSTR GetNameSpace() = 0;
	virtual VOID SetNameSpace(LPCWSTR lpszName) = 0;

	//////////////////////////////////////////////////////////////////////////
	
	//该坐标相对于父窗口
	virtual VOID SetPos(const CDuiRect& rcPos) = 0;
	virtual CDuiRect& GetPos() = 0;

	//获取相对于根窗口的坐标
	//virtual CDuiRect GetAbsolutePos() = 0;
};


};//namespace DuiKit{;
/*
控件对象接口，每个控件都应实现该接口
*/

