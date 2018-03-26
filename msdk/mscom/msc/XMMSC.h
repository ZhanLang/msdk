#ifndef _XM_MSC_H_
#define _XM_MSC_H_

#include "msc\XMessage.h"

/////////////////////////////////////////////////////////////////////////////
#pragma pack(push, Enter_XSubscriber, 4)
#pragma warning (disable : 4200)

typedef struct tagXSubscriber
{
	DWORD dwProcessID;
	DWORD dwThreadID;
	DWORD dwSubSystemID;
	DWORD dwSubscriberID;
}XSubscriber;

typedef struct tagXRelation
{
	DWORD   dwCount;
	RSMSGID  dwMessageID;
	XSubscriber xSubscribers[0];
}XRelation;

typedef struct tagXRelations
{
	DWORD dwCount;
	union
	{
		XRelation xRelation;
		DWORD dwDummy;
	}/*xRelations[0]*/xRelations;
}XRelations;

#pragma warning (default : 4200)
#pragma pack(pop, Enter_XSubscriber)

/////////////////////////////////////////////////////////////////////////////
#define XMMSC_RESERVE                       XMNOTIFY  (0,0,(XMPARTIDBASE_MSC + 0))
// Data=XCridet
#define XMMSC_LOGIN_REQUEST                 XMREQUEST (0,0,(XMPARTIDBASE_MSC + 1))
// Data=NULL
#define XMMSC_LOGIN_RESPONSE                XMRESPONSE(0,0,(XMPARTIDBASE_MSC + 2))
// Data=dwCount;dwMessageIDs[dwCount];
#define XMMSC_SUBSCRIBE                     XMNOTIFY  (0,0,(XMPARTIDBASE_MSC + 3))
// Data=dwCount;dwMessageIDs[dwCount];
#define XMMSC_UNSUBSCRIBE                   XMNOTIFY  (0,0,(XMPARTIDBASE_MSC + 4))
// Data=NULL
#define XMMSC_RELATION_REQUEST              XMREQUEST (0,0,(XMPARTIDBASE_MSC + 5))
// Data=XRelations
#define XMMSC_RELATION_RESPONSE             XMRESPONSE(0,0,(XMPARTIDBASE_MSC + 6))

/////////////////////////////////////////////////////////////////////////////
#endif