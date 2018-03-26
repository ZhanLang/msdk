#pragma once
#include <SyncObject\criticalsection.h>
namespace msdk{

//只限制在本进程内使用
//尽量不要边写边读
template<typename DateType = BYTE, typename LockType = CNullCriticalSection >
class mapfilestream_imp
{
public:
	typedef CStackLockWrapper<LockType> SRVATUOLOCK;

	mapfilestream_imp(DWORD dwLeng = 0)
	{
		m_dwMaxLen = dwLeng;
		m_dwFileLen = 0;
		m_dwCurPos= 0;
		m_dwFileLen = INVALID_FILE_SIZE;
		m_hFile = INVALID_HANDLE_VALUE;
		m_hMap = 0;
		m_pMapPoint = NULL;
		m_strFileName[0] = '\0';
	}

	virtual ~mapfilestream_imp()
	{
		CloseHandle();
	}

	BOOL Open(LPCTSTR pszFilePath)
	{
		{
			SRVATUOLOCK lock(m_lock);
			RASSERT(m_hFile == INVALID_HANDLE_VALUE , FALSE);
			m_hFile = CreateFile(pszFilePath , GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL , OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL , NULL);
			RASSERT(m_hFile != INVALID_HANDLE_VALUE , FALSE);
		}

		DWORD dwHightLen = 0;
		m_dwFileLen = GetFileSize(m_hFile , &dwHightLen);

		if (dwHightLen) //不支持大于4G
		{
			CloseHandle();
			return FALSE;
		}

		//两个里面取大的
		m_dwMaxLen = max(m_dwMaxLen,m_dwFileLen);
		SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES)};
		m_hMap = CreateFileMapping(m_hFile , &sa ,PAGE_READWRITE , 0 ,m_dwMaxLen, NULL);
		if (NULL == m_hMap)
		{
			CloseHandle();
			return FALSE;
		}

		m_pMapPoint = MapViewOfFile(m_hMap , FILE_MAP_READ|FILE_MAP_WRITE , 0,0,0);

		if (NULL == m_pMapPoint)
		{
			CloseHandle();
			return FALSE;
		}

		_tcscpy_s(m_strFileName ,MAX_PATH  , pszFilePath);
		return TRUE;
	}

	BOOL Close()
	{
		return CloseHandle();
	}

	LPCTSTR GetFileName()
	{
		return m_strFileName;
	}

	BOOL Seek(DWORD dwPos)
	{
		SRVATUOLOCK lock(m_lock);
		if (dwPos > m_dwMaxLen)
		{
			return FALSE;
		}

		m_dwCurPos = dwPos;
		return TRUE;
	}

	BOOL SeekToEnd()
	{
		SRVATUOLOCK lock(m_lock);
		m_dwCurPos = m_dwMaxLen;
		return TRUE;
	}

	BOOL SeekToBegin()
	{
		SRVATUOLOCK lock(m_lock);
		m_dwCurPos = 0;
		return TRUE;
	}

	DWORD BeginRead(DWORD dwPos =0)
	{
		DWORD dwTm =dwPos;
		m_dwReadPos = dwPos;
		return dwTm;
	}

	//返回 TURE 成功
	//返回 TURE && dwReadCount ==0 ,没有数据
	BOOL Read(DateType* lpData , DWORD dwCount , DWORD& lpdwReadCount)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(lpData && dwCount &&m_pMapPoint , FALSE);

		lpdwReadCount = 0;

		DWORD dwLen = sizeof(DateType) * dwCount;

		LONG dwCanRead = m_dwMaxLen - m_dwCurPos;

		DWORD dwReadLen = 0;
		if (dwCanRead <=0)
		{
			return TRUE;
		}
		else if (dwCanRead >= (LONG)dwLen)
		{
			dwReadLen = dwLen;
		}
		else if (dwCanRead < (LONG)dwLen)
		{
			dwReadLen = dwCanRead;
		}
		else
		{
			return FALSE;
		}

		memcpy(lpData , (DateType*)m_pMapPoint + m_dwCurPos , dwReadLen);
		
		lpdwReadCount = dwReadLen / sizeof(DateType);
		

		return TRUE;
	}

	BOOL ReadNext(DateType* lpData , DWORD dwCount , DWORD& lpdwReadCount , BOOL bBegin = FALSE)
	{
		{
			SRVATUOLOCK lock(m_lock);
			RASSERT(INVALID_HANDLE_VALUE != m_hFile , FALSE);
		}


		if (bBegin)
		{
			{
				SRVATUOLOCK lock(m_lock);
				m_dwReadPos = 0;
			}
		}

		if (!Seek(m_dwReadPos))
		{
			return FALSE;
		}
		
		if(!Read(lpData , dwCount , lpdwReadCount))
		{
			return FALSE;
		}

		m_dwReadPos += lpdwReadCount;
		return TRUE;
	}

	BOOL Write(DateType*pData , DWORD dwCount , DWORD& dwWriteCount)
	{
		SRVATUOLOCK lock(m_lock);
		RASSERT(pData && dwCount &&m_pMapPoint , FALSE);

		DWORD dwLen = sizeof(DateType) * dwCount;

		LONG dwCanWrite = m_dwMaxLen - m_dwCurPos;
		DWORD dwWrite = 0;
		//没有内存了
		if (dwCanWrite <= 0)
		{
			return FALSE;
		}
		else if ((DWORD)dwCanWrite >= dwLen)
		{
			dwWrite = dwLen;
		}
		else if ((DWORD)dwCanWrite < dwLen)
		{
			dwWrite = dwCanWrite;
		}
		else
		{
			return FALSE;
		}

		memcpy((DateType*)m_pMapPoint+m_dwCurPos ,pData  , dwWrite);
		dwWriteCount = dwWrite/sizeof(DateType);

		return TRUE;
	}

	BOOL Append(DateType*pData , DWORD dwCount , DWORD& lpdwWriteCount)
	{
		RASSERT(Write(pData , dwCount , lpdwWriteCount) , FALSE);
		RASSERT(Seek(m_dwCurPos + lpdwWriteCount) , FALSE);
		return TRUE;
	}

	BOOL Clear()
	{
		SRVATUOLOCK lock(m_lock);
		m_dwCurPos = 0;
		if (m_pMapPoint)
		{
			m_pMapPoint[0] = EOF;
		}
	}

	DWORD GetSize()
	{
		return m_dwMaxLen;
	}

protected:
	BOOL CloseHandle()
	{
		SRVATUOLOCK lock(m_lock);
		if (m_pMapPoint)
		{
			FlushViewOfFile(m_pMapPoint , 0);

		}

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_hFile);
		}

		if (m_pMapPoint)
		{
			UnmapViewOfFile(m_pMapPoint);
			m_pMapPoint = NULL;
		}

		if (NULL != m_hMap)
		{
			::CloseHandle(m_hMap);
		}

		return TRUE;
	}

protected:
	HANDLE m_hFile;
	HANDLE m_hMap;
	LPVOID m_pMapPoint;
	DWORD  m_dwFileLen;
	DWORD  m_dwCurPos;
	DWORD  m_dwReadPos;
	DWORD  m_dwMaxLen;
	TCHAR  m_strFileName[MAX_PATH];

	LockType m_lock;
};

typedef mapfilestream_imp<> mapfilestream ;

template<typename DataType>
class safe_mapfilestream_imp : public mapfilestream_imp<DataType , CAutoCriticalSection>
{
	safe_mapfilestream_imp(DWORD dwLeng):mapfilestream_imp(dwLeng)
	{

	}
};

typedef mapfilestream_imp<BYTE , CAutoCriticalSection> safe_mapfilestream;
};//namespace msdk