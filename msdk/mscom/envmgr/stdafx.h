// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

//////////////////////////////////////////////////////////////////////////


#include <atlbase.h>
#include <atlstr.h>

#define GroupName _T("envmgr")
#define MODULE_NAME	GroupName

#include <assert.h>
#include <mscom/mscominc.h>
#include <mslog/msdkoutput.h>
#include <mslog/stdlog_s.h>


//////////////////////////////////////////////////////////////////////////
struct IModuleMgr
{
	virtual HRESULT InsertModule(LPCTSTR lpszPath,CMSComLoader* pLoader = NULL) = 0;
	virtual HRESULT CreateInstance( LPCTSTR lpszModule,REFCLSID rclsid, IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv) = 0;
	virtual HRESULT UnloadNow(LPCTSTR lpszModule) = 0;
};

struct IClassMgr
{
	virtual HRESULT AddClsid(LPCTSTR lpszModule,LPCTSTR lpszClsid,LPCTSTR lpszProgid = NULL,LPCTSTR lpszName = NULL) = 0;
	virtual HRESULT RemoveClsid(REFCLSID clsid) = 0;
	virtual HRESULT CreateInstance(REFCLSID rclsid, IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppv) = 0;
	virtual LPCTSTR GetModuleFileName(REFCLSID clsid) = 0;
};



// TODO: 在此处引用程序需要的其他头文件
