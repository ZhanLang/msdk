#include "StdAfx.h"
#include "DuiLang.h"
#include <DuiCore/IDuiSkin.h>



HRESULT CDuiLang::DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
{
	RASSERT( pParent, E_FAIL);
	IDuiSkin* tSkin = (IDuiSkin*)pParent->QueryInterface(IIDuiSkin);
	this->SetDuiSkin(tSkin);
	RASSERT(tSkin, E_FAIL);
	IDuiLangSet* pLangSet = tSkin->GetLangSet();
	RASSERT( pLangSet, E_FAIL);

 	IDuiLang* pLang = pLangSet->GetLang(m_strName);
 	if ( !pLang )
 	{
 		pLangSet->AddLang(m_strName, this);
 		return S_OK;
 	}

	return S_FALSE;
}

CDuiLang::CDuiLang():m_pSkin(NULL)
{
	AUTOLOCK_CS(m_langTextMap);
	m_langTextMap.Clear();
}


CDuiLang::~CDuiLang(void)
{
	AUTOLOCK_CS(m_langTextMap);
	m_langTextMap.Clear();
}

LPCWSTR CDuiLang::GetText(LPCWSTR lpszTextName)
{
	if ( lpszTextName )
	{
		AUTOLOCK_CS(m_langTextMap);
		CLangTextMap::Iterator it = m_langTextMap.Find(lpszTextName);
		if ( it )
		{
			return it->Value;
		}
	}
	

	return NULL;
}

VOID CDuiLang::AddText(LPCWSTR lpszTextName, LPCWSTR lpszText)
{
	if ( lpszTextName && wcslen(lpszTextName) && lpszText)
	{
		AUTOLOCK_CS(m_langTextMap);
		m_langTextMap.Insert(lpszTextName, lpszText);
	}
}

LPCWSTR CDuiLang::GetLangName()
{
	return m_strName;
}

VOID CDuiLang::SetDuiSkin(IDuiSkin* Skin)
{
	m_pSkin = Skin;
}

IDuiSkin* CDuiLang::GetDuiSkin()
{
	return m_pSkin;
}
