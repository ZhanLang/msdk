#pragma once

#ifndef DEFULT_IO_OPERATION_BYTES
#	define DEFULT_IO_OPERATION_BYTES	1024
#endif


#include <util/buffer.h>
namespace msdk{;
namespace process{;

class CProcessInvoke;
struct IProcessInvokeCallBack
{
	virtual VOID OnConsoleRead(CProcessInvoke* processInvoke, BYTE* lpszData, DWORD dwLen) = 0;
};

//执行进程,并通过管道获取控制台中输出的信息
class CProcessInvoke
{
public:
	CProcessInvoke(IProcessInvokeCallBack* pCallBack):
	  m_pProcessInvokeCallBack(pCallBack)
	{
		m_hProcess	= NULL;

		m_hRead		= NULL; 
		m_hWrite	= NULL; 
		m_hEerrRead = NULL;  
		m_hErrWrite = NULL; 
		m_dwExitCode = NULL;

		m_hNotifyEvent = NULL;


		m_dwPid = 0;
	}

	CProcessInvoke(): 
		m_pProcessInvokeCallBack(NULL)
	  {
		  m_hProcess	= NULL;

		  m_hRead		= NULL; 
		  m_hWrite	= NULL; 
		  m_hEerrRead = NULL;  
		  m_hErrWrite = NULL; 
		  m_dwExitCode = NULL;

		  m_hNotifyEvent = NULL;


		  m_dwPid = 0;
	  }
	~CProcessInvoke()
	{
		//首先得等待线程成功退出，要不然获取不到剩余的数据
		if ( m_hReadThread )
		{
			WaitForSingleObject(m_hReadThread, INFINITE);
			CloseHandle(m_hReadThread);
			m_hReadThread = NULL;
		}


		if ( m_hRead )
		{
			CloseHandle(m_hRead);
			m_hRead = NULL;
		}

		if ( m_hWrite )
		{
			CloseHandle(m_hWrite);
			m_hWrite = NULL;
		}
		
		if ( m_hEerrRead )
		{
			CloseHandle(m_hEerrRead);
			m_hEerrRead = NULL;
		}

		if ( m_hErrWrite )
		{
			CloseHandle(m_hErrWrite);
			m_hErrWrite = NULL;
		}
		if ( m_hProcess )
		{
			CloseHandle(m_hProcess);
			m_hProcess = NULL;
		}
	}



	DWORD GetExitCode()
	{
		return m_dwExitCode;
	}

	HANDLE GetHandle()
	{
		return m_hProcess;
	}

	DWORD GetPid()
	{
		return m_dwPid;
	}

	DWORD WaitForExit(DWORD dwMilliseconds = INFINITE)
	{
		if ( m_hProcess )
		{
			DWORD dwWait = WaitForSingleObject(m_hProcess, dwMilliseconds);
			if ( dwWait == WAIT_TIMEOUT)
			{
				TerminateProcess(m_hProcess, 0);
			}

			GetExitCodeProcess(m_hProcess, &m_dwExitCode);
		}

		return m_dwExitCode;
	}

	DWORD Create(LPCTSTR lpszFile, LPCTSTR lpszCmd, BOOL bShow)
	{
		STARTUPINFO			si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sa;

		DWORD dwThreadID = 0;

		TCHAR szCmd[1024] = { 0 };
		_tcscpy_s(szCmd, _countof(szCmd), lpszCmd);
		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		sa.bInheritHandle		= TRUE;
		sa.lpSecurityDescriptor = NULL;
		sa.nLength				= sizeof(sa);

		
		do 
		{

			if (
				CreatePipe(&m_hRead, &m_hWrite, &sa, DEFULT_IO_OPERATION_BYTES * 200) && 
				CreatePipe(&m_hEerrRead, &m_hErrWrite, &sa, DEFULT_IO_OPERATION_BYTES * 200) && 
				SetHandleInformation(m_hEerrRead, HANDLE_FLAG_INHERIT, 0)
				
				)
			{
				si.dwFlags	  = STARTF_USESTDHANDLES;
				si.hStdError  = m_hErrWrite;
				si.hStdOutput = m_hWrite;
				si.hStdInput  = m_hRead;
			}
			
			DWORD createflag = ::GetPriorityClass(::GetCurrentProcess());

			if ( !bShow ) 
				createflag |= CREATE_NO_WINDOW;

			
			
			BOOL err = ::CreateProcess(lpszFile, szCmd,NULL, NULL, TRUE, createflag | CREATE_SUSPENDED, NULL, NULL,&si, &pi);
			if ( err )
			{
				m_hProcess = pi.hProcess;
			}
			
			m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_hReadThread = CreateThread(NULL, 0, ReadConcleTread, this, 0, &dwThreadID);
			WaitForSingleObject(m_hNotifyEvent, INFINITE);
			
			ResumeThread(pi.hThread);
			if ( m_hWrite )
			{
				CloseHandle(m_hWrite);
				m_hWrite = NULL;
			}

			if ( m_hErrWrite )
			{
				CloseHandle(m_hErrWrite);
				m_hErrWrite = NULL;
			}

			if (!err) {
				break;
			}
			

		}while(0);
		return 0;
	}

	unsigned char* GetData()
	{
		return m_data.GetData();
	}

	DWORD GetDataLenght()
	{
		return m_data.GetDataSize();
	}
	

private:
	static DWORD WINAPI ReadConcleTread(LPVOID lpParameter )
	{
		CProcessInvoke* pThis = static_cast<CProcessInvoke*>(lpParameter);
		pThis->ReadConsole();
		return 0;
	}

	VOID ReadConsole()
	{
		HANDLE handle[] = { m_hProcess, m_hRead,m_hEerrRead};

		SetEvent(m_hNotifyEvent);
		BYTE _errinfo[DEFULT_IO_OPERATION_BYTES * 2 + 1] = {0};
		DWORD outlen = 0;

		DWORD  result = -1;
		while(1)
		{
			DWORD dwWait = WaitForMultipleObjects(_countof(handle), handle, FALSE, INFINITE);
			
			if ( dwWait == WAIT_OBJECT_0 + 0 || dwWait == WAIT_FAILED)
			{
				ZeroMemory(_errinfo, sizeof(_errinfo));
				outlen = 0;
				
				do 
				{
					BOOL bRet = ReadFile(m_hRead, _errinfo, sizeof(_errinfo), &outlen, NULL);
					if (outlen)
					{
						if ( m_pProcessInvokeCallBack )
							m_pProcessInvokeCallBack->OnConsoleRead(this, _errinfo, outlen);
						else
							m_data.AddTail(_errinfo,outlen);
					}else
					{
						unsigned char c = '\0';
						m_data.AddTail(&c,sizeof(c));
						break;
					}
				} while (TRUE);
				
				break;
			}
			else if ( dwWait == WAIT_OBJECT_0 + 1)
			{
				ZeroMemory(_errinfo, sizeof(_errinfo));
				outlen = 0;
				do 
				{
					BOOL bRet = ReadFile(m_hRead, _errinfo, sizeof(_errinfo), &outlen, NULL);
					if (outlen)
					{
						if ( m_pProcessInvokeCallBack )
							m_pProcessInvokeCallBack->OnConsoleRead(this, _errinfo, outlen);
						else
							m_data.AddTail(_errinfo,outlen);
					}else
						break;
				} while (TRUE);
			}
			else if ( dwWait == WAIT_OBJECT_0 + 2)
			{
				ZeroMemory(_errinfo, sizeof(_errinfo));
				outlen = 0;
				do 
				{
					BOOL bRet = ReadFile(m_hRead, _errinfo, sizeof(_errinfo), &outlen, NULL);
					if (outlen)
					{
						if ( m_pProcessInvokeCallBack )
							m_pProcessInvokeCallBack->OnConsoleRead(this, _errinfo, outlen);
						else
							m_data.AddTail(_errinfo,outlen);
					}else
						break;
				} while (TRUE);
			}
		}
	}
private:
	HANDLE m_hProcess;
	HANDLE m_hRead; 
	HANDLE m_hWrite;
	HANDLE m_hEerrRead; 
	HANDLE m_hErrWrite;
	HANDLE m_hReadThread;
	HANDLE m_hNotifyEvent;
	DWORD m_dwExitCode;
	DWORD m_dwPid;
	IProcessInvokeCallBack* m_pProcessInvokeCallBack;

	CBuffer m_data;
};


};};