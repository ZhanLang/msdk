#pragma once

#include "resource.h"
#include <atlctrls.h>
#include <atlframe.h>
#include "atlgdix.h"
#include "nfresize.h"
#define __ATLGDIX_H__
#include "atlgdi.h"

#include "download/IDownload.h"
#include <vector>
#include "safelist.h"

#include "mscom/SrvBaseImp.h"
#include <map>

struct DownLoadMsgInfo
{
	DWORD dwTaskId;
	DL_NotifyEvnet msgid;
	WPARAM wp;
	LPARAM lp;
};

#define DOWNLOAD_TASK_MESSAGE (WM_USER + 1000)

class CBVDownloadWindow:
	public IDownloadNotify,
	public IExit,
	public IMsPlugin,
	public IMsPluginRun,
	public CAxDialogImpl<CBVDownloadWindow>,
	public CNoFlickerDialogResize<CBVDownloadWindow>,
	public CUpdateUI<CBVDownloadWindow>,
	public CMessageFilter, 
	public CIdleHandler,
	public CMsComBase<CBVDownloadWindow>
{
	public:
		CBVDownloadWindow();
		~CBVDownloadWindow();
	enum{IDD = IDD_MAIN_FRAME};

	UNKNOWN_IMP4_(IDownloadNotify, IMsPluginRun, IMsPlugin, IExit);
	BEGIN_MSG_MAP(CBVDownloadWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog);
		MESSAGE_HANDLER(WM_CLOSE, OnClose);
		MESSAGE_HANDLER(WM_SIZE, OnSize);
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(DOWNLOAD_TASK_MESSAGE,OnDownLoadMsg);
		COMMAND_HANDLER(IDC_StartAll_Btn,BN_CLICKED,OnStartAllTask);
		COMMAND_HANDLER(IDC_StopAll_Btn,BN_CLICKED,OnStopAllTask);
		NOTIFY_HANDLER(IDC_DL_TASK_MGR_LIST, NM_RCLICK, OnNMRclickListLeak);

		MESSAGE_HANDLER(WM_CHANGECBCHAIN , OnChangeCBChain)
		MESSAGE_HANDLER(WM_DRAWCLIPBOARD , OnDrawClipBoard)

		CHAIN_MSG_MAP(CUpdateUI<CBVDownloadWindow>)
		CHAIN_MSG_MAP(CNoFlickerDialogResize<CBVDownloadWindow>)
	END_MSG_MAP();


	BEGIN_DLGRESIZE_MAP(CBVDownloadWindow)
		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL(IDC_DLTASKMGR_DLG_STATIC, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		END_DLGRESIZE_GROUP()

		DLGRESIZE_CONTROL(IDC_DL_TASK_MGR_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_GLOBAL_STATIC,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_StartAll_Btn, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_StopAll_Btn,DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_DelAll_Btn,DLSZ_MOVE_X|DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_UPDATE_UI_MAP(CBVDownloadWindow)
	END_UPDATE_UI_MAP()
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	LRESULT OnStartAllTask(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStopAllTask(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	

	LRESULT OnCommand(UINT , WPARAM wParam, LPARAM , BOOL& bHandled);
	LRESULT OnTimer(UINT , WPARAM wParam, LPARAM , BOOL& bHandled);
	LRESULT OnDownLoadMsg(UINT , WPARAM wParam, LPARAM , BOOL& bHandled);
	LRESULT OnNMRclickListLeak(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnMenuCommand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnChangeCBChain(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawClipBoard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


protected:
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

protected:
	STDMETHOD(OnDownloadNotify)(DL_NotifyEvnet notifyType, DWORD dwTaskId);
	
protected:
	STDMETHOD(NotifyExit)(bool* bExit = NULL);
private:
	HRESULT InitDLTaskMgrListCtr();
	HRESULT RefreshTask();
	DWORD   GetTaskIndex(DWORD dwTaskID);
	HRESULT	InertTaskItem(DWORD dwTaskID);
	HRESULT CreateDownloadTask(LPCTSTR lpUrl,LPCTSTR lpsavePath,BOOL bAutoStart=FALSE);
private:
	CListViewCtrl m_dlTaskMgrListControl;
	//CMSComLoader m_dlHelper;
	int					m_currentSelect;
	UTIL::com_ptr<IDownloadMgr> m_pDownloadMgr;
	typedef std::map<DWORD, DWORD>	CTaskMap;
	CTaskMap m_downloadTasks;
	msdk::CSafeList<DownLoadMsgInfo>  m_SafeList;
	HWND m_NextClipWnd;
};



// {C50E17A8-85B7-4C83-B0B1-EE19CBB81662}
MS_DEFINE_GUID(CLSID_BvDownload, 
	0xc50e17a8, 0x85b7, 0x4c83, 0xb0, 0xb1, 0xee, 0x19, 0xcb, 0xb8, 0x16, 0x62);
