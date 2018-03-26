#pragma once

#ifdef MSUI_STATIC
#	define MSUI_API 
#else
#	ifdef MSUI_BUILD
#		define MSUI_API _declspec(dllexport)
#	else
#		define MSUI_API _declspec(dllimport)
#	endif
#endif


#include "DuiDefine.h"
#include <DuiImpl/DuiSize.h>
#include <DuiImpl/DuiRect.h>
#include <DuiImpl/DuiPoint.h>
#include "DuiCore/IDuiCore.h"
#include "DuiCtrl/IDuiControlCtrl.h"
#include "DuiCore/IDuiBuilder.h"
#include "DuiCtrl/IDuiContainerCtrl.h"
#include "DuiCtrl/IDuiWindowCtrl.h"


namespace DuiKit{;

template <class I, IIDef _IIDef, BOOL t_bManaged = FALSE>
class CDuiObjectPtr
{
public:
	CDuiObjectPtr()
	{
		m_p = NULL;
	}

	CDuiObjectPtr(IDuiObject* p)
	{
		m_p = NULL;
		if ( p )
		{
			m_p = static_cast<I*>(p->QueryInterface(_IIDef));
		}
	}
	~CDuiObjectPtr()
	{
		if( t_bManaged )
		{
			IDuiObject* p = m_p->QueryInterface(IIDuiObject);
			if(p)
			{
				p->DeleteThis();
				m_p = NULL;
			}
		}
	}

	template<class U, IIDef _I>
	CDuiObjectPtr(const CDuiObjectPtr<U, _I>& rhs)
	{ 
		if(rhs.m_p) 
			m_p = static_cast<I*>(rhs.m_p->QueryInterface(_I));
	}

	template<class U, IIDef _I>
	CDuiObjectPtr& operator = (const CDuiObjectPtr<U, _I>& rhs) 
	{ 
		if((void*)m_p != (void*)rhs.m_p) 
			*this = rhs.m_p; 
		return *this; 
	}


	CDuiObjectPtr& operator = (IDuiObject* p)
	{
		if ( p )
		{
			m_p = static_cast<I*>(p->QueryInterface(_IIDef));
		}

		return *this;
	}

	operator I* () const { return m_p; }
	I* operator -> () const { return m_p; }

public:
	I* m_p;
};


};

extern "C"
{
	 MSUI_API VOID DuiKitDllMaim(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved);
	 MSUI_API DuiKit::IDuiObjectFactory*  GetObjectFactory();
	 MSUI_API DuiKit::IDuiObject*  CreateObject(LPCWSTR lpszObjectClassName);
	 MSUI_API VOID DestoryObject(DuiKit::IDuiObject* pObject);
};

