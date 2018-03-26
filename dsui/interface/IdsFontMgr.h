/************************************************************************/
/* 
Author:

	lourking. (languang).All rights reserved.

Create Time:

	1,21th,2014

Module Name:

	IdsFontMgr.h 

Abstract: ×ÖÌå½Ó¿Ú


*/
/************************************************************************/

#ifndef __IDSFONTMGR_H__
#define __IDSFONTMGR_H__

interface DSUI_API IdsFontMgr
{
public:
	virtual int NewFont(__in LPCWSTR lpszFontKeyName, int nPointSize, __in LPCWSTR lpszFaceName,bool bBold, bool bItalic, bool bForceReplace) DSUI_PURE;
	virtual int NewFont(__in LPCWSTR lpszFontKeyName, int nPointSize, __in LPCWSTR lpszFaceName,bool bBold, bool bItalic, bool bUnderline, bool bForceReplace) DSUI_PURE;
	virtual int NewFont(__in LPCWSTR lpszFontKeyName, HFONT hFont, BOOL bForceReplace) DSUI_PURE;
	virtual int GetFontID(__in LPCWSTR lpszFontKeyName) DSUI_PURE;
	virtual HFONT GetFont(__in int nFontID) DSUI_PURE;
};


DSUI_API IdsFontMgr *IdsGet_FontMgr();





#endif /*__IDSFONTMGR_H__*/