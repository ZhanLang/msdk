#ifndef _ASYNFUNC_INCLUDE_H_
#define _ASYNFUNC_INCLUDE_H_

#ifdef _WIN32
#pragma once
#endif

#include "criticalsection.h"

/*
 *****************设计思想说明*********************
 需求：很多资源的应用在反初始化后就不能使用了
		实际有可能在其他线程（子线程、回调）里用，不好控制
 要求：
	1、其他资源使用能并行（不能粗暴的一把锁全锁死整个函数）
	2、一旦开始反初始化，其他线程的资源使用应该就不用了，或者尽快释放资源的使用

 实现：
	1、资源的使用申请和反初始化标识短暂的锁定抢占
	2、资源的使用使用引用计数统计
	3、反初始化的时候标识以后不能再申请资源使用了
		其次再如果发现有正在使用的资源，需要等待资源释放
	4、所以资源使用后要及时及时释放引用计数

 具体函数:
	1、使用资源前使用CanRun()是否可以运行，可以的话抢占到了资源
	2、使用完调用ExitRun释放资源
	3、反初始化调用CancelRun等待标志不运行再使用资源且等待正在使用的资源释放

 资源使用方便的快捷应用，自动帮助释放资源：
	用CAutoAsynFunc帮助类吧
*/



class CAsynFunc
{
public:
//////////////////////////////////////////////////////////////////////////
//抢占使用资源的函数使用

	//要使用资源的函数入口处调用，判断是否可以运行
	BOOL CanRun()
	{
		CStackAutoCSLock lock(AsynFunc);
		RASSERT(m_canRun, FALSE);

		//可以运行，那就增加运行的引用计数
		m_runCount++;
		if(1 == m_runCount && m_hRunFinish)	//如果是第一个，那就得重置运行完成的信号
			ResetEvent(m_hRunFinish);

		return TRUE;
	}

	BOOL ExitRun()
	{
		CStackAutoCSLock lock(AsynFunc);
		RASSERT(m_runCount > 0, TRUE);

		m_runCount--;
		if(0 == m_runCount && m_hRunFinish)//如果是最后一个，那就得置运行完成的信号
			SetEvent(m_hRunFinish);

		return TRUE;
	}

//////////////////////////////////////////////////////////////////////////
//释放资源的位置使用

	//即将释放资源的函数调用，并且可以等到所有使用资源的函数都结束
	BOOL CancelRun(BOOL bNeedWaitFinish = TRUE)
	{
		BOOL bWaitFinish = FALSE;

		{
			 CStackAutoCSLock lock(AsynFunc);
			//先置标志，确保再调用CanRun立马返回FALSE
			m_canRun = FALSE;

			//再判断现在已经有进入函数多少次了，只有要有那就等到他们结束
			if(m_runCount > 0)
				bWaitFinish = TRUE;	//那就需要等待他们退出罗
		}

		if(!bNeedWaitFinish)
			m_bWaitFinish = bWaitFinish;

		if(bNeedWaitFinish && bWaitFinish)
			WaitFinish();

		return TRUE;
	}

	BOOL WaitFinish()
	{
		if(m_bWaitFinish && m_hRunFinish)
			WaitForSingleObject(m_hRunFinish, INFINITE);

		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////
	CAsynFunc():m_canRun(TRUE),m_runCount(0),m_bWaitFinish(FALSE),m_hRunFinish(NULL)
	{
		m_hRunFinish = CreateEvent(NULL, TRUE, TRUE, NULL);
	}

	~CAsynFunc()
	{
		if(m_hRunFinish)
		{
			CloseHandle(m_hRunFinish);
			m_hRunFinish = NULL;
		}
	}
private:
	CAutoCriticalSection AsynFunc;
	BOOL	m_canRun;
	DWORD	m_runCount;
	HANDLE  m_hRunFinish;
	BOOL	m_bWaitFinish;
};


/*自动调用CanRun和ExitRun的帮助类
用法：
1、本身具备CAsynFunc对象，如派生、成员变量(m_asynFunc)
2、在要同步的函数一进入就写
	HRESULT func()
	{
		CAutoAsynFunc autoAsyn(m_asynFunc);
		if(!autoAsyn.CanRun())
			return S_FALSE;

		...
		return S_FALSE;

		...
		return S_OK;
	}

*/
class CAutoAsynFunc
{
public:
	CAutoAsynFunc(CAsynFunc* pAsynFunc):m_canRun(TRUE), m_pAsynFunc(pAsynFunc)
	{
		if(m_pAsynFunc)
			m_canRun = m_pAsynFunc->CanRun();
	}

	~CAutoAsynFunc()
	{
		if(m_pAsynFunc)
			m_pAsynFunc->ExitRun();
	}

	BOOL CanRun(){return m_canRun;}

	CAsynFunc* m_pAsynFunc;
	BOOL m_canRun;
};

#ifndef DeclareDefaultAsynFunc
#	define DeclareDefaultAsynFunc CAsynFunc m_asynFunc;
#endif

#ifndef CancleRunAsynFunc
#	define CancleRunAsynFunc  m_asynFunc.CancelRun();
#endif 

#ifndef CanRunAsynFunc
#	define CanRunAsynFunc		   CAutoAsynFunc autoAsyn(&m_asynFunc);if(!autoAsyn.CanRun())return S_FALSE;
#endif

#endif // _ASYNFUNC_INCLUDE_H_