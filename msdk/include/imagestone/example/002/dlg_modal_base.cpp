#include "stdafx.h"
#include "002.h"
#include "dlg_modal_base.h"

//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(DlgModalBase, CDialog)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
void DlgModalBase::SetSlider_1 (int nMin, int nMax, int nInit, int nNameText)
{
    SLIDER_INFO   t = {IDC_SLIDER_NAME_1, IDC_SLIDER_1, IDC_SLIDER_VALUE_1, nMin, nMax, FClamp(nInit, nMin, nMax), nNameText} ;
    m_slider.push_back(t) ;
}
void DlgModalBase::SetSlider_2 (int nMin, int nMax, int nInit, int nNameText)
{
    SLIDER_INFO   t = {IDC_SLIDER_NAME_2, IDC_SLIDER_2, IDC_SLIDER_VALUE_2, nMin, nMax, FClamp(nInit, nMin, nMax), nNameText} ;
    m_slider.push_back(t) ;
}
void DlgModalBase::SetSlider_3 (int nMin, int nMax, int nInit, int nNameText)
{
    SLIDER_INFO   t = {IDC_SLIDER_NAME_3, IDC_SLIDER_3, IDC_SLIDER_VALUE_3, nMin, nMax, FClamp(nInit, nMin, nMax), nNameText} ;
    m_slider.push_back(t) ;
}
//-------------------------------------------------------------------------------------
int DlgModalBase::GetSlider_n (size_t n)
{
    if (n <= m_slider.size())
        return m_slider[n-1].m_current ;
    ASSERT(FALSE) ;
    return 0 ;
}
//-------------------------------------------------------------------------------------
CString DlgModalBase::QueryLanguageText (int nTextID) const
{
    return theApp.GetText(m_section, nTextID) ;
}
//-------------------------------------------------------------------------------------
BOOL DlgModalBase::OnInitDialog()
{
    // set slider range & initialize
    for (size_t i=0 ; i < m_slider.size() ; i++)
    {
        SLIDER_INFO   & r = m_slider[i] ;
        CSliderCtrl   * pSlider = (CSliderCtrl*)GetDlgItem(r.m_slider_id) ;
        if (pSlider)
        {
            SetControlText (r.m_name_id, r.m_name_text) ;
            pSlider->SetRange (r.m_min, r.m_max, TRUE) ;
            pSlider->SetPageSize (__max(2, (r.m_max - r.m_min) / 10)) ;
        }
    }

    // title from menu text
    if (m_title_menu_cmd)
    {
        SetWindowText (theApp.GetMenuPureText(m_title_menu_cmd)) ;
    }

    // set title, IDOK, IDCANCEL
    SetDlgItemText (IDOK, theApp.GetText(L"app", L"IDOK")) ;
    SetDlgItemText (IDCANCEL, theApp.GetText(L"app", L"IDCANCEL")) ;

    return __super::OnInitDialog() ;
}
//-------------------------------------------------------------------------------------
void DlgModalBase::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX) ;

    for (size_t i=0 ; i < m_slider.size() ; i++)
    {
        SLIDER_INFO   & r = m_slider[i] ;
        DDX_Text(pDX, r.m_value_id, r.m_current) ;
        DDX_Slider(pDX, r.m_slider_id, r.m_current) ;
    }
}
//-------------------------------------------------------------------------------------
void DlgModalBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    __super::OnHScroll(nSBCode, nPos, pScrollBar);
    UpdateData() ;
    UpdateData(FALSE) ;
}
