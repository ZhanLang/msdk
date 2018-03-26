#include "stdafx.h"
#include "CommunicateFunc.h"
#include <Strsafe.h>


ICommunicateFunc * GetStaticCommonObject()
{
	return new CCommunicateFunc();
}

DWORD NamedPipeSendMessageToServer(LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize)
{
	return CNamedPipe::NPSendMessage(lpPipeName,lpInBuff,dwInBuffSize,lpOutBuff,dwOutBuffSize);
}

CCommunicateFunc::CCommunicateFunc()
{

}

CCommunicateFunc::~CCommunicateFunc()
{

}

DWORD CCommunicateFunc::ICFStartNamedPipeServer( LPWSTR lpPipeName,DWORD dwMaxConn,pPipeNotifyCallBack callback,LPVOID lpThis )
{
	DWORD dwRet = 0;
	for(DWORD dwIndex = 0; dwIndex < dwMaxConn; dwIndex++)
	{
		CNamedPipe *pNp = new CNamedPipe();
		if(pNp)
		{
			dwRet = pNp->NPStartModule(lpPipeName,dwMaxConn,!dwIndex,callback,lpThis);
			if(dwRet == 0)
			{
				m_listNp.push_back(pNp);
			}
		}
	}
	return (DWORD)m_listNp.size();
}

DWORD CCommunicateFunc::ICFStopNamedPipeServer()
{
	DWORD dwRet = 0;

	list<CNamedPipe*>::iterator it = m_listNp.begin();
	
	while(it != m_listNp.end())
	{
		(*it)->NPStopModule();
		delete (*it);
		it++;
	}

	m_listNp.clear();
	
	return dwRet;
}

DWORD CCommunicateFunc::ICFSendMessageToServer( LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize )
{
	DWORD dwRet = 0;
	dwRet = CNamedPipe::NPSendMessage(lpPipeName,lpInBuff,dwInBuffSize,lpOutBuff,dwOutBuffSize);
	return dwRet;
}


DWORD CCommunicateFunc::ICFNamedPipeExists( LPWSTR lpPipeName )
{
	return CNamedPipe::NPNamedPipeExists(lpPipeName);
}

