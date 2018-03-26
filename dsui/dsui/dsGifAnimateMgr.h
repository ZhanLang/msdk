/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	3,17th,2014

Module Name:

	dsGifAnimateMgr.h 

Abstract:


*/
/************************************************************************/


#ifndef __DSGIFANIMATEMGR_H__
#define __DSGIFANIMATEMGR_H__

#include <map>
#include <set>
using namespace std;

#include "timer.h"
#include "dsImageMgr.h"
#include "dsImageInfo.h"
#include "dsLock.h"

#include "IdsUIBase.h"

typedef map<Image*, dsImageInfo>MAP_IDSII;

class dsImageInfoTable{
public:

	static dsImageInfoTable g_instance;
	static dsImageInfoTable *getInstance(){
		return &g_instance;
	}

	

public:
	MAP_IDSII m_mapIdsii;
	
public:
	BOOL AddMark(int nImgID){
		return AddMark(dsImageMgr::getInstance()->GetImagePtr(nImgID));
	}

	BOOL AddMark(Image* pimg){

		if(NULL == pimg)
			return FALSE;

		MAP_IDSII::iterator it = m_mapIdsii.find(pimg);
		if(it == m_mapIdsii.end()){

			MAP_IDSII::_Pairib pib = m_mapIdsii.insert(make_pair(pimg, dsImageInfo(pimg)));

			if(pib.second){
				pib.first->second.Attach(pimg);
			}
		}

		return FALSE;
	}

	BOOL AddMark(const dsImageInfo &dsii){

		if(NULL == dsii.m_pimg)
			return FALSE;

		MAP_IDSII::iterator it = m_mapIdsii.find(dsii.m_pimg);
		if(it != m_mapIdsii.end()){

			MAP_IDSII::_Pairib pib = m_mapIdsii.insert(make_pair(dsii.m_pimg, dsii));
		}

		return FALSE;
	}

	BOOL DeleteMark(Image *pimg){

		MAP_IDSII::iterator it = m_mapIdsii.find(pimg);
		
		if(it != m_mapIdsii.end()){
			m_mapIdsii.erase(it);
			return TRUE;
		}

		return FALSE;

	}

	dsImageInfo *GetDsiiMark(int nImgID){

		return GetDsiiMark(dsImageMgr::getInstance()->GetImagePtr(nImgID));
	}

	dsImageInfo *GetDsiiMark(Image *pimg){
		
		MAP_IDSII::iterator it = m_mapIdsii.find(pimg);

		if(it != m_mapIdsii.end()){
			return &it->second;
		}

		return NULL;
	}

};

__declspec(selectany) dsImageInfoTable dsImageInfoTable::g_instance;




typedef struct tagGifAnimateInfo{
	Image* pimg;
	IdsUIBase *pui;
	CRect rc;
	CRect rcSrc;
	int nFramePos;
	BOOL bPause;
	UINT uFrameCount;
}GAI,*PGAI;

typedef map<DWORD, GAI> MAP_GAI;
          
 
class dsGifAnimateMgr:public dsTimer{
public:

	static dsGifAnimateMgr g_instance;
	static dsGifAnimateMgr *getInstance(){
		return &g_instance;
	}

public:
	
	MAP_GAI m_mapGai;
	dsLock m_lock;

public:

	dsGifAnimateMgr()
	{}

	~dsGifAnimateMgr(){}

	static void CALLBACK TimerProc(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
	{
		dsGifAnimateMgr *pthis = (dsGifAnimateMgr*)dwUser;

		pthis->RefreshGifs();
	}

public:


	BOOL StartAnimate(IdsUIBase *pui, DWORD dwKey, Image *pimg){
		if(NULL == pui || 0 == dwKey || NULL == pimg)
			return FALSE;


		dsImageInfoTable *piit = dsImageInfoTable::getInstance();

		m_lock.Lock();

		GAI gai = {0};

		gai.pui = pui;
		gai.pimg = pimg;


		if(0 == m_uTimerID)
			CreateTimer(200, 200, 0, TimerProc);

		if(!piit->GetDsiiMark(pimg))
			piit->AddMark(gai.pimg);

		dsImageInfo *pii = piit->GetDsiiMark(gai.pimg);

		if(NULL != pii)
			gai.uFrameCount = pii->m_nFrameCount;

		m_mapGai.insert(make_pair(dwKey, gai));

		m_lock.UnLock();

		

		return FALSE;
	}

//	void SuspendAnimate(DWORD dwK)

	void DestroyAnimate(DWORD dwKey){

		m_lock.Lock();

		MAP_GAI::iterator it = m_mapGai.find(dwKey);

		if(it != m_mapGai.end())
		{
			m_mapGai.erase(it);
		}

		if(m_mapGai.empty())
			DestroyTimer();

		m_lock.UnLock();
	}

	UINT GetImgFrmPos(DWORD dwKey){

		UINT uPos = 0;

		m_lock.Lock();

		MAP_GAI::iterator it = m_mapGai.find(dwKey);

		if(it != m_mapGai.end())
		{
			uPos = it->second.nFramePos ++;
			if(uPos == it->second.uFrameCount)
				it->second.nFramePos = 0;
		}

		m_lock.UnLock();

		return uPos;
	}

	BOOL CheckGifAnimate(DWORD dwKey){

		BOOL bRet = FALSE;
		m_lock.Lock();

		MAP_GAI::iterator it = m_mapGai.find(dwKey);

		if(it != m_mapGai.end())
		{
			bRet = TRUE;
		}

		m_lock.UnLock();

		return bRet;
	}

	void RefreshGifs(){

		set<HWND> setHWND;

		m_lock.Lock();
		MAP_GAI::iterator it = m_mapGai.begin();

		while(it != m_mapGai.end())
		{
			if(NULL != it->second.pui)
			{
				setHWND.insert(it->second.pui->GetHWND());
				it->second.pui->RedrawUI();
			}
			
			it ++;
		}

		m_lock.UnLock();

		set<HWND>::iterator itHWND = setHWND.begin();

		while(itHWND != setHWND.end()){
			::UpdateWindow(*itHWND);
			itHWND ++;
		}
	}
};

__declspec(selectany) dsGifAnimateMgr dsGifAnimateMgr::g_instance;






#endif /*__DSGIFANIMATEMGR_H__*/