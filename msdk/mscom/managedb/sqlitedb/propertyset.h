/********************************************************************
	created:	2009/09/07
	created:	7:9:2009   14:29
	filename: 	d:\C++Work\approduct2010\app\managedb\propertyset.h
	file path:	d:\C++Work\approduct2010\app\managedb\
	file base:	propertyset
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once

//#include "common/imanagedb.h"
//#include <rscom/tproperty.h>

class CPropertySet
{
public:
	static IMsObjectLoader*	ms_pLoader;

	static HRESULT CreateProp( IProperty2** ppProp )
	{
		TProperty<IProperty2>* pImpl = new TProperty<IProperty2>;
		RASSERTP(pImpl, E_OUTOFMEMORY);

		return pImpl->QueryInterface( __uuidof(IProperty2), (void**)ppProp );

		//RASSERTP( ms_pLoader, E_UNEXPECTED );

		//return ms_pLoader->CreateInstance( NULL, CLSID_CSrvProperty2, NULL, __uuidof(IProperty2), (void**)ppProp );
	}
	static HRESULT CreateProp( IPropertyStr** ppProp )
	{
		CPropertyStr* pImpl = new CPropertyStr;
		RASSERTP(pImpl, E_OUTOFMEMORY);

		return pImpl->QueryInterface( __uuidof(IPropertyStr), (void**)ppProp );

		//RASSERTP( ms_pLoader, E_UNEXPECTED );

		//return ms_pLoader->CreateInstance( NULL, CLSID_CSrvProperty2, NULL, __uuidof(IProperty2), (void**)ppProp );
	}
};


