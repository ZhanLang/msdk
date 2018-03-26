// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <ctype.h>
#include <atlbase.h>
#include <atlstr.h>


#include "rscom\rscom_ex.h"
#include "rscom\rscominc.h"
using namespace rscom;

#include <rscom/rsplugin.h>
#include <map>
#include "rscom\tproperty.h"
#include "rscom\prophelpers.h"


#include <sstream>
using namespace std;
typedef basic_ostringstream<wchar_t> owstringstream;
typedef basic_istringstream<wchar_t> iwstringstream;

using namespace rscom;

#include "msc/IRsBuffer.h"

#define MODULE_NAME	 _T("FastDBDll")

enum Msg_Level
{
	MsgLevel_Error		=0,	//1
	MsgLevel_Warning	=3,	//3
	MsgLevel_Log		=5,	//5	
	MsgLevel_Notify		=10	//10g namespace rscom;
};

VOID MyLogOut(LPCSTR dataMsg, ...);

#define FAILEXIT(x, _h_r_) {if(!(x)){MyLogOut(("[FAILEXIT:FastDBRemote]:%s,%s,%s,%d\n"),#x,__FUNCDNAME__,__FILE__,__LINE__);return _h_r_; }}
#define FAILEXIT_FALSE(x) {FAILEXIT((x),FALSE)}
#define FAILEXIT_NULL(x) {FAILEXIT((x),NULL)}
#define FAILEXIT_FAIL(x) {FAILEXIT((x),E_FAIL)}
#define FAILEXIT_OTHER(x) {FAILEXIT((x),0xFFFFFFFF)}

#define FAILEXIT_N(x, _h_r_) {if(!(x)){MyLogOut(("[FAILEXIT:FastDBRemote]:%s,%s,%s,%d\n"),#x,__FUNCDNAME__,__FILE__,__LINE__); return _h_r_; }}
#define FAILEXIT_FALSE_N(x) {FAILEXIT_N((x),FALSE)}
#define FAILEXIT_NULL_N(x) {FAILEXIT_N((x),NULL)}
#define FAILEXIT_FAIL_N(x) {FAILEXIT_N((x),E_FAIL)}
#define FAILEXIT_OTHER_N(x) {FAILEXIT_N((x),0xFFFFFFFF)}

#define FAILD_LOGONLY(x)    {if(!(x)){MyLogOut(("[FAILEXIT:FastDBRemote]:%s,%s,%s,%d\n"),#x,__FUNCDNAME__,__FILE__,__LINE__);}}



