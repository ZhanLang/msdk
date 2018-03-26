// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include <DuiDefine.h>
#include <DuiImpl/DuiObjectFactoryRegistHelp.h>
#include <DuiApi.h>
#include <atlconv.h>


// #include <atlbase.h>
// #include <atlapp.h>
// #include <atlbase.h>
// #include <atlcom.h>
// #include <atlctl.h>
// #include <atlmisc.h>

#define GroupName _T("DuiKit")
#define GroupNameA "DuiKit"

#define MODULE_NAME _T("FUNC_DUIKIT")
#include <msdkoutput.h>
// TODO: 在此处引用程序需要的其他头文件
