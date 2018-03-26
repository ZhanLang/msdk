// DuiTest.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DuiTest.h"

#include "DuiApi.h"
#include "DuiCore\IDuiSkin.h"
#include <DuiImpl\DuiString.h>
#include <DuiCore\IDuiLang.h>
#include <Objbase.h>
using namespace DuiKit;

//处理全局事件
class CDuiRoutedEventHandler : public IDuiRoutedEventHandler
{
	virtual BOOL OnRoutedEvent(const DuiEvent& event)
	{
		switch(event.nEvent)
		{
		case DuiEvent_Timer:
			{
				UINT nTimerID = event.wParam;
				break;
			}
		}
		return TRUE;
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{


	CoInitializeEx(NULL,COINIT_MULTITHREADED );
	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);
	//AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
	INT n = CLAMP(1,4,3);

	CDuiObjectPtr<IDuiCore, IIDuiCore> pCore = CreateObject(OBJECT_CORE);
	if ( !pCore )
	{
		return FALSE;
	}


	CDuiObjectPtr<IDuiSkin, IIDuiSkin> pDuiFileSkin = CreateObject(OBJECT_CORE_SKIN_FILE);
	if ( !pDuiFileSkin )
	{
		return FALSE;
	}
	pDuiFileSkin->SetName(L"mainframe");
	pDuiFileSkin->SetCore(pCore);

	CDuiString sFileSkin;
	GetModuleFileName(NULL, sFileSkin.GetBufferSetLength(MAX_PATH), MAX_PATH);
	sFileSkin.ReleaseBuffer();
	sFileSkin = sFileSkin.Left(sFileSkin.ReverseFind('\\'));
	sFileSkin.Append(_T("\\Skin\\"));
	pDuiFileSkin->Open(sFileSkin.GetBuffer(), sFileSkin.Length());
	
	CDuiObjectPtr<IDuiBuilder, IIDuiBuilder> pBuild = CreateObject(OBJECT_CORE_BUILD);
	pBuild->BuildAsFile(_T("skin.xml"), pDuiFileSkin, NULL);

	DuiMsg duiMsg = { 0 };
	duiMsg.CtrlTo = pBuild->GetRootControl();
	duiMsg.CtrlTo->SetTimer(100, 1000);
	CDuiRoutedEventHandler RoutedEventHandler ;
	duiMsg.CtrlTo->AddRoutedEventHandler(&RoutedEventHandler);
	duiMsg.nMsg = 100;

	IDuiWindowCtrl* tWnd = (IDuiWindowCtrl*)pBuild->GetRootControl()->QueryInterface(IIDuiWindowCtrl);
	tWnd->CenterWindow();
	//duiMsg.CtrlTo->SetWidth(1000);
	//pCore->PostMessage(duiMsg);
	pCore->MessageLoop();
	//CDuiString ss = pCore->GetLangSet()->GetLang(L"zh")->GetText(L"lang.2");
	
	pDuiFileSkin->DeleteThis();
	pCore->DeleteThis();
	return (int) NULL;
}