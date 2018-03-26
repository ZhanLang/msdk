
#pragma once

#include <DuiCore/IDuiCore.h>
#include <DuiCore/IDuiSkin.h>
#include <DuiImpl/DuiString.h>
#include "rapidxml/rapidxml.hpp"
namespace DuiKit{;


class CDuiBuilder : public IDuiBuilder
{
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiBuilder, OBJECT_CORE_BUILD)
		DUI_DEFINE_INTERFACE(IDuiBuilder, IIDuiBuilder)
	DUI_END_DEFINE_INTERFACEMAP;
	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue)
	{

	}
	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		return TRUE;
	}
	CDuiBuilder();
	~CDuiBuilder();

	virtual BOOL BuildAsFile(LPCWSTR lpszXmlFile, IDuiSkin* pSkin, IDuiObject* pParenCtrl = NULL, IDuiBuilderCallBack* pCallBack = NULL);
	virtual BOOL BuildAsXml(LPCWSTR lpszXml, IDuiSkin* pSkin,IDuiObject* pParentCtrl = NULL, IDuiBuilderCallBack* pCallBack = NULL);

	virtual IDuiControlCtrl* GetRootControl();
	virtual IDuiBuilderCallBack* GetCallBack();
private:
	BOOL Prase(IDuiObject* pParent);
	BOOL _Prase(IDuiObject* pParent, rapidxml::xml_node<WCHAR>* pNode );
private:
	rapidxml::xml_document<WCHAR> m_buildDocument;

	IDuiSkin* m_pSkin;
	IDuiCore* m_pCore;
	CDuiString m_strContext;
	IDuiControlCtrl* m_pRootCtrl;
	IDuiBuilderCallBack* m_pCallBack;
};

};//namespace DuiKit{;