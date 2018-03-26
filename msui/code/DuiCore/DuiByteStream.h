#pragma once

#include <DuiCore/IDuiStream.h>
using namespace DuiKit;
class CDuiByteStream : public IDuiByteStream
{
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiByteStream, OBJECT_CORE_BYTE_STREAM)
		DUI_DEFINE_INTERFACE(IDuiByteStream, IIDuiByteStream)
	DUI_END_DEFINE_INTERFACEMAP;
	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue)
	{

	}
	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		return TRUE;
	}
public:
	CDuiByteStream();
	~CDuiByteStream(void);

	virtual BOOL SetBuffer(LPBYTE lpBuf,DWORD dwSize,DWORD dwStartIndex = 0);
	virtual BOOL Reserve(DWORD dwDesiredSize = 0);
	virtual DWORD GetReservedSize();
	virtual DWORD GetBufferSize();
	virtual VOID  SetBufSize(DWORD dwSize);

	virtual LPBYTE GetData();
	virtual IDuiByteStream* Clone();
	virtual BOOL AddTail(LPBYTE lpBuf,DWORD dwSize);
	virtual BOOL AddTail(BYTE lpBuf,DWORD dwCch);
	virtual BOOL AddHead(LPBYTE lpBuf,DWORD dwSize);
	virtual BOOL Clear();
	virtual BOOL ClearAndFree();


private:
	LPBYTE m_lpByteBuf;		//内存体
	DWORD  m_dwBufSize;		//内存的大小
	DWORD  m_dwDataSize;	//数据的大小
};

