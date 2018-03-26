#pragma once
#include "dlg_modal_base.h"

//-------------------------------------------------------------------------------------
//
// when click radio or check button, apply effect
//
class DlgEffectBase : public DlgModalBase
{
private:
    // process in thread
    class CProcessTask : public FCProgressObserver
    {
        // process image and notify observer
        static DWORD WINAPI ImageProcessThread (LPVOID lpParameter) ;

        // this callback still in thread
        virtual bool OnProgressUpdate (int nFinishPercentage) ;

    public:
        int      m_id ;
        HANDLE   m_thread ;
        DlgEffectBase   * m_pDlg ;
        volatile LONG   m_continue_process ;
        int   m_nLastUpdate ;
        int   m_nLastPercent ;
        std::auto_ptr<FCImageEffect>   m_cmd ;

    public:
        CProcessTask (DlgEffectBase* pDlg) ;
        ~CProcessTask () ;
    };

private:
    FCObjImage   m_backup_img ;

    CView        * m_view ;
    FCObjImage   * m_layer ;
    FCObjImage   m_curr ;
    BOOL         m_finish_close ;

    std::auto_ptr<CProcessTask>   m_task ;

public:
    DlgEffectBase (LPCWSTR strSection, UINT nDlgID, CMy002Doc* pDoc) ;
    ~DlgEffectBase() ;

    CSize GetProcessImageSize() {return CSize(m_curr.Width(),m_curr.Height());}

private:
    static BOOL CALLBACK disable_child_ctrl (HWND hwnd, LPARAM lParam)
    {
        if (::GetDlgCtrlID(hwnd) != IDCANCEL)
            ::EnableWindow(hwnd, FALSE) ;
        return TRUE ;
    }

    virtual void OnOK() ;
    virtual void OnCancel() ;

protected:
    virtual FCImageEffect* CreateProcessCommand() =0 ;

    void ApplyEffect() ;

protected:
    virtual BOOL OnInitDialog() ;
    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam) ;

    afx_msg void OnPostInit();
    afx_msg LRESULT OnProcessStep(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcessFinish(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
