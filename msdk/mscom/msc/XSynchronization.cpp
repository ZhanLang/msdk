#include "StdAfx.h"
#include "XSynchronization.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/////////////////////////////////////////////////////////////////////////////
XSynchronization::XSynchronization(XMutex* lpMutex)
	:m_lpMutex(lpMutex)
{
	_ASSERT(NULL != m_lpMutex);
	m_lpMutex->Lock();
}

/////////////////////////////////////////////////////////////////////////////
XSynchronization::~XSynchronization()
{
	m_lpMutex->Unlock();
}