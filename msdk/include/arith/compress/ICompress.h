#ifndef __ICOMPRESS_H__
#define __ICOMPRESS_H__
namespace msdk{
////////////////////////////////////////////////////////////////////////////////
//压缩算法类型
enum COMPRESSARITHMETIC_TYPE
{ 
	eNone = 0,
	eZip  = 1,
	e7z   = 2
};

////////////////////////////////////////////////////////////////////////////////
//注意:Zlib库要求,要压缩的数据不能<=6字节,否则压缩失败
interface ICompress : public IUnknown
{
	virtual BYTE GetType() = 0;
	virtual LPBYTE Compress(const unsigned char* pSrc, size_t SrcLen, size_t* pDestLen) = 0;  //最大8M
	virtual LPBYTE DeCompress(const unsigned char* pSrc, size_t pSrcLen, size_t* pDestLen) = 0;
};

MS_DEFINE_IID(ICompress, "{55345158-5B3E-44fe-9213-E96DC0C3CF19}");

// {ECE8E44A-39E2-4138-9FBA-ADC99469E325}
MS_DEFINE_GUID(CLSID_ZlibAdapter , 
			   0xece8e44a, 0x39e2, 0x4138, 0x9f, 0xba, 0xad, 0xc9, 0x94, 0x69, 0xe3, 0x25);


// {A05FCDBB-06E8-4c2b-96AA-27947D22E0C9}
MS_DEFINE_GUID(CLSID_7zAdapter , 
			   0xa05fcdbb, 0x6e8, 0x4c2b, 0x96, 0xaa, 0x27, 0x94, 0x7d, 0x22, 0xe0, 0xc9);
}//namespace msdk
#endif //__ICOMPRESS_H__
