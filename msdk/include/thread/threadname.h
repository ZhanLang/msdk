##pragma once

//给线程起名字
//建议只用在调试版本上。
class CThreadName
{
	// 来源于：
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxtsksettingthreadname.asp
	static VOID Sys_SetThreadName(ULONG dwThreadID , LPCSTR pName )
	{
		const ULONG MS_VC_EXCEPTION = 0x406d1388;

		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType;        // must be 0x1000
			LPCSTR szName;       // pointer to name (in same addr space)
			DWORD dwThreadID;    // thread ID (-1 caller thread)
			DWORD dwFlags;       // reserved for future use, most be zero
		} THREADNAME_INFO;


		THREADNAME_INFO info = {0};
		info.dwType = 0x1000;
		info.szName = pName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;

		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(DWORD), (DWORD *)&info);
		}
		__except( EXCEPTION_CONTINUE_EXECUTION )
		{
		}
	}

	static DWORD Sys_RegisterThread( LPCSTR pName )
	{
		RASSERT(pName , 0);
		DWORD dwThreadId = GetCurrentThreadId();
		Sys_SetThreadName( dwThreadId, pName );
		return dwThreadId;
	}
};