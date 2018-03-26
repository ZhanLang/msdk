#ifndef _X_THREAD_H_
#define _X_THREAD_H_

/////////////////////////////////////////////////////////////////////////////
class _declspec(novtable) XThread
{
public:
	XThread();
	virtual ~XThread();

	virtual BOOL Start();
	virtual VOID Stop();

protected:
	virtual VOID SVC() = 0;

protected:
	VOID ThreadProcImpl();
	static UINT WINAPI ThreadProc(LPVOID lpParameter);

	HANDLE m_hThread;
	HANDLE m_hExitEvent;
};

/////////////////////////////////////////////////////////////////////////////
#endif