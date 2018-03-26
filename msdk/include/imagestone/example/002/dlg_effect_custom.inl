#pragma once

//-------------------------------------------------------------------------------------
class DlgEffectCustom : public DlgEffectBase
{
public:
    DlgEffectCustom (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_CUSTOM", IDD_EFFECT_CUSTOM, pDoc)
    {
        ZeroMemory (m_element, 25*sizeof(int)) ;
        m_element[12] = 1 ;
        m_scale = 1 ;
        m_offset = 0 ;
    }

private:
    int   m_element[25] ;
    int   m_scale ;
    int   m_offset ;

    enum
    {
        IDC_CUSTOM_FILTER0 = 2000,
        IDC_STR_SCALE = 2200,
        IDC_STR_OFFSET = 2201,
        IDC_EDIT_SCALE = 2500,
        IDC_EDIT_OFFSET = 2501,
        ID_LOAD_ACF = 2600,
        ID_SAVE_ACF = 2601,
    };

    virtual FCImageEffect* CreateProcessCommand()
    {
        FCEffectConvolute   * p = new FCEffectConvolute ;
        p->SetKernel (m_element, 5, m_scale, m_offset) ;
        return p ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_SCALE, 1) ;
        SetControlText(IDC_STR_OFFSET, 2) ;
        SetControlText(ID_LOAD_ACF, 3) ;
        SetControlText(ID_SAVE_ACF, 4) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;

        for (int i=0 ; i < 25 ; i++)
        {
            DDX_Text_No_Tip(pDX, IDC_CUSTOM_FILTER0+i, m_element[i]) ;
        }
        DDX_Text_No_Tip(pDX, IDC_EDIT_SCALE, m_scale) ;
        DDX_Text_No_Tip(pDX, IDC_EDIT_OFFSET, m_offset) ;
    }

private:
    static int PS_WORD2INT (INT16 n)
    {
        std::swap (((BYTE*)&n)[0], ((BYTE*)&n)[1]) ;
        return n ;
    }

    static INT16 PS_INT2WORD (int n)
    {
        INT16   t = (INT16)n ;
        std::swap (((BYTE*)&t)[0], ((BYTE*)&t)[1]) ;
        return t ;
    }

    afx_msg void OnLoadACF() ;
    afx_msg void OnSaveACF() ;
    afx_msg void OnInputFilter() ;
    DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(DlgEffectCustom, DlgEffectBase)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+1, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+2, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+3, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+4, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+5, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+6, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+7, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+8, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+9, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+10, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+11, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+12, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+13, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+14, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+15, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+16, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+17, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+18, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+19, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+20, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+21, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+22, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+23, OnInputFilter)
    ON_EN_CHANGE(IDC_CUSTOM_FILTER0+24, OnInputFilter)
    ON_EN_CHANGE(IDC_EDIT_SCALE, OnInputFilter)
    ON_EN_CHANGE(IDC_EDIT_OFFSET, OnInputFilter)
    ON_COMMAND(ID_LOAD_ACF, OnLoadACF)
    ON_COMMAND(ID_SAVE_ACF, OnSaveACF)
END_MESSAGE_MAP()

void DlgEffectCustom::OnInputFilter()
{
    ApplyEffect() ;
}

void DlgEffectCustom::OnLoadACF()
{
    // load ACF file
    CFileDialog  dlg (TRUE, L"acf", NULL, OFN_HIDEREADONLY, L"custom filters (*.ACF)|*.acf||") ;
    if (dlg.DoModal() == IDOK)
    {
        // load filter from .acf
        std::vector<BYTE>   buf ;
        FCFileEx::Read (dlg.GetPathName(), buf) ;

        if (buf.size() == 54)
        {
            INT16   * p = (INT16*)&buf[0] ;

            // 5 x 5 filter & scale & offset
            for (int i=0 ; i < 25 ; i++)
            {
                m_element[i] = PS_WORD2INT(p[i]) ;
            }
            m_scale = PS_WORD2INT(p[25]) ;
            m_offset = PS_WORD2INT(p[26]) ;
            UpdateData(FALSE) ;
            ApplyEffect() ;
        }
    }
}

void DlgEffectCustom::OnSaveACF()
{
    // save ACF file
    CFileDialog  dlg(FALSE, L"acf", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"custom filters (*.ACF)|*.acf||") ;
    if (dlg.DoModal() == IDOK)
    {
        INT16   buf[27] ;
        for (int i=0 ; i < 25 ; i++)
            buf[i] = PS_INT2WORD(m_element[i]) ;
        buf[25] = PS_INT2WORD(m_scale) ;
        buf[26] = PS_INT2WORD(m_offset) ;
        FCFileEx::Write (dlg.GetPathName(), buf, 27*2) ;
    }
}
