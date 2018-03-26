/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,16th,2014

Module Name:

	IdsProgress.h 

Abstract: progress ½Ó¿Ú


*/
/************************************************************************/


#ifndef __IDSPROGRESS_H__
#define __IDSPROGRESS_H__


class DSUI_API IdsProgress
{
public:
	virtual void SetRange(int nMin, int nMax) DSUI_PURE;
	virtual BOOL GetRange(__out LPINT lpnMin, __out LPINT lpnMax) DSUI_PURE;
	virtual int SetPos(int nPos) DSUI_PURE;
	virtual int GetPos() DSUI_PURE;

	virtual void SetForegndPaint(LPARAM lParamForFind) DSUI_PURE;
};

#endif /*__IDSPROGRESS_H__*/