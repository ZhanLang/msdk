#ifndef _COMMUNICATEFUNC_H_
#define _COMMUNICATEFUNC_H_

#include <cscomm/ICommunicateFunc.h>
#include "namedpipe.h"
#include <list>
using namespace std;

class CCommunicateFunc : public ICommunicateFunc
{
public:
	CCommunicateFunc();
	virtual ~CCommunicateFunc();

public:
	DWORD ICFStartNamedPipeServer(LPWSTR lpPipeName,DWORD dwMaxConn,pPipeNotifyCallBack callback,LPVOID lpThis);
	DWORD ICFStopNamedPipeServer();
	DWORD ICFSendMessageToServer(LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize);
	DWORD ICFNamedPipeExists(LPWSTR lpPipeName);

private:
	list<CNamedPipe*> m_listNp;
};

#endif