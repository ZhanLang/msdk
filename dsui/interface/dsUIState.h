/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,2th,2014

Module Name:

	dsframe.h

Abstract: dsui ÔªËØ×´Ì¬


*/
/************************************************************************/


#ifndef __DSUISTATE_H__
#define __DSUISTATE_H__

#define DSUI_STATE_NULL			0
#define DSUI_STATE_UNCHECKED	0
#define DSUI_STATE_NORMAL	0x0001
#define DSUI_STATE_HOT		0x0002
#define DSUI_STATE_PUSHED	0x0004
#define DSUI_STATE_DISABLED	0x0008
#define DSUI_STATE_CHECKED	0x0010
#define DSUI_STATE_PARTCHECKED 0x0020

#define DSUI_STATE_ALL		0xffff

#endif