#pragma once
#include "wnd_palette_slider.h"

//-------------------------------------------------------------------------------------
class CWndTransparencySlider : public CWndPaletteSlider
{
public:
    CWndTransparencySlider (int nMin, int nMax, int nInit) : CWndPaletteSlider(nMin, nMax, nInit)
    {
    }

private:
    virtual void OnDrawSliderImage (CDC* pDC, CRect rc)
    {
        // draw transparency grid
        FCObjImage   t ;
        t.Create (rc.Width(), rc.Height(), 24) ;

        FCEffectFillGrid   c (FCColor(255,255,255), FCColor(192,192,192), 4) ;
        t.ApplyEffect(c) ;
        t.Draw (*pDC, rc) ;

        // draw pos
        if (GetMax())
        {
            rc.right = rc.left + rc.Width() * GetPos() / GetMax() ;
            pDC->FillSolidRect (rc, RGB(51,153,255)) ; // RGB(0,84,227)
        }
    }
};
