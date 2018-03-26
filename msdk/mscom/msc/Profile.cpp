#include "StdAfx.h"
#include "Profile.h"

#include "util/globalevent.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

CProfile* CProfile::s_this = NULL;
/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile()
{
	m_lpszPipeName = NULL;
    m_lpszMutexName = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CProfile::~CProfile()
{
	SAFE_DELETE_BUFFER(m_lpszPipeName);
	SAFE_DELETE_BUFFER(m_lpszMutexName);
 
	s_this = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CProfile* CProfile::GetInstance()
{
	if (NULL == s_this)
	{
		s_this = new CProfile();
	}

	return s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID CProfile::DestroyInstance()
{
	SAFE_DELETE(s_this);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfile::SetName(LPCTSTR lpszName)
{
	SAFE_DELETE_BUFFER(m_lpszPipeName);
	SAFE_DELETE_BUFFER(m_lpszMutexName);
  
	LPCTSTR lpszPipeName  = (NULL != lpszName) ? lpszName : GetDefaultPipeName();
    LPCTSTR lpszMutexName = (NULL != lpszName) ? lpszName : GetDefaultMutexName();

	DWORD dwLen = _tcslen(lpszPipeName);
	m_lpszPipeName = new TCHAR[dwLen + 10];
	if (NULL == m_lpszPipeName)
	{
		return FALSE;
	}
    m_lpszMutexName = new TCHAR[dwLen + 14];
    if (NULL == m_lpszMutexName)
    {
		SAFE_DELETE_BUFFER(m_lpszPipeName);
        return FALSE;
    }
	
	_tcscpy(m_lpszPipeName, _T("\\\\.\\PIPE\\"));
	_tcscat(m_lpszPipeName, lpszPipeName);

    _tcscpy(m_lpszMutexName, _T("Global\\Mutex."));
    _tcscat(m_lpszMutexName, lpszPipeName);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CProfile::GetPipeName()
{
	_ASSERT(NULL != m_lpszPipeName);
	return m_lpszPipeName;
}

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CProfile::GetMutexName()
{
    _ASSERT(NULL != m_lpszMutexName);
    return m_lpszMutexName;
}

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CProfile::GetDefaultPipeName()
{
	return _T("MSC.{E7A429B6-DE48-48D0-B48A-85F524FE9AA6}");
}

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CProfile::GetDefaultMutexName()
{
	return _T("Global\\Mutex.MSC.{E7A429B6-DE48-48D0-B48A-85F524FE9AA6}");
}

/////////////////////////////////////////////////////////////////////////////