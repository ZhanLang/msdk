#pragma once

namespace DuiKit{;

//typedef 
//容器
struct IDuiContainerCtrl : public IDuiObject
{
	virtual VOID AddChild(IDuiControlCtrl* pChild) = 0;
	virtual VOID RemoveChild(IDuiControlCtrl* pChild, BOOL bDel = FALSE) = 0;

	virtual DWORD GetChildCount() = 0;
	virtual IDuiControlCtrl* GetChild(DWORD dwIndex) = 0;

	virtual IDuiControlCtrl* GetChild(LPCWSTR lpszName, BOOL hasSub = TRUE) = 0;

	//通过坐标寻找子控件
	virtual IDuiControlCtrl* GetChild(DWORD x, DWORD y,BOOL bFinalCtrl = TRUE) = 0;



	virtual CDuiRect GetInsetSize() = 0;
	virtual VOID SetInsetSize(const CDuiRect& InsetSize) = 0;

	virtual LPCWSTR GetGroup() = 0;
	virtual VOID SetGroup(LPCWSTR lpszGroupName) = 0;

	virtual BOOL InvalidateRect(const CDuiRect& rect) = 0;
	//向所有子窗口广播消息
	virtual VOID BroadCastMessage(IDuiControlCtrl* pSender, INT nMsg, INT nCtrt = IIDuiControlCtrl, WPARAM wParam = 0, LPARAM lParam = 0, LPVOID lpData = 0) = 0;
};

};//namespace DuiKit{;