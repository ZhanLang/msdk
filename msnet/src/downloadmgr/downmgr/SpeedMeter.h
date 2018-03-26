#pragma once

#ifndef _SPEEDMETER_H_
#define _SPEEDMETER_H_

#include "TicksMgr.h"

class CSpeedMeter
{
public:
	CSpeedMeter()
	{
		m_dwSpeed = 0;
		m_iCurPrevSpeed = 0;
		m_iMaxPrevSpeed = 0;
		m_cMeasures = 0;
		m_done = 0;
		SetInertness (5);
	}

	void Reset()
	{
		m_past.Now ();
		m_done = 0;
		m_iCurPrevSpeed = 0;
		m_dwSpeed = 0;
		m_cMeasures = 0;
	}

	void Done (DWORD dwDone)
	{
		UpdateSpeed ();
		m_done += dwDone;
	}

	DWORD GetSpeed()
	{
		UpdateSpeed ();
		if (m_iCurPrevSpeed > 0)
			return m_dwSpeed;	
		else{
			m_now.Now ();
			if (m_now - m_past == 0)
				return 0;

			return (UINT) ((double) m_done / (m_now - m_past) * 1000.f);
		}
	}
	void SetInertness (int val)
	{
		if (val < 10)
		{
			m_iMaxPrevSpeed = val;
			m_iCurPrevSpeed = 0;
			m_dwSpeed = 0;
		}
	}
	
protected:
	int m_cMeasures;	
	DWORD m_dwSpeed;	
	DWORD m_aPrevSpeed [10]; 
	int m_iCurPrevSpeed; 
	int m_iMaxPrevSpeed;	

	void UpdateSpeed()
	{
		m_now.Now ();
		if (m_now - m_past > 1000){
			if (m_iCurPrevSpeed > m_iMaxPrevSpeed)
			{
				BOOL bAllGreater = TRUE;	
				BOOL bAllLess = TRUE;		
				for (int i = 0; i < m_iMaxPrevSpeed; i++){
					if (m_aPrevSpeed [i] < m_dwSpeed)
						bAllGreater = FALSE;
					if (m_aPrevSpeed [i] > m_dwSpeed)
						bAllLess = FALSE;
				}


				if (bAllGreater || bAllLess)
					m_cMeasures = 0;	

				m_iCurPrevSpeed = 0;
			}

			m_aPrevSpeed [m_iCurPrevSpeed++] = (UINT) ((double) m_done / (m_now - m_past) * 1000.f);
			if (m_cMeasures == 0)
				m_dwSpeed = m_aPrevSpeed [m_iCurPrevSpeed-1]; 

			m_cMeasures++;
			m_dwSpeed = DWORD (m_dwSpeed * ((double)(m_cMeasures-1) / m_cMeasures) + 
				(double) m_aPrevSpeed [m_iCurPrevSpeed-1] / m_cMeasures);

			m_past.Now ();
			m_done = 0;
		}
	}
	

	CTicksMgr m_now;	
	CTicksMgr m_past;	
	DWORD m_done;		
};

#endif//_SPEEDMETER_H_

