/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

1,2th,2014

Module Name:

dsframe.h

*/
/************************************************************************/
#pragma once

#ifndef __DSUIBASE_H__
#define __DSUIBASE_H__

#include <list>
using namespace std;

#include "dsTools.h"
#include "dsLayoutInfo.h"
#include "dsUIState.h"
#include "dsPaintMission.h"
#include "dsUIClassInfo.h"

class dsUIBase;

typedef BOOL (CALLBACK *ENUMUIPROC)(dsUIBase *pbase, LPARAM lParam, int nIndex);

class dsUIBase:public IdsUIBase
{
public:
	typedef list<dsUIBase*> LIST_UIBASE;	
	typedef map<WORD,dsUIBase*> MAP_UIBASE;	
	typedef list<dsPaintMission> LIST_DSPM;

public:
	WORD m_wID;
	int m_nState;
	CRect m_rcUI;
	CRect m_rcUISrc;
	CRect m_rcCoordMark;
	CString m_strText;
	CString m_strTip;
	BOOL m_bVisible;
	BOOL m_bAutoGrow;

	dsUIBase *m_pbaseParent;
	LAYOUTPROC m_procLayout;

	HWND m_hwnd;

	LPARAM m_lParam;

	WORD m_wSysCursorID;


	LIST_DSPM m_listDSPM;
	LIST_UIBASE m_listUIBase;	//用于遍历
	MAP_UIBASE m_mapUIBase;		//用于快速查找

public:

	static dsUIBase* CALLBACK NewDsuiElesInst();
	static BOOL GetClassInfo(__out PDSUICLASSINFO pci);

	dsUIBase();
	virtual ~dsUIBase();
		
public:

	void SetRect(LPRECT lprc);

	int SetState(int nState);

	virtual BOOL EnableUI(BOOL bEnabled);

	virtual void SetVisible(BOOL bVisible);

	void SetID(WORD wID);

	virtual void SetParam(LPARAM lParam);
	virtual LPARAM GetParam();

	virtual IdsPaintMission *GetPaintMisson(LPARAM lParamForFind);
	virtual BOOL AddPaintMisson(IdsPaintMission *pdspm);


	//int GetState();

	virtual int GetState();

	virtual BOOL CopyPaint(IdsUIBase *psrc);

	virtual IdsUIBase * SetMouseWheelCapture();
	virtual void ReleaseMouseWheelCapture();

	virtual IdsUIBase *GetParent();
	virtual BOOL MoveUI(__in LPRECT lprcDst, BOOL bRedraw);
	virtual BOOL MoveSrcUI(__in LPRECT lprcDst, BOOL bRedraw);

	virtual BOOL GetText(__out LPWSTR lpszText, int nMaxCount);

	virtual BOOL SetAutoGrow(BOOL bAutoGrow);
	virtual BOOL IsAutoGrow();

	virtual int GetChildCount();

	virtual void RedrawUI();

	virtual BOOL dsuiInvalidateRect(__in_opt HWND hWnd, __in_opt CONST RECT *lpRect, __in BOOL bErase);

	virtual void SetTooltipText(__in LPCWSTR lpszTooltip);

	BOOL GetTooltipText(__out LPWSTR lpszText, int nMaxCount);

	
	

public:
	//work

	virtual BOOL ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void DoPaint(CDCHandle dc, LPPAINTSTRUCT lpps);

	virtual BOOL DrawImage(CDCHandle dc, __in Graphics *pgrah, __in Image *pimg, __in LPRECT lprcDst, __in LPRECT lprcSrc, __in LPRECT lprcPaint, int nPaintMode, BOOL bDrawAsIcon = FALSE);

	virtual void DealPaintMisson(dsPaintMission *pdspm, CDCHandle dc, LPPAINTSTRUCT lpps);



	
	virtual BOOL DoUpdateLayout(__in PDSLAYOUTINFO pdsli, int nIndex);
	
	virtual void SetLayoutProc(__in LAYOUTPROC procLayout);

	virtual BOOL GetRect(__out LPRECT lprcRect);
	virtual BOOL GetSrcRect(__out LPRECT lprcRect);

	virtual IdsUIBase *EnumUI(__in ENUMPROC procFind, __in LPARAM lParam);

	virtual int SetSystemCursor(WORD wSysCursorID)
	{
		return dsTools::Exchange2Objects(m_wSysCursorID,wSysCursorID);
	}
	//virtual void OnFrameSizeChange(__in LPSIZE lpSizeFrame);

	virtual HWND GetHWND(){
		return m_hwnd;
	}

public:

	//////////////////////////////////////////////////////////////////////////
	//for child ,可以用于递归

	virtual IdsUIBase *CreateDsuiElement(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, WORD wUIID, __in LPRECT lprc, __in LPRECT lprcCoordMark, BOOL bUpdateLayout);
	virtual IdsUIBase *GetDsuiElement(WORD wUIID);

	dsUIBase *InsertDsuiElement(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, int nIndex = -1, LPARAM lParam = 0);

	void DeleteAllUIElement();

	BOOL IsUIElesExist(int wUIID);

	dsUIBase *EnumUIElements(ENUMUIPROC procEnumUI, LPARAM lParam, BOOL bReverse = FALSE, int nFirst = 0, int nCount = -1);

	//////////////////////////////////////////////////////////////////////////
	//for paint

	CRect GetVisibleRect();

	//////////////////////////////////////////////////////////////////////////
	//
	void FreeGifAnimate(){
		LIST_DSPM::iterator it = m_listDSPM.begin();

		while(it != m_listDSPM.end())
		{
			
			(*it).MarkAsGif(0);

			it ++;
		}
	}

	public:


};




#endif /*__DSUIBASE_H__*/