#pragma once
#include <functional>
#include <atlutil.h>

namespace msdk{;
namespace worker{;

class CWorkerBaseEx
{
public:
	typedef DWORD_PTR RequestType;
	CWorkerBaseEx()
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
		auto func = (std::function<void()> *)dw;
		(*func)();
		delete func;
	}
};

//作业管理中心
class CWorkCenterEx
{
public:
	CWorkCenterEx(DWORD dwCount = 1)
	{
		m_pool.Initialize(NULL, dwCount);
	}

	BOOL Post(const std::function<void()> &f)
	{
		auto func = new std::function<void()>(f);
		return m_pool.QueueRequest( (LPARAM)func );
	}

	VOID WaitForExit(DWORD dwWait = INFINITE)
	{
		m_pool.Shutdown(dwWait);
	}
private:
	CThreadPool<CWorkerBaseEx> m_pool;
};

};};