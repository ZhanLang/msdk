// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <WinInet.h>


//#define  _WTL_NO_CSTRING	
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>
//#include <atlstr.h>
#include <map>
#include <list>
#include <queue>
using namespace std;


#define MAXLEN_LOCAL_PIECE	4*1024*1024
#define MINLEN_LOCAL_PIECE	100*1024
extern DWORD g_dwMaxLocalPieceSize;

// TODO: 在此处引用程序需要的其他头文件
