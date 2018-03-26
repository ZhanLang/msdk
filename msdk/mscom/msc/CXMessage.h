#ifndef _CX_MESSAGE_H_
#define _CX_MESSAGE_H_

#include "XMessage.h"
/////////////////////////////////////////////////////////////////////////////
class CXMessage :public XMessage
{
private:
	CXMessage();
	~CXMessage();

public:
	BYTE   VersionMajor();
	BYTE   VersionMinor();
	RSMSGID  MessageID();
	BOOL   IsNotify();
	BOOL   IsRequest();
	BOOL   IsResponse();
	DWORD  UniqueID();
	DWORD  DataLen();
	LPBYTE Data();
	DWORD  ProductID();
	VOID   Initialize(RSMSGID dwMessageID, DWORD dwDataLen, DWORD dwUniqueID = 0xFFFFFFFF);
};

/////////////////////////////////////////////////////////////////////////////
#endif