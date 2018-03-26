#include "StdAfx.h"
#include "ObjectRun.h"
#include "MainRun.h"



CObjectRun::CObjectRun(void)
{
	m_bCreateCenter = FALSE;
}

CObjectRun::~CObjectRun(void)
{

}

HRESULT CObjectRun::init_class(IMSBase* prot, IMSBase* punkOuter)
{
	
	return S_OK;
}

HRESULT CObjectRun::StartPlugins()
{
	
	return S_OK;
}

HRESULT CObjectRun::StopPlugins()
{
	
	return S_OK;
}

HRESULT CObjectRun::InitPlugins()
{
	return S_OK;
}

HRESULT CObjectRun::UnInitPlugins()
{
	return S_OK;
}

STDMETHODIMP CObjectRun::Init(void*)
{
	m_Loader.Init();
	m_Rot.Init(NULL);
//	m_Rot.Register(CLSID_ObjectLoader , UTIL::com_ptr<IMSBase>(&m_Loader));
	
	OnRegiestRot();

	HRESULT hr = InitPlugins();
	return hr;
}

STDMETHODIMP CObjectRun::Uninit()
{
	UnInitPlugins();
//	m_Rot.Revoke(CLSID_ObjectLoader);
	m_Rot.Uninit();
	m_Loader.Uninit();

	return S_OK;
}

STDMETHODIMP CObjectRun::Start()
{
	return StartPlugins();
}

STDMETHODIMP CObjectRun::Stop()
{
	//m_Rot.Revoke(CLSID_ObjectLoader);
	return StopPlugins();
}



HRESULT CObjectRun::AddMsComFile(LPCTSTR lpFileName)
{
	
}

/*
<mscomcfg>
	<moudle guid="{9E5F158A-1199-4675-9BCF-E3C2670E7992}"  path=".\Common File\msxml3w.dll">
		<class clsid="{35FD921E-B758-46d8-B0AA-FCD033B0E66D}"  progid="IXMLTreeW"  name="CLSID_IXMLTreeW"/>
	</moudle>
</mscomcfg>
*/
HRESULT CObjectRun::AddMsComXml(LPCTSTR  lpXml)
{
	CRapidTreeS xmlDoc;
	RASSERT(xmlDoc.LoadBuff(lpXml) , E_FAIL);
	RASSERT(xmlDoc.GetRoot() , E_FAIL);
	
	//USES_GUIDCONVERSION;

	HXMLTREEITEM hRoot = xmlDoc.GetChild(xmlDoc.GetRoot());
	RASSERT(hRoot , E_FAIL);

	for (HXMLTREEITEM pMoudle = xmlDoc.GetChild(hRoot) ;
		pMoudle ;
		pMoudle = xmlDoc.GetNextItem(pMoudle))
	{
		LPCTSTR lpStrPaht = xmlDoc.GetAttribute(pMoudle , _T("path"));

		if (!lpStrPaht )
		{
			continue;
		}
		if (!PathFileExists(lpStrPaht))
		{
			continue;
		}

		TCHAR strFullPath[MAX_PATH] = {0};
		GetFullPathName(lpStrPaht , MAX_PATH , strFullPath , NULL);

		for (HXMLTREEITEM pClass = xmlDoc.GetChild(pMoudle);
			pClass;
			pClass = xmlDoc.GetNextItem(pClass))
		{
			LPCTSTR lpGuid = xmlDoc.GetAttribute(pClass , _T("clsid"));
			if (!lpGuid)
			{
				continue;
			}
			

			//m_Loader.Register(S2GUID(lpGuid) , item);
		}
	}

	return S_FALSE;
}

HRESULT CObjectRun::AddMsRotFile(LPCTSTR lpFileName)
{
	CRapidTreeS xmlDoc;
	RASSERT(xmlDoc.Load(lpFileName) , E_FAIL);

	int nLen = xmlDoc.GetXML(xmlDoc.GetRoot() , NULL , 0 , 0);
	RASSERT(nLen , E_FAIL);

	CString strXml;
	int nRead = 0;
	xmlDoc.GetXML(xmlDoc.GetRoot(),strXml.GetBufferSetLength(nLen+1) , nLen+1,&nRead);
	strXml.ReleaseBuffer();
	return AddMsRotXml(strXml.GetBuffer());
}


/*
<?xml version="1.0" encoding="utf-8"?>
<mscomcfg>
	<process>
		<mutex>Global\msdl_{B8C12EC5-292C-4196-90E2-1DA148D19289}</mutex>
		<msgcrt pipename="msdownload" iscenter="1"/>
	</process>
	<mainplugin>
			<class clsid="{53F64940-23FB-4170-9894-C38D17E019C0}" name="CLSID_DlUI"/>
	</mainplugin>
	<plugins/>
</mscomcfg>

*/
HRESULT CObjectRun::AddMsRotXml(LPCTSTR  lpXml)
{
	CRapidTreeS xmlDoc;
	RASSERT(xmlDoc.LoadBuff(lpXml) , E_FAIL);
	HXMLTREEITEM pMain = xmlDoc.LocalItem(xmlDoc.GetRoot(),_T("mscomcfg\\mainplugin\\class"));
	if (pMain)
	{
		
		
		LPCTSTR lpStrGuid = xmlDoc.GetAttribute(pMain ,_T("clsid"));
		   
		if (lpStrGuid)
		{
			UTIL::com_ptr<IMscomRunningObjectTable> pRot;
			m_Rot.QueryInterface(re_uuidof(IMscomRunningObjectTable) , (void**)&pRot);
			CLSID clsid = S2GUID(lpStrGuid);

			UTIL::com_ptr<IMSBase> puuk;
// 			if (SUCCEEDED(m_Loader.CreateInstance(pRot , clsid , NULL , re_uuidof(IMSBase) , (void**)&puuk)) && puuk)
// 			{
// 				//m_Rot.SetMainPlugin(clsid, puuk);
// 			}
		}
	}
	
	HXMLTREEITEM pPlugins = xmlDoc.LocalItem(xmlDoc.GetRoot(),_T("mscomcfg\\plugins"));
	
	if (pPlugins)
	{
		for (HXMLTREEITEM pPlugin = xmlDoc.GetChild(pPlugins);pPlugin; pPlugin = xmlDoc.GetNextItem(pPlugin))
		{
			LPCTSTR lpStrGuid = xmlDoc.GetAttribute(pPlugin ,_T("clsid"));
			if (lpStrGuid)
			{

				UTIL::com_ptr<IMscomRunningObjectTable> pRot;
				m_Rot.QueryInterface(re_uuidof(IMscomRunningObjectTable) , (void**)&pRot);
				CLSID clsid = S2GUID(lpStrGuid);
			}
		}
	}
	
	return S_OK;
}



HRESULT CObjectRun::OnRegiestRot()
{
	return S_OK;
}
