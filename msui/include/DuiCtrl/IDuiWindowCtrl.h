
#pragma once

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW )
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)

namespace DuiKit{;

struct IDuiWindowCtrl : public IDuiObject
{

	// Ù–‘
	virtual INT GetTransparent() = 0;
	virtual VOID SetTransparent(INT nOpacity) = 0;

	virtual HWND GetHWND() = 0;
	virtual HDC  GetHDC() = 0;
	virtual HWND Subclass(HWND hWnd) = 0;
	virtual VOID Unsubclass() = 0;

	virtual HWND Create(
						HWND hwndParent, 
						LPCTSTR pstrName, 
						LPCWSTR lpszWindowClassName,
						LPCWSTR lpszSuperClassName = NULL,
						DWORD	dwClassStyle = CS_DBLCLKS,
						int x = CW_USEDEFAULT, 
						int y = CW_USEDEFAULT, 
						int cx = CW_USEDEFAULT, 
						int cy = CW_USEDEFAULT, 
						HMENU hMenu = NULL
						) = 0;



	virtual DWORD DoModal() = 0;
	virtual VOID Close(DWORD dwExitCode) = 0;

	virtual LPCWSTR GetSuperClassName() = 0;
	virtual LPCWSTR GetWindowClassName() = 0;

	virtual DWORD GetStyle() = 0;
	virtual DWORD GetStyleEx() = 0;

	virtual VOID SetStyle(DWORD dwClassStyle) = 0;
	virtual VOID SetStyleEx(DWORD dwClassStyleEx) = 0;

	virtual VOID CenterWindow() = 0;


	virtual BOOL SetTimer(IDuiControlCtrl* Ctrl, UINT nTimerID, UINT uElapse) = 0;
	virtual BOOL KillTimer(IDuiControlCtrl* pControl, UINT nTimerID) = 0;
	virtual VOID KillTimer(IDuiControlCtrl* pControl) = 0;
	virtual VOID RemoveAllTimers() = 0;


	virtual VOID SetSizeBox(const CDuiRect& sizeBox) = 0;
	virtual CDuiRect GetSizeBox() = 0;

	virtual VOID SetCaptionRect(const CDuiRect& caption) = 0;
	virtual CDuiRect GetCaptionRect() = 0;

	virtual VOID SetRoundCorner(const CDuiSize& corner ) = 0;
	virtual CDuiSize GetRoundCorner( ) = 0;

	virtual VOID SetBackgroundTransparent(BOOL bTrans) = 0;
	virtual BOOL GetBackgroundTransparent() = 0;

	virtual VOID SetShowDirtyRect(BOOL bShowDirty) = 0;
	virtual BOOL GetShowDirtyRect() = 0;

	
	virtual BOOL MapPoint(IDuiWindowCtrl* WndCtrl, CDuiPoint& point) = 0;
	virtual BOOL MapRect(IDuiWindowCtrl* WndCtrl, CDuiRect& rect) = 0;

};

};//namespace DuiKit{;