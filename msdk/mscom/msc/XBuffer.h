#ifndef _X_BUFFER_H_
#define _X_BUFFER_H_

/////////////////////////////////////////////////////////////////////////////
class XBuffer
{
public:
	XBuffer();
	XBuffer(DWORD dwLen, BOOL& bResult);
	~XBuffer();
	
private:
	// I don't want that this constructor is used somewhere
	XBuffer(const XBuffer& other);
	XBuffer& operator=(const XBuffer& other);
	
public:
	BOOL   Alloc(DWORD dwLen);
	BOOL   Realloc(DWORD dwLen);
	VOID   Consume(XBuffer& other);
	VOID   AttachBuffer(LPBYTE lpBuffer, DWORD dwLen);
	VOID   DettachBuffer(LPBYTE& lpBuffer, DWORD& dwLen);
	VOID   DettachBuffer();
	LPBYTE Buffer();
	DWORD  Len();
	
protected:
	LPBYTE m_lpBuffer;
	DWORD  m_dwLen;
};

/////////////////////////////////////////////////////////////////////////////
#endif