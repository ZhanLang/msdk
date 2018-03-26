#include "stdafx.h"
#include "002.h"
#include "002Doc.h"
#include "wnd_bottom_status.h"
#include "dlg_effect_dialog.h"
#include "dlg_effect_color_tone.inl"
#include "dlg_effect_color_level.inl"
#include "dlg_effect_color_balance.inl"
#include "dlg_effect_shadow.h"
#include "dlg_effect_lensflare.inl"
#include "dlg_effect_supernova.inl"
#include "dlg_effect_blinds.inl"
#include "dlg_effect_fillgrid.inl"
#include "dlg_effect_custom.inl"
#include "dlg_save_image.h"
#include "dlg_image_size.inl"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CMy002Doc, CDocument)

BEGIN_MESSAGE_MAP(CMy002Doc, CDocument)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
    ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND_RANGE(ID_EFFECT_FLIP, ID_EFFECT_END, OnImageEffect)
    ON_COMMAND(ID_TOOL_EXPORT_ASCII, OnToolExportAscii)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
CMy002Doc::~CMy002Doc()
{
    std::for_each(m_undo.begin(), m_undo.end(), FCDeleteEachObject()) ;
    std::for_each(m_redo.begin(), m_redo.end(), FCDeleteEachObject()) ;
}
//-------------------------------------------------------------------------------------
BOOL CMy002Doc::OnOpenDocument(LPCTSTR szPathName)
{
    if (!CDocument::OnOpenDocument(szPathName))
        return FALSE;

    m_img.Load(szPathName, &m_property) ;

    if (m_img.IsValidImage())
    {
        m_img.ConvertTo32Bit() ;
        SetPathName (szPathName) ;
    }
    else
    {
        ::AfxMessageBox (theApp.GetText(L"app", L"UNKNOW_IMAGE"), MB_ICONEXCLAMATION) ;
    }

    return (m_img.IsValidImage() ? TRUE : FALSE) ;
}
//-------------------------------------------------------------------------------------
void CMy002Doc::OnEditCopy() 
{
    CWaitCursor   aCursor ;

    m_img.CopyToClipboard() ;
    AfxMessageBox(L"Copy to clipboard success!") ;
}
//-------------------------------------------------------------------------------------
CMy002Doc::TARGET_STATUS CMy002Doc::QuerySaveTargetFileStatus (LPCTSTR strDestFile)
{
    TARGET_STATUS   nRet ;
    if (PathFileExists(strDestFile))
    {
        SetFileAttributes(strDestFile, FILE_ATTRIBUTE_NORMAL) ;
        if (DeleteFile(strDestFile))
        {
            nRet = TARGET_FILE_OK ;
        }
        else
        {
            nRet = TARGET_FILE_DENY ;
            if (GetLastError() == ERROR_SHARING_VIOLATION)
            {
                nRet = TARGET_FILE_VIOLATION ;
            }
        }
    }
    else
    {
        HANDLE   h = CreateFile (strDestFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL) ;
        if (h == INVALID_HANDLE_VALUE)
        {
            nRet = TARGET_FILE_DENY ;
        }
        else
        {
            CloseHandle(h) ;
            DeleteFile(strDestFile) ;
            nRet = TARGET_FILE_OK ;
        }
    }
    return nRet ;
}
//-------------------------------------------------------------------------------------
class CStatusProgress : public FCProgressObserver
{
    virtual bool OnProgressUpdate (int nFinishPercentage)
    {
        CWndBottomStatus::SetProgress(nFinishPercentage) ;

        MSG   msg ;
        if (PeekMessage (&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE) && (msg.wParam == VK_ESCAPE))
        {
            return false ;
        }

        return true ;
    }
public:
    ~CStatusProgress()
    {
        CWndBottomStatus::HideProgress() ;
    }
};
//-------------------------------------------------------------------------------------
void CMy002Doc::OnImageEffect (UINT nID)
{
    DlgEffectBase   * pEffectDlg = NULL ;
    FCImageEffect   * pCmd = NULL ;

    switch (nID)
    {
        case ID_EFFECT_BRIGHTNESS :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectBrightnessContrast>(L"IDD_EFFECT_BRIGHTNESS", this) ;
            pEffectDlg->SetSlider_1 (-100, 100, 0, 1) ;
            pEffectDlg->SetSlider_2 (-100, 100, 0, 2) ;
            break;
        case ID_EFFECT_HUE :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectHueSaturation>(L"IDD_EFFECT_HUE", this) ;
            pEffectDlg->SetSlider_1 (-100, 100, 0, 1) ;
            pEffectDlg->SetSlider_2 (-100, 100, 0, 2) ;
            break;
        case ID_EFFECT_COLORTONE :
            pEffectDlg = new DlgEffectColorTone(this) ;
            break;
        case ID_EFFECT_GRAYSCALE : pCmd = new FCEffectGrayscale ; break;
        case ID_EFFECT_SOFTGLOW :
            pEffectDlg = new DlgEffectThreeSlider<FCEffectSoftGlow>(L"IDD_EFFECT_GLOW", this) ;
            pEffectDlg->SetSlider_1 (1, 16, 10, 1) ;
            pEffectDlg->SetSlider_2 (-100, 100, 10, 2) ;
            pEffectDlg->SetSlider_3 (-100, 100, 10, 3) ;
            break;
        case ID_EFFECT_SOFT_PORTRAIT :
            pEffectDlg = new DlgEffectThreeSlider<FCEffectSoftPortrait>(L"IDD_EFFECT_SOFT_PORTRAIT", this) ;
            pEffectDlg->SetSlider_1 (1, 10, 5, 1) ;
            pEffectDlg->SetSlider_2 (-20, 20, 0, 2) ;
            pEffectDlg->SetSlider_3 (0, 20, 10, 3) ;
            break;
        case ID_EFFECT_ADJUSTRGB :
            pEffectDlg = new DlgEffectThreeSlider<FCEffectAdjustRGB>(L"IDD_EFFECT_ADJUSTRGB", this) ;
            pEffectDlg->SetSlider_1 (-128, 128, 0, 1) ;
            pEffectDlg->SetSlider_2 (-128, 128, 0, 2) ;
            pEffectDlg->SetSlider_3 (-128, 128, 0, 3) ;
            break;
        case ID_EFFECT_GAMMA :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectGamma>(L"IDD_EFFECT_GAMMA", this) ;
            pEffectDlg->SetSlider_1 (1, 200, 100, 1) ;
            break;
        case ID_EFFECT_COLORLEVEL :
            pEffectDlg = new DlgEffectColorLevel(this) ;
            break;
        case ID_EFFECT_COLORBALANCE :
            pEffectDlg = new DlgEffectColorBalance(this) ;
            break;
        case ID_EFFECT_MOSAIC :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectMosaic>(L"IDD_EFFECT_MOSAIC", this) ;
            pEffectDlg->SetSlider_1 (2, 100, 20, 1) ;
            break;
        case ID_EFFECT_INVERT : pCmd = new FCEffectInvert ; break;
        case ID_EFFECT_AUTO_COLOR_LEVEL : pCmd = new FCEffectAutoColorLevel ; break;
        case ID_EFFECT_AUTO_CONTRAST : pCmd = new FCEffectAutoContrast ; break;
        case ID_EFFECT_AUTO_COLORENHANCE : pCmd = new FCEffectAutoColorEnhance ; break;
        case ID_EFFECT_FLIP : pCmd = new FCEffectFlip ; break;
        case ID_EFFECT_ROTATE90 : pCmd = new FCEffectRotate90 ; break;
        case ID_EFFECT_ROTATE270 : pCmd = new FCEffectRotate270 ; break;
        case ID_EFFECT_MIRROR : pCmd = new FCEffectMirror ; break;
        case ID_EFFECT_SMOOTHEDGE :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectSmoothEdge>(L"IDD_EFFECT_SMOOTHEDGE", this) ;
            pEffectDlg->SetSlider_1 (1, 32, 10, 1) ;
            break;
        case ID_EFFECT_SHADOW :
            {
                DlgEffectShadow   dlg ;
                if (dlg.DoModal() == IDOK)
                {
                    pCmd = new FCEffectAddShadow(dlg.m_shadow_data) ;
                    ((FCEffectAddShadow*)pCmd)->SetPadding(16) ;
                }
            }
            break;
        case ID_EFFECT_BLUR_GAUSS :
            pEffectDlg = new DlgEffectBlurGauss(this) ;
            break;
        case ID_EFFECT_DESPECKLE : pCmd = new FCEffectReduceNoise ; break;
        case ID_EFFECT_BLURMOTION :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectBlur_Motion>(L"IDD_EFFECT_BLURMOTION", this) ;
            pEffectDlg->SetSlider_1 (1, 200, 50, 1) ;
            pEffectDlg->SetSlider_2 (0, 360, 0, 2) ;
            break;
        case ID_EFFECT_BLURZOOM :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectBlur_Zoom>(L"IDD_EFFECT_BLURZOOM", this) ;
            pEffectDlg->SetSlider_1 (1, 100, 15, 1) ;
            break;
        case ID_EFFECT_BLURRADIAL :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectBlur_Radial>(L"IDD_EFFECT_BLURRADIAL", this) ;
            pEffectDlg->SetSlider_1 (1, 45, 3, 1) ;
            break;
        case ID_EFFECT_SHARP :
            pEffectDlg = new DlgEffectSharpen(this) ;
            break;
        case ID_EFFECT_USMSHARP :
            pEffectDlg = new DlgEffectThreeSlider<FCEffectUNSharpMask>(L"IDD_EFFECT_USM", this) ;
            pEffectDlg->SetSlider_1 (1, 32, 5, 1) ;
            pEffectDlg->SetSlider_2 (1, 100, 70, 2) ;
            pEffectDlg->SetSlider_3 (0, 255, 0, 3) ;
            break;
        case ID_EFFECT_NOISIFY :
            pEffectDlg = new DlgEffectNoisify(this) ;
            break;
        case ID_EFFECT_OILPAINT :
            pEffectDlg = new DlgEffectOilPaint(this) ;
            break;
        case ID_EFFECT_EMBOSS :
            pEffectDlg = new DlgEffectEmboss(this) ;
            break;
        case ID_EFFECT_SPLASH :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectSplash>(L"IDD_EFFECT_SPLASH", this) ;
            pEffectDlg->SetSlider_1 (3, 64, 20, 1) ;
            break;
        case ID_EFFECT_POSTERIZE :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectPosterize>(L"IDD_EFFECT_POSTERIZE", this) ;
            pEffectDlg->SetSlider_1 (2, 20, 3, 1) ;
            break;
        case ID_EFFECT_TEXTURE :
            pEffectDlg = new DlgEffectTexture(this) ;
            break;
        case ID_EFFECT_3DGRID :
            pEffectDlg = new DlgEffect3DGrid(this) ;
            break;
        case ID_EFFECT_PENCILSKETCH :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectPencilSketch>(L"IDD_EFFECT_PENCILSKETCH", this) ;
            pEffectDlg->SetSlider_1 (1, 20, 5, 1) ;
            pEffectDlg->SetSlider_2 (-20, 20, 3, 2) ;
            break;
        case ID_EFFECT_SHIFT :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectShift>(L"IDD_EFFECT_SHIFT", this) ;
            pEffectDlg->SetSlider_1 (2, 50, 20, 1) ;
            break;
        case ID_EFFECT_RIBBON :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectRibbon>(L"IDD_EFFECT_RIBBON", this) ;
            pEffectDlg->SetSlider_1 (0, 100, 35, 1) ;
            pEffectDlg->SetSlider_2 (0, 100, 25, 2) ;
            break;
        case ID_EFFECT_BULGE :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectBulge>(L"IDD_EFFECT_BULGE", this) ;
            pEffectDlg->SetSlider_1 (-200, 100, -100, 1) ;
            break;
        case ID_EFFECT_TWIST :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectTwist>(L"IDD_EFFECT_TWIST", this) ;
            pEffectDlg->SetSlider_1 (-45, 45, 20, 1) ;
            pEffectDlg->SetSlider_2 (1, 200, 100, 2) ;
            break;
        case ID_EFFECT_ILLUSION :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectIllusion>(L"IDD_EFFECT_ILLUSION", this) ;
            pEffectDlg->SetSlider_1 (1, 10, 3, 1) ;
            break;
        case ID_EFFECT_WAVE :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectWave>(L"IDD_EFFECT_WAVE", this) ;
            pEffectDlg->SetSlider_1 (1, 50, 30, 1) ;
            pEffectDlg->SetSlider_2 (1, 50, 10, 2) ;
            break;
        case ID_EFFECT_RIPPLE :
            pEffectDlg = new DlgEffectRipple(this) ;
            break;
        case ID_EFFECT_LENSFLARE :
            pEffectDlg = new DlgEffectLensFlare(this) ;
            break;
        case ID_EFFECT_SUPERNOVA :
            pEffectDlg = new DlgEffectSupernova(this) ;
            break;
        case ID_EFFECT_GLASSTILE :
            pEffectDlg = new DlgEffectTwoSlider<FCEffectTileReflection>(L"IDD_EFFECT_GLASSTILE", this) ;
            pEffectDlg->SetSlider_1 (2, 200, 40, 1) ;
            pEffectDlg->SetSlider_2 (-20, 20, 8, 2) ;
            break;
        case ID_EFFECT_BLINDS :
            pEffectDlg = new DlgEffectBlinds(this) ;
            break;
        case ID_EFFECT_RAISEFRAME :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectRaiseFrame>(L"IDD_EFFECT_RAISEFRAME", this) ;
            pEffectDlg->SetSlider_1 (1, 100, 20, 1) ;
            break;
        case ID_EFFECT_HALFTONE : pCmd = new FCEffectHalftoneM3 ; break;
        case ID_EFFECT_THRESHOLD :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectThreshold>(L"IDD_EFFECT_THRESHOLD", this) ;
            pEffectDlg->SetSlider_1 (0, 255, 128, 1) ;
            break;
        case ID_EFFECT_SOLARIZE :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectSolarize>(L"IDD_EFFECT_SOLARIZE", this) ;
            pEffectDlg->SetSlider_1 (0, 255, 128, 1) ;
            break;
        case ID_EFFECT_OLDPHOTO :
            pEffectDlg = new DlgEffectSingleSlider<FCEffectOldPhoto>(L"IDD_EFFECT_OLDPHOTO", this) ;
            pEffectDlg->SetSlider_1 (1, 8, 3, 1) ;
            break;
        case ID_EFFECT_FILLGRID :
            pEffectDlg = new DlgEffectFillGrid(this) ;
            break;
        case ID_EFFECT_VIDEO :
            pEffectDlg = new DlgEffectVideo(this) ;
            break;
        case ID_EFFECT_CUSTOMFILTER :
            pEffectDlg = new DlgEffectCustom(this) ;
            break;
        case ID_EFFECT_DRAW_TEXT :
            pCmd = new CDrawTextCommand() ;
            break;
        case ID_EFFECT_CANVAS_STRETCH :
            {
                const CSize    sz (m_img.Width(), m_img.Height()) ;
                DlgImageSize   dlg (sz.cx, sz.cy) ;
                if (dlg.DoModal() == IDOK)
                {
                    CSize   new_size(dlg.m_width,dlg.m_height) ;
                    if (new_size != sz)
                        pCmd = new CResizeImageCommand (new_size) ;
                }
            }
            break;
    }

    if (!pEffectDlg && !pCmd)
        return ;

    FCObjImage   * pUndo = new FCObjImage ;
    *pUndo = m_img ;

    if (pEffectDlg)
    {
        pEffectDlg->SetTitleFromMenu(nID) ;

        if (pEffectDlg->DoModal() == IDOK)
        {
            ClearRedo() ;
            m_undo.push_back(pUndo) ;
        }
        else
        {
            delete pUndo ;
        }
    }
    else
    {
        CStatusProgress   aProgress ;
        m_img.ApplyEffect(*pCmd, &aProgress) ;

        if (aProgress.IsCanceled())
        {
            m_img = *pUndo ;
            delete pUndo ;
        }
        else
        {
            ClearRedo() ;
            m_undo.push_back(pUndo) ;
        }
    }

    if (pEffectDlg)
        delete pEffectDlg ;
    if (pCmd)
        delete pCmd ;

    UpdateAllViews(NULL) ;
}
//-------------------------------------------------------------------------------------
void CMy002Doc::ClearRedo()
{
    std::for_each(m_redo.begin(), m_redo.end(), FCDeleteEachObject()) ;
    m_redo.clear() ;
}
//-------------------------------------------------------------------------------------
void CMy002Doc::SaveCurrentToUndo()
{
    FCObjImage   * pUndo = new FCObjImage ;
    *pUndo = m_img ;
    m_undo.push_back(pUndo) ;
}
void CMy002Doc::SaveCurrentToRedo()
{
    FCObjImage   * pRedo = new FCObjImage ;
    *pRedo = m_img ;
    m_redo.push_back(pRedo) ;
}
//-------------------------------------------------------------------------------------
void CMy002Doc::OnEditUndo()
{
    CWaitCursor   aCursor ;

    SaveCurrentToRedo() ;

    FCObjImage   * p = m_undo.back() ;
    m_img = *p ;
    m_undo.pop_back() ;
    delete p ;

    UpdateAllViews(NULL) ;
}
void CMy002Doc::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (m_undo.size() != 0) ;
}
void CMy002Doc::OnEditRedo()
{
    CWaitCursor   aCursor ;

    SaveCurrentToUndo() ;

    FCObjImage   * p = m_redo.front() ;
    m_img = *p ;
    m_redo.pop_front() ;
    delete p ;

    UpdateAllViews(NULL) ;
}
void CMy002Doc::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (m_redo.size() != 0) ;
}
//-------------------------------------------------------------------------------------
namespace inner
{
    struct CAutoDeleteTempFile
    {
        CString   m_file ;
        CAutoDeleteTempFile()
        {
            CString   s ;
            s.Format(L"phoxo.ascii_%d.txt", (int)GetTickCount()) ;

            TCHAR   szTmpPath[MAX_PATH] ;
            GetTempPath (MAX_PATH, szTmpPath) ;
            PathAddBackslash(szTmpPath) ;

            m_file = szTmpPath + s ;
        }
        ~CAutoDeleteTempFile() { DeleteFile(m_file); }
    };
}
void CMy002Doc::OnToolExportAscii()
{
    static inner::CAutoDeleteTempFile   s_ascii_file ;

    FCEffectExportAscII   aASCII ;
    m_img.ApplyEffect (aASCII) ;
    std::string   s = aASCII.GetASCII() ;
    FCFileEx::Write (s_ascii_file.m_file, s.c_str(), (int)s.length()) ;

    ::ShellExecute (NULL, L"open", s_ascii_file.m_file, NULL, NULL, SW_SHOWNORMAL) ;
}
//-------------------------------------------------------------------------------------
void CMy002Doc::OnFileSaveAs()
{
    static CString   g_last_save_folder ;
    if (PathFileExists(g_last_save_folder) && (GetFileAttributes(g_last_save_folder) & FILE_ATTRIBUTE_DIRECTORY))
    {}
    else
    {
        TCHAR   s[MAX_PATH] = {0} ;
        SHGetFolderPath (NULL, CSIDL_MYPICTURES, NULL, 0, s) ;
        PathAddBackslash (s) ;
        g_last_save_folder = s ;
    }

    // remove file's ext name
    TCHAR   szPath[MAX_PATH] = {0} ;
    lstrcpyn (szPath, GetPathName(), MAX_PATH) ;
    PathRemoveExtension (szPath) ;

    // add title to path if it's empty
    if (CString(szPath).IsEmpty())
    {
        lstrcpy (szPath, g_last_save_folder) ;
        lstrcat (szPath, GetTitle()) ;
    }

    DlgSaveImage   dlg (szPath) ;
    if (dlg.DoModal() != IDOK)
        return ;

    g_last_save_folder = FCFileEx::GetFileFolder(dlg.GetPathName()) ;

    CString        img_file = dlg.GetPathName() ;
    TARGET_STATUS  target_status = QuerySaveTargetFileStatus(img_file) ;
    IMAGE_TYPE     imgType = FCImageCodecFactory::GetTypeByFileExt(img_file) ;

    // open target fail
    if (target_status != TARGET_FILE_OK)
    {
        CString   k = L"SAVE_ERROR_FAIL" ;
        switch (target_status)
        {
            case TARGET_FILE_VIOLATION :
                k = L"SAVE_ERROR_VIOLATION" ;
                break;

            case TARGET_FILE_DENY :
                k = L"SAVE_ERROR_NO_AUTHORITY" ;
                break;
        }

        ::AfxMessageBox (theApp.GetText(L"app", k), MB_ICONEXCLAMATION) ;
        return ;
    }

    // create save image
    FCObjImage   imgSave = m_img ;
    if (dlg.GetBPP() != 32)
    {
        imgSave.ApplyEffect (FCEffectFillBackGround(FCColor(0xFF,0xFF,0xFF,0xFF))) ;
        imgSave.ConvertTo24Bit() ;
    }

    int   nTransparency = -1 ;
    switch (dlg.GetBPP())
    {
        case 8 :
            {
/*                FCEffectFreeImageQuantize   aCmd ;
                imgSave.ApplyEffect (aCmd) ;
                nTransparency = aCmd.GetTransparencyIndex() ;*/
            }
            break;
        case 24 : imgSave.ConvertTo24Bit() ; break;
        case 32 : imgSave.ConvertTo32Bit() ; break;
    }

    BOOL     bRet = FALSE ;
    std::auto_ptr<CWaitCursor>   pWaitCursor (new CWaitCursor) ;
    if (imgType == IMG_JPG)
    {
        // jpeg need quality
        int   nFlag = m_property.m_SaveFlag ;
        m_property.m_SaveFlag = dlg.GetQuality() ;
        if (dlg.IsKeepEXIF())
        {
            bRet = imgSave.Save (img_file, m_property) ;
        }
        else
        {
            bRet = imgSave.Save (img_file, m_property.m_SaveFlag) ;
        }
        m_property.m_SaveFlag = nFlag ;
    }
    else if (imgType == IMG_GIF)
    {
        // gif's transparency's index
        bRet = imgSave.Save (img_file, nTransparency) ;
    }
    else
    {
        bRet = imgSave.Save (img_file) ;
    }
    pWaitCursor.reset() ;

    if (bRet && FCFileEx::GetSize(img_file))
    {
        SetModifiedFlag(FALSE) ;
        SetPathName (img_file) ;
    }
    else
    {
        ::AfxMessageBox (theApp.GetText(L"app", L"SAVE_ERROR_FAIL"), MB_ICONEXCLAMATION) ;
    }
}



//-------------------------------------------------------------------------------------
void CDrawTextCommand::ProcessWholeImage (FCObjImage& img, FCProgressObserver* pProgress)
{
    FCImageDrawDC   dc(img) ;
    {
        Gdiplus::Graphics   g(dc) ;
        g.SetSmoothingMode (Gdiplus::SmoothingModeAntiAlias) ;
        g.SetInterpolationMode (Gdiplus::InterpolationModeHighQualityBicubic) ;

        Gdiplus::FontFamily   ffami (L"Arial") ;
        Gdiplus::StringFormat fmt ;

        Gdiplus::GraphicsPath   str_path ;
        str_path.AddString (L"PhoXo", -1, &ffami, Gdiplus::FontStyleBold, 48, Gdiplus::Point(20,20), &fmt) ;

        Gdiplus::Pen   gp (Gdiplus::Color(0,0,160), 8) ;
        gp.SetLineJoin (Gdiplus::LineJoinRound) ;

        Gdiplus::LinearGradientBrush  gb (Gdiplus::Rect(20, 20, 30, 60),
                                          Gdiplus::Color(255,255,255),
                                          Gdiplus::Color(0,128,255),
                                          Gdiplus::LinearGradientModeVertical) ;

        g.DrawPath(&gp, &str_path) ;
        g.FillPath(&gb, &str_path) ;
    } // <- select img out of dc
}
