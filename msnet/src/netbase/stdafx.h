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

#include <atlbase.h>
#include <atlstr.h>

#define GrpName _T("netbase")
#include <mscom/mscominc.h>
#include "msnetlog.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
using namespace mscom;
using namespace boost::asio::ip;
extern HINSTANCE g_hInstance;
// TODO: 在此处引用程序需要的其他头文件
static BOOL IsIPV6Address(LPCSTR lpAddr)
{
	if(NULL == lpAddr)
		return FALSE;
	char*pPos = strstr((char*)lpAddr,":");
	if(pPos != NULL)
		return TRUE;
	return FALSE;
}


//将IPv6地址转换成 XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX格式
static std::string GetIPString(tcp::endpoint& endpoint)
{
    //先不判断了，有%的地址去掉连不上
    return endpoint.address().to_string();
/*
   if(endpoint.protocol() == boost::asio::ip::tcp::v6())
   {
       std::string strIP ="";
       boost::array<unsigned char, 16> arrayAddr;
       arrayAddr = endpoint.address().to_v6().to_bytes();
       if( 16 != arrayAddr.size())
           return strIP;
       char szIP[MAX_PATH] = {0};
       sprintf_s(szIP,MAX_PATH,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
           arrayAddr[0],arrayAddr[1],arrayAddr[2],arrayAddr[3],
           arrayAddr[4],arrayAddr[5],arrayAddr[6],arrayAddr[7],
           arrayAddr[8],arrayAddr[9],arrayAddr[10],arrayAddr[11],
           arrayAddr[12],arrayAddr[13],arrayAddr[14],arrayAddr[15] );
       strIP = szIP;
       return strIP;
   }
   return endpoint.address().to_string();
 //*/
}
