/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,6th,2014

Module Name:

	dsUIClassInfo.h 

Abstract: ui类信息，注册时使用


*/
/************************************************************************/


#ifndef __DSUICLASSINFO_H__
#define __DSUICLASSINFO_H__

//#include "dsUIBase.h"
//
//typedef dsUIBase* (CALLBACK* NEWUIPROC)(void);
//
//typedef struct tagDSUIClassInfo
//{
//	CString strClassName;
//	NEWUIPROC procNewUI;
//
//}DSUICLASSINFO,*PDSUICLASSINFO,*LPDSUICLASSINFO;
//#include "dsUIBase.h"
class dsUIBase;


typedef dsUIBase* (CALLBACK* NEWUIPROC)(void);

typedef struct tagDSUIClassInfo
{
	CString strClassName;
	NEWUIPROC procNewUI;

}DSUICLASSINFO,*PDSUICLASSINFO,*LPDSUICLASSINFO;


typedef LRESULT (CALLBACK* OWNERPROC)(HWND, UINT, WPARAM, LPARAM);



#endif /*__DSUICLASSINFO_H__*/