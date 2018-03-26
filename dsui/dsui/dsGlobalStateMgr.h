/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,7th,2014

Module Name:

	dsGlobalStateMgr.h

Abstract: dsui 常规宏定义 以及头文件集合


*/
/************************************************************************/


#ifndef __DSGLOBALSTATEMGR_H__
#define __DSGLOBALSTATEMGR_H__

class dsUIBase;

class dsGlobalStateMgr
{
public:
	static dsGlobalStateMgr g_fminstance;

public:
	static dsGlobalStateMgr *getInstance(){
		return &g_fminstance;
	}

private:
	dsUIBase *m_puiFocus;
	dsUIBase *m_puiCapture;
	dsUIBase *m_puiHot;
	dsUIBase *m_puiMouseWheel;

public:

	dsGlobalStateMgr():
	m_puiFocus(NULL)
	,m_puiCapture(NULL)
	,m_puiHot(NULL)
	,m_puiMouseWheel(NULL)
	{
	}

	inline dsUIBase *GetFocusUIPtr() const{//for keyboard
		return m_puiFocus;
	}

	inline dsUIBase *GetCaptureUIPtr() const{//for mouse
		return m_puiCapture;
	}

	inline dsUIBase *GetHotUIPtr() const{//for mouse
		return m_puiHot;
	}

	inline dsUIBase *GetMouseWheelCaptureUIPtr() const{
		return m_puiMouseWheel;
	}

	dsUIBase *SetFocus(dsUIBase *puiFocus);
	
	dsUIBase *SetCapture(dsUIBase *puiCapture);

	dsUIBase *ReleaseCapture();

	dsUIBase *SetHot(dsUIBase *puiHot);

	dsUIBase *SetMouseWheelCaptureUIPtr(__in dsUIBase *puiMouseWheel);

	void ReleaseMouseWheelCaptureUIPtr();

	void CheckDisabledUI(dsUIBase *pui)
	{
		if(NULL == pui)
			return;

		if(pui == m_puiHot)
			m_puiHot = NULL;

		if(pui == m_puiCapture)
			m_puiCapture = NULL;

		if(pui == m_puiFocus)
			m_puiFocus = NULL;
	}

	void CheckDeletedUIPtr(dsUIBase *pui)
	{
		if(NULL == pui)
			return;

		if(pui == m_puiHot)
			m_puiHot = NULL;

		if(pui == m_puiCapture)
			m_puiCapture = NULL;

		if(pui == m_puiFocus)
			m_puiFocus = NULL;

		if(NULL == pui)
			m_puiMouseWheel = NULL;

		
	}

};

#endif /*__DSGLOBALSTATEMGR_H__*/
