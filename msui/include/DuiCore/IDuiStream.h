
#pragma once
namespace DuiKit{;

struct IDuiByteStream : public IDuiObject
{
	virtual BOOL SetBuffer(LPBYTE lpBuf,DWORD dwSize,DWORD dwStartIndex = 0) = 0;

	//保证buffer内部的内存是DwDesiredSize 的大小
	//0,默认 256 字节
	virtual BOOL Reserve(DWORD dwDesiredSize = 0) = 0;

	//取得buffer内部的内存是的大小
	virtual DWORD GetReservedSize() = 0;

	//取得当前buffer 中有效数据的大小
	virtual DWORD GetBufferSize() = 0;
	virtual VOID  SetBufSize(DWORD dwSize) = 0;

	//获取内部数据，不安全
	virtual LPBYTE GetData() = 0;
	

	//克隆一个跟当前buffer一样的buffer,两个buf互相不干扰
	virtual IDuiByteStream* Clone() = 0;

	//将一段buffer增加到尾部，原先数据不变
	//会影响到 数据的大小，内部缓冲的大小
	virtual BOOL AddTail(LPBYTE lpBuf,DWORD dwSize) = 0;
	virtual BOOL AddTail(BYTE lpBuf,DWORD dwCch) = 0;

	//将一段buffer增加到头部，原先数据不变
	//会影响到 数据的大小，内部缓冲的大小
	virtual BOOL AddHead(LPBYTE lpBuf,DWORD dwSize) = 0;

	//清空内部数据，但是缓冲区长度保持不变
	virtual BOOL Clear() = 0;

	//清空内部数据，同时释放内存
	virtual BOOL ClearAndFree() = 0;
};

};//namespace DuiKit{;