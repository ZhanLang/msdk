#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

//-------------------------------------------------------------------------------------
class CMy002App : public CWinApp,
                  public FCAutoInitGDIPlus
{
public:
    CBitmap   m_canvas_back ;

    CString GetText (LPCTSTR strSection, LPCTSTR strKey) ;
    CString GetText (LPCTSTR strSection, int nKey) ;

    void InitMenuLanguageText (HMENU hRootMenu) ;
    CString GetMenuPureText (int nMenuCommand) ;

private:
    CString   m_language_file ;

private:
    void CreateLanguageFile() ;
    void CreateCanvasBack() ;

	virtual BOOL InitInstance();
    virtual int ExitInstance();

    afx_msg void OnOpenHome();
    afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};

extern CMy002App theApp;
