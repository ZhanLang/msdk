/************************************************************************/
/* 
Author:

	lourking. All rights reserved.

Create Time:

	4,8th,2014

Module Name:

	dsEvent.h 

Abstract: ÊÂ¼þ


*/
/************************************************************************/

#ifndef __DSEVENT_H__
#define __DSEVENT_H__

class dsEvent
{
private:
	HANDLE m_hEvent;

public:
	dsEvent():m_hEvent(NULL)
	{}

	inline HANDLE Create(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes = NULL, LPCTSTR lpName = NULL)
	{
		return m_hEvent = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
	}

	~dsEvent(){
		Delete();
	}

	inline BOOL Set(){
		return ::SetEvent(m_hEvent);
	}

	inline BOOL Reset(){
		return ::ResetEvent(m_hEvent);
	}


	inline DWORD Wait( DWORD dwMilliseconds ){
		return ::WaitForSingleObject(m_hEvent, dwMilliseconds);
	}

	
	operator HANDLE() const{
		return m_hEvent;
	}


	inline BOOL Delete(){

		if(NULL == m_hEvent)
			return FALSE;

		BOOL bRet = ::CloseHandle(m_hEvent);
		if(bRet)
			m_hEvent = NULL;

		return bRet;
	}

};

#endif /*__DSEVENT_H__*/