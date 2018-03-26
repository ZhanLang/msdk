//
// Copyright (C) www.phoxo.com
//

#ifdef _MSC_VER
    #define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <time.h>

#if (defined(_WIN32) || defined(__WIN32__))
    #ifdef __MINGW32__
        #define WINVER 0x0500
    #endif
#else

    #include "FreeImage/Dist/FreeImage.h"

#endif

#define IMAGESTONE_USE_EXT_EFFECT
#include "../../ImageStone.h"

// operation menu
const char * szMenu = "please enter a number to choice a effect.\n\n\
    1) invert image color\n\
    2) gray scale\n\
    3) threshold\n\
    4) flip\n\
    5) emboss\n\
    6) splash\n\
    7) mosaic\n\
    8) oil paint\n\
    9) add 3D grid\n\
    10) bulge\n\
    11) gradient fill radial\n\
    12) adjust gamma\n\
    13) rotate 90'\n\
    14) ribbon\n\
    15) halftone\n\
    16) lens flare\n\
    17) adjust saturation\n\
    18) gauss blur\n\
    19) zoom blur\n\
    20) radial blur\n\
    21) motion blur\n\
    22) add shadow\n\
    23) color tone\n\
    24) soft glow\n\
    25) tile reflection\n\
" ;
//-------------------------------------------------------------------------------------
class FCShowProgress : public FCProgressObserver
{
    int   m_last ;
public :
    FCShowProgress()
    {
        m_last = 0 ;
    }
    virtual bool OnProgressUpdate (int nFinishPercentage)
    {
        if (nFinishPercentage - m_last >= 2)
        {
            printf ("#") ;
            m_last = nFinishPercentage ;
        }
        return true ;
    }
};
//-------------------------------------------------------------------------------------
std::wstring STR_A2W (const char* p)
{
    std::wstring   ws ;
    if (p)
    {
#if (defined(_WIN32) || defined(__WIN32__))
        int   nCount = MultiByteToWideChar(CP_THREAD_ACP, 0, p, -1, NULL, 0) ;
        if (nCount)
        {
            std::vector<wchar_t>   wps (nCount+1) ;
            MultiByteToWideChar(CP_THREAD_ACP, 0, p, -1, &wps[0], nCount) ;
            ws = &wps[0] ;
        }
#else
        size_t   nCount = strlen(p) ;
        std::vector<wchar_t>   wps (nCount+1) ;
        setlocale (LC_CTYPE, "") ;
        mbstowcs (&wps[0], p, nCount) ;
        ws = &wps[0] ;
#endif
    }
    return ws ;
}
//-------------------------------------------------------------------------------------
bool IsImageExist (const char* file_name)
{
    FILE   * pf = fopen (file_name, "rb") ;
    if (pf)
    {
        fclose(pf) ;
    }
    return (pf != 0) ;
}
//-------------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
    srand((unsigned int)time(0)) ;

    const char* szTestFile = "test.jpg" ; // image to load
    const char* szSaveFile = "save.jpg" ; // image to save

    // not found
    if (!IsImageExist(szTestFile))
    {
        printf ("can't load %s, please put an image named %s in same folder to binary file.\n", szTestFile, szTestFile) ;
        return 0 ;
    }

    // load test image
    FCObjImage   img ;
    if (!img.Load (STR_A2W(szTestFile).c_str()))
    {
        printf ("load %s failed, not a valid image file.\n", szTestFile) ;
        return 0 ;
    }

    printf ("now, the %s image has been loaded successfully!\n\n", szTestFile) ;
    printf ("image's information:\n") ;
    printf ("width   : %d\n", img.Width()) ;
    printf ("height  : %d\n", img.Height()) ;

    img.ConvertTo32Bit() ;

    // print menu && choice a effect
    char     szInput[255] = {0} ;
    printf ("%s", szMenu) ;
    scanf ("%s", szInput) ;

    FCImageEffect   * pEffect = 0 ;
    switch (atoi(szInput))
    {
        case 1 : pEffect = new FCEffectInvert() ; break;
        case 2 : pEffect = new FCEffectGrayscale() ; break;
        case 3 : pEffect = new FCEffectThreshold(100) ; break;
        case 4 : pEffect = new FCEffectFlip() ; break;
        case 5 : pEffect = new FCEffectEmboss(0) ; break;
        case 6 : pEffect = new FCEffectSplash(20) ; break;
        case 7 : pEffect = new FCEffectMosaic(20) ; break;
        case 8 : pEffect = new FCEffectOilPaint(3,50) ; break;
        case 9 : pEffect = new FCEffect3DGrid(16,100) ; break;
        case 10 : pEffect = new FCEffectBulge(100) ; break;
        case 11 :
            {
                RECT    rcEllipse = {0,0,img.Width(),img.Height()} ;
                RGBQUAD crStart = FCColor(255,255,255), crEnd = FCColor(255,0,0) ;
                pEffect = new FCEffectGradientRadial(rcEllipse, crStart, crEnd) ;
            }
            break;
        case 12 : pEffect = new FCEffectGamma(200) ; break;
        case 13 : pEffect = new FCEffectRotate90() ; break;
        case 14 : pEffect = new FCEffectRibbon(35,25) ; break;
        case 15 : pEffect = new FCEffectHalftoneM3() ; break;
        case 16 :
            {
                POINT   pt = {100, 100} ;
                pEffect = new FCEffectLensFlare(pt) ;
            }
            break;
        case 17 : pEffect = new FCEffectHueSaturation(0,50) ; break;
        case 18 : pEffect = new FCEffectBlur_Gauss(8,true) ; break;
        case 19 : pEffect = new FCEffectBlur_Zoom(30) ; break;
        case 20 : pEffect = new FCEffectBlur_Radial(10) ; break;
        case 21 : pEffect = new FCEffectBlur_Motion(30, 45) ; break;
        case 22 :
            {
                SHADOWDATA    ShData ;
                ShData.m_color = FCColor(0,0,0) ;
                ShData.m_opacity = 75 ;
                ShData.m_smooth = 10 ;
                ShData.m_offset_x = 5 ;
                ShData.m_offset_y = 5 ;
                pEffect = new FCEffectAddShadow(ShData) ;
                ((FCEffectAddShadow*)pEffect)->SetPadding(16) ;
            }
            break;
        case 23 : pEffect = new FCEffectColorTone(FCColor(254,168,33), 192) ; break;
        case 24 : pEffect = new FCEffectSoftGlow(10, -40, 10) ; break;
        case 25 : pEffect = new FCEffectTileReflection(20, 8) ; break;
        default :
            printf ("choice invalid. quit do nothing.\n") ;
            return 0 ;
    }

    // to show progress, it's obvious in large image and slower algorithm (such as : OilPaint)
    FCShowProgress     showPro ;
    printf ("\ncurrent progress : ") ;
    img.ApplyEffect (*pEffect, &showPro) ;
    printf ("\n\n") ;
    delete pEffect ;

    // save image
    FCEffectFillBackGround   t2 (FCColor(255,255,255)) ;
    img.ApplyEffect(t2) ;
    img.ConvertTo24Bit() ;
    img.Save (STR_A2W(szSaveFile).c_str()) ;
    printf ("the effected image has been saved into %s !\n\n", szSaveFile) ;

    return 0 ;
}
