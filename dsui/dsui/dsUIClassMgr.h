/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,6th,2014

Module Name:

	dsUIClassMgr.h 

Abstract: ui类厂，用于类注册和实例ui创建等


*/
/************************************************************************/

#ifndef __DSUICLASSMGR_H__
#define __DSUICLASSMGR_H__

#include <map>
using namespace std;

#include "dsUIClassInfo.h"
#include "dsUIBase.h"
#include "dsUIHeaders.h"


class dsUIClassMgr
{
public:
	typedef map<CString,DSUICLASSINFO> MAP_DSUICI;

private:
	static dsUIClassMgr g_instance;

private:
	MAP_DSUICI m_mapDSUICI;


public:

	dsUIClassMgr();

	inline static dsUIClassMgr *getInstance(){
		return &g_instance;
	}

	dsUIBase *CreateDsuiElement(__in LPCWSTR lpszClassName);
	
	BOOL RegisterClass(__in PDSUICLASSINFO pci);

	BOOL IsClassExist(__in LPCWSTR lpszClassName);

	BOOL GetClassInfo(__in LPCWSTR lpszClassName, __out PDSUICLASSINFO pci);

};


#endif /*__DSUICLASSMGR_H__*/