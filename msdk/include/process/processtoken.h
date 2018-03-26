#if !defined(AFX_PROCESSTOKEN_H__5B5CF775_78D5_4FD8_949E_63A988A598B8__INCLUDED_)
#define AFX_PROCESSTOKEN_H__5B5CF775_78D5_4FD8_949E_63A988A598B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace msproc
{


//模拟进程令牌
class CProcessToken
{

public:
	CProcessToken():
		m_hAdvapi32Module(NULL),
		m_pOpenProcessToken(NULL),
		m_pImpersonateLoggedOnUser(NULL),
		m_pRevertToSelf(NULL)
	{
		m_hAdvapi32Module = LoadLibrary(_T("Advapi32.dll"));
		if(m_hAdvapi32Module)
		{
			m_pOpenProcessToken = (POPENPROCESSTOKEN)GetProcAddress(m_hAdvapi32Module, "OpenProcessToken");
			m_pImpersonateLoggedOnUser = (PIMPERSONATELOGGEDONUSER)GetProcAddress(m_hAdvapi32Module, "ImpersonateLoggedOnUser");
			m_pRevertToSelf = (PREVERTTOSELF)GetProcAddress(m_hAdvapi32Module, "RevertToSelf");
		}
	}

	virtual ~CProcessToken()
	{
		if (m_hAdvapi32Module)
		{
			FreeLibrary(m_hAdvapi32Module);
		}
		m_hAdvapi32Module = NULL;
		m_hAdvapi32Module = NULL;
		m_pOpenProcessToken = NULL;
		m_pImpersonateLoggedOnUser = NULL;
		m_pRevertToSelf = NULL;
	}

	/* 将当前线程的令牌权限调整到指定进程的令牌权限\n
	 *	@param[in] dwProcessID 进程id
	 *	@return 成功返回TRUE，否则返回FLASE
	 */
	BOOL SetThreadTokenAsProcessUser(DWORD dwProcessID)
	{
		BOOL bRet = FALSE;

		if(	m_pOpenProcessToken && m_pImpersonateLoggedOnUser && dwProcessID >= 1)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, dwProcessID);
			if(NULL == hProcess)
				return bRet;

			HANDLE hToken = NULL;
			if(m_pOpenProcessToken(hProcess, TOKEN_QUERY|TOKEN_DUPLICATE, &hToken))
			{
				bRet = m_pImpersonateLoggedOnUser(hToken);
				CloseHandle(hToken);
			}

			CloseHandle(hProcess);
		}

		return bRet;
	}

	/* 将当前线程的令牌权限恢复成原先的权限\n
	 *	@return 成功返回TRUE，否则返回FLASE
	 */
	BOOL RevertToSelf()
	{
		if (m_pRevertToSelf)
			return m_pRevertToSelf();
		else
			return FALSE;
	}

private:
	typedef BOOL (WINAPI* POPENPROCESSTOKEN)(HANDLE, DWORD, PHANDLE);
	typedef BOOL (WINAPI* PIMPERSONATELOGGEDONUSER)(HANDLE);
	typedef BOOL (WINAPI* PREVERTTOSELF)();

	/** Advapi32的实例
	*/
	HMODULE						m_hAdvapi32Module;

	/** 必要函数
	*/
	POPENPROCESSTOKEN			m_pOpenProcessToken;
	PIMPERSONATELOGGEDONUSER	m_pImpersonateLoggedOnUser;
	PREVERTTOSELF				m_pRevertToSelf;
};

};//msproc
