/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSMUTEX_H__683BE375_0B5F_4CF1_A332_0715B2B3D77C__INCLUDED_)
#define AFX_FSMUTEX_H__683BE375_0B5F_4CF1_A332_0715B2B3D77C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

class fsMutex  
{
public:
	void Unlock();
	void Lock();
	fsMutex();
	virtual ~fsMutex();

protected:
	HANDLE m_mx;
	int m_cLocks;
};

#endif 
