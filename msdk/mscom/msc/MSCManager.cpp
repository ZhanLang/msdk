#include "StdAfx.h"
#include "MSCManager.h"
#include "Profile.h"
#include "SMSCSubscriberQueue.h"
#include "MSCHandler.h"
#include "MSCListener.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


CMSCManager* CMSCManager::s_this = NULL;
/////////////////////////////////////////////////////////////////////////////
CMSCManager::CMSCManager()
	:m_hMutex(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////
CMSCManager::~CMSCManager()
{
	s_this = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMSCManager* CMSCManager::GetInstance()
{
	if (NULL == s_this)
	{
		s_this = new CMSCManager();
	}

	return s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCManager::DestroyInstance()
{
	if (s_this)
	{
		delete s_this;
		s_this = NULL;
	}
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCManager::Initialize(LPCTSTR lpszName)
{
	CProfile* lpProfile = CProfile::GetInstance();
	if (NULL == lpProfile)
	{
		return FALSE;
	}
	if (!lpProfile->SetName(lpszName))
	{
		Uninitialize();
		return FALSE;
	}

	m_hMutex = CreateMutex(NULL, TRUE, lpProfile->GetMutexName());
	if (NULL == m_hMutex)
	{
		Uninitialize();
		return FALSE;
	}

	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		Uninitialize();
		return FALSE;
	}

	CSMSCSubscriberQueue* lpSMSCSubscriberQueue = CSMSCSubscriberQueue::GetInstance();
	if (NULL == lpSMSCSubscriberQueue)
	{
		Uninitialize();
		return FALSE;
	}
	if (!lpSMSCSubscriberQueue->Initialize())
	{
		Uninitialize();
		return FALSE;
	}


	CMSCHandler* lpMSCHandler = CMSCHandler::GetInstance();
	if (NULL == lpMSCHandler)
	{
		Uninitialize();
		return FALSE;
	}

	CMSCListener* lpMSCListener = CMSCListener::GetInstance();
	if (NULL == lpMSCListener)
	{
		Uninitialize();
		return FALSE;
	}
	if (!lpMSCListener->Initialize())
	{
		Uninitialize();
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCManager::Uninitialize()
{
	CMSCListener::DestroyInstance();
	CMSCHandler::DestroyInstance();
	CSMSCSubscriberQueue::DestroyInstance();
	
	if (NULL != m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	CProfile::DestroyInstance();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMSCManager::Start()
{
	if (!CMSCHandler::GetInstance()->Start())
	{
		return FALSE;
	}

	if (!CMSCListener::GetInstance()->Start())
	{
		Stop();
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMSCManager::Stop()
{
	CMSCListener::GetInstance()->Stop();
	CMSCHandler::GetInstance()->Stop();
}

