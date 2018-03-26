#include "stdafx.h"
#include "002.h"
#include "MainFrm.h"
#include "002Doc.h"
#include "002View.h"
#include "dlg_open_image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMy002App theApp;

//-------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMy002App, CWinApp)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_VIEW_HOME, OnOpenHome)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()
//-------------------------------------------------------------------------------------
BOOL CMy002App::InitInstance()
{
    srand((UINT)GetTickCount()) ;

    CreateLanguageFile() ;
    CreateCanvasBack() ;

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_My002TYPE,
		RUNTIME_CLASS(CMy002Doc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CMy002View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

    InitMenuLanguageText (pDocTemplate->m_hMenuShared) ;

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
    m_pMainWnd->DragAcceptFiles (TRUE) ;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing ; // close first window

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(SW_MAXIMIZE);
	pMainFrame->UpdateWindow();

	return TRUE;
}
//-------------------------------------------------------------------------------------
int CMy002App::ExitInstance()
{
    DeleteFile(m_language_file) ;
    m_canvas_back.DeleteObject() ;
    return CWinApp::ExitInstance() ;
}
//-------------------------------------------------------------------------------------
void CMy002App::CreateLanguageFile()
{
    HMODULE  hModule = ::GetModuleHandle(NULL) ;
    HRSRC    hRes = ::FindResource (hModule, MAKEINTRESOURCE(IDR_LANGUAGE_FILE), L"INI") ;
    DWORD    nSize = ::SizeofResource (hModule, hRes) ;
    void     * pData = ::LockResource (::LoadResource (hModule, hRes)) ;

    m_language_file = FCFileEx::QueryTempFilePath() ;
    FCFileEx::Write (m_language_file, pData, nSize) ;
}
//-------------------------------------------------------------------------------------
CString CMy002App::GetText (LPCTSTR strSection, LPCTSTR strKey)
{
    CString   s ;
    FCFileEx::INIRead (m_language_file, strKey, s, strSection) ;
    s.Replace (L"\\n", L"\n") ;
    return s ;
}
CString CMy002App::GetText (LPCTSTR strSection, int nKey)
{
    CString   s ;
    s.Format (L"%d", nKey) ;
    return GetText (strSection, s) ;
}
//-------------------------------------------------------------------------------------
CString CMy002App::GetMenuPureText (int nMenuCommand)
{
    CString   s = GetText(L"MENU_TEXT", nMenuCommand) ;

    int   n = s.Find(L"(&") ;
    if (n != -1)
    {
        s = s.Left(n) ;
    }

    n = s.Find(L"\\t") ;
    if (n != -1)
    {
        s = s.Left(n) ;
    }

    s.Replace (L"...", L"") ;
    s.Replace (L"&", L"") ;
    return s ;
}
//-------------------------------------------------------------------------------------
void CMy002App::CreateCanvasBack()
{
    m_canvas_back.DeleteObject() ;

    CClientDC   screen_dc(NULL) ;
    m_canvas_back.CreateCompatibleBitmap(&screen_dc, 32, 32) ;

    FCObjImage   img ;
    img.Create (32, 32, 24) ;
    img.ApplyEffect (FCEffectFillGrid(FCColor(192,192,192), FCColor(0xFF,0xFF,0xFF), 16)) ;

    img.Draw (FCImageDrawDC(m_canvas_back), 0, 0) ;
}
//-------------------------------------------------------------------------------------
void CMy002App::OnFileOpen() 
{
    DlgOpenImage   dlg ;
    if (dlg.DoModal() == IDOK)
    {
        CWinApp::OpenDocumentFile (dlg.GetPathName()) ;
    }
}
//-------------------------------------------------------------------------------------
void CMy002App::OnOpenHome() 
{
    ::ShellExecute (NULL, L"open", L"http://www.phoxo.com/en/", NULL, NULL, SW_SHOW) ;
}
//-------------------------------------------------------------------------------------
void CMy002App::InitMenuLanguageText (HMENU hRootMenu)
{
    std::deque<CMenu*>   ls ;
    ls.push_back (CMenu::FromHandle(hRootMenu)) ;

    while (ls.size())
    {
        CMenu   * pMenu = ls.back() ;
        ls.pop_back() ;

        for (UINT i=0 ; i < pMenu->GetMenuItemCount() ; i++)
        {
            CMenu     * pSub = pMenu->GetSubMenu(i) ;
            UINT      nID = pMenu->GetMenuItemID(i) ;
            CString   strNew ;
            if (pSub)
            {
                // pop menu, no ID
                CString   s ;
                pMenu->GetMenuString(i, s, MF_BYPOSITION) ;
                strNew = GetText(L"MENU_TEXT", s) ;
                ls.push_back(pSub) ;
            }
            else
            {
                strNew = GetText(L"MENU_TEXT", nID) ;
                strNew.Replace (L"\\t", L"\t") ;
            }

            if (strNew.GetLength())
            {
                pMenu->ModifyMenu(i, MF_STRING|MF_BYPOSITION, nID, strNew) ;
            }
        }
    }
}
