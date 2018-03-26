#pragma once
//////////////////////////////////////////////////////////////////////////
//所有的内存的大小单位都是字节
//////////////////////////////////////////////////////////////////////////


namespace msdk{
namespace mscom{
struct IMsBuffer : public IUnknown
{
	//获取原始buffer指针
	//这个动作比较危险,之所以要暴露是为了提速
	//dwBufSize 是当前buffer的大小，不是实际内存的大小
	//这个动作时为了提速，少一次copy动作
	//原则上是不要修改lppoutData 里面的东西，如果用这个接口尽量是仅仅读这段内存
	STDMETHOD(GetRawBuffer)(LPBYTE* lppoutData,DWORD* dwBufSize) = 0;

	//dwStartIndex 是从buffer的那个内存段开始写 , 覆盖原始数据
	STDMETHOD(SetBuffer)(LPBYTE lpBuf,DWORD dwSize,DWORD dwStartIndex = 0) = 0;

	//将buf中的内存复制到用户指定的内存中
	//lpBuf 和 dwSize 是数据的指针和大小
	//dwStartIndex 是从buffer的那个内存段开始写
	//pDwReadCount 是读取了多少字节
	STDMETHOD(GetBuffer)(LPBYTE lpoutData, DWORD dwSize,DWORD* pDwReadCount,DWORD dwStartIndex = 0) = 0;

	//保证buffer内部的内存是DwDesiredSize 的大小
	//0,默认 256 字节
	STDMETHOD(Reserve)(DWORD dwDesiredSize = 0) = 0;

	//取得buffer内部的内存是的大小
	STDMETHOD_(DWORD,GetReservedSize)() = 0;

	//取得当前buffer 中有效数据的大小
	STDMETHOD_(DWORD,GetBufferSize)() = 0;

	//获取内部数据，不安全
	STDMETHOD_(const LPBYTE, GetData)() = 0;

	//克隆一个跟当前buffer一样的buffer,两个buf互相不干扰
	STDMETHOD(Clone)(IMsBuffer** lpIBuufer) = 0;

	//将一段buffer增加到尾部，原先数据不变
	//会影响到 数据的大小，内部缓冲的大小
	STDMETHOD(AddTail)(LPBYTE lpBuf,DWORD dwSize) = 0;

	//将一段buffer增加到头部，原先数据不变
	//会影响到 数据的大小，内部缓冲的大小
	STDMETHOD(AddHead)(LPBYTE lpBuf,DWORD dwSize) = 0;

	//清空内部数据，但是缓冲区长度保持不变
	STDMETHOD(Clear)() = 0;

	//清空内部数据，同时释放内存
	STDMETHOD(ClearAndFree)() = 0;
};

MS_DEFINE_IID(IMsBuffer, "{42A79FCF-4F0C-44b5-8CDC-A8627CFAFBB1}");



struct IMsBufferEx
	:public IMsBuffer
{
	//设定数据大小的长度
	//基础网络通信层专用，其他的模块尽量不要用
	STDMETHOD(SetDataSize)(DWORD dwDataSize) = 0;
};

MS_DEFINE_IID(IMsBufferEx, "{AC44D6C2-68BF-428a-8147-29FEE7EDD682}");
	// {B021C8D6-7D21-4F41-99ED-259DD3DB996B}
MS_DEFINE_GUID(CLSID_MsBuffer, 
	0xb021c8d6, 0x7d21, 0x4f41, 0x99, 0xed, 0x25, 0x9d, 0xd3, 0xdb, 0x99, 0x6b);

// {3593C74E-E047-4868-8E30-3761AEEBD3E7}
MS_DEFINE_GUID(CLSID_MsBuffer_Safe, 
	0x3593c74e, 0xe047, 0x4868, 0x8e, 0x30, 0x37, 0x61, 0xae, 0xeb, 0xd3, 0xe7);


//com对象定义在envmgr.dll
//直接可在rot中取到
//内存管理器 ，是一个监控服务插件
interface IMsBufferMgr :public IUnknown
{
	//dwDesiredSize 是初始内存的大小 , dwDesiredSize < 256? 256:dwDesiredSize
	STDMETHOD(CreateMsBuf)(IMsBuffer** lpbuf,DWORD dwDesiredSize) = 0;

	//创建线程安全的buffer
	STDMETHOD(CreateSafeBuf)(IMsBuffer** lpbuf,DWORD dwDesiredSize) = 0;
};	
MS_DEFINE_IID(IMsBufferMgr, "{EFF8F1E1-5316-44fe-916D-186283D6A064}");

};//namespace mscom
};//namespace msdk

//{98FB74FA-6BE0-401b-B986-A78A80EED7A1}
MS_DEFINE_GUID(CLSID_MsBufferMgr,
			0x98fb74fa, 0x6be0, 0x401b, 0xb9, 0x86, 0xa7, 0x8a, 0x80, 0xee, 0xd7, 0xa1);
