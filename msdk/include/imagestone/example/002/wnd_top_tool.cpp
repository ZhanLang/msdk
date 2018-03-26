#include "stdafx.h"
#include "002.h"
#include "wnd_top_tool.h"

//-------------------------------------------------------------------------------------
void CWndTopTool::Create (CMDIFrameWnd* pMainFrame)
{
    const UINT btn[] =
    {
        ID_FILE_OPEN,
        ID_FILE_SAVE_AS,
        ID_FILE_PRINT,
        ID_EDIT_COPY,
        ID_SEPARATOR,
        ID_EDIT_UNDO,
        ID_EDIT_REDO,
        ID_SEPARATOR,
        ID_VIEW_ZOOMIN_STEP,
        ID_VIEW_ZOOMOUT_STEP,
        ID_VIEW_ACTUAL,
        ID_SEPARATOR,
        ID_VIEW_HOME,
    };

    const CSize  img_size(26,26) ;
    const int    nIDCount = sizeof(btn) / sizeof(btn[0]) ;

    if (!CreateEx (pMainFrame, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT|TBSTYLE_TOOLTIPS, WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED))
        return ;

    SetButtons (btn, nIDCount) ;
    SetSizes (img_size + CSize(7,7), img_size) ;
    LoadPngToolBar() ;
}
//-------------------------------------------------------------------------------------
LRESULT CWndTopTool::WindowProc (UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        CImageList().Attach ((HIMAGELIST)GetToolBarCtrl().SendMessage(TB_SETIMAGELIST)) ;
        CImageList().Attach ((HIMAGELIST)GetToolBarCtrl().SendMessage(TB_SETHOTIMAGELIST)) ;
        CImageList().Attach ((HIMAGELIST)GetToolBarCtrl().SendMessage(TB_SETDISABLEDIMAGELIST)) ;
    }
    return __super::WindowProc(msg, wParam, lParam) ;
}
//-------------------------------------------------------------------------------------
void CWndTopTool::LoadPngToolBar()
{
    FCObjImage   img ;
    img.LoadResource (IDR_PNG_TOP_TOOLBAR, L"PNG", IMG_PNG) ;

    SetImageList(TB_SETIMAGELIST, img) ;
    SetImageList(TB_SETHOTIMAGELIST, img) ;

    FCEffectGrayscale   c ;
    img.ApplyEffect (c) ;
    SetImageList(TB_SETDISABLEDIMAGELIST, img) ;
}
//-------------------------------------------------------------------------------------
void CWndTopTool::SetImageList (UINT msg, const FCObjImage& img)
{
    if (!img.IsValidImage() || (img.ColorBits() != 32))
    {
        ASSERT(FALSE) ;
        return ;
    }

    CSize  item_image_size (img.Height(), img.Height()) ;
    int    nItemCount = img.Width() / item_image_size.cx ;

    CImageList   il ;
    il.Create (item_image_size.cx, item_image_size.cy, ILC_COLOR32 | ILC_MASK, nItemCount, 1) ;
    il.Add (CBitmap::FromHandle(img), RGB(0,0,0)) ;

    GetToolBarCtrl().SendMessage (msg, 0, (LPARAM)il.Detach()) ;
}
//-------------------------------------------------------------------------------------
