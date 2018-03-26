#ifndef _MSC_MANAGER_H_
#define _MSC_MANAGER_H_

/////////////////////////////////////////////////////////////////////////////
class CMSCManager
{
private:
	CMSCManager();
	~CMSCManager();

public:
	static CMSCManager* GetInstance();
	static VOID DestroyInstance();

	BOOL Initialize(LPCTSTR lpszName);
	VOID Uninitialize();

	BOOL Start();
	VOID Stop();

private:
	static CMSCManager* s_this;
	HANDLE m_hMutex;
};

/////////////////////////////////////////////////////////////////////////////
#endif
