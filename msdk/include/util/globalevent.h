
// 项目名称: 
//
// 实现功能: 
//
// 文件名称: globalevent.h
//
// 创建作者: 
//
// 创建日期: 
//
// 修改作者: 
//
// 修改日期: 
//
// （如有新的修改人请在这里添加）
//
// 使用说明: 
//
//=============================================================================

#ifndef _GLOBALEVENT_INCLUDE_H_
#define _GLOBALEVENT_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif


#include <windows.h> //for WaitForSingleObject, CreateEvent...
#include <stdlib.h>	// for _MAX_PATH

namespace msdk {

#define	PREFIX_LOGFILE_WRITE_EVENT			"RS_WRITE_LOG_NOTIFY"

#define LOWER_CHAR(c)	(((c)>=_T('A')&&(c)<=_T('Z'))?(c+_T('a')-_T('A')):c)
#define UP_CHAR(c)		(((c)>=_T('a')&&(c)<=_T('z'))?(c+_T('A')-_T('a')):c)

/**
* 生成一个everyone访问的SECURITY_DESCRIPTOR
* <p>生成一个everyone访问的SECURITY_DESCRIPTOR
* @param sd SECURITY_DESCRIPTOR
* @param sid PSID
* @param acl PACL
* @return  返回TRUE是成功，FALSE是失败
*/
static BOOL Init_sd_to_everyone (SECURITY_DESCRIPTOR &sd, PSID &sid, PACL &acl)
{

	//return FALSE;
	SID_IDENTIFIER_AUTHORITY sid_auth = SECURITY_WORLD_SID_AUTHORITY;
	if (!AllocateAndInitializeSid (&sid_auth, 1,
		SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &sid))
		return FALSE;

	BOOL bOk = FALSE;
	do {
		unsigned long acl_length = sizeof(ACL) +
			sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(sid);
		if (!acl_length)
			break;

		acl = (PACL)(new char[acl_length]);
		if (!acl)
			break;

		do {
			if (!InitializeAcl (acl, acl_length, ACL_REVISION))
				break;
			if (!AddAccessAllowedAce (acl, ACL_REVISION, GENERIC_ALL, sid))
				break;
			if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
				break;
			if (SetSecurityDescriptorDacl (&sd, TRUE, acl, FALSE))
			{
				bOk = TRUE;
				break;
			}
		} while (0);

	} while (0);

	if(!bOk)
	{
		delete []acl;
		FreeSid(sid);
		sid = 0; acl = 0;
	}
	return bOk;
}

static void Free_sid_acl(PSID sid, PACL acl)
{
	if(acl)
	{
		delete []acl;
		acl = NULL;
	}
	if(sid)
	{
		FreeSid(sid);
		sid = NULL;
	}
}

class CGlobalEvent
{
	TCHAR m_szName[_MAX_PATH];
	HANDLE m_hObject;
	BOOL m_IsNt;	
	BOOL m_bIs9X;
public:
	CGlobalEvent():m_hObject(NULL)
	{
		//m_IsNt = !(GetVersion() & 0x80000000);
		m_bIs9X							= FALSE;
		m_IsNt							= FALSE;

		DWORD dwVersion					= GetVersion();
		DWORD dwWindowsMajorVersion		=  (DWORD)(LOBYTE(LOWORD(dwVersion)));

		m_bIs9X							= (dwVersion & 0x80000000)?TRUE:FALSE;
		m_IsNt							= (!m_bIs9X && dwWindowsMajorVersion<5)?TRUE:FALSE;

		ZeroMemory(m_szName, sizeof(m_szName));
	}
	CGlobalEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,  BOOL bManualReset,  BOOL bInitialState,  LPCTSTR lpName):m_hObject(NULL)
	{
		//m_IsNt = !(GetVersion() & 0x80000000);
		m_bIs9X							= FALSE;
		m_IsNt							= FALSE;

		DWORD dwVersion					= GetVersion();
		DWORD dwWindowsMajorVersion		=  (DWORD)(LOBYTE(LOWORD(dwVersion)));

		m_bIs9X							= (dwVersion & 0x80000000)?TRUE:FALSE;
		m_IsNt							= (!m_bIs9X && dwWindowsMajorVersion<5)?TRUE:FALSE;

		ZeroMemory(m_szName, sizeof(m_szName));
		CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
		//Assert(m_hObject);
	}
	~CGlobalEvent()
	{
		CloseHandle();
	}
	operator HANDLE() const
	{
		return m_hObject;
	}
	BOOL Lock(DWORD dwTimeout = INFINITE)
	{
		if (::WaitForSingleObject(m_hObject, dwTimeout) == WAIT_OBJECT_0)
			return TRUE;
		else
			return FALSE;
	}
	BOOL Unlock()
	{
		return TRUE;
	}	
	BOOL CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,  BOOL bManualReset,  BOOL bInitialState,  LPCTSTR lpName)
	{
		SECURITY_DESCRIPTOR sd;
		PSID sid = NULL;
		PACL acl = NULL;
		SECURITY_ATTRIBUTES sa = {0};

		if (!lpEventAttributes && Init_sd_to_everyone(sd, sid, acl))
		{
			sa.nLength = sizeof(sa);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = &sd;
			lpEventAttributes = &sa;
		}

		TCHAR szName[_MAX_PATH] = {0};
		if(lpName)
		{
			if( lstrlen(lpName)>250 ) return FALSE;		
			if ( !m_bIs9X && !m_IsNt )
			{
				lstrcpy(szName, _T("Global\\Event"));
				lstrcat(szName, lpName);
			}
			else 
				lstrcpy(szName, lpName);
			// 过滤掉非法字符，并且文件名都大写
			for(int i=(( !m_bIs9X && !m_IsNt )?12:0);i<lstrlen(szName);i++)
			{
				if( szName[i] == _T(':') ||
					szName[i] == _T('/') ||
					szName[i] == _T('\\')|| 
					szName[i] == _T(' ')) szName[i] = _T('_');
				else
					szName[i] = LOWER_CHAR(szName[i]);
			}
			if(0==lstrcmp(m_szName, szName) && m_hObject) return TRUE;

			m_hObject = ::CreateEvent(lpEventAttributes,bManualReset, bInitialState, szName );
			if(m_hObject) lstrcpy(m_szName, szName);
			else m_szName[0] = 0;
		}
		else
		{
			m_szName[0] = 0;
			m_hObject = ::CreateEvent(lpEventAttributes,bManualReset, bInitialState, NULL );
		}

		if(sid || acl)
		{
			Free_sid_acl(sid, acl);
			sid = NULL;
			acl = NULL;
		}

		return NULL!=m_hObject;
	}

	BOOL OpenEvent(DWORD dwDesiredAccess,  BOOL bInheritHandle,  LPCTSTR lpName)
	{
		TCHAR szName[_MAX_PATH] = {0};
		if( lstrlen(lpName)>250 ) return FALSE;		
		if ( !m_bIs9X && !m_IsNt )
		{
			lstrcpy(szName, _T("Global\\Event"));
			lstrcat(szName, lpName);
		}
		else 
			lstrcpy(szName, lpName);
		// 过滤掉非法字符，并且文件名都大写
		for(int i=(( !m_bIs9X && !m_IsNt )?12:0);i<lstrlen(szName);i++)
		{
			if( szName[i] == _T(':') ||
				szName[i] == _T('/') ||
				szName[i] == _T('\\')|| 
				szName[i] == _T(' ')) szName[i] = _T('_');
			else
				szName[i] = LOWER_CHAR(szName[i]);
		}

		if(m_hObject) ::CloseHandle(m_hObject);

		m_hObject = ::OpenEvent(dwDesiredAccess, bInheritHandle, szName);		
		if(m_hObject) lstrcpy(m_szName, szName);
		else m_szName[0] = 0;
		return NULL!=m_hObject;
	}
	void CloseHandle()
	{
		if(m_hObject)
		{			
			::CloseHandle(m_hObject);
			m_hObject = NULL;
			m_szName[0] = 0;
		}
	}
	BOOL SetEvent()
	{
		if(m_hObject) return ::SetEvent(m_hObject);
		return FALSE;
	}
	BOOL ResetEvent()
	{		
		if(m_hObject) return ::ResetEvent( (HANDLE)m_hObject );
		return FALSE;
	}
	void Attach(HANDLE hObject)
	{
		m_hObject = hObject;
	}
	HANDLE Detach()
	{
		HANDLE hTemp = m_hObject;
		m_hObject = NULL;
		return m_hObject;
	}
};

class CGlobalMutex
{
	TCHAR m_szName[_MAX_PATH];
	HANDLE m_hObject;
	BOOL m_IsNt;
	BOOL m_bIs9X;
public:
	CGlobalMutex():m_hObject(NULL)
	{
		//m_IsNt = !(GetVersion() & 0x80000000);
		m_bIs9X							= FALSE;
		m_IsNt							= FALSE;

		DWORD dwVersion					= GetVersion();
		DWORD dwWindowsMajorVersion		=  (DWORD)(LOBYTE(LOWORD(dwVersion)));

		m_bIs9X							= (dwVersion & 0x80000000)?TRUE:FALSE;
		m_IsNt							= (!m_bIs9X && dwWindowsMajorVersion<5)?TRUE:FALSE;

		ZeroMemory(m_szName, sizeof(m_szName));
	}
	CGlobalMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,  BOOL bInitialOwner,  LPCTSTR lpName):m_hObject(NULL)
	{
		//m_IsNt = !(GetVersion() & 0x80000000);

		m_bIs9X							= FALSE;
		m_IsNt							= FALSE;

		DWORD dwVersion					= GetVersion();
		DWORD dwWindowsMajorVersion		=  (DWORD)(LOBYTE(LOWORD(dwVersion)));

		m_bIs9X							= (dwVersion & 0x80000000)?TRUE:FALSE;
		m_IsNt							= (!m_bIs9X && dwWindowsMajorVersion<5)?TRUE:FALSE;

		ZeroMemory(m_szName, sizeof(m_szName));
		CreateMutex(lpMutexAttributes, bInitialOwner, lpName);
		//Assert(m_hObject);
	}
	~CGlobalMutex()
	{
		CloseHandle();
	}
	operator HANDLE() const
	{
		return m_hObject;
	}
	BOOL Lock(DWORD dwTimeout = INFINITE)
	{
		switch(WaitForSingleObject(m_hObject, dwTimeout))
		{
		case WAIT_ABANDONED:
			//此为程序非法退出时状态，即无信号状态，这时应释放Mutex，获得信号，否则程序再一次启动时就会出现死等
			ReleaseMutex(m_hObject);
			return TRUE;
		case WAIT_OBJECT_0:
			return TRUE;
		case WAIT_TIMEOUT:
			return FALSE;
		default:
			break;
		}
		return FALSE;
	}
	BOOL Unlock()
	{
		::ReleaseMutex(m_hObject);
		return TRUE;
	}	
	BOOL CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,  BOOL bInitialOwner,  LPCTSTR lpName )
	{
		SECURITY_DESCRIPTOR sd;
		PSID sid = NULL;
		PACL acl = NULL;
		SECURITY_ATTRIBUTES sa = {0};
		if (!lpMutexAttributes && Init_sd_to_everyone(sd, sid, acl))
		{
			sa.nLength = sizeof(sa);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = &sd;
			lpMutexAttributes = &sa;
		}

		TCHAR szName[_MAX_PATH] = {0};
		if(lpName)
		{
			if( lstrlen(lpName)>250 ) return FALSE;		
			if ( !m_bIs9X && !m_IsNt )
			{
				lstrcpy(szName, _T("Global\\Mutex"));
				lstrcat(szName, lpName);
			}
			else 
				lstrcpy(szName, lpName);
			// 过滤掉非法字符，并且文件名都大写
			for(int i=(( !m_bIs9X && !m_IsNt )?12:0);i<lstrlen(szName);i++)
			{
				if( szName[i] == _T(':') ||
					szName[i] == _T('/') ||
					szName[i] == _T('\\')|| 
					szName[i] == _T(' ')) szName[i] = _T('_');
				else
					szName[i] = UP_CHAR(szName[i]);
			}
			if(0==lstrcmp(m_szName, szName) && m_hObject) return TRUE;

			m_hObject = ::CreateMutex(lpMutexAttributes,bInitialOwner, szName );
			if(m_hObject) lstrcpy(m_szName, szName);
			else m_szName[0] = 0;
		}
		else
		{
			m_szName[0] = 0;
			m_hObject = ::CreateMutex(lpMutexAttributes,bInitialOwner, NULL );
		}
		
		if(sid || acl)
		{
			Free_sid_acl(sid, acl);
			sid = NULL;
			acl = NULL;
		}

		return NULL!=m_hObject;
	}
	BOOL OpenMutex(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
	{
		TCHAR szName[_MAX_PATH] = {0};
		if( lstrlen(lpName)>250 ) return FALSE;		
		if ( !m_bIs9X && !m_IsNt )
		{
			lstrcpy(szName, _T("Global\\Mutex"));
			lstrcat(szName, lpName);
		}
		else 
			lstrcpy(szName, lpName);
		// 过滤掉非法字符，并且文件名都大写
		for(int i=(( !m_bIs9X && !m_IsNt )?12:0);i<lstrlen(szName);i++)
		{
			if( szName[i] == _T(':') ||
				szName[i] == _T('/') ||
				szName[i] == _T('\\')|| 
				szName[i] == _T(' ')) szName[i] = _T('_');
			else
				szName[i] = UP_CHAR(szName[i]);
		}

		if(m_hObject) ::CloseHandle(m_hObject);

		m_hObject = ::OpenMutex(dwDesiredAccess, bInheritHandle, szName);		
		if(m_hObject) lstrcpy(m_szName, szName);
		else m_szName[0] = 0;
		return NULL!=m_hObject;
	}
	void CloseHandle()
	{
		if(m_hObject)
		{			
			::CloseHandle(m_hObject);
			m_hObject = NULL;
			m_szName[0] = 0;
		}
	}
	void Attach(HANDLE hObject)
	{
		m_hObject = hObject;
	}
	HANDLE Detach()
	{
		HANDLE hTemp = m_hObject;
		m_hObject = NULL;
		return m_hObject;
	}
};
} //namespace msdk

#endif // _GLOBALEVENT_INCLUDE_H_
