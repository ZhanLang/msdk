/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	2,27th,2014

Module Name:

	timer.h

Abstract:


*/
/************************************************************************/

#ifndef __DSTIMER_H__
#define __DSTIMER_H__

#include <mmsystem.h>  //head file
#pragma comment(lib,"winmm.lib")  //lib file

class dsTimer{

public:
	UINT m_uTimerID;
	BOOL m_bRun;
	int m_nPeriod;

	LPARAM m_lParam;
	dsTimer():m_uTimerID(0),m_bRun(FALSE),m_nPeriod(0)
	{}
 
public://work

	UINT CreateTimer(int nDelay = 2, int nPeriod = 1, DWORD_PTR dwUserData = 0, LPTIMECALLBACK procTime = NULL)
	{
		if(!m_uTimerID)
		{
			timeBeginPeriod(nPeriod);
			m_uTimerID = timeSetEvent(
				nDelay,
				nPeriod, 
				procTime?procTime:TimerProc,
				dwUserData?dwUserData:(DWORD)this,
				TIME_PERIODIC);

			m_nPeriod = nPeriod;

			m_bRun = TRUE;
		}
		
		return m_uTimerID;
	}


	void DestroyTimer()
	{

		if ( m_bRun )
		{
			timeKillEvent(m_uTimerID);
			timeEndPeriod(m_nPeriod);

			m_bRun = FALSE;
			m_uTimerID = 0;
		}

	}


	static void CALLBACK TimerProc(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
	{
		dsTimer* pThis = (dsTimer*)dwUser;

		ATLTRACE("be in timerproc\n");

	}

public://param

	BOOL SetParam(LPARAM lParam)
	{
		if(m_bRun)
			return FALSE;

		m_lParam = lParam;

		return TRUE;
	}

	inline LPARAM GetParam(){
		return m_lParam;
	}

};



#endif /*__DSTIMER_H__*/