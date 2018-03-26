/*******************************************************************************
* com的基接口IUnknown
*
*******************************************************************************/
#ifndef _IMSBASE_H
#define _IMSBASE_H

#include <unknwn.h>
#include <tchar.h>
#include <stdio.h>

namespace msdk {
namespace mscom {

//定义IUnknown
#define  IMSBase IUnknown
#define INULL	((IUnknown*)0)


#define MS_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define MS_DEFINE_IID(iface, uuid_string)	struct __declspec(uuid(uuid_string)) iface
#define re_uuidof(iface)	__uuidof(iface)

template<class T>
struct memless
{
	bool operator() (const T&left, const T&right) const
	{
		return memcmp(&left, &right, sizeof(T)) < 0;
	}
};

template<class T>
struct strless
{
	bool operator() (const T& left, const T& right) const
	{
		return left.compare(right) < 0;
	}
};

#define GUID_SLEN	40

#define USES_GUIDCONVERSIONW WCHAR lpGuidBuf[GUID_SLEN];
#define USES_GUIDCONVERSIONA CHAR lpGuidBuf[GUID_SLEN];

#ifdef UNICODE
	#define USES_GUIDCONVERSION USES_GUIDCONVERSIONW
#else
	#define USES_GUIDCONVERSION USES_GUIDCONVERSIONA
#endif


#pragma warning( push )
#pragma warning( disable : 4996 )

//bHaveBracket是否带括号
static GUID S2GUIDA(LPCSTR lpString, bool bHaveBracket = true)
{
	GUID guid = GUID_NULL;
	//todo it's lpString shit thing
	if(
		(bHaveBracket && strlen(lpString)!=38)
		|| (!bHaveBracket && strlen(lpString)!=36))
		return guid;

	CHAR szTemp[128]={0};
	strcpy(szTemp, lpString);

	long  Data234[ 10 ] = {0};
	sscanf(szTemp,
		bHaveBracket?"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}":"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&guid.Data1, Data234, Data234+1,
		Data234+2, Data234+3, Data234+4, Data234+5, Data234+6, Data234+7, Data234+8, Data234+9);

	guid.Data2 = (unsigned short)Data234[0];
	guid.Data3 = (unsigned short)Data234[1];
	guid.Data4[0] = (unsigned char)Data234[2];
	guid.Data4[1] = (unsigned char)Data234[3];
	guid.Data4[2] = (unsigned char)Data234[4];
	guid.Data4[3] = (unsigned char)Data234[5];
	guid.Data4[4] = (unsigned char)Data234[6];
	guid.Data4[5] = (unsigned char)Data234[7];
	guid.Data4[6] = (unsigned char)Data234[8];
	guid.Data4[7] = (unsigned char)Data234[9];

	return guid;
};

static GUID S2GUIDW(LPCWSTR lpString, bool bHaveBracket = true)
{
	GUID guid = GUID_NULL;
	//todo it's lpString shit thing
	if(
		(bHaveBracket && wcslen(lpString)!=38)
		|| (!bHaveBracket && wcslen(lpString)!=36))
		return guid;

	WCHAR szTemp[128]={0};
	wcscpy(szTemp, lpString);

	long  Data234[ 10 ] = {0};
	swscanf(szTemp,
		bHaveBracket?L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}":L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&guid.Data1, Data234, Data234+1,
		Data234+2, Data234+3, Data234+4, Data234+5, Data234+6, Data234+7, Data234+8, Data234+9);

	guid.Data2 = (unsigned short)Data234[0];
	guid.Data3 = (unsigned short)Data234[1];
	guid.Data4[0] = (unsigned char)Data234[2];
	guid.Data4[1] = (unsigned char)Data234[3];
	guid.Data4[2] = (unsigned char)Data234[4];
	guid.Data4[3] = (unsigned char)Data234[5];
	guid.Data4[4] = (unsigned char)Data234[6];
	guid.Data4[5] = (unsigned char)Data234[7];
	guid.Data4[6] = (unsigned char)Data234[8];
	guid.Data4[7] = (unsigned char)Data234[9];

	return guid;
};

//bHaveBracket是否带括号
static LPCSTR GUIDToSA(const GUID& guid, LPSTR lpGuidBuf, DWORD dwSize, bool bHaveBracket = true)
{
	if(bHaveBracket && !(lpGuidBuf && dwSize >= 38))
		return NULL;
	if(!bHaveBracket && !(lpGuidBuf && dwSize >= 36))
		return NULL;

	sprintf(lpGuidBuf,
		bHaveBracket?"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}":"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return lpGuidBuf;
};

static LPCWSTR GUIDToSW(const GUID& guid, LPWSTR lpGuidBuf, DWORD dwSize, bool bHaveBracket = true)
{
	if(bHaveBracket && !(lpGuidBuf && dwSize >= 38))
		return NULL;
	if(!bHaveBracket && !(lpGuidBuf && dwSize >= 36))
		return NULL;

	swprintf(lpGuidBuf,
		bHaveBracket?L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}":L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return lpGuidBuf;
};
#pragma warning( pop )

#define GUID2SA(guid) GUIDToSA(guid, lpGuidBuf, GUID_SLEN)
#define GUID2SW(guid) GUIDToSW(guid, lpGuidBuf, GUID_SLEN)

#define GUID2S_NOBracketA(guid) GUIDToSA(guid, lpGuidBuf, GUID_SLEN, false)
#define GUID2S_NOBracketW(guid) GUIDToSW(guid, lpGuidBuf, GUID_SLEN, false)

#ifdef UNICODE
	#define S2GUID				S2GUIDW
	#define GUIDToS				GUIDToSW
	#define GUID2S				GUID2SW
	#define GUID2S_NOBracket	GUID2S_NOBracketW
#else
	#define S2GUID				S2GUIDA
	#define GUIDToS				GUIDToSA
	#define GUID2S				GUID2SA
	#define GUID2S_NOBracket	GUID2S_NOBracketA
#endif

//////////////////////////////////////////////////////////////////////////
//特殊的类工厂
interface IMSClassFactory: public IClassFactory
{
public:
	STDMETHOD(CreateInstance)(IMSBase *prot, IMSBase *punkOuter, const IID& riid, void **ppvObject) PURE;
	STDMETHOD_(CLSID, GetAt)(LONG nIndex) PURE;
	STDMETHOD_(LONG, GetCount)() PURE;
	STDMETHOD_(LPCTSTR, ProgIDFromCLSID)(REFCLSID clsid) PURE;

};
MS_DEFINE_IID(IMSClassFactory,"{6966E385-DBFA-4131-A29E-D0E9464F3F53}");

/*
// {E2247B54-E329-4ca8-8361-6499FDFF98F4}
MS_DEFINE_GUID(CLSID_CMSClassFactory,
			   0xe2247b54, 0xe329, 0x4ca8, 0x83, 0x61, 0x64, 0x99, 0xfd, 0xff, 0x98, 0xf4);
*/
// {E9678781-A3CB-46fb-9121-3ED22C24CFAD}
MS_DEFINE_GUID(CLSID_MSClassFactory,
			   0xe9678781, 0xa3cb, 0x46fb, 0x91, 0x21, 0x3e, 0xd2, 0x2c, 0x24, 0xcf, 0xad);

//////////////////////////////////////////////////////////////////////////
// Property:
interface IProperty : public IMSBase
{
	typedef GUID KeyType;

	STDMETHOD(SetProperty)(const GUID& rpid, const PROPVARIANT *pv) = 0;
	STDMETHOD(GetProperty)(const GUID& rpid, const PROPVARIANT **ppv) = 0;
	STDMETHOD(DeleteProperty)(const GUID& rpid) = 0;
	STDMETHOD(Clear)() = 0;
};
MS_DEFINE_IID(IProperty, "{39AA125E-D7F1-46b1-82EB-6D8527269279}");

// {ED20E0E5-2357-4825-B3FA-198AEC674E81}
MS_DEFINE_GUID(CLSID_CProperty,
			   0xed20e0e5, 0x2357, 0x4825, 0xb3, 0xfa, 0x19, 0x8a, 0xec, 0x67, 0x4e, 0x81);

//多线程版本
// {AD4F3A47-0CD6-43de-BC22-E8BE24FFD424}
MS_DEFINE_GUID(CLSID_CProperty_thread,
			   0xad4f3a47, 0xcd6, 0x43de, 0xbc, 0x22, 0xe8, 0xbe, 0x24, 0xff, 0xd4, 0x24);

//-------------------------------------------------------------------------------
interface IProperty2 : public IMSBase
{
	typedef DWORD KeyType;

	STDMETHOD(SetProperty)(const DWORD& rpid, const PROPVARIANT *pv) = 0;
	STDMETHOD(GetProperty)(const DWORD& rpid, const PROPVARIANT **ppv) = 0;
	STDMETHOD(DeleteProperty)(const DWORD& rpid) = 0;
	STDMETHOD(Clear)() = 0;
};
MS_DEFINE_IID(IProperty2, "{21C975F4-EEE1-4c90-8D39-7D94A6AEDAB7}");

// {2100E98D-B13E-4306-8081-50F325B10586}
MS_DEFINE_GUID(CLSID_CProperty2,
			   0x2100e98d, 0xb13e, 0x4306, 0x80, 0x81, 0x50, 0xf3, 0x25, 0xb1, 0x5, 0x86);

//多线程版本
// {0AEF80FB-9BAF-4e66-96B3-784ED0FCECF1}
MS_DEFINE_GUID(CLSID_CProperty2_thread,
			   0xaef80fb, 0x9baf, 0x4e66, 0x96, 0xb3, 0x78, 0x4e, 0xd0, 0xfc, 0xec, 0xf1);


//----------------------------------------------------------------------------------
interface IPropertyStr : public IMSBase
{
	STDMETHOD(SetProperty)( LPCSTR rpid, const PROPVARIANT *pv) = 0;
	STDMETHOD(GetProperty)( LPCSTR rpid, const PROPVARIANT **ppv) = 0;
	STDMETHOD(DeleteProperty)(LPCSTR rpid) = 0;
	STDMETHOD(Clear)() = 0;
};
MS_DEFINE_IID(IPropertyStr, "{1187FFD7-5BAD-405B-9667-9BF093248E0B}");


// {C152A044-68CC-4F6D-AC27-3BC4E34593C3}
MS_DEFINE_GUID(CLSID_CPropertyStr,
	0xc152a044, 0x68cc, 0x4f6d, 0xac, 0x27, 0x3b, 0xc4, 0xe3, 0x45, 0x93, 0xc3);


//多线程版本
// {BA204A27-A494-4EBA-A5FA-80B87D3CEEF8}
MS_DEFINE_GUID(CLSID_CPropertyStr_thread,
	0xba204a27, 0xa494, 0x4eba, 0xa5, 0xfa, 0x80, 0xb8, 0x7d, 0x3c, 0xee, 0xf8);



} //namespace mscom
} //namespace msdk
#endif // _IMSBASE_H