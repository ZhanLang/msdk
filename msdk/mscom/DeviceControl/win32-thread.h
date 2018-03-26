#ifndef __USER_THREAD__
#define __USER_THREAD__

#include <Windows.h>
#include <intrin.h>
#include <process.h>

class Win32Thread
{
protected:
	HANDLE			_handle;
	volatile long	_exitSignal;
public:
	void SetStopEvent()
	{
		_InterlockedIncrement( &_exitSignal );
	}
	bool TestStopEvent()
	{
		return _InterlockedOr( &_exitSignal, 0 ) > 0;
	}
	void ResetStopEvent()
	{
		_InterlockedAnd( &_exitSignal, 0 );
	}
public:
	Win32Thread() : _handle(0),_exitSignal(0) {SetStopEvent();}
	bool Run( bool bSuspend = false )
	{
		DWORD dwFlags = bSuspend ? CREATE_SUSPENDED : 0;
		_handle = (HANDLE)_beginthreadex( 0, 0, _thread_body, this, dwFlags, 0 ); 
		if( !_handle ) return false;
		return true;
	}
	bool IsActived()
	{
		if( !_handle ) return false;
		DWORD wr = WaitForSingleObject( _handle, 0 );
		if( wr == WAIT_TIMEOUT ) return true;
		return false;
	}
	bool Resume()
	{
		return ResumeThread( _handle ) == 0 ; 
	}
	bool Suspend()
	{
		return SuspendThread( _handle ) > 0;
	}
	UINT32 WaitForExit( UINT32 timeout,bool bManual = false /*ÊÖ¶¯Í£Ö¹*/, bool bForceKill = false)
	{
		if( !_handle ) return WAIT_OBJECT_0;
		if (bManual)
		{
			SetStopEvent();
		}
		
		UINT32 wr = WaitForSingleObject( _handle, timeout );
		if( wr == WAIT_TIMEOUT )
		{
			if( bForceKill )
			{
				Close();
				return wr;
			}
		}
		return wr;
	}
	void Close()
	{
		if( _handle )
		{
			TerminateThread( _handle, -12345678 );
			CloseHandle( _handle );
		}
		_handle = 0;
	}
private:
	static UINT WINAPI _thread_body( LPVOID p )
	{
		((Win32Thread *)p)->_threadBody();
		return 0;
	}
	void _threadBody() { ResetStopEvent(); threadBody(); SetStopEvent(); onThreadDead(); }
protected:
	virtual void threadBody(){}
	virtual void onThreadDead(){};
};


class Win32LoopThread : public Win32Thread
{
private:
	virtual void threadBody()
	{
		for(;;)
		{
			if( Win32Thread::TestStopEvent() )
				break;
			if( !loopBody() )
				break;
		}
		Win32Thread::onThreadDead();
	}
public:
	virtual bool loopBody(){ return false; }

};


#endif