#include "stdafx.h"
#include "dsUIClassMgr.h"


dsUIClassMgr dsUIClassMgr::g_instance;


dsUIClassMgr::dsUIClassMgr()
{
	//这里注册各种类？

	DSUICLASSINFO dsuici;

	dsUIBase::GetClassInfo(&dsuici);	
	RegisterClass(&dsuici);

	dsButton::GetClassInfo(&dsuici);	
	RegisterClass(&dsuici);

	dsListView::GetClassInfo(&dsuici);	
	RegisterClass(&dsuici);

	dsCheckBox::GetClassInfo(&dsuici);
	RegisterClass(&dsuici);

	dsProgress::GetClassInfo(&dsuici);
	RegisterClass(&dsuici);

	dsScroll::GetClassInfo(&dsuici);
	RegisterClass(&dsuici);


}

dsUIBase * dsUIClassMgr::CreateDsuiElement( LPCWSTR lpszClassName )
{
	if(NULL == lpszClassName)
		return NULL;

	MAP_DSUICI::iterator it = m_mapDSUICI.find(lpszClassName);

	if (it != m_mapDSUICI.end())
	{
		if(NULL != it->second.procNewUI)
			return it->second.procNewUI();
	}

	return NULL;
}

BOOL dsUIClassMgr::RegisterClass( PDSUICLASSINFO pci )
{
	if(NULL == pci)
		return FALSE;

	if(NULL == pci->procNewUI || pci->strClassName.IsEmpty())
		return FALSE;

	if(IsClassExist(pci->strClassName))
		return FALSE;

	m_mapDSUICI.insert(make_pair(pci->strClassName, *pci));

	return TRUE;

}

BOOL dsUIClassMgr::IsClassExist( LPCWSTR lpszClassName )
{
	if(NULL == lpszClassName)
		return FALSE;

	MAP_DSUICI::iterator it = m_mapDSUICI.find(lpszClassName);

	if (it != m_mapDSUICI.end())
		return TRUE;


	return  FALSE;
}


BOOL dsUIClassMgr::GetClassInfo( __in LPCWSTR lpszClassName, __out PDSUICLASSINFO pci )
{
	if(NULL == lpszClassName)
		return FALSE;

	MAP_DSUICI::iterator it = m_mapDSUICI.find(lpszClassName);

	if (it != m_mapDSUICI.end())
	{
		*pci = it->second;
		return TRUE;
	}

	return  FALSE;
}