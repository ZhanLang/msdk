#include "stdafx.h"
#include "DuiBuilder.h"
#include <DuiCore/IDuiStream.h>
#include <DuiCore/IDuiFont.h>
#include <DuiCore/IDuiLang.h>
#include <DuiCore/IDuiImage.h>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

using namespace DuiKit;



CDuiBuilder::CDuiBuilder()
{
	m_pSkin = NULL;
	m_pCore = NULL;
	m_pRootCtrl = NULL;
	m_pCallBack = NULL;
}

CDuiBuilder::~CDuiBuilder()
{

}



BOOL CDuiBuilder::BuildAsFile(LPCWSTR lpszXmlFile, IDuiSkin* pSkin,  IDuiObject* pParenCtrl, IDuiBuilderCallBack* pCallBack)
{
	RASSERT( lpszXmlFile && wcslen(lpszXmlFile) && pSkin, FALSE);
	m_pSkin = pSkin;
	m_pCallBack = pCallBack;
	m_pCore = pSkin->GetCore();
	if(!m_pCore)
	{
//		GrpError(GroupName, MsgLevel_Error, L"必须为 IDuiSkin 设置 IDuiCore 对象");
		return FALSE;
	}

	IDuiByteStream* pStream = pSkin->GetStream(lpszXmlFile);
	if ( !(pStream && pStream->GetBufferSize()) )
	{
		return NULL;
	}

	pStream->AddTail((BYTE)'\0', 1);
	PBYTE lpData = pStream->GetData(); //utf8
	if ( lpData[0] == 0XEF && lpData[1] == 0XBB && lpData[2] == 0XBF)
	{
		lpData += 3;
		m_strContext = MultiByteToUnicodeString((LPCSTR)lpData,CP_UTF8);
	}
	else if (lpData[0] == 0xFF && lpData[1] == 0XFE ) //unicode
	{
		lpData += 2;
		m_strContext = (LPCWSTR) lpData;
	}
	else if (lpData[0] == 0XFE && lpData[1] == 0XFF) // unicode big endian
	{
		lpData += 2;

		DWORD dwSize = pStream->GetBufferSize() / 2 - 1;
		LPWSTR lpszBigEndian = (LPWSTR) lpData;

		for (DWORD dwLoop = 0 ; dwLoop < dwSize ; dwLoop++)
		{
			WCHAR cch = lpszBigEndian[dwLoop];
			UCHAR ccHigh = (cch & 0xFF00) >> 8;
			UCHAR ccLow  = cch & 0x00FF;

			cch = (ccLow << 8) | ccHigh;

			lpszBigEndian[dwLoop] = cch;
		}

		m_strContext = lpszBigEndian;
	}
	else //ansi
	{
		m_strContext = MultiByteToUnicodeString((LPCSTR)lpData,CP_ACP);
	}
	try
	{
		m_buildDocument.parse<rapidxml::parse_full>(m_strContext.GetBuffer()); 
	}
	catch (rapidxml::parse_error e)
	{
//		GrpErrorA(GroupNameA, MsgLevel_Error, " BuildAsFile Faild. msg:%s", e.what());
		return FALSE;
	}
	 
	if ( !Prase(pParenCtrl) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDuiBuilder::BuildAsXml(LPCWSTR lpszXml, IDuiSkin* pSkin, IDuiObject* pParentCtrl /*= NULL*/, IDuiBuilderCallBack* pCallBack)
{
	RASSERT( lpszXml && wcslen(lpszXml) && pSkin, FALSE);
	m_pCallBack = pCallBack;
	m_pSkin = pSkin;
	m_pCore = pSkin->GetCore();

	if ( !m_pCore )
	{
		//GrpError(GroupName, MsgLevel_Error, L"必须为 IDuiSkin 设置 IDuiCore 对象");
		return FALSE;
	}

	m_strContext = lpszXml;

	try
	{
		m_buildDocument.parse<rapidxml::parse_full>(m_strContext.GetBuffer()); 
	}
	catch (rapidxml::parse_error e)
	{
		//GrpErrorA(GroupNameA, MsgLevel_Error, " BuildAsXml Faild. msg:%s", e.what());
		return FALSE;
	}

	if ( !Prase(pParentCtrl) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL DuiKit::CDuiBuilder::Prase(IDuiObject* pParent)
{
	for (rapidxml::xml_node<WCHAR>* pNode = m_buildDocument.first_node() ; pNode ; pNode = pNode->next_sibling())
	{
		if ( !_Prase(pParent, pNode) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL DuiKit::CDuiBuilder::_Prase(IDuiObject* pParent,rapidxml::xml_node<WCHAR>* pNode)
{
	LPCWSTR lpszName = pNode->name();
	IDuiControlCtrl* tCtrl = NULL;
	IDuiObject* pObject = CreateObject(lpszName);
	HRESULT hDoCreate = S_OK;
	do 
	{
		if ( ! pObject )
		{
			break;
		}

		tCtrl = (IDuiControlCtrl*)pObject->QueryInterface(IIDuiControlCtrl);
		if ( tCtrl )
		{
			tCtrl->SetDuiCore(m_pCore);
			tCtrl->SetDuiSkin(m_pSkin);
		}

		for (rapidxml::xml_attribute<WCHAR>* pAttr = pNode->first_attribute() ; pAttr ; pAttr = pAttr->next_attribute())
		{
			pObject->SetAttribute(pAttr->name(), pAttr->value());
		}

		hDoCreate = pObject->DoCreate(pParent ? pParent : m_pSkin, m_pCore,this);
		if ( hDoCreate == E_FAIL)
		{
			pObject->DeleteThis();
			return TRUE;
		}
		
		if ( !m_pRootCtrl && tCtrl)
		{
			m_pRootCtrl = tCtrl;
		}

		pParent = pObject;

	} while (FALSE);
	
	for (rapidxml::xml_node<WCHAR>* tNode = pNode->first_node() ; tNode ; tNode = tNode->next_sibling())
	{
		if ( !_Prase(pParent, tNode) )
		{
			if ( pObject )
			{
				pObject->DeleteThis();
			}
			return FALSE;
		}
	}

	if (hDoCreate == S_FALSE)
	{
		pObject->DeleteThis();
		return TRUE;
	}

    if ( tCtrl )
    {
		tCtrl->SendMessage(tCtrl,DuiMsg_Initialize);
    }

	return TRUE;
}

IDuiControlCtrl* DuiKit::CDuiBuilder::GetRootControl()
{
	return m_pRootCtrl;
}

IDuiBuilderCallBack* DuiKit::CDuiBuilder::GetCallBack()
{
	return m_pCallBack;
}


