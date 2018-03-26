#include "stdafx.h"

#ifdef _MS_P2P_LOG
	CCriticalSetionObject CLogLock::m_loglock;

	LogFile_t g_lfm;
#endif


#include "SimpleThread.h"


CSimpleThreadPool CSimpleThread::m_pool;
