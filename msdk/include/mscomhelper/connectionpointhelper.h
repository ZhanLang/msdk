
#ifndef _MSCONNECTIONPOINT_HELPER_H_
#define _MSCONNECTIONPOINT_HELPER_H_

#include <mscom/connectionpoint.h>
#include <vector>

namespace msdk {
//////////////////////////////////////////////////////////////////////////
struct ST_MsConnectionPoint
{
	IID	iid;
	UTIL::com_ptr<IMsConnectionPoint> p;
};
typedef std::vector<ST_MsConnectionPoint> CONNECTIONPOINTS, *LPCONNECTIONPOINTS;

class CConnectionPointHelper;
class CMsEnumConnectionPoints
	: public IMsEnumConnectionPoints
	, public CUnknownImp
{
public:
	CMsEnumConnectionPoints(CONNECTIONPOINTS& ConnectionPoints)
		: m_ulEnumConnectionPointsPos(0)
		, m_ConnectionPoints(ConnectionPoints)
	{}

	UNKNOWN_IMP1(IMsEnumConnectionPoints);

	//IMsEnumConnectionPoints
	STDMETHOD(Next)( ULONG cConnections, LPMSCONNECTIONPOINT *ppCP, ULONG *pcFetched)
	{
		if(pcFetched)
			*pcFetched = 0;

		//从当前位置枚举cConnections个出去
		RASSERT(cConnections, S_OK); //请求就是0个直接返回成功
		RASSERT(ppCP, S_FALSE);

		size_t sSize = m_ConnectionPoints.size();
		if(0 == sSize)
		{
			return S_FALSE;
		}

		ULONG i = 0;
		while((i < cConnections) && (m_ulEnumConnectionPointsPos < sSize))
		{
			if(m_ConnectionPoints[m_ulEnumConnectionPointsPos].p)
				m_ConnectionPoints[m_ulEnumConnectionPointsPos].p->QueryInterface(__uuidof(IMsConnectionPoint), (void**)&ppCP[i]);

			++m_ulEnumConnectionPointsPos;
			++i;
		}

		if(i < cConnections)//实际请求到的数量不够
		{
			if(pcFetched)
				*pcFetched = 0;
			return S_FALSE;
		}

		return S_OK;
	}

	STDMETHOD(Skip)( ULONG cConnections )
	{
		m_ulEnumConnectionPointsPos += cConnections;
		return S_OK;
	}

	STDMETHOD(Reset)( void )
	{
		m_ulEnumConnectionPointsPos = 0;
		return S_OK;
	}

	STDMETHOD(Clone)(IMsEnumConnectionPoints **ppEnum)
	{
		RASSERT(ppEnum, E_POINTER);

		CMsEnumConnectionPoints* pNew = new CMsEnumConnectionPoints(m_ConnectionPoints);
		HRESULT hr = pNew->QueryInterface(__uuidof(IMsEnumConnectionPoints), (void**)ppEnum);
		if(NULL == *ppEnum) //失败
		{
			delete pNew;
			pNew = NULL;
		}

		return hr;
	}

private:
	ULONG m_ulEnumConnectionPointsPos;
	CONNECTIONPOINTS& m_ConnectionPoints;
};

//偷懒用的，免得每个函数写一样的东西
#define CallConnectPointFunc(obj, iid, func)	{	\
	UTIL::com_ptr<IMsEnumConnections> pEnum;	\
	if(SUCCEEDED(obj.EnumConnections((IMsEnumConnections**)&pEnum)) && pEnum){	\
	HRESULT hNext = S_OK;	\
	while(S_OK == hNext){	\
	MSCONNECTDATA data[1];	\
	/*一个一个遍历的，S_FALSE不行	*/ \
	if(S_OK == (hNext=pEnum->Next(1, data, NULL))){	\
	UTIL::com_ptr<iid> p = data[0].pUnk;	\
	if(p) p->func;	\
	}}}\
}

//可以检查返回值的：rc 返回值， tc 检查返回值，为true就不继续枚举了
#define CallConnectPointFunc_Tc(obj, iid, func, rc, tc)	{	\
	UTIL::com_ptr<IMsEnumConnections> pEnum;	\
	if(SUCCEEDED(obj.EnumConnections((IMsEnumConnections**)&pEnum)) && pEnum){	\
	HRESULT hNext = S_OK;	\
	while(S_OK == hNext){	\
	MSCONNECTDATA data[1];	\
	/*一个一个遍历的，S_FALSE不行	*/ \
	if(S_OK == (hNext=pEnum->Next(1, data, NULL))){	\
	UTIL::com_ptr<iid> p = data[0].pUnk;	\
	if(p){ rc=p->func;	if(tc(rc)) break;}\
	}}}\
}


//连接点容器 实现者要支持QI IMsConnectionPointContainer
template<class T>
class CConnectionPointContainerHelper
	: public IMsConnectionPointContainer
{
protected:
	CONNECTIONPOINTS m_ConnectionPoints;	//连接点列表

public:
	//IMsConnectionPointContainer
	STDMETHOD(EnumConnectionPoints)(IMsEnumConnectionPoints **ppEnum)
	{
		RASSERT(ppEnum, E_POINTER);

		CMsEnumConnectionPoints* pNew = new CMsEnumConnectionPoints(m_ConnectionPoints);
		HRESULT hr = pNew->QueryInterface(__uuidof(IMsEnumConnectionPoints), (void**)ppEnum);
		if(NULL == *ppEnum) //失败
		{
			delete pNew;
			pNew = NULL;
		}

		return hr;
	}

	STDMETHOD(FindConnectionPoint)(REFIID riid, IMsConnectionPoint **ppCP)
	{
		RASSERT(ppCP, E_POINTER);
		RASSERT( m_ConnectionPoints.size() > 0, E_NOINTERFACE ); //本来应该是这个的CONNECT_E_NOCONNECTION ;

		CONNECTIONPOINTS::const_iterator it = m_ConnectionPoints.begin();
		for(; it != m_ConnectionPoints.end(); it++)
		{
			if(it->iid == riid) //找到了
			{
				RASSERT(it->p, E_POINTER);
				RFAILED(it->p->QueryInterface(__uuidof(IMsConnectionPoint), (void**)ppCP));
				RASSERT(*ppCP, E_POINTER);
				return S_OK;
			}
		}

		return E_NOINTERFACE; //本来应该是这个的CONNECT_E_NOCONNECTION ;
	}

	virtual void EmptyConnectionPointContainer()
	{
		m_ConnectionPoints.clear();
	}

	virtual BOOL AddConnectionPoint(IID iid, IMsConnectionPoint* p)
	{
		RASSERT(p, FALSE);

		ST_MsConnectionPoint point;
		point.iid = iid;
		point.p = p;
		m_ConnectionPoints.push_back(point);
		return TRUE;
	}

	virtual BOOL AddConnectionPoint(IID iid, CConnectionPointHelper &connectPointHelper, BOOL bAddRef = TRUE)
	{
		T* pThis = static_cast<T*>(this);
		UTIL::com_ptr<IMsConnectionPointContainer> p = pThis;
		RASSERT(p, FALSE);
		connectPointHelper.SetInfo(iid, p);

		if(bAddRef)
			connectPointHelper.AddRef(); //增加一次引用计数

		UTIL::com_ptr<IMsConnectionPoint> pPoint(&connectPointHelper);
		return AddConnectionPoint(iid, pPoint);
	}

	virtual BOOL DelConnectionPoint(REFIID riid)
	{
		CONNECTIONPOINTS::iterator it = m_ConnectionPoints.begin();
		for(; it != m_ConnectionPoints.end(); it++)
		{
			if(it->iid == riid) //找到了
			{
				m_ConnectionPoints.erase(it);
				return TRUE;
			}
		}
		return FALSE;
	}

	virtual BOOL DelConnectionPoint(REFIID riid, CConnectionPointHelper &connectPointHelper)
	{
		connectPointHelper.EmptyConnection();
		return DelConnectionPoint(riid);
	}
};

//////////////////////////////////////////////////////////////////////////
//连接点实现
struct ST_MSCONNECTDATA
{
	DWORD dwCookie;
	UTIL::com_ptr<IUnknown> p;
};
typedef std::vector<ST_MSCONNECTDATA> CONNECTIONS, *LPCONNECTIONS;

class CConnectionPointHelper
	: public IMsConnectionPoint
	, public CUnknownImp
{
public:
	CConnectionPointHelper(IID iid = GUID_NULL, IMsConnectionPointContainer* pContainer = NULL)
		: m_iidConnectionPoint(iid)
		, m_pContainer(pContainer)
		, m_dwCookieNum(0)
	{}

	virtual ~CConnectionPointHelper()
	{
		EmptyConnection();
	}
	
	UNKNOWN_IMP1(IMsConnectionPoint);

	void SetInfo(IID iid, IMsConnectionPointContainer* pContainer = NULL)
	{
		m_iidConnectionPoint = iid;
		m_pContainer = pContainer;
	}

	//IMsConnectionPoint
	STDMETHOD(GetConnectionInterface)(IID *pIID)
	{
		RASSERT(pIID, E_POINTER);

		*pIID = m_iidConnectionPoint;
		return S_OK;
	}
	STDMETHOD(GetConnectionPointContainer)(IMsConnectionPointContainer **ppCPC)
	{
		RASSERT(ppCPC, E_POINTER);

		*ppCPC = m_pContainer;
		RASSERT(*ppCPC, E_POINTER);

		return S_OK;
	}
	STDMETHOD(Advise)(IUnknown *pUnkSink, DWORD *pdwCookie)
	{
		RASSERT(pUnkSink, E_POINTER);
		RASSERT(pdwCookie, E_POINTER);

		*pdwCookie = ++m_dwCookieNum;

		ST_MSCONNECTDATA data;
		data.dwCookie = *pdwCookie;
		data.p = pUnkSink;
		m_Connections.push_back(data);

		return S_OK;
	}
	STDMETHOD(Unadvise)(DWORD dwCookie)
	{
		CONNECTIONS::iterator it = m_Connections.begin();
		for(; it != m_Connections.end(); it++)
		{
			if(it->dwCookie == dwCookie) //找到了
			{
				m_Connections.erase(it);
				return S_OK;
			}
		}

		return E_NOINTERFACE; //本来应该是这个的CONNECT_E_NOCONNECTION ;
	}
	STDMETHOD(EnumConnections)(IMsEnumConnections **ppEnum)
	{
		RASSERT(ppEnum, E_POINTER);

		CMsEnumConnections* pNew = new CMsEnumConnections(m_Connections);
		HRESULT hr = pNew->QueryInterface(__uuidof(IMsEnumConnections), (void**)ppEnum);
		if(NULL == *ppEnum) //失败
		{
			delete pNew;
			pNew = NULL;
		}

		return hr;
	}

	virtual void EmptyConnection()
	{
		m_Connections.clear();
		m_pContainer = INULL;
	}

	virtual HRESULT FindConnection(DWORD dwCookie, IID iid, void** p)
	{
		RASSERT(m_Connections.size() > 0, E_NOINTERFACE);

		for(size_t i = 0; i < m_Connections.size(); i++)
		{
			ST_MSCONNECTDATA &data = m_Connections[i];
			if(data.dwCookie == dwCookie)
			{
				RASSERT(data.p, E_NOINTERFACE);
				return data.p->QueryInterface(iid, p);
			}
		}

		return E_NOINTERFACE;
	}
	//////////////////////////////////////////////////////////////////////////
	class CMsEnumConnections
		: public IMsEnumConnections
		, public CUnknownImp
	{
	public:
		CMsEnumConnections(CONNECTIONS& Connections)
		: m_ulEnumConnectionPos(0)
		, m_Connections(Connections)
		{}
		UNKNOWN_IMP1(IMsEnumConnections);

		//IMsEnumConnections
		STDMETHOD(Next)( ULONG cConnections, LPMSCONNECTDATA rgcd, ULONG *pcFetched )
		{
			if(pcFetched)
				*pcFetched = 0;

			//从当前位置枚举cConnections个出去
			RASSERT(cConnections, S_OK); //请求就是0个直接返回成功
			RASSERT(rgcd, S_FALSE);

			size_t sSize = m_Connections.size();
			if(0 == sSize)
			{
				return S_FALSE;
			}

			ULONG i = 0;
			while((i < cConnections) && (m_ulEnumConnectionPos < sSize))
			{
				rgcd[i].dwCookie = m_Connections[m_ulEnumConnectionPos].dwCookie;
				if(m_Connections[m_ulEnumConnectionPos].p)
					m_Connections[m_ulEnumConnectionPos].p->QueryInterface(__uuidof(IUnknown), (void**)&rgcd[i].pUnk);

				++m_ulEnumConnectionPos;
				++i;
			}

			if(i < cConnections)//实际请求到的数量不够
			{
				if(pcFetched)
					*pcFetched = 0;

				return S_FALSE;
			}

			return S_OK;
		}

		STDMETHOD(Skip)( ULONG cConnections )
		{
			m_ulEnumConnectionPos += cConnections;
			return S_OK;
		}

		STDMETHOD(Reset)( void )
		{
			m_ulEnumConnectionPos = 0;
			return S_OK;
		}

		STDMETHOD(Clone)(IMsEnumConnections **ppEnum)
		{
			RASSERT(ppEnum, E_POINTER);

			CMsEnumConnections* pNew = new CMsEnumConnections(m_Connections);
			HRESULT hr = pNew->QueryInterface(__uuidof(IMsEnumConnections), (void**)ppEnum);
			if(NULL == *ppEnum) //失败
			{
				delete pNew;
				pNew = NULL;
			}

			return hr;
		}
	private:
		ULONG	m_ulEnumConnectionPos;
		CONNECTIONS &m_Connections;
	};

private:
	IID		m_iidConnectionPoint;
	UTIL::com_ptr<IMsConnectionPointContainer>  m_pContainer;
	DWORD	m_dwCookieNum;
	CONNECTIONS m_Connections;	//连接点列表
};

} //namespace msdk

#endif //_MSCONNECTIONPOINT_HELPER_H_