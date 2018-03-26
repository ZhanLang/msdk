/************************************************************************/
/* 
Author:

lourking.All rights reserved.

Create Time:

	4,3th,2014

Module Name:

	dsLocalBuf.h 

Abstract: 局部缓存，跳出局部就销毁


*/
/************************************************************************/


#ifndef __DSLOCALBUF_H__
#define __DSLOCALBUF_H__


template<typename T>
class dsLocalBuf
{
public:
	T *m_pData;
	DWORD m_dwLen;

public:

	dsLocalBuf():m_pData(NULL),m_dwLen(0){

	}

	dsLocalBuf(T *pData, DWORD dwLen){
		m_pData = pData;
		m_dwLen = dwLen;
	}

	~dsLocalBuf(){
		Release();
	}

	inline T *Allocate(DWORD dwCount){
		m_dwLen = dwCount;
		return m_pData = new T[dwCount];
	}

	inline void Release(){
		if(NULL != m_pData){
			delete []m_pData;

			m_pData = NULL;
		}
	}

	operator T*(){
		return m_pData;
	}

	T *GetData(){
		return m_pData;
	}

	inline DWORD GetLenght(){
		return m_dwLen;
	}

	inline void SetEndOfZero(){
		if(NULL != m_pData)
			m_pData[m_dwLen - 1] = 0;
	}

	inline void SetZeroMemory(){
		if(NULL != m_pData)
			memset(m_pData, 0, m_dwLen * sizeof T);
	}
};


#endif /*__DSLOCALBUF_H__*/