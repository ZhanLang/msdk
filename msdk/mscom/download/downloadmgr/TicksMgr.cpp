#include "StdAfx.h"
#include "TicksMgr.h"

CTicksMgr::CTicksMgr()
{
	Now ();
}

CTicksMgr::~CTicksMgr()
{

}

void CTicksMgr::Now()
{
	m_dwTicks = GetTickCount ();
}

DWORD CTicksMgr::operator -(CTicksMgr &ticks)
{

	if (m_dwTicks >= ticks.m_dwTicks)
		return m_dwTicks - ticks.m_dwTicks;
	else 
		return (0xffffffff - ticks.m_dwTicks + m_dwTicks);
}
