#include "StdAfx.h"
#include "DuiObjectBuilder.h"


CDuiObjectBuilder::CDuiObjectBuilder(void)
{
}


CDuiObjectBuilder::~CDuiObjectBuilder(void)
{
}

VOID CDuiObjectBuilder::RegisterObjectBuilder(LPCWSTR lpszObjectClassName, IDuiObjectBuilder* pCallBack)
{
	if ( pCallBack )
	{
		AUTOLOCK_CS(m_UIObjectFactoryMap);
		m_UIObjectFactoryMap.insert(m_UIObjectFactoryMap.end(), CUIObjectFactoryMap::value_type(lpszObjectClassName, pCallBack));
	}
}

VOID CDuiObjectBuilder::UnRegisterObjectBuilder(LPCWSTR lpszObjectClassName)
{
	if ( lpszObjectClassName )
	{
		m_UIObjectFactoryMap.erase(lpszObjectClassName);
	}
}

IDuiObject* CDuiObjectBuilder::CreateObject(LPCWSTR lpszObjectClassName)
{
	IDuiObject* pObject = NULL;
	IDuiObjectBuilder* pFactory = NULL;
	if ( lpszObjectClassName && wcslen(lpszObjectClassName)){
		AUTOLOCK_CS(m_UIObjectFactoryMap);
		CUIObjectFactoryMap::iterator it = m_UIObjectFactoryMap.find(lpszObjectClassName);
		if (it != m_UIObjectFactoryMap.end())
		{
			pObject = it->second->CreateObject();
		} 
	}
	

	return pObject;
}

VOID CDuiObjectBuilder::DestoryObject(IDuiObject* pObject)
{
	if ( pObject ){
		AUTOLOCK_CS(m_UIObjectFactoryMap);
		CUIObjectFactoryMap::iterator it = m_UIObjectFactoryMap.find(pObject->GetObjectClassName());
		if ( it != m_UIObjectFactoryMap.end()){
			it->second->DestoryObject(pObject);
		}else{
			delete pObject;
		}
	}
}

CDuiObjectBuilder& CDuiObjectBuilder::GetInstance()
{
	static CDuiObjectBuilder ___g_CDuiObjectFactoryMgr;
	return ___g_CDuiObjectFactoryMgr;
}
