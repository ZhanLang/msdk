#include "StdAfx.h"
#include "ObjeceQueue.h"

CObjeceQueue::CObjeceQueue(void)
{
}

CObjeceQueue::~CObjeceQueue(void)
{
}

STDMETHODIMP CObjeceQueue::Push(IMSBase* pObj)
{
	RASSERT(pObj, E_INVALIDARG);
	AUTOLOCK_CS(queue);
	m_queue.push(pObj);
	return S_OK;
}

STDMETHODIMP CObjeceQueue::Pop(IMSBase** pObj)
{
	RASSERT(pObj, E_INVALIDARG);
	*pObj = INULL;

	AUTOLOCK_CS(queue);
	if (m_queue.size())
	{
		UTIL::com_ptr<IMSBase> pItem = m_queue.front();
		return pItem->QueryInterface(__uuidof(IMSBase), (void**)pObj);
	}
	
	return E_FAIL;
}

STDMETHODIMP_(DWORD) CObjeceQueue::Size()
{
	AUTOLOCK_CS(queue);
	return m_queue.size();
}

STDMETHODIMP CObjeceQueue::Clear()
{
	AUTOLOCK_CS(queue);
	m_queue.empty();
	return S_OK;
}