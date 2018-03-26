// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "DuiCore.h"
#include "DuiObjectBuilder.h"
using namespace DuiKit;


#include <CtrlImpl/DuiControlCtrl.h>
DEFINE_OBJECT(CDuiControlCtrl, CTRL_CONTROL);

#include <CtrlImpl/DuiContainerCtrl.h>
DEFINE_OBJECT(CDuiContainerCtrl, CTRL_CONTAINER);

#include <CtrlImpl/DuiWindowCtrl.h>
DEFINE_OBJECT(CDuiWindowCtrl, CTRL_WINDOW);

#include <CtrlImpl/DuiHostWindowCtrl.h>
DEFINE_OBJECT(CDuiHostWindowCtrl, CTRL_HOST_WINDOW);

#include "DuiBuilder.h"

#include <CtrlImpl/DuiVerticalLayoutCtrl.h>
DEFINE_OBJECT(CDuiVerticalLayoutCtrl, CTRL_VERTICALLAYOUT);

#include <CtrlImpl/DuiHorizontalLayoutCtrl.h>
DEFINE_OBJECT(CDuiHorizontalLayoutCtrl, CTRL_HORIZONTALLAYOUT);

#include <CtrlImpl/DuiLabelCtrl.h>
DEFINE_OBJECT(CDuiLabelCtrl, CTRL_LABEL);

#include <CtrlImpl/DuiButtonCtrl.h>
DEFINE_OBJECT(CDuiButtonCtrl, CTRL_BUTTON);

#include <CtrlImpl/DuiOptionCtrl.h>
DEFINE_OBJECT(CDuiOptionCtrl, CTRL_OPTION);

#include <CtrlImpl/DuiProgressCtrl.h>
DEFINE_OBJECT(CDuiProgressCtrl, CTRL_PROGRESS);

#include<CtrlImpl/DuiTextCtrl.h>
DEFINE_OBJECT(CDuiTextCtrl, CTRL_TEXT);

#include <CtrlImpl/DuiTabLayoutCtrl.h>
DEFINE_OBJECT(CDuiTableLayoutCtrl, CTRL_TABLELAYOUT);

#include <CtrlImpl/DuiEditCtrl.h>
DEFINE_OBJECT(CDuiEditCtrl, CTRL_EDIT)

#include <CtrlImpl/DuiGroupCtrl.h>
DEFINE_OBJECT(CDuiGroupCtrl, CTRL_GROUP)

#include <CtrlImpl/DuiPictureCtrl.h>
DEFINE_OBJECT(CDuiPictureCtrl, CTRL_PICTURE)

#include "CtrlImpl/DuiGifCtrl.h"
DEFINE_OBJECT(CDuiGifCtrl, CTRL_GIF)


#include "CtrlImpl/DuiScrollCtrl.h"
DEFINE_OBJECT(CDuiScrollCtrl, CTRL_SCROLL)


#include "DuiSkinFile.h"

DEFINE_OBJECT(CDuiSkinFile, OBJECT_CORE_SKIN_FILE);

#include "DuiFont.h"

DEFINE_OBJECT(CDuiFont, OBJECT_CORE_FONT)

#include "DuiLang.h"

DEFINE_OBJECT(CDuiLang,OBJECT_CORE_LANG);

#include "DuiLangSet.h"

DEFINE_OBJECT(CDuiLangSet,OBJECT_CORE_LANG_SET);

DEFINE_OBJECT(CDuiBuilder, OBJECT_CORE_BUILD);

#include "DuiByteStream.h"

DEFINE_OBJECT(CDuiByteStream, OBJECT_CORE_BYTE_STREAM);

#include "DuiImage.h"

DEFINE_OBJECT(CDuiImage, OBJECT_CORE_IMAGE)

#include "DuiLangText.h"
DEFINE_OBJECT(CDuiLangText, OBJECT_CORE_LANGTEXT)


#include "CtrlImpl\DuiThemeCtrl.h"
DEFINE_OBJECT(CDuiThemeCtrl, CTRL_Theme)

#include "CtrlImpl/DuiRichEditCtrl.h"
//DEFINE_OBJECT(CDuiRichEditCtrl, CTRL_RichEdit)

extern "C"
{
	DuiKit::IDuiObjectFactory*  GetObjectFactory()
	{
		return static_cast<DuiKit::IDuiObjectFactory*>(&CDuiObjectBuilder::GetInstance());
	}

	DuiKit::IDuiObject*  CreateObject(LPCWSTR lpszObjectClassName)
	{
		return CDuiObjectBuilder::GetInstance().CreateObject(lpszObjectClassName);
	}

	VOID DestoryObject(DuiKit::IDuiObject* pObject)
	{
		CDuiObjectBuilder::GetInstance().DestoryObject(pObject);
	}


	VOID DuiKitDllMaim(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
	{
		if ( dwReason == DLL_PROCESS_ATTACH)
		{
			
		}
	}

};