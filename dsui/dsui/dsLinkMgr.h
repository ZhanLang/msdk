/************************************************************************/
/* 
Author:

	lourking. All rights reserved.

Create Time:

	2,7th,2014

Module Name:

	dsLinkMgr.h 

Abstract: dsui窗口消息关联管理器


*/
/************************************************************************/


#ifndef __DSLINKMGR_H__
#define __DSLINKMGR_H__

#include <map>
using namespace std;

class dsActivity;

class dsLinkMgr
{
public:
	typedef map<HWND, dsActivity*> MAP_LINK;

public:
	static dsLinkMgr g_instance;
public:

	MAP_LINK m_mapLink;

public:

	static dsLinkMgr *getInstance(){
		return &g_instance;
	}

public:
	
	BOOL AddLink(HWND hwnd, dsActivity* pact, BOOL bReplace = TRUE){

		if(NULL == hwnd || NULL == pact)
			return FALSE;

		MAP_LINK::iterator it = m_mapLink.find(hwnd);

		if(it != m_mapLink.end())
		{
			if(!bReplace)
				return FALSE;
			else
			{
				it->second = pact;
			}
		}else{
			
			m_mapLink.insert(make_pair(hwnd, pact));
		}


		return TRUE;
	}

	BOOL DeleteLink(HWND hwnd){
		if(NULL == hwnd)
			return FALSE;

		MAP_LINK::iterator it = m_mapLink.find(hwnd);

		if(it != m_mapLink.end())
		{
			m_mapLink.erase(it);
			return TRUE;
		}

		return FALSE;
	}

	dsActivity *GetLinkActivity(HWND hwnd){

		if(NULL == hwnd)
			return NULL;

		MAP_LINK::iterator it = m_mapLink.find(hwnd);

		if(it != m_mapLink.end())
		{
			return it->second;
		}

		return NULL;
		
	}

};

__declspec(selectany) dsLinkMgr dsLinkMgr::g_instance;






#endif /*__DSLINKMGR_H__*/