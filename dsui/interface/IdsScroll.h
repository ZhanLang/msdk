/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,16th,2014

Module Name:

	IdsScroll.h 

Abstract: scroll ½Ó¿Ú


*/
/************************************************************************/


#ifndef __IDSSCROLL_H__
#define __IDSSCROLL_H__

interface DSUI_API IdsScroll
{
public:
	virtual int SetScrollInfo(__in LPSCROLLINFO psi) DSUI_PURE;
	virtual BOOL GetScrollInfo(__inout LPSCROLLINFO psi) DSUI_PURE;

	virtual int SetPos(int nPos) DSUI_PURE;
	virtual int GetPos() DSUI_PURE;

	virtual void SetForegndPaint(LPARAM lParamForFind) DSUI_PURE;

	virtual BOOL PaintSwitch(BOOL bPaint) DSUI_PURE;
};



#endif /*__IDSSCROLL_H__*/