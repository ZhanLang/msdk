#pragma once

namespace DuiKit{;

struct IDuiSkin;
struct IDuiCore;

enum BuilderCallBack
{
	BuilderCallBack_GetHostWindow, //IIDuiHostWindow
};

struct IDuiBuilderCallBack
{
	virtual LRESULT OnBuilderCallBack( IDuiObject* pObj, int nCallBack) = 0;
};

struct IDuiBuilder : public IDuiObject
{
	virtual BOOL BuildAsFile(LPCWSTR lpszXmlFile, IDuiSkin* pSkin, IDuiObject* pParenCtrl = NULL, IDuiBuilderCallBack* pCallBack = NULL) = 0;
	virtual BOOL BuildAsXml(LPCWSTR lpszXml, IDuiSkin* pSkin,IDuiObject* pParentCtrl = NULL, IDuiBuilderCallBack* pCallBack = NULL) = 0;
	virtual IDuiControlCtrl* GetRootControl() = 0;
	virtual IDuiBuilderCallBack* GetCallBack() = 0;
};

};