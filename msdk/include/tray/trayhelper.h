

#ifndef _MSTRAY_HELPER_H_
#define _MSTRAY_HELPER_H_

#include <tray\trayplugin.h>

namespace msdk {

class CCreateTrayHelper
{
public:
	~CCreateTrayHelper(){DestroyTray();}

	UTIL::com_ptr<ITraySrv> m_pTraySrv;

	HRESULT CreateTray(IMscomRunningObjectTable* pRot, UINT nMsgID, UINT nUID, CLSID clsTraySrvPlugin/* = ClSID_CTrayMenuBase*/)
	{
		RFAILED(CreateTraySrv(pRot));

		RFAILED(m_pTraySrv->CreateTray(nMsgID, nUID, clsTraySrvPlugin));	//把托盘创建出来吧

		return S_OK;
	}

	HRESULT CreateTray(IMscomRunningObjectTable* pRot, UINT nMsgID, UINT nUID, ITraySrvPlugin* pTraySrvPlugin = NULL)
	{
		RFAILED(CreateTraySrv(pRot));

		RFAILED(m_pTraySrv->CreateTray(nMsgID, nUID, pTraySrvPlugin));	//把托盘创建出来吧

		return S_OK;
	}

	HRESULT CreateTraySrv(IMscomRunningObjectTable* pRot)
	{
		RASSERT(pRot, E_FAIL);

		//一定是通过ObjectLoader创建一份新的
		UTIL::com_ptr<IMsObjectLoader> pObjectLoader;
		RFAILED(pRot->GetObject(CLSID_ObjectLoader, __uuidof(IMsObjectLoader), (VOID**)&pObjectLoader));
		RASSERT(pObjectLoader, E_FAIL);

		RFAILED(pObjectLoader->CreateInstance(pRot, ClSID_CTraySrv, NULL, __uuidof(ITraySrv), (void**)&m_pTraySrv));
		RASSERT(m_pTraySrv, E_FAIL);

		UTIL::com_ptr<IMsPlugin> pPlugin(m_pTraySrv);
		if(pPlugin)
		{
			RFAILED(pPlugin->Init(NULL));
		}

		return S_OK;
	}

	HRESULT DestroyTray()
	{
		if(m_pTraySrv)
		{
			m_pTraySrv->DestroyTray();

			UTIL::com_ptr<IMsPlugin> pPlugin(m_pTraySrv);
			if(pPlugin)
			{
				pPlugin->Uninit();
			}

			m_pTraySrv = INULL;
		}

		return S_OK;
	}

	operator ITraySrv* () const { return m_pTraySrv; }
	ITraySrv* operator ->() const {return m_pTraySrv;}
};

} //namespace msdk

#endif	//_MSTRAY_HELPER_H_