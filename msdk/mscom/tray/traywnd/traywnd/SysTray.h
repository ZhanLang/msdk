#if !defined(AFX_SYSTRAY_H__0D3E266A_3299_49EA_B650_9B01CC628D94__INCLUDED_)
#define AFX_SYSTRAY_H__0D3E266A_3299_49EA_B650_9B01CC628D94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef  _WIN32_IE
#define _WIN32_IE 0x0600
#include <shellapi.h>
#include <shlwapi.h>
#include <vector>


#define TRAY_ACTION_NONE				0
#define TRAY_ACTION_FILEMON_CLOSED		1
#define TRAY_ACTION_WILL_QUIT			2
#define TRAY_ACTION_FULL_SCAN			3


typedef struct
{
	UINT nAction;
	UINT Style;
	TCHAR szTipTitle[64];
	TCHAR szTipInfo[256];
	UINT uTimeout;
} ST_BALLOONTIP;

using namespace std;
typedef vector<ST_BALLOONTIP> BALLOON_TIPS;


const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(TEXT("TaskbarCreated"));

/* 
	注意使用本类时要在预编译头文件中（一般为StdAfx.h）
	加入#define _WIN32_IE 0x0600，否则编译可能不通过				*/


//#define	S_IGNORE			(MAKE_HRESULT(0, FACILITY_NULL, 1))
//////////////////////////////////////////////////////////////////////
//
//	接       口:	ITrayEvents
//	功 能 描 述:	接口，提供对系统托盘产生事件的处理。
//
//////////////////////////////////////////////////////////////////////
class ITrayEvents
{
public:
	/*		以下函数如果不想对其做任何处理则直接返回S_IGNORE		*/
	/*		否则处理完毕请返回S_OK									*/
	virtual LRESULT OnTrayIconMsg(LPARAM lParam, INT x, INT y, BOOL &bHandle) = 0;	//托盘图标消息
	virtual LRESULT OnShowLeftButtonMenu(INT x, INT y, BOOL &bHandle) = 0;			//左键菜单
	virtual LRESULT OnShowRightButtonMenu(INT x, INT y, BOOL &bHandle) = 0;		//右键菜单
	virtual LRESULT OnBalloonClicked(UINT nActionID, BOOL &bHandle)=0;				//气球被点击
	virtual LRESULT OnOtherMsg(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle) = 0; //其他消息
};


// 气泡提示的图标样式，供函数PopupTip的第三个参数使用
#define	POPTIPSTYLE_INFORMATION 0x00000001	// 叹号
#define	POPTIPSTYLE_WARNING     0x00000002	// 警告(黄)
#define	POPTIPSTYLE_ERROR       0x00000003	// 错误(红)


//////////////////////////////////////////////////////////////////////
//
//	类		 名:	CSysTray
//  基		 类:    N/A
//	功 能 描 述:	提供系统托盘相关操作。
//  代 码 编 写:	
//  完 成 日 期:	2010/10/27
//
//////////////////////////////////////////////////////////////////////
class CSysTray  
{
public:
	// Methods
	CSysTray(HINSTANCE hInst = NULL, UINT uEmptyIconId = 0);
	virtual ~CSysTray();
	
	// 创建Tray并维护相应结构
	virtual BOOL CreateTray(HWND hOwnerWnd, UINT nMsgID, UINT nUID, ITrayEvents *pEvents=NULL);

	virtual LRESULT WinProc(UINT, WPARAM, LPARAM, BOOL &bHandle);
	// 销毁Tray
	virtual VOID Destroy();	

	// 设置图标和提示
	virtual BOOL UpdateTray(UINT nIcon, LPCTSTR lpcszTips, BOOL bOfficial, IN HINSTANCE = NULL);

	// 设置图标和提示(for path)
	virtual BOOL UpdateTray(LPCTSTR lpcszIcon, LPCTSTR lpcszTips, BOOL bOfficial);

	// 设置图标和提示(for HICON)
	virtual BOOL UpdateTray(HICON, LPCTSTR lpcszTips, BOOL bOfficial, BOOL bLoadIcon = FALSE);

	// 显示Tray气球信息(异步方式)
	virtual BOOL ShowBalloon(LPCTSTR lpcszTips, LPCTSTR lpcszTitle=NULL, UINT uTimeout = 5000, UINT uStyle=POPTIPSTYLE_INFORMATION, UINT nActionID=TRAY_ACTION_NONE);

	// 显示Tray气球信息(同步方式)
	virtual BOOL ShowBalloonimmediately(LPCTSTR lpcszTips, LPCTSTR lpcszTitle=NULL, UINT uTimeout = 5000, UINT uStyle =POPTIPSTYLE_INFORMATION, UINT nActionID =TRAY_ACTION_NONE);

	//托盘闪动，nTimerElapse闪动间隔，0：停止闪动
	virtual BOOL FlickerTray(UINT nTimerElapse);

	virtual BOOL IsFlicker();

	//托盘复位，回到之前最后设定的稳定状态
	virtual BOOL ResetTray();

	// Tip的最大容量(字节)
	virtual int GetTipCap();

	virtual BOOL ShowTray(bool bShow = TRUE);

	HWND GetWnd() {return m_hHideWnd;};

	
private:
	// Methods

	// 检查操作系统的某个DLL的主版本号(将来可放入公共函数库中?)
	UINT CheckSysDllMajorVersion(LPCTSTR lpcszSystemDllFilename);

	// 检查并删除已经添加过的ICON
	VOID DelIcon();

	// 在vector中取得一个气球提示内容并显示
	VOID PopBalloon(BALLOON_TIPS& vtBalloonTips);
	
	// 检测Explorer怎么了?(输出LOG)
	VOID AnalyzeExplorer();

	void FlickerTray(BOOL bInit = FALSE);

	// Attributes
	enum
	{
		ICONTYPE_EMPTY = 0,
		ICONTYPE_ICON = 1,
		ICONTYPE_ICONPATH,
		ICONTYPE_ICONID,
	};
	struct ST_OfficialInfo
	{
		TCHAR	szTip[128];
		UINT	iconType;
		HICON	hIcon;
		TCHAR	szIconPath[MAX_PATH*2];
		UINT	iconId;
	};
	//////////////////////////////////////////////////////////////////////////
	HINSTANCE m_hInst;
	UINT m_uEmptyIconId;

	HWND			m_hHideWnd;			// 隐藏窗体,用于接收explorer的icon消息
	HWND			m_hOwnerWnd;		// 外部希望处理Tray消息的窗体
	HICON			m_hEmpty;			//空的那个图标
	BOOL			m_bIsFlicker;
	BOOL			m_bHideIcon;		//隐藏托盘图标

	BOOL			m_bLoadIcon;
	NOTIFYICONDATA	m_stNID;			// 操作Tray使用的结构
	NOTIFYICONDATA	m_stEmptyNID;		// 操作Tray使用的结构
	ST_OfficialInfo	m_stSaveNID;		// 记录最后一个正规tray信息

	ITrayEvents	   *m_pEvents;			// 保存Tray事件处理对象指针
	UINT			m_nMsgID;			// 保存Tray的消息ID
	UINT			SHELL_VEMSION;		// 保存Shell32.dll的版本

	BOOL			m_bHoldRButton;		// 鼠标右键是否按下(辅助判断右键单击事件)

	BALLOON_TIPS	m_vtBalloonTips;	// BalloonTips
	ST_BALLOONTIP	m_stCurTip;
	BOOL			m_bBalloonShowing;


	BOOL			m_bSetMouseLeaveTimer;
	POINT			m_ptMouse;

	BOOL			m_bNotifyMouseHover;
	DWORD			m_dwMouseHoverCount;  //时间计数
};

#endif // !defined(AFX_SYSTRAY_H__0D3E266A_3299_49EA_B650_9B01CC628D94__INCLUDED_)
