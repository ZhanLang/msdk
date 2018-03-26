#ifndef _S_MSC_MESSAGE_MAP_H_
#define _S_MSC_MESSAGE_MAP_H_

#include <map>
#include <list>
#include "XMMSC.h"
#include "Subscriber.h"
/////////////////////////////////////////////////////////////////////////////
class CSMSCMessageMap
{
public:
	CSMSCMessageMap();
	~CSMSCMessageMap();

	VOID  Subscribe(DWORD dwCount, RSMSGID* lpMessageID, CSubscriber* lpSubscriber);
	VOID  UnSubscribe(DWORD dwCount, RSMSGID* lpMessageID, CSubscriber* lpSubscriber);
	VOID  Remove(CSubscriber* lpSubscribe);
	VOID  Handle(XMessageBuffer* lpMessageBuffer, CSubscriber* lpSubscriber);
	VOID  Relation(XMessageBuffer* lpMessageBuffer, CSubscriber* lpSubscriber);
	DWORD Size();
	
private:
	DWORD CalcRelationBufferLen();
	VOID  FillRelations(LPBYTE lpData);
	VOID  FillRelation(XRelation** lppXRelation, __int64 dwMessageID, std::list<CSubscriber*>& rList);
typedef std::map<RSMSGID, std::list<CSubscriber*> > MAPMSG;
typedef std::queue<RSMSGID> QUEUEMSG;

	MAPMSG m_mapMessage;
	std::list<CSubscriber*> m_listSubscriber;
};

/////////////////////////////////////////////////////////////////////////////
#endif