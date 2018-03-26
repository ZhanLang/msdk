#pragma once

#include <map>
#include <atlutil.h>

namespace msdk{;
namespace worker{;



struct IWorkerDispatcher
{
	virtual VOID OnWorkStatus(LPARAM lParam) = 0;
};


class CWorkerBase
{
public:
	typedef DWORD_PTR RequestType;
	CWorkerBase()
	{

	}
	
	virtual BOOL Initialize(void *pvParam)
	{
		return TRUE;
	}

	virtual void Terminate(void* /*pvParam*/)
	{

	}

	void Execute(RequestType dw, void *pvParam, OVERLAPPED* pOverlapped) throw()
	{
		IWorkerDispatcher* pDispatcher = static_cast<IWorkerDispatcher*>(pvParam);
		pDispatcher->OnWorkStatus((LPARAM)dw);
	}
};

//作业管理中心
class CWorkCenter
{
public:
	CWorkCenter(IWorkerDispatcher* pDisp, DWORD dwCount = 1)
	{
		m_pool.Initialize(pDisp, dwCount);
	}

	BOOL Dispatch(LPARAM lParam)
	{
		return m_pool.QueueRequest( lParam );
	}

	VOID WaitForExit(DWORD dwWait = INFINITE)
	{
		m_pool.Shutdown(dwWait);
	}
private:
	CThreadPool<CWorkerBase> m_pool;
};




};};