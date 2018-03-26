#pragma once

//-------------------------------------------------------------------------------------
// language support with auto set TITLE, IDOK, IDCANCEL
// slider info bind
class DlgModalBase : public CDialog
{
    struct SLIDER_INFO
    {
        int   m_name_id ;
        int   m_slider_id ;
        int   m_value_id ;
        int   m_min ;
        int   m_max ;
        int   m_current ;
        int   m_name_text ;
    };

public:
    DlgModalBase (LPCWSTR strSection) : m_section(strSection), m_title_menu_cmd(0) {}
    DlgModalBase (LPCWSTR strSection, UINT nDialogID) : CDialog(nDialogID, NULL), m_section(strSection), m_title_menu_cmd(0) {}

    void SetSlider_1 (int nMin, int nMax, int nInit, int nNameText) ;
    void SetSlider_2 (int nMin, int nMax, int nInit, int nNameText) ;
    void SetSlider_3 (int nMin, int nMax, int nInit, int nNameText) ;

    int GetSlider_1() {return GetSlider_n(1);}
    int GetSlider_2() {return GetSlider_n(2);}
    int GetSlider_3() {return GetSlider_n(3);}

    void SetTitleFromMenu (int nMenuCommand) {m_title_menu_cmd=nMenuCommand;}

protected:
    CString QueryLanguageText (int nTextID) const ;

    void SetControlText (int nCtrlID, int nTextID)
    {
        SetDlgItemText(nCtrlID, QueryLanguageText(nTextID)) ;
    }

private:
    CString   m_section ;
    std::deque<SLIDER_INFO>   m_slider ;
    int   m_title_menu_cmd ;

    int GetSlider_n (size_t n) ;

protected:
    virtual BOOL OnInitDialog() ;
    virtual void DoDataExchange(CDataExchange* pDX) ;

    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
};
