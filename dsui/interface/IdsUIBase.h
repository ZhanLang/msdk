/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,2th,2014

Module Name:

	IdsUIBase.h 

Abstract: UI元素基类接口


*/
/************************************************************************/


#ifndef __IDSUIBASE_H__
#define __IDSUIBASE_H__

interface IdsUIBase;

//////////////////////////////////////////////////////////////////////////
/*
LAYOUTPROC:
这个回调会在框架的循环中处理，如果继续让框架处理子 ui元素的布局就返回 TRUE,如果自己处理不让框架处理就返回 FALSE；
*/
//////////////////////////////////////////////////////////////////////////
typedef BOOL (CALLBACK *LAYOUTPROC) (__in IdsUIBase* /*pbase*/,__in PDSLAYOUTINFO /*pdsli*/, int /*nIndex*/);

typedef BOOL (CALLBACK *ENUMPROC) (__in IdsUIBase* /*pbaseChild*/, __in LPARAM lParam, __in int nIndex);



interface DSUI_API IdsUIBase
{
public:
	virtual IdsPaintMission *GetPaintMisson(LPARAM lParamForFind) DSUI_PURE;
	virtual BOOL AddPaintMisson(IdsPaintMission *pdspm) DSUI_PURE;
	virtual BOOL CopyPaint(IdsUIBase *psrc) DSUI_PURE;
	virtual IdsUIBase *SetMouseWheelCapture() DSUI_PURE;
	virtual void ReleaseMouseWheelCapture() DSUI_PURE;
	virtual IdsUIBase *GetParent() DSUI_PURE;
	virtual void SetLayoutProc(__in LAYOUTPROC procLayout) DSUI_PURE;
	virtual IdsUIBase *CreateDsuiElement(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, WORD wUIID, __in LPRECT lprc, __in LPRECT lprcCoordMark, BOOL bUpdateLayout) DSUI_PURE;
	virtual IdsUIBase *GetDsuiElement(WORD wUIID) DSUI_PURE;
	virtual BOOL MoveUI(__in LPRECT lprcDst, BOOL bRedraw) DSUI_PURE;
	virtual BOOL MoveSrcUI(__in LPRECT lprcDst, BOOL bRedraw) DSUI_PURE;
	virtual BOOL GetText(__out LPWSTR lpszText, int nMaxCount) DSUI_PURE;
	virtual void SetParam(LPARAM lParam) DSUI_PURE;
	virtual LPARAM GetParam() DSUI_PURE;
	virtual BOOL SetAutoGrow(BOOL bAutoGrow) DSUI_PURE;
	virtual BOOL IsAutoGrow() DSUI_PURE;
	virtual int GetChildCount() DSUI_PURE;
	virtual void RedrawUI() DSUI_PURE;
	virtual BOOL GetRect(__out LPRECT lprcRect) DSUI_PURE;
	virtual BOOL GetSrcRect(__out LPRECT lprcRect) DSUI_PURE;
	virtual IdsUIBase *EnumUI(__in ENUMPROC procFind, __in LPARAM lParam) DSUI_PURE;
	virtual BOOL EnableUI(BOOL bEnabled) DSUI_PURE;
	virtual void SetVisible(BOOL bVisible) DSUI_PURE;
	virtual int SetSystemCursor(WORD wSysCursorID) DSUI_PURE;
	virtual void SetTooltipText(__in LPCWSTR lpszTooltip) DSUI_PURE;
	virtual int SetState(int nState) DSUI_PURE;
	virtual int GetState() DSUI_PURE;
	virtual HWND GetHWND() DSUI_PURE;
	
};



#endif /*__IDSUIBASE_H__*/