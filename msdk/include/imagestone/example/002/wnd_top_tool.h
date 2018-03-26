#pragma once

//-------------------------------------------------------------------------------------
class CWndTopTool : public CToolBar
{
public:
    void Create (CMDIFrameWnd* pMainFrame) ;

private:
    void LoadPngToolBar() ;
    void SetImageList (UINT msg, const FCObjImage& img) ;

    virtual LRESULT WindowProc (UINT msg, WPARAM wParam, LPARAM lParam) ;
};
