// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AEDB1B9E_EF96_43AB_B852_CE72EAD1E1F1__INCLUDED_)
#define AFX_STDAFX_H__AEDB1B9E_EF96_43AB_B852_CE72EAD1E1F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

#include <stdio.h>
#include <atlbase.h>
#include "mscom/mscominc.h"
using namespace mscom;

#include <SyncObject/AsynFunc.h>
extern CAsynFunc g_rpcCall;

//#include "syslay/dbg.h"
void ErrorMsg(const char * msgDescriptor, ... );

#define  Assert     sizeof
#define  Msg		sizeof
#define  Error		sizeof
#define  Warning	sizeof
#define  DBG_CODE( _code )				((void)0)

//#define  RAVDONOTHASDEBUGLOG
//#include <rsdebug.h>

//#ifdef _DEBUG 
//#define  _DEBUG_INTERLOCKED_VIEW	//for debug
//#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// 快速创建本模块自己的对象
#ifndef DllQuickCreateInstance
	#define DllQuickCreateInstance(clsid, iid, ppv)	__Comx3_DllGetClassObject(clsid, iid, (void**)&ppv, NULL)
#endif


#ifndef EXPORT_STATICLIB
MARGIN_API MSDllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);
#else
//HRESULT MSDllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);
#endif

inline HRESULT __Comx3_DllGetClassObject(REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv, IMSBase *prot)
{
	UTIL::com_ptr<IMSClassFactory> pRSClassFactory;

	if(SUCCEEDED(MSDllGetClassObject(rclsid, re_uuidof(IMSClassFactory), (void**)&pRSClassFactory)) && pRSClassFactory)
	{
		return pRSClassFactory->CreateInstance(prot, NULL, riid, ppv);
	}

	return E_FAIL;
}

#endif // !defined(AFX_STDAFX_H__AEDB1B9E_EF96_43AB_B852_CE72EAD1E1F1__INCLUDED_)
