#ifndef _SUBSCRIBER_H_
#define _SUBSCRIBER_H_

#include <queue>
#include "XMessageBuffer.h"
/////////////////////////////////////////////////////////////////////////////
class CSubscriber
{
public:
	CSubscriber(DWORD dwProcessID, DWORD dwThreadID, DWORD dwSubSystemID, HANDLE hPipe);
	~CSubscriber();

	LPOVERLAPPED ReadOverlapped();
	LPOVERLAPPED WriteOverlapped();
	HANDLE Handle();
	LPBYTE ReadBuffer();
	DWORD  ReadLen();
	LPBYTE WriteBuffer();
	DWORD  WriteLen();
	
	BOOL   IsReadFull();
	BOOL   IsWriteFull();
	BOOL   Read(DWORD dwDelta);
	VOID   Write(DWORD dwDelta);
	VOID   SetBroken();
	BOOL   IsBroken();
	
	DWORD  AddRef();
	DWORD  Release();

	BOOL   AttachWriteBuffer();
	VOID   DettachWriteBuffer();
	VOID   PendingWriteBuffer(XMessageBuffer* lpMessageBuffer);
	XMessageBuffer* DettachReadBuffer();

	DWORD ProcessID();
	DWORD ThreadID();
	DWORD SubSystemID();
	DWORD SubscriberID();
	
private:
	OVERLAPPED m_readOverlapped;
	OVERLAPPED m_writeOverlapped;
	DWORD  m_dwProcessID;
	DWORD  m_dwThreadID;
	DWORD  m_dwSubSystemID;
	DWORD  m_dwRefCount;
	HANDLE m_hPipe;
	BOOL   m_bBroken;
	
	XMessage        m_xMessage;
	XMessageBuffer  m_xMessageBuffer;
	XMessageBuffer* m_lpMessageInBuffer;
	XMessageBuffer* m_lpMessageOutBuffer;
	std::queue<XMessageBuffer*> m_queueMessageBuffer;
};

/////////////////////////////////////////////////////////////////////////////
#endif
