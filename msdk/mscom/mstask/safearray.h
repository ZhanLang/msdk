///////////////////////////////////////////////////////////////
//	文 件 名 : SafeArray.h
//	文件功能 : 一个简单的list封装类，是线程安全的
//	作    者 : guoyibin
//	创建时间 : 2005年7月13日
//	项目名称 ：ravtask
//	操作系统 : 
//	备    注 :
//	历史记录： : 
///////////////////////////////////////////////////////////////


#if !defined(AFX_SAFEARRAY_H__0F54E1F8_D8A3_4218_9B67_832FBAE96ADB__INCLUDED_)
#define AFX_SAFEARRAY_H__0F54E1F8_D8A3_4218_9B67_832FBAE96ADB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4078)

#include <vector>

template <class T > class CSafeArray  
{
public:
	typedef std::vector<T> SAFEARRAYVECTOR;

	CSafeArray()
	{
		InitializeCriticalSection(&this->m_csLock);
	}
	
	~CSafeArray()
	{
		DeleteCriticalSection(&this->m_csLock);
	}
	void GetAll(SAFEARRAYVECTOR& myVector)
	{
		CritLock lock(&this->m_csLock);
		SAFEARRAYVECTOR::iterator it = m_Vector.begin();
		for (;it != m_Vector.end();it++)
		{
			myVector.push_back(*it);
		}
	}

	inline size_t GetSize()
	{
		CritLock lock(&this->m_csLock);
		return m_Vector.size();
	}

	void PushBack(const T& myT)
	{
		CritLock lock(&this->m_csLock);
		
		m_Vector.push_back(myT);
	}


	void DeleteAllAndInsert(const SAFEARRAYVECTOR& myVector)
	{
		CritLock lock(&this->m_csLock);
		m_Vector.clear();
		SAFEARRAYVECTOR::const_iterator it = myVector.begin();
		for (;it != myVector.end();it++)
		{
			m_Vector.push_back(*it);
		}
	}
	void Insert(const SAFEARRAYVECTOR& myVector)
	{
		CritLock lock(&this->m_csLock);
		SAFEARRAYVECTOR::const_iterator it = myVector.begin();
		for (;it != myVector.end();it++)
		{
			m_Vector.push_back(*it);
		}
	}

	BOOL GetData(DWORD nIndex, T& myT)
	{
		CritLock lock(&this->m_csLock);
		if(nIndex>=0 && nIndex < m_Vector.size())
		{
			myT = m_Vector[nIndex];
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetData(DWORD nIndex,const T& myT)
	{
		CritLock lock(&this->m_csLock);
		if(nIndex>=0 && nIndex < m_Vector.size())
		{
			m_Vector[nIndex] = myT;
			return TRUE;
		}
		return FALSE;
	}
	BOOL RemoveData(DWORD nIndex)
	{
		CritLock lock(&this->m_csLock);
		if(nIndex>=0 && nIndex < m_Vector.size())
		{
			m_Vector.erase(m_Vector.begin()+nIndex);
			return TRUE;
		}
		return FALSE;
	}
private:
	class CritLock
	{
	public:
		CritLock(LPCRITICAL_SECTION lpCriticalSection)
			:m_prit(lpCriticalSection)
		{
			EnterCriticalSection(lpCriticalSection);
		}
		
		~CritLock()
		{
			LeaveCriticalSection(m_prit);
		}
		
	private:
		LPCRITICAL_SECTION m_prit;
		CritLock& operator=(const CritLock& rhs);
		CritLock();
	};

	CRITICAL_SECTION m_csLock;
	SAFEARRAYVECTOR m_Vector;
		
};

#endif // !defined(AFX_SAFEARRAY_H__0F54E1F8_D8A3_4218_9B67_832FBAE96ADB__INCLUDED_)
