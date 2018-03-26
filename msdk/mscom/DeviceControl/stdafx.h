// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>
#include <string>
#include "3rd/crc32.h"

#include "mscom/mscominc.h"
#include "atlstr.h"

#define GroupName _T("DeviceControl")
#include "mslog/logtool/logtool.h"
//#define DEVICECONTROL_DLL

#define RASSERT(x, _h_r_) { if(!(x)) return _h_r_; }


static BOOL CompareString(std::wstring strLength, std::wstring strLow )
{
	BOOL  bRet = FALSE;

	if( strLength.empty() &&
		strLow.empty() )
	{
		return bRet;
	}

	// 全部转换成大写
	std::transform(strLength.begin(), strLength.end(), strLength.begin(), toupper);

	// 转换成大写
	std::transform( strLow.begin(), strLow.end(),strLow.begin(), toupper );

	// 执行比对, 如果对比相等
	if( strLength.compare( 0, strLow.size(),strLow ) == 0 )
	{
		bRet = TRUE;
	}

	return bRet;
}

static DWORD CalcCrc32(std::wstring str)
{
	return DWORD(std_crc32((const char*)str.c_str(),str.length() * sizeof(WCHAR)));
}