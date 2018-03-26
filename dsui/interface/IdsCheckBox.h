/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,16th,2014

Module Name:

	IdsCheckBox.h 

Abstract: checkbox ½Ó¿Ú


*/
/************************************************************************/


#ifndef __IDSCHECKBOX_H__
#define __IDSCHECKBOX_H__


interface DSUI_API IdsCheckBox
{
public:
	virtual int SetCheckState(int nCheckState) DSUI_PURE;
	virtual int GetCheckState() DSUI_PURE;
};





#endif /*__IDSCHECKBOX_H__*/