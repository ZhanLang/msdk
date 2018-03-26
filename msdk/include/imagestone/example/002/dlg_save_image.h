#pragma once

//-------------------------------------------------------------------------------------
class DlgSaveImage : public CFileDialog
{
    enum
    {
        IDC_USE_32_BPP = 1250,

        IDC_QUALITY_TITLE = 1260,
        IDC_QUALITY_SLIDER = 1261,
        IDC_QUALITY_VALUE = 1262,

        IDC_KEEP_EXIF = 1270,
    };

    static CString QueryLanguageText (int nID)
    {
        return theApp.GetText (L"IDD_SAVE_IMAGE", nID) ;
    }

public:
	DlgSaveImage (LPCTSTR strInitFile) ;

    BOOL IsKeepEXIF() {return m_keep_EXIF;}
    int GetBPP() ;
    int GetQuality() ;

private:
    enum IMG_INDEX
    {
        IMG_INDEX_BMP = 1,
        IMG_INDEX_JPG = 2,
        IMG_INDEX_GIF = 3,
        IMG_INDEX_PNG = 4,
        IMG_INDEX_TIF = 5,
    };

private:
    TCHAR   m_DefExt[32] ;
    BOOL    m_keep_EXIF ;
    BOOL    m_32bpp ;
    int     m_quality ;

    static int        g_last_quality ;
    static IMG_INDEX  g_last_img_index ;

    static CString GetFileExt (IMG_INDEX nType) ;
    void SetQualityValue() ;

    virtual BOOL OnInitDialog() ;
    virtual void OnTypeChange() ;
    virtual BOOL OnFileNameOK() ;

    afx_msg void OnUse32BPP();
    afx_msg void OnEXIF();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    DECLARE_MESSAGE_MAP()
};
