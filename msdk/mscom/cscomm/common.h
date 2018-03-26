#ifndef _COMMON_H_
#define _COMMON_H_

#include "ICommon.h"
#include "namedpipe.h"
#include <list>
using namespace std;

class CCommonFun : public ICommonFun
{
public:
	CCommonFun();
	virtual ~CCommonFun();

public:
	DWORD ICFStartNamedPipeServer(LPTSTR lpPipeName,DWORD dwMaxConn,pPipeNotifyCallBack callback,LPVOID lpThis);
	DWORD ICFStopNamedPipeServer();
	DWORD ICFSendMessageToServer(LPTSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize);
	BOOL  ICFGetDiskSerialNumber(LPSTR lpDiskNum,DWORD dwDiskNumLen);

private:
	list<CNamedPipe*> m_listNp;
};



#endif