#include "StdAfx.h"
#include <algorithm>
#include "SMSCMessageMap.h"
#include "MSCHandler.h"
#include "CXMessage.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
CSMSCMessageMap::CSMSCMessageMap()
{
}

/////////////////////////////////////////////////////////////////////////////
CSMSCMessageMap::~CSMSCMessageMap()
{
	MAPMSG::iterator itMap;
	std::list<CSubscriber*>::iterator itList;

	for (itList = m_listSubscriber.begin(); itList != m_listSubscriber.end(); itList++)
	{
		(*itList)->Release();
	}
	m_listSubscriber.clear();


	for (itMap = m_mapMessage.begin(); itMap != m_mapMessage.end(); itMap++)
	{
		for (itList = itMap->second.begin(); itList != itMap->second.end(); itList++)
		{
			(*itList)->Release();
		}
		itMap->second.clear();
	}
	m_mapMessage.clear();
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::Subscribe(DWORD dwCount, RSMSGID* lpMessageID, CSubscriber* lpSubscriber)
{
	if (0 == dwCount)
	{
		Remove(lpSubscriber);
		lpSubscriber->AddRef();
		m_listSubscriber.push_back(lpSubscriber);
		return;
	}

	std::list<CSubscriber*>::iterator itList = std::find(m_listSubscriber.begin(), m_listSubscriber.end(), lpSubscriber);
	if (itList != m_listSubscriber.end())
	{
		return;
	}

	for (DWORD i = 0; i < dwCount; i++)
	{
		std::list<CSubscriber*>& rList = m_mapMessage[lpMessageID[i]];
		itList = std::find(rList.begin(), rList.end(), lpSubscriber);
		if (itList == rList.end())
		{
			if (lpMessageID[i] != XMMSC_RESERVE)
			{
				lpSubscriber->AddRef();
				m_mapMessage[lpMessageID[i]].push_back(lpSubscriber);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::UnSubscribe(DWORD dwCount, RSMSGID* lpMessageID, CSubscriber* lpSubscriber)
{
	if (0 == dwCount)
	{
		Remove(lpSubscriber);
		return;
	}

	std::list<CSubscriber*>::iterator itList = std::find(m_listSubscriber.begin(), m_listSubscriber.end(), lpSubscriber);
	if (itList != m_listSubscriber.end())
	{
		return;
	}

	QUEUEMSG queueMessage;
	for (DWORD i = 0; i < dwCount; i++)
	{
		std::list<CSubscriber*>& rList = m_mapMessage[lpMessageID[i]];
		itList = std::find(rList.begin(), rList.end(), lpSubscriber);
		if (itList != rList.end())
		{
			lpSubscriber->Release();
			rList.erase(itList);
		}

		if (rList.empty())
		{
			queueMessage.push(lpMessageID[i]);
		}
	}

	while (!queueMessage.empty())
	{
		m_mapMessage.erase(queueMessage.front());
		queueMessage.pop();
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::Remove(CSubscriber* lpSubscriber)
{
	std::list<CSubscriber*>::iterator itList = std::find(m_listSubscriber.begin(), m_listSubscriber.end(), lpSubscriber);
	if (itList != m_listSubscriber.end())
	{
		lpSubscriber->Release();
		m_listSubscriber.erase(itList);
		return;
	}

	QUEUEMSG queueMessage;
	MAPMSG::iterator itMap;
	for (itMap = m_mapMessage.begin(); itMap != m_mapMessage.end(); itMap++)
	{
		std::list<CSubscriber*>& rList = itMap->second;
		std::list<CSubscriber*>::iterator itList = std::find(rList.begin(), rList.end(), lpSubscriber);
		if (itList != rList.end())
		{
			lpSubscriber->Release();
			rList.erase(itList);
		}

		if (rList.empty())
		{
			queueMessage.push(itMap->first);
		}
	}

	while (!queueMessage.empty())
	{
		m_mapMessage.erase(queueMessage.front());
		queueMessage.pop();
	}	
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::Handle(XMessageBuffer* lpMessageBuffer, CSubscriber* lpSubscriber)
{
	std::list<CSubscriber*>::iterator itList;
	std::queue<CSubscriber*> queueSubscriber;
	for (itList = m_listSubscriber.begin(); itList != m_listSubscriber.end(); itList++)
	{
		(*itList)->AddRef();
		queueSubscriber.push(*itList);
	}

	CXMessage* lpXMessage = (CXMessage*)lpMessageBuffer->Header();
    RSMSGID uMsgID  = lpXMessage->MessageID();
	MAPMSG::iterator itMap;

	////////////////////////////////////////////////////////////
	//找消息格式完全一样的
	itMap = m_mapMessage.find(uMsgID);
	if (itMap != m_mapMessage.end())
	{
		std::list<CSubscriber*>& rList = itMap->second;	
		for (itList = rList.begin(); itList != rList.end(); itList++)
		{
			(*itList)->AddRef();
			queueSubscriber.push(*itList);
		}
	}
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	//caoliang old
	//itMap = m_mapMessage.find(lpXMessage->MessageID());
	//if (itMap != m_mapMessage.end())
	//{
	//	std::list<CSubscriber*>& rList = itMap->second;	
	//	for (itList = rList.begin(); itList != rList.end(); itList++)
	//	{
	//		(*itList)->AddRef();
	//		queueSubscriber.push(*itList);
	//	}
	//}

	if (queueSubscriber.empty())
	{
		// 没有任何人关心此消息，是否给消息发送者回送一个消息?
		SAFE_DELETE(lpMessageBuffer)
		return;
	}

	CMSCHandler* lpMSCHandler = CMSCHandler::GetInstance();
	_ASSERT(NULL != lpMSCHandler);

	// 可以把消息做引用计数，但是依赖系统内核锁内存的实现机制，以后调试时再优化
	XMessageBuffer* lpTempMessageBuffer = NULL;
	while (!queueSubscriber.empty())
	{	
		lpTempMessageBuffer = lpMessageBuffer->Clone();
		if (NULL != lpTempMessageBuffer)
		{
			queueSubscriber.front()->PendingWriteBuffer(lpTempMessageBuffer);
			lpMSCHandler->TryPendingWrite(queueSubscriber.front());
		}
		else
		{  
			queueSubscriber.front()->Release();
		}

		queueSubscriber.pop();
	}
	SAFE_DELETE(lpMessageBuffer);
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::Relation(XMessageBuffer* lpMessageBuffer, CSubscriber* lpSubscriber)
{
	DWORD dwDataLen = CalcRelationBufferLen();

	BOOL bResult = FALSE;
	XMessageBuffer* lpTempMessageBuffer = new XMessageBuffer(dwDataLen, bResult);
	if (NULL == lpTempMessageBuffer)
	{
		SAFE_DELETE(lpMessageBuffer);
		return;
	}

	if (!bResult)
	{
		SAFE_DELETE(lpTempMessageBuffer);
		SAFE_DELETE(lpMessageBuffer);
		return;
	}

	CXMessage* lpXMessage = (CXMessage*)lpTempMessageBuffer->Header();
	CXMessage* lpTempXMessage = (CXMessage*)lpMessageBuffer->Header();
	lpXMessage->Initialize(XMMSC_RELATION_RESPONSE, dwDataLen, lpTempXMessage->UniqueID());
	SAFE_DELETE(lpMessageBuffer);

	FillRelations(lpXMessage->Data());

	CMSCHandler* lpMSCHandler = CMSCHandler::GetInstance();
	_ASSERT(NULL != lpMSCHandler);

	lpSubscriber->AddRef();
	lpSubscriber->PendingWriteBuffer(lpTempMessageBuffer);
	lpMSCHandler->TryPendingWrite(lpSubscriber);
	return;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSMSCMessageMap::Size()
{
	return m_mapMessage.size();
}

/////////////////////////////////////////////////////////////////////////////
DWORD CSMSCMessageMap::CalcRelationBufferLen()
{
	DWORD dwDataLen = sizeof(XRelations);
	if (!m_listSubscriber.empty())
	{
		dwDataLen += sizeof(XRelation);
		dwDataLen += m_listSubscriber.size() * sizeof(XSubscriber);
	}
	
	MAPMSG::iterator itMap;
	for (itMap = m_mapMessage.begin(); itMap != m_mapMessage.end(); itMap++)
	{
		dwDataLen += sizeof(XRelation);
		dwDataLen += itMap->second.size() * sizeof(XSubscriber);
	}
	return dwDataLen;
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::FillRelations(LPBYTE lpData)
{
	XRelations* lpXRelations = (XRelations*)lpData;
	XRelation* lpXRelation = &(lpXRelations->/*xRelations[0]*/xRelations.xRelation);
	if (m_listSubscriber.empty())
	{
		lpXRelations->dwCount = m_mapMessage.size();
	}
	else
	{
		lpXRelations->dwCount = m_mapMessage.size() + 1;
		FillRelation(&lpXRelation, XMMSC_RESERVE, m_listSubscriber);
	}

	MAPMSG::iterator itMap;
	for (itMap = m_mapMessage.begin(); itMap != m_mapMessage.end(); itMap++)
	{
		FillRelation(&lpXRelation, itMap->first, itMap->second);
	}
}

/////////////////////////////////////////////////////////////////////////////
VOID CSMSCMessageMap::FillRelation(XRelation** lppXRelation, __int64 dwMessageID, std::list<CSubscriber*>& rList)
{
	XRelation& rXRelation  = **lppXRelation;
	rXRelation.dwCount     = rList.size();
	rXRelation.dwMessageID = dwMessageID;

	DWORD dwIndex = 0;
	std::list<CSubscriber*>::iterator it;
	for (it = rList.begin(); it != rList.end(); it++, dwIndex++)
	{
		rXRelation.xSubscribers[dwIndex].dwProcessID    = (*it)->ProcessID();
		rXRelation.xSubscribers[dwIndex].dwThreadID     = (*it)->ThreadID();
		rXRelation.xSubscribers[dwIndex].dwSubSystemID  = (*it)->SubSystemID();
		rXRelation.xSubscribers[dwIndex].dwSubscriberID = (*it)->SubscriberID();
	}

	LPBYTE lpBuffer = (LPBYTE)(*lppXRelation);
	lpBuffer += sizeof(XRelation);
	lpBuffer += sizeof(XSubscriber) * (rList.size());
	(*lppXRelation) = (XRelation*)lpBuffer;
}

