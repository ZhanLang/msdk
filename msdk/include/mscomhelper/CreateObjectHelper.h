
#ifndef _MSCREATEOBJECT_HELPER_H_
#define _MSCREATEOBJECT_HELPER_H_

namespace msdk {

template<class I>
class CCreateObjectHelper
{
public:
	UTIL::com_ptr<I> m_pObject;

	CCreateObjectHelper():m_bInitObject(FALSE){}
	~CCreateObjectHelper(){Release();}

	HRESULT CreateInstance(IMscomRunningObjectTable* pRot, const CLSID& rclsid, IMSBase *punkOuter = NULL)
	{
		RASSERT(pRot, E_FAIL);
		RASSERT(rclsid != GUID_NULL, E_NOINTERFACE);

		if(rclsid != GUID_NULL)
		{
			UTIL::com_ptr<IMsObjectLoader> pObjectLoader;
			RFAILED(pRot->GetObject(CLSID_ObjectLoader, __uuidof(IMsObjectLoader), (VOID**)&pObjectLoader));
			RASSERT(pObjectLoader, E_FAIL);

			RFAILED(pObjectLoader->CreateInstance(pRot, rclsid, NULL, __uuidof(I), (void**)&m_pObject));
			RASSERT(m_pObject, E_FAIL);

			//判断m_pObject是否支持初始化啥的，支持的话，初始化一下吧
			UTIL::com_ptr<IMsPlugin> pPlugin(m_pObject);
			if(pPlugin)
			{
				m_bInitObject = TRUE;
				RFAILED(pPlugin->Init(NULL));
			}
		}
		
		return S_OK;
	}

	HRESULT Release()
	{
		if(m_pObject)
		{
			if(m_bInitObject)
			{
				m_bInitObject = FALSE;
				UTIL::com_ptr<IMsPlugin> pPlugin(m_pObject);
				if(pPlugin)
					pPlugin->Uninit();
			}
			m_pObject = INULL;
		}

		return S_OK;
	}

	CCreateObjectHelper& operator = (I* p) 
	{
		m_pObject = p;
		return *this;
	}

	I* operator -> () const {return m_pObject;}
	operator I* () const {return m_pObject;}

private:
	BOOL	m_bInitObject;
};

} //namespace msdk

#endif //_MSCREATEOBJECT_HELPER_H_