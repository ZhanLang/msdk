// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

#include "mscom/mscominc.h"
#include "msapi/mswinapi.h"
#include "util/msmap.h"
using namespace mscom;


// TODO: 在此处引用程序需要的其他头文件

#include <atlstr.h>

#define GroupName _T("luahost")
#define GroupNameA "luahost"

#define RSLOG_DETAIL GroupName

static void GrpErrorW_N( wchar_t const *pGroupName, int level, wchar_t const *pMsg, ... )
{

}
//#ifdef _DEBUG
	#define RSLOG GrpError
// #else
// 	#define RSLOG GrpErrorW_N  
// #endif // _DEBUG


#define GRP_NAME _T("luahost")
#define LOG_OUTPUT_DIR _T("C:\\") //将日志输出到C:\下
#define LOG_FILE_NAME _T("luahost.log")

#define BOOL2bool( X )			(( X )==TRUE?true:false)
#define bool2BOOL( X )			(( X )== true ? TRUE : FALSE)


#define RSLOG_ALERT GroupName

#define  GroupNameA "luahost"

#include "mslog/stdlog_dll.h"


#ifndef tstring
#if UNICODE
#	define tstring std::wstring
#else
#	define tstring std::string
#endif
#endif
//#include "luahost/ILuaHost.h"
//#include "luahost/luahost.h"