//
// 项目名称: 
//
// 实现功能: 1、获得CPU时钟
//			 2、可以统计模块的运行的时间
//
// 文件名称: fasttimer.h
//
// 创建作者: 
//
// 创建日期: 
//
// 修改作者: 
//
// 修改日期: 
//
// （如有新的修改人请在这里添加）
//
// 使用说明: 
//
//=============================================================================

#ifndef _FASTTIMER_INCLUDE_H_
#define _FASTTIMER_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif


#include "mslog/Syslay/platform.h"


PLATFORM_INTERFACE __int64 g_ClockFreq;
PLATFORM_INTERFACE unsigned long g_dwClockFreq;

PLATFORM_INTERFACE double g_MrPeriods;
PLATFORM_INTERFACE double g_MsPeriods;
PLATFORM_INTERFACE double g_SsPeriods;



class CCycleCount
{
friend class CFastTimer;


public:
					CCycleCount();

	void			Sample();	// Sample the clock. This takes about 34 clocks to execute (or 26,000 calls per millisecond on a P900).

	void			Init();		// Set to zero.
	void			Init( float initTimeMsec );
	bool			IsLessThan( CCycleCount const &other ) const;					// Compare two counts.

	// Convert to other time representations. These functions are slow, so it's preferable to call them
	// during display rather than inside a timing block.
	unsigned long	GetCycles()  const;

	unsigned long	GetMicroseconds() const; 	
	double			GetMicrosecondsF() const; 	

	unsigned long	GetMilliseconds() const;
	double			GetMillisecondsF() const;

	double			GetSeconds() const;

	CCycleCount&	operator+=( CCycleCount const &other );

	// dest = rSrc1 + rSrc2
	static void		Add( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest );	// Add two samples together.
	
	// dest = rSrc1 - rSrc2
	static void		Sub( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest );	// Add two samples together.


	__int64	m_Int64;
};

class CClockSpeedInit
{
public:
	CClockSpeedInit()
	{
		Init();
	}

	static void Init()
	{
		const CPUInformation& pi = Sys_CPUInfo();

		g_ClockFreq = pi.m_Speed;
		g_dwClockFreq = (unsigned long)g_ClockFreq;

		g_MrPeriods = 1000000.0 / (double)g_ClockFreq;
		g_MsPeriods = 1000.0 / (double)g_ClockFreq;
		g_SsPeriods = 1.0f / (double)g_ClockFreq;
	}

};

// 计算模块的运行的时间
class CFastTimer
{
public:
	// These functions are fast to call and should be called from your sampling code.
	void				Start();
	void				End();

	const CCycleCount &	GetDuration() const;	// Get the elapsed time between Start and End calls.
	CCycleCount 		GetDurationInProgress() const; // Call without ending. Not that cheap.

	// Return number of cycles per second on this processor.
	static inline unsigned long	GetClockSpeed();

private:

	CCycleCount	m_Duration;
};


// 计算模块的运行的时间，封装了 CFastTimer
class CTimeScope
{
public:
				CTimeScope( CFastTimer *pTimer );
				~CTimeScope();

private:	
	CFastTimer	*m_pTimer;
};

inline CTimeScope::CTimeScope( CFastTimer *pTotal )
{
	m_pTimer = pTotal;
	m_pTimer->Start();
}

inline CTimeScope::~CTimeScope()
{
	m_pTimer->End();
}

// 把需要计算的代码块的运行时间累加
class CTimeAdder
{
public:
				CTimeAdder( CCycleCount *pTotal );
				~CTimeAdder();

	void		End();

private:	
	CCycleCount	*m_pTotal;
	CFastTimer	m_Timer;
};

inline CTimeAdder::CTimeAdder( CCycleCount *pTotal )
{
	m_pTotal = pTotal;
	m_Timer.Start();
}

inline CTimeAdder::~CTimeAdder()
{
	End();
}

inline void CTimeAdder::End()
{
	if( m_pTotal )
	{
		m_Timer.End();
		*m_pTotal += m_Timer.GetDuration();
		m_pTotal = 0;
	}
}



// ----------------------------------------------------------------------------
// 对某一块代码的执行时间进行统计
// ----------------------------------------------------------------------------
#define PROFILE_SCOPE(name) \
	class C##name##ACC : public CAverageCycleCounter \
	{ \
	public: \
		~C##name##ACC() \
		{ \
			MsgA("%-48s: %6.3f avg (%8.1f total, %7.3f peak, %5d iters)\n",  \
				#name, \
				GetAverageMilliseconds(), \
				GetTotalMilliseconds(), \
				GetPeakMilliseconds(), \
				GetIters() ); \
		} \
	}; \
	static C##name##ACC name##_ACC; \
	CAverageTimeMarker name##_ATM( &name##_ACC )


//-----------------------------------------------------------------------------
// 目的: 代码的执行时间进行统计
//-----------------------------------------------------------------------------
class CAverageCycleCounter
{
public:
	CAverageCycleCounter();
	
	void Init();
	void MarkIter( const CCycleCount &duration );
	
	unsigned GetIters() const;
	
	double GetAverageMilliseconds() const;
	double GetTotalMilliseconds() const;
	double GetPeakMilliseconds() const;

private:
	unsigned	m_nIters;
	CCycleCount m_Total;
	CCycleCount	m_Peak;
	bool		m_fReport;
	const char *m_pszName;
};



//-----------------------------------------------------------------------------
// 目的: 管理时间刻度
//-----------------------------------------------------------------------------
class CAverageTimeMarker
{
public:
	CAverageTimeMarker( CAverageCycleCounter *pCounter );
	~CAverageTimeMarker();
	
private:
	CAverageCycleCounter *m_pCounter;
	CFastTimer	m_Timer;
};


inline CCycleCount::CCycleCount()
{
	m_Int64 = 0;
}

inline void CCycleCount::Init()
{
	m_Int64 = 0;
}

inline void CCycleCount::Init( float initTimeMsec )
{
	if ( g_MsPeriods > 0 )
		m_Int64 = initTimeMsec / g_MsPeriods;
	else
		m_Int64 = 0;
}

inline void CCycleCount::Sample()
{
	/*
	unsigned long* pSample = (unsigned long *)&m_Int64;
	__asm
	{
		// force the cpu to synchronize the instruction queue
		// NJS: CPUID can really impact performance in tight loops.
		//cpuid
		//cpuid
		//cpuid
		mov		ecx, pSample
		rdtsc
		mov		[ecx],     eax
		mov		[ecx+4],   edx
	}
	*/
}


inline CCycleCount& CCycleCount::operator+=( CCycleCount const &other )
{
	m_Int64 += other.m_Int64;
	return *this;
}


inline void CCycleCount::Add( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest )
{
	dest.m_Int64 = rSrc1.m_Int64 + rSrc2.m_Int64;
}

inline void CCycleCount::Sub( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest )
{
	dest.m_Int64 = rSrc1.m_Int64 - rSrc2.m_Int64;
}

inline bool CCycleCount::IsLessThan(CCycleCount const &other) const
{
	return m_Int64 < other.m_Int64;
}


inline unsigned long CCycleCount::GetCycles() const
{
	return (unsigned long)m_Int64;
}


inline unsigned long CCycleCount::GetMicroseconds() const
{
	return (unsigned long)((m_Int64 * 1000000) / g_ClockFreq);
}


inline double CCycleCount::GetMicrosecondsF() const
{
	return (double)( m_Int64 * g_MrPeriods );
}


inline unsigned long CCycleCount::GetMilliseconds() const
{
	return (unsigned long)((m_Int64 * 1000) / g_ClockFreq);
}


inline double CCycleCount::GetMillisecondsF() const
{
	return (double)( m_Int64 * g_MsPeriods );
}


inline double CCycleCount::GetSeconds() const
{
	return (double)( m_Int64 * g_SsPeriods );
}


//-----------------------------------------------------------------------------
// CFastTimer inlines.
//-----------------------------------------------------------------------------
inline void CFastTimer::Start()
{
	m_Duration.Sample();
}


inline void CFastTimer::End()
{
	CCycleCount cnt;
	cnt.Sample();
	m_Duration.m_Int64 = cnt.m_Int64 - m_Duration.m_Int64;
}

inline CCycleCount CFastTimer::GetDurationInProgress() const
{
	CCycleCount cnt;
	cnt.Sample();
	
	CCycleCount result;
	result.m_Int64 = cnt.m_Int64 - m_Duration.m_Int64;
	
	return result;
}


inline unsigned long CFastTimer::GetClockSpeed()
{
	return g_dwClockFreq;
}


inline CCycleCount const& CFastTimer::GetDuration() const
{
	return m_Duration;
}


//-----------------------------------------------------------------------------
// CAverageCycleCounter inlines
inline CAverageCycleCounter::CAverageCycleCounter()
 :	m_nIters( 0 )
{
}

inline void CAverageCycleCounter::Init()
{
	m_Total.Init();
	m_Peak.Init();
	m_nIters = 0;
}

inline void CAverageCycleCounter::MarkIter( const CCycleCount &duration )
{
	++m_nIters;
	m_Total += duration;
	if ( m_Peak.IsLessThan( duration ) )
		m_Peak = duration;
}

inline unsigned CAverageCycleCounter::GetIters() const
{
	return m_nIters;
}

inline double CAverageCycleCounter::GetAverageMilliseconds() const
{
	if ( m_nIters )
		return (m_Total.GetMillisecondsF() / (double)m_nIters);
	else
		return 0;
}

inline double CAverageCycleCounter::GetTotalMilliseconds() const
{
	return m_Total.GetMillisecondsF();
}

inline double CAverageCycleCounter::GetPeakMilliseconds() const
{
	return m_Peak.GetMillisecondsF();
}

//-----------------------------------------------------------------------------
// CAverageTimeMarker inlines
inline CAverageTimeMarker::CAverageTimeMarker( CAverageCycleCounter *pCounter )
{
	m_pCounter = pCounter;
	m_Timer.Start();
}

inline CAverageTimeMarker::~CAverageTimeMarker()
{
	m_Timer.End();
	m_pCounter->MarkIter( m_Timer.GetDuration() );
}


#endif // _FASTTIMER_INCLUDE_H_
