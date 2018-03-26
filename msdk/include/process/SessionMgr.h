#pragma once
#include "SyncObject/criticalsection.h"
#include <set>

interface IUserSessionNotify
{
	virtual void UserSessionLogon(DWORD dwSessionId, LPVOID lpVoid) = 0;
};

class CSessionMgr
{
public:
	CSessionMgr(void);
	~CSessionMgr(void);

	BOOL Init(IUserSessionNotify* pNotify = NULL);
	void Uninit();
	void Stop();

	BOOL SessionLogon(DWORD dwSessionId, LPVOID lpVoid = NULL);
	void WaitLogon(LPVOID lpVoid);

	static CSessionMgr* g_sessionMgr;
	static DWORD WINAPI WaitLogonThread(LPVOID lpVoid);

private:
	DECLARE_AUTOLOCK_CS(Thread);

	struct ST_THREAD_PARAM
	{
		DWORD  dwSessionId;
		LPVOID lpParam;
	};
	typedef ST_THREAD_PARAM* LPST_THREAD_PARAM;
	
	typedef std::set<HANDLE> HANDLE_SET;
	HANDLE_SET	m_handleSet;

	IUserSessionNotify* m_pNotifyCallback;
	HANDLE	m_hExit;
};
