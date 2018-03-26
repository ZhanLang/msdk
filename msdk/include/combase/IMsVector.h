#pragma once

namespace msdk{;
namespace mscom{;

template<typename T>
struct IMsVectorBase : public IMSBase
{

	//设置申请内存的步长
	//主要为了解决频繁申请内存
	//如果不设置默认值为1024
	STDMETHOD(SetStep)(DWORD dwStep) = 0;

	//获取个数
	STDMETHOD_(DWORD, GetCount)() = 0;

	//获取元素
	//为了提高效率没有设置异常检查机制
	//请注意需要读取元素的位置
	STDMETHOD_(T, GetAt)(DWORD dwAt) = 0;

	//向后插入一个元素
	STDMETHOD(Add)(T item) = 0;

	//在指定的位置插入一个元素
	STDMETHOD(AddAt)(DWORD dwAt, T item) = 0;

	STDMETHOD(Clear)() = 0;


	STDMETHOD(Lock)(DWORD dwTickCount = -1) = 0;
	STDMETHOD(UnLock)() = 0;
};


struct _declspec(novtable) IMsDwordVector : public IMsVectorBase<DWORD>
{
	typedef DWORD ItemType;
};
MS_DEFINE_IID(IMsDwordVector,"{D6569815-0BCD-4F18-9525-05744B0C8898}");

};};
//////////////////////////////////////////////////////////////////////////
//DWORD 
// {24DC5D6E-5713-4AD7-BF84-664ADA71FD58}
MS_DEFINE_GUID(CLSID_MsDwordVector, 
	0x24dc5d6e, 0x5713, 0x4ad7, 0xbf, 0x84, 0x66, 0x4a, 0xda, 0x71, 0xfd, 0x58);

// {827A98B8-EE4B-4D28-A49C-5F002090E0C5}
MS_DEFINE_GUID(CLSID_MsSafeDwordVector, 
	0x827a98b8, 0xee4b, 0x4d28, 0xa4, 0x9c, 0x5f, 0x0, 0x20, 0x90, 0xe0, 0xc5);

