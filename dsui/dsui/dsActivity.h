/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,2th,2014

Module Name:

	dsActivity.h

Abstract: dsui 元素的集合体，一个完整的ui抽象页面


*/
/************************************************************************/

#ifndef __DSACTIVITY_H__
#define __DSACTIVITY_H__

#include <list>
#include <map>

using namespace std;

#include "dsUIClassMgr.h"
#include "dsUIBase.h"
#include "dsToolTip.h"

typedef struct tagDsPaintInfo
{
	LPPAINTSTRUCT lpps;
}DSPAINTINFO,*PDSPAINTINFO,*LPDSPAINTINFO;

typedef struct tagDsMouseInfo
{
	CPoint ptMouse;
	dsUIBase *pbase;
}DSMOUSEINFO,*PDSMOUSEINFO,*LPDSMOUSEINFO;

class dsActivity:public IdsActivity
{
public:
	typedef list<dsUIBase*> LIST_UIBASE;	
	typedef map<WORD,dsUIBase*> MAP_UIBASE;	
	//typedef multimap<WORD,dsUIBase*> MULTIMAP_LAYOUT;


public:
	LIST_UIBASE m_listUIBase;	//用于遍历
	MAP_UIBASE m_mapUIBase;		//用于快速查找
	//MULTIMAP_LAYOUT m_multimapLayout;//用于布局
	
	HWND m_hwnd;

	CDC m_dcMem;
	CBitmap m_bmMem;

	CDC m_dcSkinMem;
	CBitmap m_bmSkinMem;
	CRect m_rcSkin;
	BOOL m_bNeedBkgnd;
	COLORREF m_crBk;

	WNDPROC m_procOriginal;
	BKGNDPROC m_procBkgnd;

	LPVOID m_lpBkgndParam;

private:

	dsUIBase m_uiSkin;

	dsUIBase *m_puiCapture,*m_puiHot;

	BOOL m_bTracking;

	dsTip m_tooltip;

	BOOL m_bLockPaintForAnimate;

	BOOL m_bUpdateLayeredMark;

public:
	dsActivity();
	~dsActivity();;
public:

	virtual IdsUIBase *CreateDsuiElement(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, WORD wUIID, __in LPRECT lprc, __in LPRECT lprcCoordMark, BOOL bUpdateLayout);
	virtual IdsUIBase *GetDsuiElement(WORD wUIID);
	virtual IdsUIBase *GetDsuiElement(LPARAM lParam, LPVOID pUserInfo);

public:
	//msg
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreTranslateMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void *DispatchMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK UIProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


public://data func

	void DeleteAllUIElement();

	BOOL IsUIElesExist(int wUIID);

	void EnumUIElements(ENUMUIPROC procEnumUI, LPARAM lParam, BOOL bReverse = FALSE);
	

	static BOOL CALLBACK EnumUIProc_Paint(__in dsUIBase *pbase, LPARAM lParam, int nIndex);
	static BOOL CALLBACK EnumUIProc_MouseHit(__in dsUIBase *pbase, LPARAM lParam, int nIndex);
	static BOOL CALLBACK EnumUIProc_InheritDNA(__in dsUIBase *pbase, LPARAM lParam, int nIndex);
	static BOOL CALLBACK EnumUIProc_UpdateLayout(__in dsUIBase *pbase, LPARAM lParam, int nIndex);
	static BOOL CALLBACK EnumUIProc_UpdateLayout_ForceLayout(__in dsUIBase *pbase, LPARAM lParam, int nIndex);


	virtual BOOL LinkWindow(HWND hwnd);

public:
	//skin

	virtual BOOL AddSkinPaintMission(IdsPaintMission *pdspm);
	virtual IdsPaintMission *GetSkinPaintMisson(LPARAM lParamForFind);

	void DrawSkin(CDCHandle dc, __in LPPAINTSTRUCT lpps);
	virtual BOOL SetBkgndProc(BKGNDPROC procBkgnd, LPVOID lpParam);

	virtual BOOL UpdateSize(BOOL bForUpdateLayout = FALSE);
	void UpdatePaintBase(BOOL bForce = FALSE);

	virtual BOOL InheritDNA(PDSDNAINFO pddi);

	virtual COLORREF SetBkColor(COLORREF crBk);
	virtual void ForceRefreshBkgnd();
	virtual void ForceRefreshBkgnd_JustSetFlag();

	virtual BOOL SetTipFont(int nFontID);
	virtual BOOL SetTipMargin(__in LPRECT lprcMargin);
	virtual void SetTipExtent(int cxMax, int cyMax);
	virtual COLORREF SetTipBkColor(COLORREF crBk);
	virtual COLORREF SetTipTextColor(COLORREF crText);

	virtual BOOL SetTransparentAttribute(BOOL bTransparent, COLORREF cr, BYTE bAlpha);

	virtual HBITMAP	GetPrintBitmap();
	virtual int SetState(int nState);

	virtual void SetTipModifyProc(TIP_MODIFY_PROC proc, LPVOID lpParam);

	virtual BOOL SetUpdateLayeredMark(BOOL bMark);

	virtual void Animate(int nFlag, LPRECT lprcStart, LPRECT lprcEnd);

	virtual void Animate(int nFlag, LPRECT lprcStart, LPRECT lprcEnd, int nFrameTime, int nFrameCount);

	virtual void MakeAnimateEmptyView();

	virtual void LockPaintForAnimate(BOOL bLock);

public:

	//////////////////////////////////////////////////////////////////////////
	//query


	//IdsListView *Query_ListView( __in IdsUIBase *pbase );


	void DoPaint_Normal();
	void DoPaint_UpdateLayered();
	void DoPaint_UpdateLayered2();
	//void DoPaint_UpdateLayered(int nFlag, POINT ptStart, POINT ptEnd);



};

#endif /*__DSACTIVITY_H__*/
