// RsXMLDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "RsXMLLoader.h"

#include "XMLProfile.h"
//#include "ixmltree.h"
#include "xmltree.h"
#include "rscom_interface/ixmltree2.h"
#include "rscom/rsdllexports.h"

#ifdef _DEBUG
	#pragma  comment (lib, "RsXmlLibMdD.lib")
#else
	#pragma  comment (lib, "RsXmlLibMd.lib")
#endif

R_SIMPLE_EXPORT;

R_EXPORT1(IXMLTree2, CXMLTree);


#define RsXML_EXPORTS

#ifdef RsXML_EXPORTS
#define RsXML_API __declspec(dllexport)
#else
#define RsXML_API __declspec(dllimport)
#endif

#define CALLAGREEMENT __cdecl

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

RsXML_API PRISING_XML CALLAGREEMENT xLoadFiles(LPCTSTR lpPath, LPCTSTR lpFileName, BOOL bLoadModuleXML)
{
	PRISING_XML pXML = new RISING_XML;

	CRsXMLLoader Temp;
	if(!Temp.LoadXMLFiles(lpPath, lpFileName, pXML, sizeof(RISING_XML), bLoadModuleXML))
	{
		delete pXML;
		return NULL;
	}
	return pXML;
}

RsXML_API PRSCOMPONENT CALLAGREEMENT xLoadComFiles(LPCTSTR lpPath, LPCTSTR lpFileName, BOOL bLoadModuleXML)
{
	PRSCOMPONENT pXML = new RSCOMPONENT;
	
	CRsXMLLoader Temp;
	if(!Temp.LoadXMLFiles(lpPath, lpFileName, (PRISING_XML)pXML, sizeof(RSCOMPONENT)), bLoadModuleXML)
	{
		delete pXML;
		return NULL;
	}
	return pXML;
}

RsXML_API BOOL CALLAGREEMENT xSaveFiles(LPCTSTR lpPath, LPCTSTR lpFileName, PRISING_XML pXML, UINT uSize, BOOL bSaveModuleXML)
{
	CRsXMLLoader Temp;
	return Temp.SaveXMLFiles(lpPath, lpFileName, pXML, uSize, bSaveModuleXML);
}

RsXML_API BOOL CALLAGREEMENT xFreeBuff(PRISING_XML lpBuff, UINT uSize)
{
	if(!lpBuff) return TRUE;

	if(uSize == sizeof(RISING_XML))
	{
		delete lpBuff;
	}
	else if(uSize == sizeof(RSCOMPONENT))
	{
		delete ((PRSCOMPONENT)lpBuff);
	}
	else
		return FALSE;

	return TRUE;
}

RsXML_API LPVOID CALLAGREEMENT GetDLLObject(DWORD dwType)
{
	switch(dwType)
	{
	case TYPE_XMLTREE:
		{
			IXMLTree2 *pTree = (IXMLTree2 *)new CXMLTree;
			return pTree;
		}
	default:
		return 0;
	}
}

#define S_FAIL_LOAD		0x10000001
#define S_FAIL_TOODEEP	0x10000002
#define S_FAIL_NOFOUND	0x10000003
#define S_FAIL_COREVER	0x10000004

RsXML_API LRESULT CALLAGREEMENT CheckNeedsXML(PRISING_XML pXML, IN LPCTSTR lpCodeName, LPCTSTR lpCoreVer, INT nDeep, OUT LPTSTR lpError, OUT INT nLen)
{
	if(!pXML || !lpCodeName) return FALSE;
	if (nDeep > 50)
	{
		if(lpError)
			_tcsncpy(lpError, lpCodeName, nLen);
		return S_FAIL_TOODEEP;
	}

	// 寻找当前模块
	PRSCOMPONENT pCom = NULL;
	TCHAR szComCodeName[32] ={0};
	for(COMPONENT_LIST_ITER i = pXML->component_list.begin();
			i != pXML->component_list.end(); i ++)
	{
		pCom = (PRSCOMPONENT)*i;
		ZeroMemory(szComCodeName, sizeof(szComCodeName));
		_tcsncpy(szComCodeName, pCom->attrib.szCodeName, sizeof(szComCodeName) - 1);
		_tcsupr(szComCodeName);
		if(pCom && _tcsstr(szComCodeName, lpCodeName) == szComCodeName)
			break;
		pCom = NULL;
	}
	if(!pCom)
	{
		if(lpError)
			_tcsncpy(lpError, lpCodeName, nLen);
		return S_FAIL_NOFOUND;// 没有找到依赖模块
	}

	// 检查当前模块版本
	if(lpCoreVer)
	{
		if(_tcsicmp(lpCoreVer, pCom->attrib.szCoreVersion))
		{
			_tcsncpy(lpError, lpCodeName, nLen);
			return S_FAIL_COREVER;
		}
	}

	// 检查当前模块依赖模块
	LRESULT lRet = 0;
	PDEPEND_ITEM pItem = NULL;
	for(DEPEND_LIST_ITER d = pCom->depend.begin();
			d != pCom->depend.end(); d ++)
	{
		PDEPEND_ITEM pItem = (PDEPEND_ITEM)*d;
		lRet = CheckNeedsXML(pXML, pItem->szCodeName, pItem->szCoreVersion, nDeep++, lpError, nLen);
		if(lRet)
			return lRet;
	}

	return S_OK;
}

RsXML_API LRESULT CALLAGREEMENT CheckNeeds(IN LPCTSTR lpPath, IN LPCTSTR lpFileName, IN LPCTSTR lpCodeName, OUT LPTSTR lpError, OUT INT nLen)
{
	PRISING_XML pXML = new RISING_XML;
	
	CRsXMLLoader Temp;
	if(!Temp.LoadXMLFiles(lpPath, lpFileName, pXML, sizeof(RISING_XML), TRUE))
	{
		delete pXML;
		return S_FAIL_LOAD;
	}
	
	BOOL bRet = CheckNeedsXML(pXML, lpCodeName, NULL, 0, lpError, nLen);
	
	delete pXML;
	return bRet;
}