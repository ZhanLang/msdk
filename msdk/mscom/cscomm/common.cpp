#include "stdafx.h"
#include "common.h"
#include "GetDiskInfo.h"
#include <Strsafe.h>


CCommonFun::CCommonFun()
{

}

CCommonFun::~CCommonFun()
{

}

DWORD CCommonFun::ICFStartNamedPipeServer( LPTSTR lpPipeName,DWORD dwMaxConn,pPipeNotifyCallBack callback,LPVOID lpThis )
{
	DWORD dwRet = 0;
	for(DWORD dwIndex = 0; dwIndex < dwMaxConn; dwIndex++)
	{
		CNamedPipe *pNp = new CNamedPipe();
		if(pNp)
		{
			dwRet = pNp->NPStartModule(lpPipeName,dwMaxConn,callback,lpThis);
			if(dwRet == 0)
			{
				m_listNp.push_back(pNp);
			}
		}
	}
	return (DWORD)m_listNp.size();
}

DWORD CCommonFun::ICFStopNamedPipeServer()
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

DWORD CCommonFun::ICFSendMessageToServer( LPTSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize )
{
	DWORD dwRet = 0;
	dwRet = CNamedPipe::NPSendMessage(lpPipeName,lpInBuff,dwInBuffSize,lpOutBuff,dwOutBuffSize);
	return dwRet;
}

BOOL CCommonFun::ICFGetDiskSerialNumber( LPSTR lpDiskNum,DWORD dwDiskNumLen )
{
	BOOL bRet = FALSE;
	CHAR szDiskNumber[MAX_PATH] = {0};
	bRet = msdk::CGetDiskInfo::GetDiskInfo(szDiskNumber);
	if(bRet)
	{
		StringCchCopyA(lpDiskNum,dwDiskNumLen,szDiskNumber);
	}
	return bRet;
}

