#pragma once
#include "TicksMgr.h"

class CSpeedMeter  
{
public:

	void Reset();


	void Done (DWORD dwDone);

	DWORD GetSpeed();

	void SetInertness (int val);

	CSpeedMeter();
	virtual ~CSpeedMeter();

protected:
	int m_cMeasures;	
	DWORD m_dwSpeed;	
	DWORD m_aPrevSpeed [10]; 
	int m_iCurPrevSpeed; 
	int m_iMaxPrevSpeed;	

	void UpdateSpeed();	

	CTicksMgr m_now;	
	CTicksMgr m_past;	
	DWORD m_done;		
};