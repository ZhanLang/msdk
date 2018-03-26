#ifndef _MSC_LISTENER_H_
#define _MSC_LISTENER_H_

#include <list>
#include "XThread.h"
#include "SPipe.h"
/////////////////////////////////////////////////////////////////////////////
class CMSCListener :public XThread
{
private:
	CMSCListener();
	virtual ~CMSCListener();

public:
	static CMSCListener* GetInstance();
	static VOID DestroyInstance();
	BOOL Initialize();
	VOID Uninitialize();

    //要判断是否管道启动了
    virtual BOOL Start();
    virtual VOID Stop();

protected:
	virtual VOID SVC();

private:
	BOOL PendingAccept();
	BOOL PendingAcceptEx();
	BOOL PendingRead(CSPipe* lpSPipe);
	BOOL PendingWrite(CSPipe* lpSPipe);
	BOOL Accept();
	BOOL Read(DWORD dwErrCode, DWORD dwTransfered, CSPipe* lpSPipe);
	BOOL Write(DWORD dwErrCode, DWORD dwTransfered, CSPipe* lpSPipe);
	VOID Close(CSPipe* lpSPipe);
	VOID LoginCheck();
	
	static VOID WINAPI CompletedReadRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped);
	static VOID WINAPI CompletedWriteRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped);
	
	enum
	{
		MSC_BUFSIZE           = 2 * 1024,
		MSC_RETRYMAXCOUNT     = 3,
		MSC_LOGONCHECKPERIOD  = 2 * 1000,
		MSC_CLIENTWAITTIMEOUT = 1000,
	};

    //等待线程启动成功
    HANDLE m_hEvent;
    BOOL   m_bStartResult;

	BOOL   m_bExitting;
	HANDLE m_hAcceptEvent;
	
	HANDLE m_hPipeAccept;
	LPTSTR m_lpszPipeName;
	OVERLAPPED m_overlappedAccept;

	// I think there is not so much Subsystem that list is effective than map.
	std::list<CSPipe*> m_listSPipe;
	static CMSCListener* s_this;
};

/////////////////////////////////////////////////////////////////////////////
#endif
