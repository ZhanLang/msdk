/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	4,11th,2014

Module Name:

	dsNetMsg.h 

Abstract: 消息


*/
/************************************************************************/

#ifndef __DSNETMSG_H__
#define __DSNETMSG_H__

//////////////////////////////////////////////////////////////////////////
//用户交互消息

typedef struct dsNetMsg
{
	IdsHftpEngine *pEngine;
	int nMissionID;
	UINT uNetMsg;
	WPARAM wParam;
	LPARAM lParam;
}DSNETMSG, *PDSNETMSG;



#define DSHFTP_MSG_LITTLEPIECEIN	0x00010000



#endif /*__DSNETMSG_H__*/
