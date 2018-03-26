#include "stdafx.h"
#include "002.h"
#include "dlg_open_image.h"


CString DlgOpenImage::g_last_file = L"" ;


//-------------------------------------------------------------------------------------
#if _MSC_VER >= 1500
DlgOpenImage::DlgOpenImage() : CFileDialog(TRUE, NULL, GetInitialFile(), OFN_HIDEREADONLY, GetFilterText(), NULL, 0, FALSE)
#else
DlgOpenImage::DlgOpenImage() : CFileDialog(TRUE, NULL, GetInitialFile(), OFN_HIDEREADONLY, GetFilterText(), NULL)
#endif
{
    m_ofn.Flags |= (OFN_ENABLETEMPLATE | OFN_ENABLESIZING) ;
    m_ofn.lpTemplateName = MAKEINTRESOURCE (IDD_OPEN_IMAGE) ;
    m_ctrl.m_img = &m_thumb ;
}
//-------------------------------------------------------------------------------------
void DlgOpenImage::OnFileNameChange()
{
    m_thumb.ApplyEffect (FCEffectFillColor(FCColor(255,255,255))) ;

    CString   szFile = GetPathName() ;

    FCImageProperty   prop ;
    CSize   sizeImage (0,0) ;

    if (PathFileExists(szFile))
    {
        IMAGE_TYPE  imgType = FCImageCodecFactory::GetTypeByFileExt(szFile) ;

        if ( (imgType == IMG_JPG) ||
             (imgType == IMG_BMP) ||
             (imgType == IMG_PNG) ||
             (imgType == IMG_TIF) ||
             (imgType == IMG_GIF) )
        {
            Gdiplus::Bitmap   bmp (szFile) ;
            if (bmp.GetLastStatus() == Gdiplus::Ok)
            {
                sizeImage = CSize (bmp.GetWidth(), bmp.GetHeight()) ;
                FCImageCodec_Gdiplus::GetPropertyFromBitmap(bmp, prop) ;

                // calculate thumb size
                CRect   rc (0, 0, m_thumb.Width(), m_thumb.Height()) ;
                rc = FCObjGraph::CalcFitWindowSize (sizeImage, rc) ;

                FCImageDrawDC   memDC (m_thumb) ;
                Gdiplus::Graphics(memDC).DrawImage (&bmp, rc.left, rc.top, rc.Width(), rc.Height()) ;
            }
        }
    }

    // update size
    CString   s ;
    if (sizeImage.cx && sizeImage.cy)
    {
        s.Format(L"%d x %d", sizeImage.cx, sizeImage.cy) ;
        s = L"Size :" + s ;
    }
    SetDlgItemText(IDC_SIZE_VALUE, s) ;

    // update date
    s = L"" ;
    if (prop.m_ExifDTOrig.length())
    {
        s = L"Date :" + CString(prop.m_ExifDTOrig.c_str()) ;
    }
    SetDlgItemText(IDC_DATE_VALUE, s) ;

    m_ctrl.Invalidate() ;
}
//-------------------------------------------------------------------------------------
