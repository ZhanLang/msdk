#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectLib : public CDialogBar
{
private:
    //---------------------------------------------------------------------------------
    class CEffectThumb : public FCListItem_Base
    {
    public:
        CEffectThumb (int nCommandID, LPCWSTR sText) : m_command_id(nCommandID), m_text(sText) {}

        int      m_command_id ;
        CString  m_text ;
        CBitmap  m_img ;

        void SetImage (HBITMAP hBmp) ;

        void DrawTitle (CDC& dc, CRect rcItemOnDC) ;
        void DrawThumb (CDC& dc, CRect rcItemOnDC) ;
        virtual void OnPaint_Item (CDC& dc) ;
    };

    //---------------------------------------------------------------------------------
    class CThumbLayout : public FCLayout_HandlerBase
    {
        virtual void Layout_ListItem (CWnd* pWnd, std::deque<FCListItem_Base*>& item_list) ;
    };

    //---------------------------------------------------------------------------------
    class CEffectList : public FCListWindow
    {
        HANDLE   m_thread ;
    public:
        CEffectList() ;
        ~CEffectList() ;

    private:
        void AddThumb (int nCommand, LPCWSTR sText) ;
        static CString GetPureMenuText (LPCWSTR sName) ;

        virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam) ;

        struct THREAD_PARAM
        {
            HWND   m_wnd ;
            std::deque<int>   m_cmd ;
        };

        static FCImageEffect* CreateEffect (int nCommandID) ;
        static DWORD WINAPI MakeThumbThread (LPVOID lpParameter) ;
    };

public:
    void Create (CMDIFrameWnd* pMainFrame) ;

private:
    std::auto_ptr<CEffectList>   m_list ;

    void ShowList() ;
    void HideList() ;

    virtual CSize CalcDynamicLayout (int nLength, DWORD nMode) ;
    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam) ;
    virtual void OnUpdateCmdUI (CFrameWnd* pTarget, BOOL bDisableIfNoHndler) {} // enable all button
};
