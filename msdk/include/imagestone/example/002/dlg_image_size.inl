#pragma once

//-------------------------------------------------------------------------------------
class DlgImageSize : public DlgModalBase
{
    enum
    {
        IDC_KEEP_RATIO = 1000,
        IDC_SIZE_WIDTH = 1001,
        IDC_SIZE_HEIGHT = 1002,
        IDC_SCALE_WIDTH = 1003,
        IDC_SCALE_HEIGHT = 1004,

        IDC_STR_WIDTH = 1200,
        IDC_STR_HEIGHT = 1201,
        IDC_STR_SCALE_X = 1202,
        IDC_STR_SCALE_Y = 1203,
        IDC_STR_X_PIXEL = 1204,
        IDC_STR_Y_PIXEL = 1205,

        IDC_STR_GROUP = 1300,
        IDC_STRETCH_NORMAL = 1500,
        IDC_STRETCH_BILINEAR = 1501,
    };

public:
	DlgImageSize (int nOldWidth, int nOldHeight) : DlgModalBase(L"IDD_IMAGE_RESIZE", IDD_IMAGE_RESIZE)
    {
        SetTitleFromMenu(ID_EFFECT_CANVAS_STRETCH) ;

        m_old_w = __max(1,nOldWidth) ;
        m_old_h = __max(1,nOldHeight) ;
        m_stretch_type = 1 ;
        m_keep = TRUE ;

        m_height = m_old_h ;
        m_width = m_old_w ;
        m_percent_h = m_percent_w = 100 ;
    }

	int		m_width ;
	int		m_height ;
    int     m_stretch_type ;

private :
    int      m_percent_w ;
    int      m_percent_h ;
    int      m_keep ;

    int      m_old_w ;
    int      m_old_h ;

private:
    virtual void OnOK();

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgModalBase::DoDataExchange(pDX);
        DDX_Text_No_Tip(pDX, IDC_SIZE_WIDTH, m_width);
        DDX_Text_No_Tip(pDX, IDC_SIZE_HEIGHT, m_height);
        DDX_Text_No_Tip(pDX, IDC_SCALE_WIDTH, m_percent_w);
        DDX_Text_No_Tip(pDX, IDC_SCALE_HEIGHT, m_percent_h);
        DDX_Check(pDX, IDC_KEEP_RATIO, m_keep);
        DDX_Radio(pDX, IDC_STRETCH_NORMAL, m_stretch_type);
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_KEEP_RATIO, 5) ;
        SetControlText(IDC_STR_SCALE_X, 6) ;
        SetControlText(IDC_STR_SCALE_Y, 7) ;
        SetControlText(IDC_STR_WIDTH, 8) ;
        SetControlText(IDC_STR_HEIGHT, 9) ;
        SetControlText(IDC_STR_X_PIXEL, 10) ;
        SetControlText(IDC_STR_Y_PIXEL, 10) ;
        SetControlText(IDC_STR_GROUP, 50) ;
        SetControlText(IDC_STRETCH_NORMAL, 30) ;
        SetControlText(IDC_STRETCH_BILINEAR, 31) ;

        DlgModalBase::OnInitDialog() ;

        CEdit   * p = (CEdit*)GetDlgItem(IDC_SIZE_WIDTH) ;
        if (p)
        {
            p->SetSel(0,-1) ;
            p->SetFocus() ;
        }
        return FALSE ;
    }

    // round double to int
    int FRound (double x)
    {
        if (x > 0.0)
            return (int)(x + 0.5) ;
        else
            return (int)(x - 0.5) ;
    }

    afx_msg void OnChangeWidth();
    afx_msg void OnChangeHeight();
    afx_msg void OnChangeWidthScale();
    afx_msg void OnChangeHeightScale();
	DECLARE_MESSAGE_MAP()
};

//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(DlgImageSize, DlgModalBase)
    ON_EN_CHANGE(IDC_SIZE_WIDTH, OnChangeWidth)
    ON_EN_CHANGE(IDC_SIZE_HEIGHT, OnChangeHeight)
    ON_EN_CHANGE(IDC_SCALE_WIDTH, OnChangeWidthScale)
    ON_EN_CHANGE(IDC_SCALE_HEIGHT, OnChangeHeightScale)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
void DlgImageSize::OnChangeWidth()
{
    UpdateData() ;
    m_percent_w = FRound (100 * m_width / (double)m_old_w) ;
    if (m_keep)
    {
        m_percent_h = m_percent_w ;
        m_height = FRound (m_old_h * m_width / (double)m_old_w) ;
    }
    UpdateData(FALSE) ;
}
void DlgImageSize::OnChangeHeight()
{
    UpdateData() ;
    m_percent_h = FRound (100 * m_height / (double)m_old_h) ;
    if (m_keep)
    {
        m_percent_w = m_percent_h ;
        m_width = FRound (m_old_w * m_height / (double)m_old_h) ;
    }
    UpdateData(FALSE) ;
}
void DlgImageSize::OnChangeWidthScale()
{
    UpdateData() ;
    m_width = FRound (m_old_w * m_percent_w / 100.0) ;
    if (m_keep)
    {
        m_percent_h = m_percent_w ;
        m_height = FRound (m_old_h * m_percent_h / 100.0) ;
    }
    UpdateData(FALSE) ;
}
void DlgImageSize::OnChangeHeightScale()
{
    UpdateData() ;
    m_height = FRound (m_old_h * m_percent_h / 100.0) ;
    if (m_keep)
    {
        m_percent_w = m_percent_h ;
        m_width = FRound (m_old_w * m_percent_w / 100.0) ;
    }
    UpdateData(FALSE) ;
}
//-------------------------------------------------------------------------------------
void DlgImageSize::OnOK()
{
    UpdateData() ;
    if ((m_width <= 0) || (m_height <= 0))
        return ;

    double   nMB = m_width * 4.0 * m_height / (1024*1024) ;

    if (nMB > 300)
    {
        AfxMessageBox(QueryLanguageText(2), MB_ICONWARNING|MB_OK) ;
        return ;
    }
    if (nMB > 100)
    {
        CString   s ;
        s.Format(QueryLanguageText(1), (int)nMB) ;
        if (::AfxMessageBox (s, MB_ICONWARNING|MB_YESNO) != IDYES)
            return ;
    }

    UpdateData(FALSE) ;
    DlgModalBase::OnOK();
}

class CResizeImageCommand : public FCImageEffect
{
    virtual PROCESS_TYPE QueryProcessType()
    {
        return PROCESS_TYPE_WHOLE ;
    }
    virtual void ProcessWholeImage (FCObjImage& img, FCProgressObserver* pProgress)
    {
        img.Stretch_Smooth (m_new_size.cx, m_new_size.cy, pProgress) ;
    }
    CSize   m_new_size ;
public:
    CResizeImageCommand (CSize new_size) : m_new_size(new_size) {}
};

class CDrawTextCommand : public FCImageEffect
{
    virtual PROCESS_TYPE QueryProcessType()
    {
        return PROCESS_TYPE_WHOLE ;
    }
    virtual void ProcessWholeImage (FCObjImage& img, FCProgressObserver* pProgress) ;
public:
    CDrawTextCommand() {}
};
