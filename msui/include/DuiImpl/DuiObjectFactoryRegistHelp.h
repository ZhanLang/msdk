#pragma once

#include <DuiCore/IDuiCore.h>
#include <string>
namespace DuiKit{;

template<typename T>
class CUIObjectFactoryRegistHelp : public IDuiObjectBuilder
{
public:
	CUIObjectFactoryRegistHelp(LPCWSTR lpszObjectClassName)
	{
		m_strObjectClassName = lpszObjectClassName;
		IDuiObjectFactory* pMgr = GetObjectFactory();
		if ( pMgr ){
			pMgr->RegisterObjectBuilder(lpszObjectClassName, static_cast<IDuiObjectBuilder*>(this));
		}
	}

	~CUIObjectFactoryRegistHelp()
	{
		IDuiObjectFactory* pMgr = GetObjectFactory();
		if ( pMgr && m_strObjectClassName.length()){
			pMgr->UnRegisterObjectBuilder(m_strObjectClassName.c_str());
		}
	}

protected:
	virtual IDuiObject* CreateObject()
	{
		T* pNew = new T();
		if ( pNew ){
			return pNew->QueryInterface(IIDuiObject);
		}
		
		return NULL;
	}

	virtual VOID	   DestoryObject(IDuiObject* pObject)
	{
		if( pObject ){
			pObject->DeleteThis();
		}
	}

private:
	std::wstring m_strObjectClassName;
};

};//namespace DuiKit{;