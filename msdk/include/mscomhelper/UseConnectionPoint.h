
#ifndef _MSUSECONNECTIONPOINT_HELPER_H_
#define _MSUSECONNECTIONPOINT_HELPER_H_

#include <mscom/connectionpoint.h>
#include <vector>

namespace msdk {

template<class iid>
class UseConnectPoint
{
public:
	UTIL::com_ptr<IMsConnectionPoint> m_pConnectPoint;
	DWORD	m_dwConnectPointCookie;
	BOOL	m_bConnected;

	UseConnectPoint():m_dwConnectPointCookie(-1),m_bConnected(FALSE){};
	~UseConnectPoint(){DisConnect();}

	HRESULT Connect(IUnknown* pConnectionPointContainer, IUnknown* pThis)
	{
		UTIL::com_ptr<IMsConnectionPointContainer> pCPC(pConnectionPointContainer);
		RASSERT(pCPC, E_FAIL);

		RFAILED(pCPC->FindConnectionPoint(__uuidof(iid), (IMsConnectionPoint**)&m_pConnectPoint));
		RASSERT(m_pConnectPoint, E_FAIL);

		UTIL::com_ptr<iid> pMCP(pThis);
		RFAILED(m_pConnectPoint->Advise(pMCP, &m_dwConnectPointCookie));

		m_bConnected = TRUE;
		return S_OK;
	}

	HRESULT DisConnect()
	{
		if(m_pConnectPoint)
		{
			if(m_bConnected)
				m_pConnectPoint->Unadvise(m_dwConnectPointCookie);
			m_pConnectPoint = INULL;
		}

		m_bConnected = FALSE;
		return FALSE;
	}
};

} //namespace msdk

#endif //_MSUSECONNECTIONPOINT_HELPER_H_