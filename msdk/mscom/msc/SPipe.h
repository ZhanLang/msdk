#ifndef _S_PIPE_H_
#define _S_PIPE_H_

#include "XMessageBuffer.h"
#include "Subscriber.h"
/////////////////////////////////////////////////////////////////////////////
class CSPipe :public OVERLAPPED
{
public:
	CSPipe(HANDLE hPipe);
	~CSPipe();

	LPOVERLAPPED Overlapped();
	HANDLE Handle();
	LPBYTE Buffer();
	DWORD  Len();

	BOOL   IsFull();
	BOOL   Read(DWORD dwDelta);
	VOID   Write(DWORD dwDelta);
	
	BOOL   Verify();
	BOOL   MakeResponse();
	BOOL   IsTimeOut(DWORD dwCheckTime);
	CSubscriber* Logon();

protected:	
	HANDLE m_hPipe;
	DWORD  m_dwAcceptTime;

	XMessage        m_xMessage;
	XMessageBuffer  m_xMessageBuffer;
	XMessageBuffer* m_lpMessageInBuffer;
	XMessageBuffer* m_lpMessageOutBuffer;
};

/////////////////////////////////////////////////////////////////////////////
#endif
