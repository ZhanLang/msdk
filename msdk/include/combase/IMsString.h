#pragma once

namespace msdk{;
namespace mscom{;

struct IStringW;
//字符串COM组件
struct IStringA : public IMSBase
{
	STDMETHOD(Clone)(IStringA** pString) = 0;
	STDMETHOD_(VOID,Clear)() = 0;
	STDMETHOD_(LPCSTR,GetBuffer)() = 0;

	STDMETHOD_(LPSTR,GetBufferSetLength)(INT dwLen) = 0;
	STDMETHOD_(VOID , ReleaseBuffer)(int nNewLength = -1) = 0;
	STDMETHOD_(INT,GetLength)() = 0;
	STDMETHOD(Append)(LPCSTR lpStr) = 0;
	
	STDMETHOD_(INT,Compare)(LPCWSTR lpStr) = 0;
	STDMETHOD_(INT,Compare)(LPCSTR lpStr) = 0;
	STDMETHOD_(INT,Compare)(IStringA* lpStr) = 0;
	STDMETHOD_(INT,Compare)(IStringW* lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(LPCWSTR lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(LPCSTR lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(IStringA* lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(IStringW* lpStr) = 0;

	virtual void Format(LPCSTR lpFormat , ...) = 0;
	virtual void AppendFormat(LPCSTR lpFormat,...) = 0;


	STDMETHOD(ToStringW)(IStringW** pString) = 0;
};

MS_DEFINE_IID(IStringA,"{A7E48A94-EA27-4B46-B5C6-E05CE88C5732}");


struct IStringW : public IMSBase
{
	STDMETHOD(Clone)(IStringW** pString) = 0;
	STDMETHOD_(VOID,Clear)() = 0;

	STDMETHOD_(LPCWSTR,GetBuffer)() = 0;

	STDMETHOD_(LPWSTR,GetBufferSetLength)(INT dwLen) = 0;
	STDMETHOD_(VOID , ReleaseBuffer)(int nNewLength = -1) = 0;

	STDMETHOD_(INT,GetLength)() = 0;
	STDMETHOD(Append)(LPCWSTR lpStr) = 0;

	STDMETHOD_(INT,Compare)(LPCWSTR lpStr) = 0;
	STDMETHOD_(INT,Compare)(LPCSTR lpStr) = 0;
	STDMETHOD_(INT,Compare)(IStringA* lpStr) = 0;
	STDMETHOD_(INT,Compare)(IStringW* lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(LPCWSTR lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(LPCSTR lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(IStringA* lpStr) = 0;
	STDMETHOD_(INT,CompareNoCase)(IStringW* lpStr) = 0;


	virtual void Format(LPCWSTR lpFormat , ...) = 0;
	virtual void AppendFormat(LPCWSTR lpFormat,...) = 0;

	STDMETHOD(ToStringA)(IStringA** pString) = 0;
};
MS_DEFINE_IID(IStringW,"{63DE1351-B51A-4a88-B386-5077C9F4505A}");


// {34A5F0CC-E038-4C0E-AE02-6291C34248FA}
MS_DEFINE_GUID(CLSID_StringA, 
			0x34a5f0cc, 0xe038, 0x4c0e, 0xae, 0x2, 0x62, 0x91, 0xc3, 0x42, 0x48, 0xfa);

// {FA208B82-7587-4dfc-BB3D-8CF85278F366}
MS_DEFINE_GUID(CLSID_StringA_Safe, 
			0xfa208b82, 0x7587, 0x4dfc, 0xbb, 0x3d, 0x8c, 0xf8, 0x52, 0x78, 0xf3, 0x66);

// {78B6343D-6D65-4F09-9AE4-2958E057B5FB}
MS_DEFINE_GUID(CLSID_StringW, 
			0x78b6343d, 0x6d65, 0x4f09, 0x9a, 0xe4, 0x29, 0x58, 0xe0, 0x57, 0xb5, 0xfb);

// {9EC1DD66-5C82-4b66-B23A-0596983F9684}
MS_DEFINE_GUID(CLSID_StringW_Safe, 
			0x9ec1dd66, 0x5c82, 0x4b66, 0xb2, 0x3a, 0x5, 0x96, 0x98, 0x3f, 0x96, 0x84);


//这个应放到ROT中
interface IStringMgr : public IMSBase
{
	STDMETHOD(CreateStringA)(IStringA** pString) = 0;
	STDMETHOD(CreateStringA_Safe)(IStringA** pString) = 0;

	STDMETHOD(CreateStringW)(IStringW** pString) = 0;
	STDMETHOD(CreateStringW_Safe)(IStringW** pString) = 0;
};
MS_DEFINE_IID(IStringMgr,"{CA121B46-AC30-4aca-BBE6-A099E65F2C63}");

// {860FAAF4-3E28-46d3-8EF6-DCD021FDCA53}
MS_DEFINE_GUID(CLSID_StringMgr, 
			0x860faaf4, 0x3e28, 0x46d3, 0x8e, 0xf6, 0xdc, 0xd0, 0x21, 0xfd, 0xca, 0x53);

};};//namespace msdk