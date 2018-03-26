#include "StdAfx.h"
#include "DuiByteStream.h"


#define DEFAULT_BUFFER_SIZE			MAX_PATH
#define DEFAULT_BLOCK_SIZE			1024	   //每次分配的内存块大小

CDuiByteStream::CDuiByteStream()
{
	m_dwBufSize = 0;
	m_dwDataSize = 0;
	m_lpByteBuf = NULL;
	Reserve();
}


CDuiByteStream::~CDuiByteStream(void)
{
	if(m_lpByteBuf)
	{
		free(m_lpByteBuf);
		m_lpByteBuf = NULL;
	}
}

BOOL CDuiByteStream::SetBuffer(LPBYTE lpBuf,DWORD dwSize,DWORD dwStartIndex /*= 0*/)
{
	if ( !( m_lpByteBuf && lpBuf && dwSize > 0 && dwStartIndex <= m_dwDataSize))
	{
		return FALSE;
	}
	
	if (dwSize + dwStartIndex <=m_dwBufSize){	
		CopyMemory(m_lpByteBuf+dwStartIndex,lpBuf,dwSize);
	}
	else{	
		DWORD dwTempBufLen = dwSize +dwStartIndex + DEFAULT_BLOCK_SIZE;
		m_lpByteBuf = (LPBYTE)realloc(m_lpByteBuf, dwTempBufLen);
		LPBYTE point = m_lpByteBuf+dwStartIndex;
		memcpy(point, lpBuf, dwSize);
		m_dwBufSize = dwTempBufLen;
	}

	m_dwDataSize = (dwSize+dwStartIndex);

	if( m_dwBufSize - m_dwDataSize )
	{
		memset(m_lpByteBuf + m_dwDataSize, 0, m_dwBufSize - m_dwDataSize);
	}
	return TRUE;
}

BOOL CDuiByteStream::Reserve(DWORD dwDesiredSize /*= 0*/)
{
	dwDesiredSize = dwDesiredSize ? dwDesiredSize : DEFAULT_BUFFER_SIZE;
	if (dwDesiredSize <= m_dwBufSize )
	{
		return TRUE;
	}

	if (m_lpByteBuf == NULL)
	{
		m_lpByteBuf = (LPBYTE)malloc(dwDesiredSize);
		memset(m_lpByteBuf, 0, dwDesiredSize);
		if ( !m_lpByteBuf )
		{
			return FALSE;
		}
	}
	else 
	{
		m_lpByteBuf = (LPBYTE) realloc(m_lpByteBuf, dwDesiredSize);
	}

	m_dwBufSize = dwDesiredSize;

	if( m_dwBufSize - m_dwDataSize )
	{
		memset(m_lpByteBuf + m_dwDataSize, 0, m_dwBufSize - m_dwDataSize);
	}

	return TRUE;
}

DWORD CDuiByteStream::GetReservedSize()
{
	return m_dwBufSize;
}

DWORD CDuiByteStream::GetBufferSize()
{
	return m_dwDataSize;
}

LPBYTE CDuiByteStream::GetData()
{
	return m_lpByteBuf;
}

IDuiByteStream* CDuiByteStream::Clone()
{
	CDuiObjectPtr<IDuiByteStream, IIDuiByteStream> pStream = CreateObject(OBJECT_CORE_BYTE_STREAM);
	if ( !pStream )
	{
		return NULL;
	}

	if ( ! (pStream->Reserve(m_dwBufSize) && pStream->SetBuffer(GetData(), GetBufferSize())))
	{
		pStream->DeleteThis();
		return NULL;
	}

	return pStream;
}

BOOL CDuiByteStream::AddTail(LPBYTE lpBuf,DWORD dwSize)
{
	return SetBuffer(lpBuf,dwSize,m_dwDataSize);
}

BOOL CDuiByteStream::AddTail(BYTE cc,DWORD dwCch)
{
	LPBYTE pBuf = (LPBYTE)malloc(dwCch);
	if ( !pBuf )
	{
		return FALSE;
	}

	memset(pBuf, cc, dwCch);

	BOOL bRet = SetBuffer(pBuf,dwCch,m_dwDataSize);
	free(pBuf);
	return bRet;
}

BOOL CDuiByteStream::AddHead(LPBYTE lpBuf,DWORD dwSize)
{
	if ( !(m_lpByteBuf && lpBuf && dwSize))
	{
		return FALSE;
	}

	if (dwSize + m_dwDataSize <=m_dwBufSize)
	{	
		CopyMemory(m_lpByteBuf+dwSize,m_lpByteBuf,m_dwDataSize);
		CopyMemory(m_lpByteBuf,lpBuf,dwSize);	
	}
	else
	{
		DWORD dwTempBufLen = dwSize + m_dwDataSize + DEFAULT_BLOCK_SIZE;
		m_lpByteBuf = (LPBYTE)realloc(m_lpByteBuf, dwTempBufLen);
		CopyMemory(m_lpByteBuf+dwSize,m_lpByteBuf,m_dwDataSize);
		CopyMemory(m_lpByteBuf,lpBuf,dwSize);
		m_dwBufSize = dwTempBufLen;
	}

	m_dwDataSize += dwSize;
	if( m_dwBufSize - m_dwDataSize )
	{
		memset(m_lpByteBuf + m_dwDataSize, 0, m_dwBufSize - m_dwDataSize);
	}
	return S_OK;
}

BOOL CDuiByteStream::Clear()
{
	m_dwDataSize = 0;
	return TRUE;	
}

BOOL CDuiByteStream::ClearAndFree()
{
	m_dwDataSize = 0;
	free(m_lpByteBuf);
	m_lpByteBuf = NULL;
	return Reserve();
}

VOID CDuiByteStream::SetBufSize(DWORD dwSize)
{
	m_dwDataSize = dwSize;
}
