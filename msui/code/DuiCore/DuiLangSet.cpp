#include "StdAfx.h"
#include "DuiLangSet.h"



CDuiLangSet::CDuiLangSet():
m_pSkin(NULL)
{
}

CDuiLangSet::~CDuiLangSet(void)
{
	AUTOLOCK_CS(m_langMap);
	for ( CLangMap::Iterator it = m_langMap.Min(); it;  it++)
	{
		IDuiLang* pLang = it->Value;
		if ( pLang )
		{
			pLang->DeleteThis();
		}
	}
	m_langMap.Clear();

}

IDuiLang* CDuiLangSet::GetLang(LPCWSTR lpszLangName)
{
	if ( lpszLangName )
	{
		AUTOLOCK_CS(m_langMap);
		CLangMap::Iterator it = m_langMap.Find(lpszLangName);
		if ( it )
		{
			return it->Value;
		}
	}

	return NULL;
}

VOID CDuiLangSet::AddLang(LPCWSTR lpszLangName, IDuiLang* pLang)
{
	if ( lpszLangName && wcslen(lpszLangName) && pLang)
	{
		AUTOLOCK_CS(m_langMap);
		CLangMap::Iterator itLang = m_langMap.Find(lpszLangName);
		if ( itLang && itLang->Value)
		{
//			GrpMsg(GroupName, MsgLevel_Msg, _T("FILE: [%s] LINE: [%d] The lang:%s is exist , it will be replace." ), __TFILE__, __LINE__, lpszLangName);
			itLang->Value->DeleteThis();
			m_langMap.Remove(lpszLangName);
		}
		
		m_langMap.Insert(lpszLangName, pLang);
	}
}

VOID CDuiLangSet::RemoveLang(LPCWSTR lpszLangName)
{
	if ( lpszLangName && wcslen(lpszLangName))
	{
		AUTOLOCK_CS(m_langMap);
		CLangMap::Iterator it = m_langMap.Find(lpszLangName);
		if ( it && it->Value)
		{
			it->Value->DeleteThis();
		}

		m_langMap.Remove(lpszLangName);
	}
}



LPCWSTR CDuiLangSet::GetText(LPCWSTR lpszLangName, LPCWSTR lpszTextName)
{
	IDuiLang* pLang = GetLang(lpszLangName);
	if ( pLang )
	{
		return pLang->GetText(lpszTextName);
	}

	return NULL;
}

LPCWSTR CDuiLangSet::GetText(LPCWSTR lpszLangExp)
{
	if ( lpszLangExp && wcslen(lpszLangExp))
	{
		CDuiString szLangName ;
		CDuiString szTextName;

		int n = swscanf_s(lpszLangExp,L"{ %[^}] }.{ %[^}] }",szLangName.GetBufferSetLength(MAX_PATH),MAX_PATH, szTextName.GetBufferSetLength(MAX_PATH), MAX_PATH);
		szLangName.ReleaseBuffer();
		szTextName.ReleaseBuffer();

		if ( n != 2)
			return NULL;
		
		if ( szLangName.Compare(L"*") == 0 )
		{
			//szLangName = GetCurrentLang();
		}

		return GetText(szLangName, szTextName);
	}

	return NULL;
}

VOID CDuiLangSet::SetDuiSkin(IDuiSkin* Skin)
{
	m_pSkin = Skin;
}

IDuiSkin* CDuiLangSet::GetDuiSkin()
{
	return m_pSkin;
}


