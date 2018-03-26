#pragma once
#include <map>
#include <string>
#include <DuiCore/IDuiCore.h>
#include <DuiImpl/DuiCriticalsection.h>

using namespace DuiKit;
class CDuiObjectBuilder:
	public IDuiObjectFactory
{
public:
	CDuiObjectBuilder(void);
	~CDuiObjectBuilder(void);

public:
	static CDuiObjectBuilder& GetInstance();
public:
	virtual IDuiObject* CreateObject(LPCWSTR lpszObjectClassName);
	virtual VOID	   DestoryObject(IDuiObject* pObject);

	virtual VOID	   RegisterObjectBuilder(LPCWSTR lpszObjectClassName,IDuiObjectBuilder* pCallBack);
	virtual VOID	   UnRegisterObjectBuilder(LPCWSTR lpszObjectClassName);

private:
	typedef std::map<std::wstring , IDuiObjectBuilder*> CUIObjectFactoryMap;
	CUIObjectFactoryMap m_UIObjectFactoryMap;
	DECLARE_AUTOLOCK_CS(m_UIObjectFactoryMap);
};

