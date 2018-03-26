#ifndef _MSC_HANDER_H_
#define _MSC_HANDER_H_

#include "XThread.h"
#include "Subscriber.h"
#include "SMSCMessageMap.h"
#include "SMSCSubscriberList.h"
#include "XMessageBuffer.h"
/////////////////////////////////////////////////////////////////////////////
class CMSCHandler :public XThread
{
private:
	CMSCHandler();
	~CMSCHandler();

public:
	static CMSCHandler* GetInstance();
	static VOID DestroyInstance();
	BOOL   TryPendingWrite(CSubscriber* lpSubscriber);

    virtual BOOL Start();
    virtual VOID Stop();

protected:
	virtual VOID SVC();

private:
	VOID AddSubscriber();
	VOID Handle(XMessageBuffer* lpMessageBuffer, CSubscriber* lpSubscriber);
	
	BOOL PendingRead(CSubscriber* lpSubscriber);
	BOOL PendingWrite(CSubscriber* lpSubscriber);
	BOOL Read(DWORD dwErrCode, DWORD dwTransfered, CSubscriber* lpSubscriber);
	BOOL Write(DWORD dwErrCode, DWORD dwTransfered, CSubscriber* lpSubscriber);
	VOID Close(CSubscriber* lpSubscriber);

	static VOID WINAPI CompletedReadRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped);
	static VOID WINAPI CompletedWriteRoutine(DWORD dwErrCode, DWORD dwTransfered, LPOVERLAPPED lpOverlapped);
	
	BOOL m_bExitting;
	CSMSCMessageMap     m_objMessageMap;
	CSMSCSubscriberList m_objSubscriberList;
	static CMSCHandler* s_this;
    HANDLE              m_hEvent;
};

/////////////////////////////////////////////////////////////////////////////
#endif