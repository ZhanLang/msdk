#pragma once

//-------------------------------------------------------------------------------------
class DlgOpenImage : public CFileDialog
{
    enum
    {
        IDC_IMAGE_WND = 1100,
        IDC_SIZE_VALUE = 1200,
        IDC_DATE_VALUE = 1210,
    };

    class CWndThumb : public FCButtonBase
    {
        virtual void OnDrawButton (CDC* pDC, CRect rcButton)
        {
            FillRect (*pDC, rcButton, CBrush(RGB(255,255,255))) ;
            FrameRect (*pDC, rcButton, CBrush(RGB(192,192,192))) ;
            rcButton.DeflateRect(1,1,1,1) ;
            m_img->Draw (*pDC, rcButton) ;
        }
    public:
        FCObjImage   * m_img ;
    };

public:
    DlgOpenImage() ;

private:
    CWndThumb    m_ctrl ;
    FCObjImage   m_thumb ;

    static CString   g_last_file ;

    static LPCTSTR GetInitialFile()
    {
        if (g_last_file.GetLength())
        {
            SetCurrentDirectory(g_last_file) ;
        }
        return NULL ;
    }

private:
    virtual void DoDataExchange(CDataExchange* pDX)
    {
        CFileDialog::DoDataExchange(pDX) ;
        DDX_Control(pDX, IDC_IMAGE_WND, m_ctrl) ;
    }

    virtual BOOL OnInitDialog()
    {
        CRect   rc ;
        GetDlgItem(IDC_IMAGE_WND)->GetClientRect(rc) ;

        rc.DeflateRect(1,1,1,1) ;
        m_thumb.Create (rc.Width(), rc.Height(), 24) ;
        m_thumb.ApplyEffect (FCEffectFillColor(FCColor(255,255,255))) ;

        SetDlgItemText(IDC_SIZE_VALUE, L"") ;
        SetDlgItemText(IDC_DATE_VALUE, L"") ;

        return CFileDialog::OnInitDialog() ;
    }

    static CString GetFilterText()
    {
        return L"All Supported Image|*.bmp; *.jpg; *.jpeg; *.gif; *.tif; *.tiff; *.png|All Files (*.*)|*.*||" ;
    }

    virtual void OnFileNameChange() ;

    virtual BOOL OnFileNameOK()
    {
        g_last_file = FCFileEx::GetFileFolder(GetPathName()) ;
        return CFileDialog::OnFileNameOK() ;
    }
};
