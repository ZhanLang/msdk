/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,9th,2014

Module Name:

	dsLayoutInfo.h

Abstract: 

Abstract: dsui 坐标、布局系统


             /\
             |
             |
bottomright  |
             |
<____________|
*/
/************************************************************************/


#ifndef __DSLAYOUTINFO_H__
#define __DSLAYOUTINFO_H__



#define LAYOUT_HORZ		0x0001
#define LAYOUT_VERT		0x0002

#define COORDMARK_TOPLEFT 0
#define COORDMARK_BOTTOMRIGHT 1
#define COORDMARK_CENTER 2

typedef struct tagDsLayoutInfo
{
	CRect rcWnd;//top and left must be zero
}DSLAYOUTINFO,*PDSLAYOUTINFO,*LPDSLAYOUTINFO;



#endif /*__DSLAYOUTINFO_H__*/
