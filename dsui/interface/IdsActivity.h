/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,15th,2014

Module Name:

	IdsActivity.h 

Abstract: 活动页面接口


*/
/************************************************************************/


#ifndef __IDSACTIVITY_H__
#define __IDSACTIVITY_H__

typedef struct tagDsDnaInof
{
	HWND hwnd;
	IdsUIBase *pbaseParent;

	tagDsDnaInof():hwnd(NULL),pbaseParent(NULL){}

}DSDNAINFO,*PDSDNAINFO,*LPDSDNAINFO;

typedef BOOL (CALLBACK *BKGNDPROC) (HDC /*hdc*/, LPRECT /*lprcBkgnd*/, LPVOID /*lpParam*/);

typedef void (CALLBACK* TIP_MODIFY_PROC) (IdsUIBase *pui, LPRECT lprcTip, LPVOID lpParam);


#define ANIMATE_MOVE_SRC				0x0001				
#define ANIMATE_MOVE_DST				0x0002
#define ANIMATE_CONTENT_USEOLD			0x0004
#define ANIMATE_ASYNC					0x0008

#define ANIMATE_ROLL_FORTH				0x0001
#define ANIMATE_ROLL_BACK				0x0002
#define ANIMATE_PULL_FORTH				0x0004
#define ANIMATE_PULL_BACK				0x0008

#define ANIMATE_HORZ_POSITIVE			0x0010
#define ANIMATE_HORZ_NEGATIVE			0x0020
#define ANIMATE_VERT_POSITIVE			0x0040
#define ANIMATE_VERT_NEGATIVE			0x0080

interface DSUI_API IdsActivity
{
public:
	virtual IdsUIBase *CreateDsuiElement(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, WORD wUIID, __in LPRECT lprc, __in LPRECT lprcCoordMark, BOOL bUpdateLayout) DSUI_PURE;
	virtual IdsUIBase *GetDsuiElement(WORD wUIID) DSUI_PURE;
	virtual IdsUIBase *GetDsuiElement(LPARAM lParam, LPVOID pUserInfo) DSUI_PURE;

	virtual BOOL PreTranslateMessage(MSG* pMsg) DSUI_PURE;
	virtual BOOL PreTranslateMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) DSUI_PURE;

	virtual BOOL AddSkinPaintMission(IdsPaintMission *pdspm) DSUI_PURE;
	virtual IdsPaintMission *GetSkinPaintMisson(LPARAM lParamForFind) DSUI_PURE;
	virtual BOOL SetBkgndProc(BKGNDPROC procBkgnd, LPVOID /*lpParam*/) DSUI_PURE;

	virtual BOOL UpdateSize(BOOL bForUpdateLayout) DSUI_PURE;
	//virtual BOOL UpdatePaintBase(HDC hdcMem, HBITMAP hbmMem) DSUI_PURE;//for dsFrame using
	virtual BOOL InheritDNA(PDSDNAINFO pddi) DSUI_PURE;

	virtual COLORREF SetBkColor(COLORREF crBk) DSUI_PURE;
	//virtual IdsListView *Query_ListView(IdsUIBase *p) DSUI_PURE;
	virtual void ForceRefreshBkgnd() DSUI_PURE;
	virtual void ForceRefreshBkgnd_JustSetFlag() DSUI_PURE;

	virtual BOOL LinkWindow(HWND hwnd) DSUI_PURE;

	virtual BOOL SetTipFont(int nFontID) DSUI_PURE;
	virtual BOOL SetTipMargin(__in LPRECT lprcMargin) DSUI_PURE;
	virtual void SetTipExtent(int cxMax, int cyMax) DSUI_PURE;
	virtual COLORREF SetTipBkColor(COLORREF crBk) DSUI_PURE;
	virtual COLORREF SetTipTextColor(COLORREF crText) DSUI_PURE;
	virtual BOOL SetTransparentAttribute(BOOL bTransparent, COLORREF cr, BYTE bAlpha) DSUI_PURE;

	virtual HBITMAP	GetPrintBitmap() DSUI_PURE;

	virtual int SetState(int nState) DSUI_PURE;

	virtual void SetTipModifyProc(TIP_MODIFY_PROC proc, LPVOID lpParam) DSUI_PURE;

	virtual BOOL SetUpdateLayeredMark(BOOL bMark) DSUI_PURE;


	virtual void Animate(int nFlag, LPRECT lprcStart, LPRECT lprcEnd) DSUI_PURE;
	virtual void Animate(int nFlag, LPRECT lprcStart, LPRECT lprcEnd, int nFrameTime, int nFrameCount) DSUI_PURE;
	virtual void MakeAnimateEmptyView() DSUI_PURE;

	virtual void LockPaintForAnimate(BOOL bLock) DSUI_PURE;
	
};

DSUI_API IdsActivity *IdsNew_Activity();
DSUI_API void IdsDel_Activity(IdsActivity *p);





#endif /*__IDSACTIVITY_H__*/