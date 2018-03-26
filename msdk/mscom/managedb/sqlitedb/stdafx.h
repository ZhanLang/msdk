// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

#ifndef tstring
#ifdef _UNICODE
#define tstring std::wstring
#else	tstring std::string
#endif
#endif

#ifndef tostringstream
#ifdef _UNICODE
#define tostringstream wostringstream
#else	tostringstream ostringstream
#endif
#endif

// TODO: 在此处引用程序需要的其他头文件

#include "mscom/mscominc.h"
using namespace mscom;

#include <map>
#include "mscom\tproperty.h"
#include "mscom\prophelpers.h"

// LPSTR MyW2A( LPCWSTR szW );
// LPWSTR MyA2W( LPCSTR szA );

#define MODULE_NAME L"sqlitedb"
#define DEFAULT_LEV	2

#include "mslog/stdlog_dll.h"
