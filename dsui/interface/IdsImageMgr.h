/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,15th,2014

Module Name:

	IdsImageMgr.h 

Abstract: 活动页面接口


*/
/************************************************************************/

#ifndef __IDSIMAGEMGR_H__
#define __IDSIMAGEMGR_H__

#include <GdiPlus.h>
using namespace Gdiplus;

interface DSUI_API IdsImageMgr
{
public:
	virtual int NewImage(__in LPCWSTR lpszFileName, __in Image *pimg, BOOL bForceReplace) DSUI_PURE;
	virtual int NewIconImage(HINSTANCE hInst, __in LPCWSTR lpszFileName, BOOL bForceReplace) DSUI_PURE;
	virtual int GetImageID(__in LPCWSTR lpszFileName) DSUI_PURE;
	virtual Image *GetImagePtr(int nImageID) DSUI_PURE;
	virtual void ReleaseAllImage() DSUI_PURE;
	virtual BOOL ReleaseImage(LPCWSTR lpszFileName) DSUI_PURE;
	virtual BOOL ReleaseImage(int nImageID) DSUI_PURE;
};


DSUI_API IdsImageMgr *IdsGet_ImageMgr();

#endif /*__IDSIMAGEMGR_H__*/