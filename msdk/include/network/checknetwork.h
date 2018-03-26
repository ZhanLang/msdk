#pragma once

//网络检查相关
#include <wininet.h>
#include <SensAPI.h>
#include <Netlistmgr.h>


#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Sensapi.lib")

namespace msdk{;


//检查VISTA及其之后版本
static BOOL IsHasInternetConnectionForVista(BOOL &bHandle)
{
	bHandle = FALSE;
	BOOL bHasInternet = FALSE;
	CComPtr<INetworkListManager> spNetList;
	HRESULT hr = CoCreateInstance( CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_INetworkListManager, (void**) &spNetList );
	if( SUCCEEDED(hr) )
	{
		VARIANT_BOOL vb = VARIANT_FALSE;
		hr = spNetList->get_IsConnectedToInternet(&vb);
		if( SUCCEEDED(hr) )
		{
			bHandle = TRUE;
			if( vb == VARIANT_TRUE )
				bHasInternet = TRUE;
		}
	}

	return bHasInternet;
}


//判断网络是否存在
//在一些特殊情况下我们需要检查当前的网络状态，
//这个函数就能检查到当前网络是否处于连接状态
static BOOL WINAPI IsHasInternetConnection()
{

	//////////////////////////////////////////////////////////////////////////
	//微软在WINDOWS VISTA之后提供了一个叫NLA(Network List Manager API)的接口，用于获取网络状态变化通知的一个接口。以COM技术实现
	//////////////////////////////////////////////////////////////////////////
	BOOL bLive = FALSE;
	OSVERSIONINFO	osver = {sizeof(OSVERSIONINFO)};
	GetVersionEx(&osver);
	if(osver.dwMajorVersion  >= 6)
	{
		//已知问题: 在跨进程互相Send WM_COPYDATA,在这里中处理COM会失败
		//例：A发送WM_COPYDATA到B,B在WM_COPYDATA消息中又给A发送WM_COPYDATA，此时，在A的WM_COPYDATA消息响应中，COM会失败
		//在有些情况下，系统网络图标显示无网，但实际是有网络的。对于这种情况，无网继续走下面判断
		BOOL bHandle = FALSE;
		bLive = IsHasInternetConnectionForVista(bHandle);
		if( bLive )
			return bLive;
	}

	
	

	DWORD dwFlag = 0;
	bLive = IsNetworkAlive(&dwFlag);
	DWORD dwError = GetLastError();
	if( dwError == 0 )
	{
		if( bLive )
			return bLive;
	}

	//这里有两种情形：1. 上面值不可用，2.System Event Notification 服务停止
	dwFlag = 0;
	if( InternetGetConnectedState(&dwFlag, 0) )
	{
		if(dwFlag&INTERNET_CONNECTION_OFFLINE)
			return FALSE;

		if( (dwFlag&INTERNET_CONNECTION_LAN) ||
			(dwFlag&INTERNET_CONNECTION_MODEM) ||
			(dwFlag&INTERNET_CONNECTION_PROXY) )
			return TRUE;
	}

	return FALSE;
}

};