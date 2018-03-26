/*
    Copyright (C) =USTC= Fu Li

    Author   :  Fu Li
    Create   :  2005-7-1
    Home     :  http://www.phoxo.com
    Mail     :  crazybitwps@hotmail.com

    This file is part of UIStone

    The code distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Redistribution and use the source code, with or without modification,
    must retain the above copyright.
*/
#pragma once

//-------------------------------------------------------------------------------------
/**
    Font helper.
*/
class FCFontManager
{
    class CAutoFont
    {
    public:
        HFONT   m_font ;
        CAutoFont()
        {
            NONCLIENTMETRICS   nm = {0} ;
            nm.cbSize = ((sizeof(TCHAR) == 2) ? 500 : 340) ; // avoid effect by WINVER macro
            SystemParametersInfo (SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0) ;

            nm.lfMenuFont.lfHeight = PointSizeToHeight(9) ;

            m_font = ::CreateFontIndirect(&nm.lfMenuFont) ;
        }
        ~CAutoFont()
        {
            ::DeleteObject(m_font) ;
            m_font = NULL ;
        }
    };

    static int PointSizeToHeight (int nPoint)
    {
        HDC   hdc = GetDC(NULL) ;
        int   t = -MulDiv (nPoint, GetDeviceCaps(hdc,LOGPIXELSY), 72) ;
        ReleaseDC(NULL, hdc) ;
        return t ;
    }

public:
    /// Get default UI font (font used by menu), <B>mustn't delete returned font</B>.
    static HFONT GetDefaultFont()
    {
        return GLOBAL_OBJ().m_font ;
    }

    /// Set default UI font.
    static void SetDefaultFont (HFONT hFont)
    {
        ::DeleteObject (GLOBAL_OBJ().m_font) ;
        GLOBAL_OBJ().m_font = hFont ;
    }

private:
    static CAutoFont& GLOBAL_OBJ()
    {
        static CAutoFont   s ;
        return s ;
    }
};
