
#ifndef _MSPLUGIN_TRAYWND_H_
#define _MSPLUGIN_TRAYWND_H_

#include <mscom/msbase.h>

namespace msdk {

//////////////////////////////////////////////////////////////////////////
interface IWndMessageConnectPoint : public IMSBase//接收隐藏窗口消息
{
	STDMETHOD_(LRESULT, OnWndMessage)(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle) = 0;
};
MS_DEFINE_IID(IWndMessageConnectPoint, "{AA47B7BB-9343-4dbc-AFA6-8EB89C7D0A97}");

interface ITrayWnd : public IMSBase	 //窗口而已
{
	STDMETHOD_(HWND, GetWndHwnd)() = 0;	//获取创建窗口句柄
};
MS_DEFINE_IID(ITrayWnd, "{07BD6852-1E67-4806-9353-97274246F0BA}");

// {EBC23555-424F-45c3-BECE-206819CB276B}
MS_DEFINE_GUID(ClSID_CTrayWnd,
			0xebc23555, 0x424f, 0x45c3, 0xbe, 0xce, 0x20, 0x68, 0x19, 0xcb, 0x27, 0x6b);

} //namespace msdk

#endif	//_MSPLUGIN_TRAYWND_H_