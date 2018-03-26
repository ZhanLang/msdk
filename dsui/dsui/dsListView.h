/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,13th,2014

Module Name:

	dsListView.h 

Abstract: tab¿Ø¼þ


*/
/************************************************************************/

#ifndef __DSLISTVIEW_H__
#define __DSLISTVIEW_H__

#include <list>
using namespace std;

#include "dsUIBase.h"




class dsListView:public dsUIBase,public IdsListView
{
public:
	typedef list<dsUIBase*> LIST_UIBASE;	
	typedef map<WORD,dsUIBase*> MAP_UIBASE;

private:
	CString m_strItemClass;

	int m_nFirstVisualItem;
	int m_nVisualItemCount;
	int m_nMaxMayShowItemCount;

	CSize m_sizeItem;
	CRect m_rcPadding;
	int m_xyDisOfItem;
	BOOL m_bVert;

	CSize m_sizeViewOffset;
	CRect m_rcTotalDraw;

	ENUMUIPROC_USER m_procExtLayout;

public:
	dsListView();
	

public:
	//////////////////////////////////////////////////////////////////////////
	//register

	static dsUIBase* CALLBACK NewDsuiElesInst();
	static BOOL GetClassInfo(__out PDSUICLASSINFO pci);

	virtual BOOL ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	//////////////////////////////////////////////////////////////////////////
	//override

public:

	virtual BOOL SetItemLayoutInfo(__in LPSIZE lpsizeItem, __in LPRECT lprcPadding, int xyDisOfItem, BOOL bVert);
	virtual BOOL SetDefaultItemClass(__in LPCWSTR lpszClass);
	virtual IdsUIBase *InsertItem(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, int nIndex, LPARAM lParam, BOOL bUpdateLayout);
	virtual void UpdateItemLayout(__in PULI puli);
	virtual BOOL SetExtLayoutProc(__in ENUMUIPROC_USER prcExtLayout);
	virtual void SetVisualOffset(int x, int y, BOOL bLayout);
	virtual CSize GetVisualOffset();
	virtual CSize GetTotalDrawSize();

	virtual void DeleteAllItem();
	//virtual IdsPaintMission *GetPaintMisson(LPARAM lParamForFind);
	//virtual BOOL AddPaintMisson(IdsPaintMission *pdspm);
	//virtual BOOL CopyPaint(IdsUIBase *psrc);
	
private:
	void UpdateLayoutBase();

	static BOOL CALLBACK EnumProc_UpdateItemLayout(dsUIBase *pbase, LPARAM lParam, int nIndex);

	void UpdateItemRect(__in dsUIBase *pbase, int nIndex);

	void ReInitializeData();


};

#endif /*__DSLISTVIEW_H__*/