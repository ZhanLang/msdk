#pragma once
#include <cscomm/ICommunicateFunc.h>
#include <accctrl.h>
#include <Sddl.h>
#include <aclapi.h>


class CNamedPipe
{
public:
	CNamedPipe();
	~CNamedPipe();

public:
	DWORD NPStartModule(LPWSTR lpPipeName,DWORD dwMaxConn,BOOL bIsFirst,pPipeNotifyCallBack callback,LPVOID lpThis);
	DWORD NPStopModule();
	static DWORD NPSendMessage(LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize);
	static DWORD NPNamedPipeExists(LPWSTR lpPipeName);


private:
	//thread
	static UINT _stdcall StartThread(LPVOID lParam);
	UINT	ThreadRoutine();

	BOOL EnablePrivilege( LPCTSTR lpszPrivilege);
	BOOL SetObjectToLowIntegrity( HANDLE hObject, SE_OBJECT_TYPE type = SE_KERNEL_OBJECT);

private:
	HANDLE	m_hThreadHandle;
	UINT	m_dwThreadId;
	HANDLE	m_hEvent;
	HANDLE	m_hNamedPipe;
	pPipeNotifyCallBack m_pCallBack;
	LPVOID	m_pThis;
	BOOL	m_bExit;
};