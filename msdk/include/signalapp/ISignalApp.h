#pragma once
namespace msdk{;

//单一实例消息通知
//该连接点在用户模式下被触发
interface ISingleAppMsgConnectPoint:public IMSBase
{
	/*
	<signalap>
		<command>"bin.exe" -file -app</command> 命令行
		<pid>999</pid> PID
	</signalap>
	*/
	STDMETHOD(OnSingleAppMsg)(LPCWSTR lpArgXml , BOOL& bHandle) = 0;
};
MS_DEFINE_IID(ISingleAppMsgConnectPoint , "{9710E5A9-A1BF-4266-A59C-2463C2833B04}");

// {EE002FCF-2DE9-45DF-B504-A9E233E9B4B6}
MS_DEFINE_GUID(CLSID_SignalApp, 
			0xee002fcf, 0x2de9, 0x45df, 0xb5, 0x4, 0xa9, 0xe2, 0x33, 0xe9, 0xb4, 0xb6);

}
