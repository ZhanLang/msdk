#pragma once

#include <DuiCore/IDuiLang.h>
#include <DuiImpl/DuiMap.h>
#include <DuiImpl/DuiString.h>
#include <DuiImpl/DuiCriticalsection.h>
using namespace DuiKit;
class CDuiLangSet :
	public IDuiLangSet
{
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiLangSet, OBJECT_CORE_LANG_SET)
		DUI_DEFINE_INTERFACE(IDuiLangSet, IIDuiLangSet)
	DUI_END_DEFINE_INTERFACEMAP;
	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue)
	{

	}
	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		RASSERT(pParent, E_FAIL);

		IDuiSkin* tSkin = (IDuiSkin*)pParent->QueryInterface(IIDuiSkin);
		RASSERT(tSkin, E_FAIL);
		SetDuiSkin(tSkin);
		return S_OK;
	}
public:
	CDuiLangSet();
	~CDuiLangSet(void);

	virtual VOID SetDuiSkin(IDuiSkin* Skin);
	virtual IDuiSkin* GetDuiSkin();

	virtual IDuiLang* GetLang(LPCWSTR lpszLangName);
	virtual VOID AddLang(LPCWSTR lpszLangName, IDuiLang* pLang);
	virtual VOID RemoveLang(LPCWSTR lpszLangName);

	//设置当前语言类型
	virtual LPCWSTR GetText(LPCWSTR lpszLangName, LPCWSTR lpszTextName);

	//表达式获取
	//{lang}.{id}
	//{cur}.{id}
	virtual LPCWSTR GetText(LPCWSTR lpszLangExp);

private:
	typedef CDuiMap<CDuiString,IDuiLang*> CLangMap;
	CLangMap m_langMap;
	DECLARE_AUTOLOCK_CS(m_langMap);
	IDuiSkin* m_pSkin;
};

