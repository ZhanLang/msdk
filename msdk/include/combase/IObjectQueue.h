#pragma once

namespace msdk{;
namespace mscom{;
//对象安全队列
struct IObjeceQueue:public IMSBase
{
	//向安全队列中添加一个元素
	STDMETHOD(Push)(IMSBase* pObj) = 0;

	//向安全队列中取出一个元素
	STDMETHOD(Pop)(IMSBase** pObj) = 0;

	//安全队列中元素的个数
	STDMETHOD_(DWORD,Size)() = 0;

	STDMETHOD(Clear)() = 0;
};
MS_DEFINE_IID(IObjeceQueue, "{0F93F606-D932-4A01-AB2A-C2DED34A273D}");

// {C67133C5-EB7D-4CB1-B584-01B0D7F8B205}
MS_DEFINE_GUID(CLSID_ObjeceQueue,
			  0xc67133c5, 0xeb7d, 0x4cb1, 0xb5, 0x84, 0x1, 0xb0, 0xd7, 0xf8, 0xb2, 0x5);
};};