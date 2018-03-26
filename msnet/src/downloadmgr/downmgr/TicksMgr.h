#pragma once

#ifndef _TICKSMGR_H_
#define _TICKSMGR_H_

class CTicksMgr
{
public:
	CTicksMgr(){Now ();}

	DWORD operator - (CTicksMgr& ticks)
	{
		if (m_dwTicks >= ticks.m_dwTicks)
			return m_dwTicks - ticks.m_dwTicks;
		else 
			return (0xffffffff - ticks.m_dwTicks + m_dwTicks);
	}


	void Now(){m_dwTicks = GetTickCount ();}

	void operator += (DWORD dwTicks) {
		DWORD dwLeft = _UI32_MAX - m_dwTicks;
		if (dwLeft >= dwTicks)
			m_dwTicks += dwTicks;
		else
			m_dwTicks = dwTicks - dwLeft;
	}

	bool operator < (const CTicksMgr &obj) const {return m_dwTicks < obj.m_dwTicks;}
	bool operator <= (const CTicksMgr &obj) const {return m_dwTicks <= obj.m_dwTicks;}

private:
	DWORD m_dwTicks;	
};



#endif //_TICKSMGR_H_
