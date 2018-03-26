#ifndef _X_SYNCHRONIZATION_H_
#define _X_SYNCHRONIZATION_H_

#include "XMutex.h"

#define SYN_OBJ()      XSynchronization _objXSynchronization(this)
#define SYN_OBJ_EX(x)  XSynchronization _objXSynchronization(x)
/////////////////////////////////////////////////////////////////////////////
class XSynchronization
{
public:
	XSynchronization(XMutex* lpMutex);
	~XSynchronization();

private:
	XMutex* m_lpMutex;
};

/////////////////////////////////////////////////////////////////////////////
#endif