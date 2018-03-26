#ifndef _PROFILE_H_
#define _PROFILE_H_

/////////////////////////////////////////////////////////////////////////////
class CProfile
{
private:
	CProfile();
	~CProfile();

public:
	static CProfile* GetInstance();
	static VOID DestroyInstance();

	LPCTSTR GetDefaultPipeName();
	LPCTSTR GetDefaultMutexName();

	BOOL    SetName(LPCTSTR lpszName);
	LPCTSTR GetPipeName();
    LPCTSTR GetMutexName();

private:
	static CProfile* s_this;
	LPTSTR m_lpszPipeName;
    LPTSTR m_lpszMutexName;
};

/////////////////////////////////////////////////////////////////////////////
#endif