#pragma once

#include"combase/IObjectQueue.h"
#include "SyncObject/criticalsection.h"
#include <queue>
class CObjeceQueue:
	public IObjeceQueue,
	private CUnknownImp
{
public:
	UNKNOWN_IMP1(IObjeceQueue);

	CObjeceQueue(void);
	~CObjeceQueue(void);

	STDMETHOD(Push)(IMSBase* pObj);
	STDMETHOD(Pop)(IMSBase** pObj);
	STDMETHOD_(DWORD,Size)();
	STDMETHOD(Clear)();
private:
	DECLARE_AUTOLOCK_CS(queue);
	std::queue<UTIL::com_ptr<IMSBase>>	m_queue;
};
