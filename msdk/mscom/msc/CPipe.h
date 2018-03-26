#ifndef _C_PIPE_H_
#define _C_PIPE_H_

#include "XMessageBuffer.h"
/////////////////////////////////////////////////////////////////////////////
class CCPipe
{
public:
	CCPipe(HANDLE hPipe);
	~CCPipe();

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
	
	VOID   DettachWriteBuffer();
	BOOL   AttachWriteBuffer(XMessage* lpXMessage);
	BOOL   IsCanAttachWriteBuffer();
	XMessage* DettachReadBuffer();
private:
	OVERLAPPED m_readOverlapped;
	OVERLAPPED m_writeOverlapped;
	HANDLE m_hPipe;
	BOOL   m_bBroken;
	
	XMessage        m_xMessage;
	XMessageBuffer  m_xMessageBuffer;
	XMessageBuffer* m_lpMessageInBuffer;
	XMessageBuffer* m_lpMessageOutBuffer;
};

/////////////////////////////////////////////////////////////////////////////
#endif