#pragma once

#include "thread\SubThreadImp.h"
#include <SyncObject\criticalsection.h>
#include <vector>


interface IProcExit
{
	virtual HRESULT OnAddProc(HANDLE hHandle, DWORD dwData) = 0;
	virtual HRESULT OnProcExit(HANDLE hHandle) = 0;
};

class CCheckProcExit : public std::vector<HANDLE>
	, public CSubThread
{
public:
	CCheckProcExit():m_pCallback(NULL)
	{
		m_hUpdate = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	~CCheckProcExit()
	{
		if(size() > 0)
		{
			iterator _it = begin();
			while(_it != end())
			{
				HANDLE& hHandle =  *_it;
				if(hHandle)
				{
					CloseHandle(hHandle);
					hHandle = NULL;
				}
				_it++;
			}

			clear();
		}

		if(m_hUpdate)
		{
			CloseHandle(m_hUpdate);
			m_hUpdate = NULL;
		}
	}

	void SetCallback(IProcExit* pCallback)
	{
		m_pCallback = pCallback;
	}

	HANDLE AddProc(DWORD dwPid, DWORD dwData = 0)
	{
		RASSERT(m_hUpdate && dwPid, NULL);

		HANDLE hHanlde = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if(hHanlde)
		{
			AUTOLOCK_CS(checkProc);
			push_back(hHanlde);
			if(m_pCallback)
				m_pCallback->OnAddProc(hHanlde, dwData);
		}
		SetEvent(m_hUpdate);
		return hHanlde;
	}

	HRESULT	Run()
	{
		RASSERT(m_hExit && m_hUpdate, E_FAIL);

		BOOL bContinue = TRUE;
		do
		{
			DWORD dwCount = size() + 2;
			HANDLE* pHandles = new HANDLE[dwCount];
			RASSERT(pHandles, E_FAIL);

			DWORD nIndex = 0;
			pHandles[nIndex++] = m_hExit;
			pHandles[nIndex++] = m_hUpdate;
			const_iterator _it = begin();
			while(nIndex < dwCount && _it != end())
			{
				pHandles[nIndex++] = *_it;
				_it++;
			}

			DWORD dwWait = WaitForMultipleObjects(dwCount, pHandles, FALSE, INFINITE);
			if(WAIT_OBJECT_0 == dwWait) //退出
			{
				bContinue = FALSE;
			}
			else if( (WAIT_OBJECT_0+1) == dwWait)
			{
				//等待进程数量变化,可以啥都不干
			}
			else if( dwWait > (WAIT_OBJECT_0+1) && dwWait < (WAIT_OBJECT_0+dwCount))
			{
				int nIndex = dwWait-WAIT_OBJECT_0;
				//有进程退出了
				if(m_pCallback)
				{
					AUTOLOCK_CS(checkProc);
					HANDLE hHandle = pHandles[nIndex];
					m_pCallback->OnProcExit(hHandle);

					iterator _it = begin() + (nIndex - 2);
					if(_it != end())
						erase(_it);

					if(hHandle)
					{
						CloseHandle(hHandle);
						hHandle = NULL;
					}
				}
			}

			if(pHandles)
			{
				delete []pHandles;
				pHandles = NULL;
			}

		}while(bContinue);

		return S_OK;
	}

public:
	DECLARE_AUTOLOCK_CS(checkProc);
	IProcExit*	m_pCallback;
	HANDLE m_hUpdate;
};