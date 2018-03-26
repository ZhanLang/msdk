#include "stdafx.h"
#include "002.h"
#include "dlg_save_image.h"


BEGIN_MESSAGE_MAP(DlgSaveImage, CFileDialog)
    ON_COMMAND(IDC_USE_32_BPP, OnUse32BPP)
    ON_COMMAND(IDC_KEEP_EXIF, OnEXIF)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()


DlgSaveImage::IMG_INDEX DlgSaveImage::g_last_img_index = DlgSaveImage::IMG_INDEX_JPG ;
int DlgSaveImage::g_last_quality = 8 ;


//-------------------------------------------------------------------------------------
#if _MSC_VER >= 1500
DlgSaveImage::DlgSaveImage (LPCTSTR strInitFile) : CFileDialog(FALSE, NULL, strInitFile, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, QueryLanguageText(0), NULL, 0, FALSE)
#else
DlgSaveImage::DlgSaveImage (LPCTSTR strInitFile) : CFileDialog(FALSE, NULL, strInitFile, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, QueryLanguageText(0), NULL)
#endif
{
    m_ofn.nFilterIndex = g_last_img_index ;

    lstrcpy (m_DefExt, GetFileExt(g_last_img_index)) ;
    m_ofn.lpstrDefExt = m_DefExt ;

    m_keep_EXIF = FALSE ;
    m_32bpp = FALSE ;
    m_quality = g_last_quality ;

    m_ofn.Flags |= (OFN_ENABLETEMPLATE | OFN_ENABLESIZING) ;
    m_ofn.lpTemplateName = MAKEINTRESOURCE (IDD_SAVE_IMAGE) ;
}
//-------------------------------------------------------------------------------------
void DlgSaveImage::OnTypeChange()
{
    IMG_INDEX   nType = (IMG_INDEX)m_ofn.nFilterIndex ;

    lstrcpy (m_DefExt, GetFileExt(nType)) ;

    std::map<int,BOOL>   t ;

    // have 32bpp
    if ( (nType == IMG_INDEX_BMP) ||
         (nType == IMG_INDEX_PNG) ||
         (nType == IMG_INDEX_TIF) )
    {
        t[IDC_USE_32_BPP] = TRUE ;
    }
    else
    {
        t[IDC_USE_32_BPP] = FALSE ;
    }

    // is jpeg
    BOOL   is_jpg = (nType == IMG_INDEX_JPG) ;
    t[IDC_QUALITY_TITLE] = is_jpg ;
    t[IDC_QUALITY_SLIDER] = is_jpg ;
    t[IDC_QUALITY_VALUE] = is_jpg ;
    t[IDC_KEEP_EXIF] = is_jpg ;

    for (std::map<int,BOOL>::iterator i=t.begin() ; i != t.end() ; i++)
    {
        ::ShowWindow(::GetDlgItem(m_hWnd,i->first), i->second ? SW_SHOWNA : SW_HIDE) ;
    }
}
//-------------------------------------------------------------------------------------
BOOL DlgSaveImage::OnFileNameOK()
{
    g_last_img_index = (IMG_INDEX)m_ofn.nFilterIndex ;

    if (g_last_img_index == IMG_INDEX_JPG)
    {
        g_last_quality = m_quality ;
    }

    return CFileDialog::OnFileNameOK() ;
}
//-------------------------------------------------------------------------------------
BOOL DlgSaveImage::OnInitDialog()
{
    SetDlgItemText(IDC_USE_32_BPP, QueryLanguageText(1)) ;
    SetDlgItemText(IDC_QUALITY_TITLE, QueryLanguageText(2)) ;
    SetDlgItemText(IDC_KEEP_EXIF, QueryLanguageText(3)) ;

    SendDlgItemMessage (IDC_QUALITY_SLIDER, TBM_SETRANGE, TRUE, MAKELPARAM(1,10)) ;
    SendDlgItemMessage (IDC_QUALITY_SLIDER, TBM_SETPOS, 1, m_quality) ;
    SetQualityValue() ;

    BOOL   b = CFileDialog::OnInitDialog() ;
    OnTypeChange() ;
    return b ;
}
//-------------------------------------------------------------------------------------
CString DlgSaveImage::GetFileExt (IMG_INDEX nType)
{
    std::map<IMG_INDEX, CString>   t ;
    t[IMG_INDEX_BMP] = "bmp" ;
    t[IMG_INDEX_JPG] = "jpg" ;
    t[IMG_INDEX_GIF] = "gif" ;
    t[IMG_INDEX_PNG] = "png" ;
    t[IMG_INDEX_TIF] = "tif" ;

    if (t.find(nType) != t.end())
        return t[nType] ;
    return t[IMG_INDEX_JPG] ;
}
//-------------------------------------------------------------------------------------
int DlgSaveImage::GetBPP()
{
    switch (m_ofn.nFilterIndex)
    {
        case IMG_INDEX_GIF : return 8 ;
        case IMG_INDEX_JPG : return 24 ;

        case IMG_INDEX_BMP :
        case IMG_INDEX_PNG :
        case IMG_INDEX_TIF :
            return (m_32bpp ? 32 : 24) ;
    }
    return 24 ;
}
//-------------------------------------------------------------------------------------
int DlgSaveImage::GetQuality()
{
    switch (m_quality)
    {
        case 1 : return 40 ;
        case 2 : return 50 ;
        case 3 : return 60 ;
        case 4 : return 70 ;
        case 5 : return 75 ;
        case 6 : return 80 ;
        case 7 : return 85 ;
        case 8 : return 90 ;
        case 9 : return 95 ;
        case 10 : return 100 ;
    }
    return 90 ;
}
//-------------------------------------------------------------------------------------
void DlgSaveImage::OnUse32BPP()
{
    m_32bpp = (BOOL)SendDlgItemMessage(IDC_USE_32_BPP, BM_GETCHECK) ;
}
//-------------------------------------------------------------------------------------
void DlgSaveImage::OnEXIF()
{
    m_keep_EXIF = (BOOL)SendDlgItemMessage(IDC_KEEP_EXIF, BM_GETCHECK) ;
}
//-------------------------------------------------------------------------------------
void DlgSaveImage::SetQualityValue()
{
    CString   s ;
    s.Format(L"%d", m_quality) ;
    SetDlgItemText(IDC_QUALITY_VALUE, s + QueryLanguageText(10+m_quality)) ;
}
//-------------------------------------------------------------------------------------
void DlgSaveImage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    m_quality = (int)SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_GETPOS) ;
    SetQualityValue() ;
}
//-------------------------------------------------------------------------------------
