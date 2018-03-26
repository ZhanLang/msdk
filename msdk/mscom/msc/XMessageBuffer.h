#ifndef _X_MESSAGE_BUFFER_H_
#define _X_MESSAGE_BUFFER_H_

#include "XBuffer.h"
#include "XMessage.h"
/////////////////////////////////////////////////////////////////////////////
class XMessageBuffer
{
public:
	XMessageBuffer();
	XMessageBuffer(DWORD dwDataLen, BOOL& bResult);
	~XMessageBuffer();

	XMessageBuffer* Clone();
	XMessage* Header();
	LPBYTE Data();
	DWORD  DataLen();

	LPBYTE RestBuffer();
	DWORD  RestLen();
	LPBYTE Buffer();
	DWORD  Len();
	
	DWORD  Pos();
	VOID   Pos(DWORD dwPos);
	VOID   ResetPos();
	VOID   Advance(DWORD dwDelta);
	BOOL   IsFull();
	
	BOOL   Alloc(DWORD dwDataLen);
	BOOL   Realloc(DWORD dwDataLen);

	VOID   AttachBuffer(LPBYTE lpBuf, DWORD dwLen);
	VOID   DettachBuffer(LPBYTE& lpBuf, DWORD& dwLen);
	VOID   DettachBuffer();

protected:
	XBuffer  m_xBuffer;
	DWORD    m_dwPos;
};

/////////////////////////////////////////////////////////////////////////////

#endif
