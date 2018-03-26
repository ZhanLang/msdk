/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,2th,2014

Module Name:

	IdsPaintMission.h 

Abstract: »æÖÆ½Ó¿Ú


*/
/************************************************************************/


#ifndef __IDSPAINTMISSON_H__
#define __IDSPAINTMISSON_H__


interface DSUI_API IdsPaintMission
{
public:
	virtual void Reset() DSUI_PURE;
	virtual void SetState(int nState) DSUI_PURE;
	virtual BOOL SetDstRect(__in LPRECT lprc) DSUI_PURE;
	virtual BOOL SetDstCoordMark(__in LPRECT lprc) DSUI_PURE;
	virtual void SetPaintMode(int nPaintMode) DSUI_PURE;
	virtual void SetImageID(int nImageID) DSUI_PURE;
	virtual BOOL SetSrcRect(__in LPRECT lprc) DSUI_PURE;
	virtual BOOL SetSquares9(__in LPRECT lprc) DSUI_PURE;
	virtual BOOL SetText(__in LPCWSTR lpszText) DSUI_PURE;
	virtual void SetTextDrawFlag(UINT uFormat) DSUI_PURE;
	virtual void SetColor(COLORREF cr) DSUI_PURE;
	virtual void SetParam(LPARAM lParam) DSUI_PURE;
	virtual void SetFont(int nFontID) DSUI_PURE;
	virtual BOOL Enable(BOOL bEnable) DSUI_PURE;
	virtual void MarkAsIcon(BOOL bEnable) DSUI_PURE;

	virtual BOOL AddSuperTextElement(LPCWSTR lpszText, COLORREF crText, int nFontID) DSUI_PURE;
	virtual void ResetSuperText() DSUI_PURE;
	virtual BOOL SetSuperTextElement(int nPos, LPCWSTR lpszText, COLORREF crText, int nFontID) DSUI_PURE;
	
	//virtual BOOL MarkAsGif(BOOL bMark) DSUI_PURE;
	virtual BOOL MarkAsGif(DWORD dwKey) DSUI_PURE;
	virtual BOOL UseUIText(BOOL bUse) DSUI_PURE;
};

DSUI_API IdsPaintMission *IdsNew_PaintMission();
DSUI_API void IdsDel_PaintMission(IdsPaintMission *p);

#endif /*__IDSPAINTMISSON_H__*/