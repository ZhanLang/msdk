
#ifndef _MSCONNECTIONPOINT_H_
#define _MSCONNECTIONPOINT_H_

#include <mscom/msbase.h>

namespace msdk {
namespace mscom {

typedef struct tagMSCONNECTDATA
{
	UTIL::com_ptr<IUnknown> pUnk;
	DWORD dwCookie;
} MSCONNECTDATA;
typedef MSCONNECTDATA *LPMSCONNECTDATA;

interface IMsEnumConnections : public IUnknown //连接点枚举器
{
public:
	STDMETHOD(Next)(ULONG cConnections, LPMSCONNECTDATA rgcd, ULONG *pcFetched) = 0;
	STDMETHOD(Skip)(ULONG cConnections) = 0;
	STDMETHOD(Reset)(void) = 0;
	STDMETHOD(Clone)(IMsEnumConnections **ppEnum) = 0;
};
MS_DEFINE_IID(IMsEnumConnections, "{966C3BF0-2452-4741-A5EB-0FE3C897B66D}");

interface IMsConnectionPointContainer;
interface IMsConnectionPoint : public IMSBase	//连接点
{
public:
	STDMETHOD(GetConnectionInterface)(IID *pIID) = 0;
	STDMETHOD(GetConnectionPointContainer)(IMsConnectionPointContainer **ppCPC) = 0;
	STDMETHOD(Advise)(IUnknown *pUnkSink, DWORD *pdwCookie) = 0;
	STDMETHOD(Unadvise)(DWORD dwCookie) = 0;
	STDMETHOD(EnumConnections)(IMsEnumConnections **ppEnum) = 0;
};
typedef IMsConnectionPoint *LPMSCONNECTIONPOINT;
MS_DEFINE_IID(IMsConnectionPoint, "{FA053C5F-9CDE-4d43-BE8D-BF9C8D6B3BC1}");

//////////////////////////////////////////////////////////////////////////
interface IMsEnumConnectionPoints : public IUnknown
{
public:
	STDMETHOD(Next)( ULONG cConnections,LPMSCONNECTIONPOINT *ppCP, ULONG *pcFetched) = 0;
	STDMETHOD(Skip)( ULONG cConnections ) = 0;
	STDMETHOD(Reset)( void) = 0;
	STDMETHOD(Clone)(IMsEnumConnectionPoints **ppEnum) = 0;
};
MS_DEFINE_IID(IMsEnumConnectionPoints, "{720E4D89-F10F-472b-9B0C-0FE9F881D448}");

interface IMsConnectionPointContainer : public IMSBase //连接点容器
{
	STDMETHOD(EnumConnectionPoints)(IMsEnumConnectionPoints **ppEnum) = 0;
	STDMETHOD(FindConnectionPoint)(REFIID riid, IMsConnectionPoint **ppCP) = 0;
};
MS_DEFINE_IID(IMsConnectionPointContainer, "{EA3E2924-A205-41a3-85DE-40DABD266745}");

} //namespace mscom
} //namespace rsd;

#endif _MSCONNECTIONPOINT_H_