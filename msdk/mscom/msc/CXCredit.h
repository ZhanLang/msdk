#ifndef _CX_CREDIT_H_
#define _CX_CREDIT_H_

#include "XCredit.h"
#include "RSA.h"
/////////////////////////////////////////////////////////////////////////////
class CXCredit :public XCredit
{
private:
	CXCredit();
	~CXCredit();

public:
	BOOL   VerifySignature(R_RSA_PUBLIC_KEY* publicKey = NULL);
	VOID   MakeSignature(R_RSA_PRIVATE_KEY* privateKey);
	DWORD  ProcessID();
	DWORD  ThreadID();
	DWORD  ConnectTime();
	DWORD  SubSystemID();
    BOOL   DotNet();
	DWORD  DataLen();
	LPBYTE Data();
	BOOL   IsExpire(DWORD dwAcceptTime);
	VOID   Initlize(DWORD dwProcessID, DWORD dwThreadID, DWORD dwSubSystemID, DWORD dwBufferLen = sizeof(XCredit));
	

private:
	VOID   SHA1(BYTE lpBuffer[20]);
	BOOL   IsMatch(BYTE lpSrcBuffer[20], BYTE lpDstBuffer[20]);

};

/////////////////////////////////////////////////////////////////////////////
#endif