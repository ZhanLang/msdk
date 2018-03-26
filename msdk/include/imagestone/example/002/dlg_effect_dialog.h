#pragma once
#include "dlg_effect_base.h"

//-------------------------------------------------------------------------------------
// one set slider + name + value
template<class T>
class DlgEffectSingleSlider : public DlgEffectBase
{
public:
    DlgEffectSingleSlider (LPCWSTR strSection, CMy002Doc* pDoc) : DlgEffectBase(strSection, IDD_SINGLE_SLIDER, pDoc) {}

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new T(GetSlider_1()) ;
    }
};

//-------------------------------------------------------------------------------------
// two set slider + name + value
template<class T>
class DlgEffectTwoSlider : public DlgEffectBase
{
public:
    DlgEffectTwoSlider (LPCWSTR strSection, CMy002Doc* pDoc) : DlgEffectBase(strSection, IDD_TWO_SLIDER, pDoc) {}

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new T(GetSlider_1(), GetSlider_2()) ;
    }
};

//-------------------------------------------------------------------------------------
// three set slider + name + value
template<class T>
class DlgEffectThreeSlider : public DlgEffectBase
{
public:
    DlgEffectThreeSlider (LPCWSTR strSection, CMy002Doc* pDoc) : DlgEffectBase(strSection, IDD_THREE_SLIDER, pDoc) {}

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new T(GetSlider_1(), GetSlider_2(), GetSlider_3()) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectBlurGauss : public DlgEffectBase
{
public:
    DlgEffectBlurGauss (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_BLUR", IDD_SINGLE_SLIDER, pDoc)
    {
        SetSlider_1 (1, 32, 5, 1) ;
    }
private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectBlur_Gauss(GetSlider_1(), true) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectSharpen : public DlgEffectBase
{
    enum
    {
        IDC_TYPE_LOW = 1000,
        IDC_TYPE_NORMAL = 1001,
        IDC_TYPE_HIGH = 1002,
        IDC_STR_GROUP = 1050,
    };

    int   m_level ;

public:
    DlgEffectSharpen (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_SHARP", IDD_EFFECT_SHARP, pDoc)
    {
        m_level = 1 ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        int   n=8 ;
        switch (m_level)
        {
            case 0 : n = 12 ; break ;
            case 1 : n = 8 ; break ;
            case 2 : n = 4 ; break ;
        }
        return new FCEffectSharp(n) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_TYPE_LOW, 0) ;
        SetControlText(IDC_TYPE_NORMAL, 1) ;
        SetControlText(IDC_TYPE_HIGH, 2) ;
        SetControlText(IDC_STR_GROUP, 3) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX);
        DDX_Radio(pDX, IDC_TYPE_LOW, m_level) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectNoisify : public DlgEffectBase
{
    enum
    {
        IDC_RANDOM_NOISIFY = 1000,
    };

    int   m_random ;

public:
    DlgEffectNoisify (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_NOISIFY", IDD_EFFECT_NOISIFY, pDoc)
    {
        SetSlider_1 (1, 100, 50, 0) ;
        m_random = FALSE ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectNoisify (GetSlider_1(), m_random ? true : false) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_RANDOM_NOISIFY, 1) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Check(pDX, IDC_RANDOM_NOISIFY, m_random) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectOilPaint : public DlgEffectBase
{
    enum
    {
        IDC_TYPE_LOW = 1000,
        IDC_TYPE_NORMAL = 1001,
        IDC_TYPE_HIGH = 1002,
        IDC_STR_GROUP = 1050,
    };

    int   m_type ;

public:
    DlgEffectOilPaint (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_OILPAINT", IDD_EFFECT_OILPAINT, pDoc)
    {
        SetSlider_1 (2, 8, 3, 0) ;
        m_type = 1 ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        int   n=30 ;
        switch (m_type)
        {
            case 0 : n = 255 ; break ;
            case 1 : n = 30 ; break ;
            case 2 : n = 3 ; break ;
        }
        return new FCEffectOilPaint (GetSlider_1(), n) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_GROUP, 1) ;
        SetControlText(IDC_TYPE_LOW, 2) ;
        SetControlText(IDC_TYPE_NORMAL, 3) ;
        SetControlText(IDC_TYPE_HIGH, 4) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX);
        DDX_Radio(pDX, IDC_TYPE_LOW, m_type) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectEmboss : public DlgEffectBase
{
    enum
    {
        IDC_TYPE_START = 1000,
        IDC_STR_GROUP = 1050,
    };

    int   m_type ;

public:
    DlgEffectEmboss (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_EMBOSS", IDD_EFFECT_EMBOSS, pDoc)
    {
        m_type = 0 ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectEmboss (m_type * 90) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_TYPE_START, 0) ;
        SetControlText(IDC_TYPE_START+1, 1) ;
        SetControlText(IDC_TYPE_START+2, 2) ;
        SetControlText(IDC_TYPE_START+3, 3) ;
        SetControlText(IDC_STR_GROUP, 4) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX);
        DDX_Radio(pDX, IDC_TYPE_START, m_type) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectTexture : public DlgEffectBase
{
public:
    DlgEffectTexture (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_TEXTURE", IDD_EFFECT_TEXTURE, pDoc)
    {
        SetSlider_1 (20, 300, 100, 0) ;
        SetSlider_2 (1, 100, 100, 1) ;
        m_type = 0 ;
    }

private:
    enum
    {
        IDC_TYPE_START = 2000,
    };

    int   m_type ;

    virtual FCImageEffect* CreateProcessCommand()
    {
        std::deque<UINT>   ls ;
        ls.push_back (IDR_PNG_TEXTURE_BRICKS) ;
        ls.push_back (IDR_PNG_TEXTURE_CANVAS_1) ;
        ls.push_back (IDR_PNG_TEXTURE_CANVAS_2) ;
        ls.push_back (IDR_PNG_TEXTURE_LEATHER) ;
        ls.push_back (IDR_PNG_TEXTURE_SANDSTONE) ;
        ls.push_back (IDR_PNG_TEXTURE_WEAVE) ;
        ls.push_back (IDR_PNG_TEXTURE_MAT) ;
        ls.push_back (IDR_PNG_TEXTURE_GRID) ;
        ls.push_back (IDR_PNG_TEXTURE_DOT) ;

        FCObjImage   * pImg = new FCObjImage ;
        pImg->LoadResource (ls[m_type], L"PNG", IMG_PNG) ;
        pImg->ConvertTo24Bit() ;

        int   nNewW = __max (2, pImg->Width()*GetSlider_1()/100),
              nNewH = __max (2, pImg->Height()*GetSlider_1()/100) ;
        pImg->Stretch_Smooth (nNewW, nNewH) ;

        return new FCEffectFillPattern (pImg, GetSlider_2()*0xFF/100, true) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_TYPE_START, 10) ;
        SetControlText(IDC_TYPE_START+1, 11) ;
        SetControlText(IDC_TYPE_START+2, 12) ;
        SetControlText(IDC_TYPE_START+3, 13) ;
        SetControlText(IDC_TYPE_START+4, 14) ;
        SetControlText(IDC_TYPE_START+5, 15) ;
        SetControlText(IDC_TYPE_START+6, 16) ;
        SetControlText(IDC_TYPE_START+7, 17) ;
        SetControlText(IDC_TYPE_START+8, 18) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Radio(pDX, IDC_TYPE_START, m_type) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffect3DGrid : public DlgEffectBase
{
    int   m_size ;
    int   m_depth ;

public:
    DlgEffect3DGrid (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_3DGRID", IDD_EFFECT_3DGRID, pDoc)
    {
        m_size=0 ; m_depth=0 ;
    }

private:
    enum
    {
        IDC_SIZE_LARGE = 1000,
        IDC_SIZE_MEDIUM = 1001,
        IDC_SIZE_SMALL = 1002,

        IDC_DEPTH_HIGH = 2000,
        IDC_DEPTH_MEDIUM = 2001,
        IDC_DEPTH_LOW = 2002,

        IDC_STR_GROUP_SIZE = 2100,
        IDC_STR_GROUP_DEPTH = 2101,
    };

    virtual FCImageEffect* CreateProcessCommand()
    {
        int   s=32, d=60 ;
        switch (m_size)
        {
            case 0 : s = 64 ; break ;
            case 1 : s = 32 ; break ;
            case 2 : s = 16 ; break ;
        }
        switch (m_depth)
        {
            case 0 : d = 92 ; break ;
            case 1 : d = 60 ; break ;
            case 2 : d = 28 ; break ;
        }
        return new FCEffect3DGrid (s, d) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_GROUP_SIZE, 1) ;
        SetControlText(IDC_STR_GROUP_DEPTH, 2) ;
        SetControlText(IDC_SIZE_LARGE, 3) ;
        SetControlText(IDC_SIZE_MEDIUM, 4) ;
        SetControlText(IDC_SIZE_SMALL, 5) ;
        SetControlText(IDC_DEPTH_HIGH, 6) ;
        SetControlText(IDC_DEPTH_MEDIUM, 7) ;
        SetControlText(IDC_DEPTH_LOW, 8) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX);
        DDX_Radio(pDX, IDC_SIZE_LARGE, m_size) ;
        DDX_Radio(pDX, IDC_DEPTH_HIGH, m_depth) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectRipple : public DlgEffectBase
{
    enum
    {
        IDC_TYPE_TRI = 2000,
        IDC_TYPE_SIN = 2001,
    };

    int   m_type ;

public:
    DlgEffectRipple (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_RIPPLE", IDD_EFFECT_RIPPLE, pDoc)
    {
        SetSlider_1 (1, 100, 30, 0) ;
        SetSlider_2 (1, 100, 10, 1) ;
        m_type = 1 ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectRipple (GetSlider_1(), GetSlider_2(), m_type ? true : false) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_TYPE_TRI, 2) ;
        SetControlText(IDC_TYPE_SIN, 3) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX) ;
        DDX_Radio(pDX, IDC_TYPE_TRI, m_type) ;
    }
};
//-------------------------------------------------------------------------------------
class DlgEffectVideo : public DlgEffectBase
{
    enum
    {
        IDC_TYPE_START = 1000,
        IDC_STR_GROUP = 1050,
    };

    int   m_type ;

public:
    DlgEffectVideo (CMy002Doc* pDoc) : DlgEffectBase(L"IDD_EFFECT_VIDEO", IDD_EFFECT_VIDEO, pDoc)
    {
        m_type=0 ;
    }

private:
    virtual FCImageEffect* CreateProcessCommand()
    {
        return new FCEffectVideo ((FCEffectVideo::VIDEO_TYPE)m_type) ;
    }

    virtual BOOL OnInitDialog()
    {
        SetControlText(IDC_STR_GROUP, 0) ;
        SetControlText(IDC_TYPE_START, 1) ;
        SetControlText(IDC_TYPE_START+1, 2) ;
        SetControlText(IDC_TYPE_START+2, 3) ;
        SetControlText(IDC_TYPE_START+3, 4) ;
        return DlgEffectBase::OnInitDialog() ;
    }

    virtual void DoDataExchange(CDataExchange* pDX)
    {
        DlgEffectBase::DoDataExchange(pDX);
        DDX_Radio(pDX, IDC_TYPE_START, m_type) ;
    }
};