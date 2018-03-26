#pragma once
#include <DuiCore/IDuiLang.h>
#include <DuiImpl/DuiMap.h>
#include <DuiImpl/DuiString.h>
#include <DuiImpl/DuiCriticalsection.h>
using namespace DuiKit;
class CDuiLang :
	public IDuiLang
{
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiLang, OBJECT_CORE_LANG)
		DUI_DEFINE_INTERFACE(IDuiLang, IIDuiLang)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiLang)
		DUI_SETATTRIBUTE_STRING(name, m_strName);
	DUI_END_SETATTRIBUTE;

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder);
public:
	CDuiLang();
	~CDuiLang(void);

	virtual VOID SetDuiSkin(IDuiSkin* Skin);
	virtual IDuiSkin* GetDuiSkin();

	virtual LPCWSTR GetLangName();
	virtual LPCWSTR GetText(LPCWSTR lpszTextName);
	virtual VOID	AddText(LPCWSTR lpszTextName, LPCWSTR lpszText);

private:
	typedef CDuiMap<CDuiString, CDuiString>	CLangTextMap;
	CLangTextMap m_langTextMap;
	DECLARE_AUTOLOCK_CS(m_langTextMap);

	CDuiString m_strName;
	IDuiSkin*	m_pSkin;
};

