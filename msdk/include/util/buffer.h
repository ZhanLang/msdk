
#ifndef _CBuffer_H
#define _CBuffer_H

#define BUFFER_DEFAULT_SIZE 1
class CBuffer
{
public:
	CBuffer()
	{
		init();
	}

	~CBuffer()
	{
		uinit();
	}

public:
	CBuffer(const CBuffer& buf)
	{
		init();
		CloneFrom(buf);
	}

	CBuffer(unsigned char* lpBuf,unsigned long dwSize)
	{
		init();
		SetBuffer(lpBuf,dwSize);
	}

	CBuffer& operator =(const CBuffer& buf)
	{
		CloneFrom(buf);
		return *this;
	}

	operator unsigned char*()
	{
		return m_lpByteBuf;
	}
public:
	unsigned long GetDataSize() const
	{
		return m_dwDataSize;
	}

	unsigned long GetBufferSize() const
	{
		return m_dwBufSize;
	}

	bool GetRawBuffer(unsigned char** lppoutData,unsigned long* dwBufSize) const
	{
		if (lppoutData != NULL &&  dwBufSize != NULL)
		{
			*lppoutData = m_lpByteBuf;
			*dwBufSize = m_dwDataSize;
			return true;
		}
		return false;
	}

	bool SetBuffer(unsigned char* lpBuf,unsigned long dwSize,unsigned long dwStartIndex = 0)
	{
		if (m_lpByteBuf && lpBuf && dwSize>0 && dwStartIndex <= m_dwDataSize)
		{
			if (dwSize + dwStartIndex <= m_dwBufSize)
			{	
				memcpy(m_lpByteBuf+dwStartIndex,lpBuf,dwSize);
			}
			else
			{
				unsigned long dwTempBufLen = dwSize +dwStartIndex;
				unsigned char* lpTempBuf = Alloc(dwTempBufLen);
				if (lpTempBuf)
				{
					memcpy(lpTempBuf,m_lpByteBuf,m_dwDataSize);
					Free(m_lpByteBuf);
					m_lpByteBuf = lpTempBuf;
					memcpy(m_lpByteBuf+dwStartIndex,lpBuf,dwSize);
					m_dwBufSize = dwTempBufLen;
				}
			}
			m_dwDataSize = (dwSize+dwStartIndex);
			return true;
		}
		return false;
	}

	bool GetBuffer(unsigned char* lpoutData, unsigned long dwSize,unsigned long* pDwReadCount = 0,unsigned long dwStartIndex = 0) const
	{
		if (m_lpByteBuf && lpoutData && dwStartIndex <= m_dwDataSize)
		{
			unsigned long dwTempSize = Min(dwSize,m_dwDataSize-dwStartIndex);
			memcpy((void *)lpoutData,m_lpByteBuf+dwStartIndex,dwTempSize);
			if (pDwReadCount)
			{
				*pDwReadCount = dwTempSize;
			}
			return true;
		}

		return false;
	}
	
	bool Reserve(unsigned long dwDesiredSize)
	{
		if (dwDesiredSize <= m_dwBufSize )
		{
			return true;
		}

		if (!m_lpByteBuf)
		{
			m_lpByteBuf = Alloc(dwDesiredSize);
			if (!m_lpByteBuf)
			{
				return false;
			}
		}
		else 
		{
			unsigned char* lpTempBuf = Alloc(dwDesiredSize);
			if (lpTempBuf)
			{
				memset(lpTempBuf , 0 , dwDesiredSize);
				if (m_dwDataSize)
				{
					memcpy(lpTempBuf,m_lpByteBuf,m_dwDataSize);
				}

				Free(m_lpByteBuf );
				m_lpByteBuf = lpTempBuf;
			}
			else
			{
				return false;
			}
		}

		m_dwBufSize = dwDesiredSize;

		return true;
	}

	bool AddTail(unsigned char* lpBuf,unsigned long dwSize)
	{
		return SetBuffer(lpBuf,dwSize,m_dwDataSize);
	}

	bool AddHead(unsigned char* lpBuf,unsigned long dwSize)
	{
		if (!m_lpByteBuf || !lpBuf)
		{
			return false;
		}
		

		if (dwSize + m_dwDataSize <= m_dwBufSize)
		{	
			memcpy(m_lpByteBuf+dwSize,m_lpByteBuf,m_dwDataSize);
			memcpy(m_lpByteBuf,lpBuf,dwSize);	
		}
		else
		{
			unsigned long dwTempBufLen = dwSize + m_dwDataSize;

			unsigned char* lpTempBuf = Alloc(dwTempBufLen);
			if (lpTempBuf)
			{	
				memcpy(lpTempBuf+dwSize,m_lpByteBuf,m_dwDataSize);
				memcpy(lpTempBuf,lpBuf,dwSize);
				Free(m_lpByteBuf);
				m_lpByteBuf = lpTempBuf;
				m_dwBufSize = dwTempBufLen;
			}
			else
			{
				return false;
			}
		}

		m_dwDataSize += dwSize;

		return true;
	}

	//首先清空自己，然后进行拷贝工作
	bool CloneFrom(const CBuffer& buf)
	{
		Clear(FALSE);
		if (this->Reserve(buf.GetDataSize()))
		{
			unsigned char* lppoutData = NULL;
			unsigned long  dwDataSize = 0;
			if (buf.GetRawBuffer(&lppoutData , &dwDataSize))
			{
				this->SetBuffer(lppoutData,dwDataSize,0);
			}
			return true;
		}
		return false;
	}

	bool IsEmpty()
	{
		return m_dwDataSize == 0;
	}

	void Clear(BOOL bFree = FALSE)
	{
		if (bFree){
			uinit();
			init();
		}else{
			m_dwDataSize = 0;
		}
	}

	unsigned char* GetData()
	{
		return m_lpByteBuf;
	}

private:
	virtual unsigned char* Alloc(unsigned long dwSize)
	{
		if (dwSize)
		{
			unsigned char* pBuf = new unsigned char[dwSize];
			if (pBuf)
			{
				memset(pBuf , 0 , dwSize);
			}
			return pBuf;
		}

		return 0;
	}
	
	virtual void Free(unsigned char* pBuf)
	{
		if (pBuf)
		{
			delete[] pBuf;
		}
		pBuf = 0;
	}

	void init()
	{
		m_lpByteBuf = 0 ;
		m_dwBufSize = 0;
		m_dwDataSize = 0;
		Reserve(BUFFER_DEFAULT_SIZE);
	}

	void uinit()
	{
		if (m_lpByteBuf)
		{
			Free(m_lpByteBuf);
		}

		m_dwBufSize = 0;
		m_dwDataSize = 0;
	}

	template<typename T> inline
	const T& Min(const T& _one , const T& _second) const 
	{
		return _one < _second ? _one : _second;
	}
private:
	unsigned char* m_lpByteBuf;
	unsigned long  m_dwBufSize;
	unsigned long  m_dwDataSize;

};
#endif //_CBuffer_H