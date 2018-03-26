#ifndef _X_CREDIT_H_
#define _X_CREDIT_H_

/////////////////////////////////////////////////////////////////////////////
#pragma pack(push, XCredit, 4)

struct XCredit
{
	// RSA512 + SHA-1
	BYTE  lpCreditData[64];
	DWORD dwProcessID;
	DWORD dwThreadID;
	DWORD dwConnectTime;
	DWORD dwMagic;
	DWORD dwSubSystemID;
	DWORD dwReserve1;
	DWORD dwReserve2;
    BOOL  bDotNet;
	DWORD dwDataLen;
	//TE  lpData[0];

	enum
	{
		XCREDIT_EXPIRE = 5 * 1000,
	};
};

#pragma pack(pop, XCredit)
/////////////////////////////////////////////////////////////////////////////
#endif