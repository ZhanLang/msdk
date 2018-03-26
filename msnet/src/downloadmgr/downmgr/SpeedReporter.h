#pragma once
#include <vector>


//主要用于速度通知
class CSpeedReporter
{
public:
	static CSpeedReporter& GetSpeedReporter();
	static VOID CALLBACK SpeedTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

public:
	HRESULT Append(ISpeed* pSpeed);
	HRESULT Remove(ISpeed* pSpeed);
	VOID SetTimeTick(DWORD dwTimeTick);
	DWORD GetTimeTick();

protected:
	virtual VOID OnTimmer();

private:
	CSpeedReporter(void);
	~CSpeedReporter(void);

private:
	typedef std::vector<UTIL::com_ptr<ISpeed>> CSpeedCollection;
	DWORD m_dwTimeTick;
	CSpeedCollection m_SpeedCollection;
	DECLARE_AUTOLOCK_CS(SpeedCollection);
	UINT_PTR  m_speedTimeHandle;
};

CSpeedReporter& CSpeedReporter::GetSpeedReporter()
{
	static CSpeedReporter g_speedReporter;
	return g_speedReporter;
}

CSpeedReporter::CSpeedReporter()
{
	m_speedTimeHandle = NULL;
	SetTimeTick(1000); //默认设置为一秒
}

CSpeedReporter::~CSpeedReporter()
{
	AUTOLOCK_CS(SpeedCollection);
	KillTimer(NULL,m_speedTimeHandle);
	m_speedTimeHandle = NULL;

	m_SpeedCollection.clear();
}

HRESULT CSpeedReporter::Append(ISpeed* pSpeed)
{
	RASSERT(pSpeed, E_INVALIDARG);

	AUTOLOCK_CS(SpeedCollection);
	BOOL bFind = FALSE;
	CSpeedCollection::iterator it = m_SpeedCollection.begin();
	for (; it != m_SpeedCollection.end() ; it++)
	{
		if (*it == pSpeed)
		{
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		m_SpeedCollection.insert(m_SpeedCollection.end(), pSpeed);
	}
	
	if (m_SpeedCollection.size() && !m_speedTimeHandle)
	{
		m_speedTimeHandle = SetTimer(NULL,NULL,1000,SpeedTimer);
	}

	return S_OK;
}

HRESULT CSpeedReporter::Remove(ISpeed* pSpeed)
{
	RASSERT(pSpeed, E_INVALIDARG);
	AUTOLOCK_CS(SpeedCollection);
	
	CSpeedCollection::iterator it = m_SpeedCollection.begin();
	for (; it != m_SpeedCollection.end() ;)
	{
		if (*it == pSpeed)
		{
			m_SpeedCollection.erase(it);
		}
		else
		{
			it++;
		}
	}

	if (m_SpeedCollection.size() == 0)
	{
		KillTimer(NULL,m_speedTimeHandle);
		m_speedTimeHandle = NULL;
	}
	return S_OK;
}

VOID CSpeedReporter::SetTimeTick(DWORD dwTimeTick)
{
	m_dwTimeTick = dwTimeTick;
}

DWORD CSpeedReporter::GetTimeTick()
{
	return m_dwTimeTick;
}

VOID CSpeedReporter::OnTimmer()
{
	AUTOLOCK_CS(SpeedCollection);
	CSpeedCollection::iterator it = m_SpeedCollection.begin();

	for (; it != m_SpeedCollection.end(); it++)
	{
		(*it)->GetSpeed();
	}
}

VOID CALLBACK CSpeedReporter::SpeedTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	CSpeedReporter::GetSpeedReporter().OnTimmer();
}