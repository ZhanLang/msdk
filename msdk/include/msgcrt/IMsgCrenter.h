#pragma once
/*
	说明：提供MSC消息订阅中心功能
*/

interface IMsgCenter
{
/*
	功能：设置订阅中心的命名空间
	参数：lpstrName 命名空间
	返回：SUCCESSED(hr)
	说明：这个接口建议在 Init 之前调用，比如在 在住插件中调用
		  如果程序没有设置命名空间则会读取EnvInfo（EnvInfo_MsgCenter）环境信息
		  如需帮助，查看 envinfo_i.h
*/
	STDMETHOD(SetNameSpace)(LPCWSTR lpstrName) = 0;
};
MS_DEFINE_IID(IMsgCenter , "{800E4D86-8297-45C6-AFD8-B1EC8B34C90D}");

// {62D8CCDE-7CA1-428c-AE51-9C92E6B39E89}
MS_DEFINE_GUID(CLSID_MsgCenter, 
	0x62d8ccde, 0x7ca1, 0x428c, 0xae, 0x51, 0x9c, 0x92, 0xe6, 0xb3, 0x9e, 0x89);
