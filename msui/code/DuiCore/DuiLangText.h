#pragma once

#include <DuiImpl/DuiString.h>

#include <DuiCore/IDuiLang.h>
#include <DuiCore/IDuiSkin.h>
using namespace DuiKit;
class CDuiLangText
	:public IDuiObject
{
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiObject, OBJECT_CORE_LANGTEXT)
	DUI_END_DEFINE_INTERFACEMAP;
	DUI_BEGIN_SETATTRIBUTE(CDuiLangText)
		DUI_SETATTRIBUTE_STRING(name, m_strName);
		DUI_SETATTRIBUTE_STRING(value, m_strValue);
	DUI_END_SETATTRIBUTE;

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		RASSERT(pParent, FALSE);
		IDuiLang* tLang = (IDuiLang*)pParent->QueryInterface(IIDuiLang);
		RASSERT(tLang, FALSE);

		IDuiSkin* tSkin = tLang->GetDuiSkin();
		RASSERT(tSkin, FALSE);

		IDuiLangSet* pLangSet = tSkin->GetLangSet();
		RASSERT(pLangSet, FALSE);

	
		LPCWSTR lpszLangName = tLang->GetLangName();
		
		if ( pLangSet )
		{
			IDuiLang* _tLang = pLangSet->GetLang(lpszLangName);
			if ( tLang )
			{
				_tLang->AddText(m_strName, m_strValue);
			}
			else
			{
				tLang->AddText(m_strName, m_strValue);
			}
		}
				
		return FALSE;
	}

public:
	CDuiLangText();
	~CDuiLangText(void);

private:
	CDuiString m_strName;
	CDuiString m_strValue;
	
};

