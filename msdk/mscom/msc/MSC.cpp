#include "StdAfx.h"
#include "MSC.h"
#include "XMSCProxy.h"
#include "CXMSCProxy.h"
#include "MSCManager.h"
#include "MSCDef.h"
#include "MsMsc.h"
#include "MsXMSCProxy.h"


/////////////////////////////////////////////////////////////////////////////
MSCAPI BOOL WINAPI MSC_Initialize(LPCTSTR lpszProduct, LPCTSTR lpszInstallPath)
{
	CMSCManager* lpMSCManager = CMSCManager::GetInstance();
	if (NULL == lpMSCManager)
	{
		return FALSE;
	}

	return lpMSCManager->Initialize(lpszProduct);
}

/////////////////////////////////////////////////////////////////////////////
MSCAPI VOID WINAPI MSC_Uninitialize()
{
	CMSCManager::GetInstance()->Uninitialize();
	CMSCManager::DestroyInstance();
}

/////////////////////////////////////////////////////////////////////////////
MSCAPI BOOL WINAPI MSC_Start()
{
	return CMSCManager::GetInstance()->Start();
}

/////////////////////////////////////////////////////////////////////////////
MSCAPI VOID WINAPI MSC_Stop()
{
	CMSCManager::GetInstance()->Stop();
}

/////////////////////////////////////////////////////////////////////////////
MSCAPI BOOL WINAPI MSC_CreateMSCProxy(XMSCProxy** lppXMSCProxy)
{
	if (NULL == lppXMSCProxy)
	{
		return FALSE;
	}

	*lppXMSCProxy = new CXMSCProxy();
	if (NULL == *lppXMSCProxy)
	{
		return FALSE;
	}

	(*lppXMSCProxy)->AddRef();
	return TRUE;
}

