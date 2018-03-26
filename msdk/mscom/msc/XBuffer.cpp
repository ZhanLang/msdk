#include "StdAfx.h"
#include "XBuffer.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
XBuffer::XBuffer()
	:m_lpBuffer(NULL),
	 m_dwLen(0)
{
}

/////////////////////////////////////////////////////////////////////////////
XBuffer::XBuffer(DWORD dwLen, BOOL& bResult)
	:m_lpBuffer(NULL),
	 m_dwLen(0)
{
	bResult = Alloc(dwLen);
}

/////////////////////////////////////////////////////////////////////////////
XBuffer::~XBuffer()
{
	//ZM1_GrpDbg(GroupName,_T("[MSC]ÊÍ·ÅÏûÏ¢=0x%x\n"),m_lpBuffer);
	SAFE_DELETE_BUFFER(m_lpBuffer);
}

/////////////////////////////////////////////////////////////////////////////
BOOL XBuffer::Alloc(DWORD dwLen)
{
	LPBYTE lpTempBuffer = m_lpBuffer;
	
	m_lpBuffer = new BYTE[dwLen];
	if (NULL == m_lpBuffer)
	{
		m_lpBuffer = lpTempBuffer;
		return FALSE;
	}
	else
	{
		m_dwLen = dwLen;
		delete[] lpTempBuffer;
		return TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL XBuffer::Realloc(DWORD dwLen)
{
	// Shrinking the buffer is illegal
	_ASSERT(dwLen > m_dwLen);
	if (dwLen <= m_dwLen)
	{
		return FALSE;
	}
	
	LPBYTE lpTempBuffer = m_lpBuffer;
	
	m_lpBuffer = new BYTE[dwLen];
	if (NULL == m_lpBuffer)
	{
		m_lpBuffer = lpTempBuffer;
		return FALSE;
	}
	else
	{
		CopyMemory(m_lpBuffer, lpTempBuffer, m_dwLen);
		m_dwLen = dwLen;
		delete[] lpTempBuffer;
		return TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID XBuffer::Consume(XBuffer& other)
{
	//Can free myself
	SAFE_DELETE_BUFFER(m_lpBuffer);

	m_lpBuffer = other.m_lpBuffer;
	m_dwLen    = other.m_dwLen;
	other.m_lpBuffer = NULL;
	other.m_dwLen    = 0;
}

/////////////////////////////////////////////////////////////////////////////
VOID XBuffer::AttachBuffer(LPBYTE lpBuffer, DWORD dwLen)
{
	SAFE_DELETE_BUFFER(m_lpBuffer);
	m_lpBuffer = lpBuffer;
	m_dwLen    = dwLen;
}

/////////////////////////////////////////////////////////////////////////////
VOID XBuffer::DettachBuffer(LPBYTE& lpBuffer, DWORD& dwLen)
{
	lpBuffer   = m_lpBuffer;
	dwLen      = m_dwLen;
	m_lpBuffer = NULL;
	m_dwLen    = 0;
}

/////////////////////////////////////////////////////////////////////////////
VOID XBuffer::DettachBuffer()
{
	SAFE_DELETE_BUFFER(m_lpBuffer);
	m_lpBuffer = NULL;
	m_dwLen = 0;
}

/////////////////////////////////////////////////////////////////////////////
LPBYTE XBuffer::Buffer()
{
	return m_lpBuffer;
}

/////////////////////////////////////////////////////////////////////////////
DWORD XBuffer::Len()
{
	return m_dwLen;
}
