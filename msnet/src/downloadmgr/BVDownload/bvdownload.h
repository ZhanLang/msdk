#pragma once

#include "resource.h"
#include <atlctrls.h>
#include <atlframe.h>
#include "atlgdix.h"
#include "nfresize.h"
#define __ATLGDIX_H__
#include "atlgdi.h"

#include "download/usedownloadmgr.h"
#include <vector>
#include "safelist.h"

#define RefreshMsg WM_USER+100
struct DownloadTaskItemInfo
{
	sha1_hash hash;
	UINT64	  createTime;
};

struct lessTaskItem
{
	bool operator()(const DownloadTaskItemInfo& r,const DownloadTaskItemInfo& l) const
	{
		return r.createTime < l.createTime;
	}
};

struct DownLoadMsgInfo
{
	sha1_hash hash;
	NotifyMessageID msgid;
	WPARAM wp;
	LPARAM lp;
};

class CBVDownloadWindow:
	public CAxDialogImpl<CBVDownloadWindow>,
	public CNoFlickerDialogResize<CBVDownloadWindow>,
	public CUpdateUI<CBVDownloadWindow>,
	public CMessageFilter, 
	public CIdleHandler,
	public IDownLoadNotify,
	public IEnumDownLoadTask
{
	public:
		CBVDownloadWindow();
		~CBVDownloadWindow();
	enum{IDD = IDD_MAIN_FRAME};
	BEGIN_MSG_MAP(CBVDownloadWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog);
		MESSAGE_HANDLER(WM_CLOSE, OnClose);
		MESSAGE_HANDLER(WM_SIZE, OnSize);
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(RefreshMsg,OnDownLoadMsg);
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
	LRESULT OnDownLoadMsg(UINT , WPARAM wParam, LPARAM , BOOL& bHandled);
	LRESULT OnNMRclickListLeak(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnMenuCommand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnChangeCBChain(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawClipBoard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	virtual VOID OnDownLaodNotify(const sha1_hash& hash , NotifyMessageID msgid ,WPARAM wp, LPARAM lp);
	virtual VOID OnEnumDownLoadTask(const sha1_hash& hash);

private:
	HRESULT InitDLTaskMgrListCtr();
	HRESULT RefreshTask();
	DWORD   GetTaskIndex(const sha1_hash& hash);
	HRESULT	InertTaskItem(const sha1_hash& hash);
	HRESULT UpdataTaskItem(DWORD dwIndex , const DownLoadTaskInfo* pInfo , BOOL bDownloading = FALSE);
	HRESULT CreateDownloadTask(LPCTSTR lpUrl,LPCTSTR lpsavePath,BOOL bAutoStart=FALSE);
private:
	CListViewCtrl m_dlTaskMgrListControl;
	CDownloadModuleHelp m_dlHelper;
	int					m_currentSelect;
	IDownLoadMgr* m_pDownloadMgr;
	std::vector<DownloadTaskItemInfo> m_downloadTasks;
	msdk::CSafeList<DownLoadMsgInfo>  m_SafeList;
	HWND m_NextClipWnd;
};