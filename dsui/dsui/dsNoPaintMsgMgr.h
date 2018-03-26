/************************************************************************/
/* 
Author:
lourking. (languang).All rights reserved.

Create Time:
10,9th,2014

Module Name:
dsNoPaintMsgMgr.h 

Abstract: 
mark of layared windows with no paint msg

*/
/************************************************************************/

#ifndef __DSNOPAINTMSGMGR_H__
#define __DSNOPAINTMSGMGR_H__

#include <set>
using namespace std;

class dsNoPaintMsgMgr
{
private:
	static dsNoPaintMsgMgr g_instance;
public:
	static dsNoPaintMsgMgr *getInstance(){
		return &g_instance;
	}

public:

	typedef set<HWND> SET_HWND;

	SET_HWND m_setHwnd;

public:

	dsNoPaintMsgMgr(){

	}

	~dsNoPaintMsgMgr(){

	}

public:

	BOOL SetMark(HWND hwnd, BOOL bMark){
		
		if(NULL == hwnd)
			return FALSE;

		DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

		if(bMark)
		{
			SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle | WS_EX_LAYERED);
			m_setHwnd.insert(hwnd);
		}
		else
		{
			SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle & ~WS_EX_LAYERED);
			m_setHwnd.erase(hwnd);
		}

		return TRUE;
	}

	BOOL IsMarked(HWND hwnd){
		return m_setHwnd.find(hwnd) != m_setHwnd.end();
	}
};


_declspec(selectany) dsNoPaintMsgMgr dsNoPaintMsgMgr::g_instance;


#endif /*__DSNOPAINTMSGMGR_H__*/