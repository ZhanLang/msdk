#pragma once

class CTicksMgr  
{
public:

	DWORD operator - (CTicksMgr& ticks);

	void Now();

	CTicksMgr();
	virtual ~CTicksMgr();

	DWORD m_dwTicks;	

	void operator += (DWORD dwTicks) {
		DWORD dwLeft = _UI32_MAX - m_dwTicks;
		if (dwLeft >= dwTicks)
			m_dwTicks += dwTicks;
		else
			m_dwTicks = dwTicks - dwLeft;
	}

	bool operator < (const CTicksMgr &obj) const {return m_dwTicks < obj.m_dwTicks;}
	bool operator <= (const CTicksMgr &obj) const {return m_dwTicks <= obj.m_dwTicks;}
};
