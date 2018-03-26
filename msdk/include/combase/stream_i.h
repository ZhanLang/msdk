
#ifndef _STREAM_I_H_
#define _STREAM_I_H_
namespace msdk{;
interface IMsSequentialStream
	:public IMSBase
{
	/*
	功能:读取一段缓冲区
	参数:
	返回:
	*/
	STDMETHOD(Read)(OUT VOID*pv,IN ULONG cb, OUT ULONG* pcbRead) = 0;

	/*
	功能:写入一段缓冲区
	参数:
	返回:
	*/
	STDMETHOD(Write)(IN CONST VOID* pv, IN ULONG cb, OUT ULONG *pcbWritten) = 0;
};
MS_DEFINE_IID(IMsSequentialStream,"{C8429D3C-E2A7-4de8-890B-4106A31E817D}");


enum SeekOrigin
{
	S_SEEK_BEGIN		= 0,
	S_SEEK_CURRENT	= 1,
	S_SEEK_END		= 2,
};

interface IMsStream
	:public IMsSequentialStream
{
	/*
	功能:偏移
	参数:
	返回:
	*/
	STDMETHOD(Seek)(IN LARGE_INTEGER dlibMove, IN SeekOrigin dwOrigin, OUT LARGE_INTEGER *plibNewPosition) = 0;

	/*
	功能:设置缓存区大小
	参数:
	返回:
	*/
	STDMETHOD(SetSize)(IN LARGE_INTEGER libNewSize) = 0;

	/*
	功能:获取缓冲区大小
	参数:
	返回:
	*/
	STDMETHOD_(LARGE_INTEGER,GetSize)() = 0;


	/*
	功能:获取有效数据长度
	参数:
	返回:
	*/
	STDMETHOD_(LARGE_INTEGER,GetDataSize)() = 0;

	/*
	功能:重置缓冲区
	参数:
	返回:
	*/
	STDMETHOD(Revert)() = 0;

	/*
	功能:重置缓冲区
	参数:
	返回:
	*/
	STDMETHOD(Clone)(IMsStream**pClone) = 0;
};
MS_DEFINE_IID(IMsStream,"{94ADE583-51D0-414b-BB83-0AA702F4B2BD}");

interface IMsFileStream
	:public IMsStream
{
	/*
	功能:打开文件
	参数:
		lpFileName 文件名称
		dwCreationDisposition 创建属性，参见CreateFile::dwCreationDisposition
	返回:
		SUCCESSED(HR)
	*/
	STDMETHOD(Open)(LPCWSTR lpFileName,DWORD dwCreationDisposition = OPEN_ALWAYS) = 0;

	/*
	功能:
	参数:
	返回:
	*/
	STDMETHOD(Close)() = 0;

	STDMETHOD(FlushFileBuffers)() = 0;
};
MS_DEFINE_IID(IMsFileStream,"{49B96122-34CA-44b0-9031-A63D0B0A8F8E}");


// {3AD2185B-836C-4ee4-9AC9-4636F818D2BE}
MS_DEFINE_GUID(CLSID_MsFileStream,
			0x3ad2185b, 0x836c, 0x4ee4, 0x9a, 0xc9, 0x46, 0x36, 0xf8, 0x18, 0xd2, 0xbe);

// {31B6A01F-6A10-4342-BDD0-46F3D74A071C}
MS_DEFINE_GUID(CLSID_MsFileStream_Safe, 
			0x31b6a01f, 0x6a10, 0x4342, 0xbd, 0xd0, 0x46, 0xf3, 0xd7, 0x4a, 0x7, 0x1c);

};
#endif

