#include "stdafx.h"
#include "002.h"
#include "002Doc.h"
#include "dlg_effect_base.h"
#include "wnd_bottom_status.h"

//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(DlgEffectBase, DlgModalBase)
    ON_COMMAND(ID_EFFECT_DIALOG_POST_INIT, OnPostInit)
    ON_MESSAGE(WM_PHOXO_PROCESS_FINISH, OnProcessFinish)
    ON_MESSAGE(WM_PHOXO_PROCESS_STEP, OnProcessStep)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
DlgEffectBase::DlgEffectBase (LPCWSTR strSection, UINT nDlgID, CMy002Doc* pDoc) : DlgModalBase(strSection, nDlgID)
{
    m_backup_img = pDoc->m_img ;

    POSITION   pos = pDoc->GetFirstViewPosition() ;
    m_view = pDoc->GetNextView(pos) ;

    m_layer = &pDoc->m_img ;
    m_curr = m_backup_img ;
    m_finish_close = FALSE ;
}
//-------------------------------------------------------------------------------------
DlgEffectBase::~DlgEffectBase()
{
    CWndBottomStatus::HideProgress() ;
}
//-------------------------------------------------------------------------------------
void DlgEffectBase::OnOK()
{
    if (m_task.get())
    {
        m_finish_close = TRUE ;
        EnumChildWindows (*this, disable_child_ctrl, 0) ;
    }
    else
    {
        DlgModalBase::OnOK() ;
    }
}
//-------------------------------------------------------------------------------------
void DlgEffectBase::OnCancel()
{
    m_task.reset() ;

    // restore layer
    m_layer->CoverBlock(m_backup_img, 0, 0) ;

    m_view->Invalidate() ;

    DlgModalBase::OnCancel() ;
}
//-------------------------------------------------------------------------------------
void DlgEffectBase::OnPostInit()
{
    ApplyEffect() ;
}
//-------------------------------------------------------------------------------------
LRESULT DlgEffectBase::OnProcessStep (WPARAM wParam, LPARAM lParam)
{
    if (m_task.get())
    {
        if (m_task->m_id == (int)wParam)
        {
            CWndBottomStatus::SetProgress (m_task->m_nLastPercent) ;
        }
    }
    return 0 ;
}
//-------------------------------------------------------------------------------------
LRESULT DlgEffectBase::OnProcessFinish (WPARAM wParam, LPARAM lParam)
{
    if (m_task.get())
    {
        if (m_task->m_id == (int)wParam)
        {
            m_task.reset() ;
            if (m_finish_close)
            {
                DlgModalBase::OnOK() ;
            }
        }
        else
        {
            // previous task message
        }
    }
    return 0 ;
}
//-------------------------------------------------------------------------------------
void DlgEffectBase::ApplyEffect()
{
    m_task.reset() ; // stop thread first

    UpdateData() ;
    m_task.reset(new CProcessTask(this)) ;
}
//-------------------------------------------------------------------------------------
BOOL DlgEffectBase::OnInitDialog()
{
    PostMessage (WM_COMMAND, MAKEWPARAM(ID_EFFECT_DIALOG_POST_INIT,0)) ;
    return DlgModalBase::OnInitDialog() ;
}
//-------------------------------------------------------------------------------------
LRESULT DlgEffectBase::WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT   l = DlgModalBase::WindowProc (msg, wParam, lParam) ;

    BOOL   bUpdate = FALSE ;

    // update when scroll
    if (msg == WM_HSCROLL)
    {
        if (LOWORD(wParam) != SB_ENDSCROLL)
            bUpdate = TRUE ;
    }

    if (msg == WM_COMMAND)
    {
        int     nID = LOWORD(wParam) ;
        TCHAR   cls[128] = {0} ;
        ::GetClassName (::GetDlgItem(m_hWnd, nID), cls, 128) ;
        // button command
        if (lstrcmpi(cls, L"button") == 0)
        {
            if ((nID != IDOK) && (nID != IDCANCEL))
                bUpdate = TRUE ;
        }
    }

    if (bUpdate)
    {
        ApplyEffect() ;
    }
    return l ;
}


//-------------------------------------------------------------------------------------
DlgEffectBase::CProcessTask::CProcessTask (DlgEffectBase* pDlg)
{
    static int   s_curr_id = 0 ;
    m_id = s_curr_id++ ;

    m_pDlg = pDlg ;
    m_continue_process = TRUE ;
    m_nLastUpdate = 0 ;
    m_nLastPercent = -1 ;
    m_cmd.reset (pDlg->CreateProcessCommand()) ;

    m_thread = CreateThread (NULL, 0, ImageProcessThread, this, 0, NULL) ;
}
//-------------------------------------------------------------------------------------
DlgEffectBase::CProcessTask::~CProcessTask ()
{
    InterlockedExchange (&m_continue_process, FALSE) ;
    WaitForSingleObject (m_thread, INFINITE) ;
    CloseHandle(m_thread) ;
}
//-------------------------------------------------------------------------------------
DWORD WINAPI DlgEffectBase::CProcessTask::ImageProcessThread (LPVOID lpParameter)
{
    CProcessTask   * p = (CProcessTask*)lpParameter ;
    DlgEffectBase  * pDlg = p->m_pDlg ;

    FCObjImage   & img = pDlg->m_curr ;

    if (img.IsValidImage())
    {
        img.CoverBlock (pDlg->m_backup_img, 0, 0) ;

        // process with progress
        img.ApplyEffect (*p->m_cmd, p) ;
    }
    pDlg->PostMessage(WM_PHOXO_PROCESS_FINISH, p->m_id) ;
    return 0 ;
}
//-------------------------------------------------------------------------------------
bool DlgEffectBase::CProcessTask::OnProgressUpdate (int nFinishPercentage)
{
    if (!m_continue_process)
        return false ;

    nFinishPercentage = FClamp(nFinishPercentage, 0, 100) ;

    if (nFinishPercentage % 5)
        return true ; // span == 5
    if (m_nLastPercent == nFinishPercentage)
        return true ;

    FCObjImage   & img = m_pDlg->m_curr ;
    FCObjImage   * pLayer = m_pDlg->m_layer ;

    // update status
    m_nLastPercent = nFinishPercentage ;
    m_pDlg->PostMessage(WM_PHOXO_PROCESS_STEP, m_id) ;

    // update view every 20%
    if ((nFinishPercentage >= m_nLastUpdate+20) || (nFinishPercentage == 100))
    {
        int   nStart = img.Height() * m_nLastUpdate / 100 ;
        int   nEnd = img.Height() * nFinishPercentage / 100 ;
        if (nFinishPercentage == 100)
            nStart = 0 ;

        // update view
        for (int y=nStart ; y < nEnd ; y++)
        {
            for (int x=0 ; x < img.Width() ; x++)
            {
                *(RGBQUAD*)pLayer->GetBits(x,y) = *(RGBQUAD*)img.GetBits(x,y) ;
            }
        }

        m_pDlg->m_view->Invalidate() ;
        m_nLastUpdate = nFinishPercentage ;
    }
    return true ;
}
//-------------------------------------------------------------------------------------
