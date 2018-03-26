#ifndef __SCREEN_SAVER_DECTECTER__H_
#define __SCREEN_SAVER_DECTECTER__H_
#include <pshpack4.h>
/*******************************************************************************
*
*
*  文件标题:    ScrDetectInfo.h 
*
*  版    本:    1.00
*  
*  相关文档:    无
*
*
*
*  Rstray向RavTask发送桌面屏保状态的通讯结构。
*  流程：Rstray检查屏保 -> 有屏保了通知RavTask -> Rstray每隔一段时间就检查是否
*　　　　还在屏保如果在 -> 通知RavTask还在屏保 -> 如果不在屏保了 -> 通知RavTask
*        离开屏保。
*
*******************************************************************************/

// 屏保状态标志
enum {
	SCRSTATE_START_SCREEN_SAVER = 1,	// 用户桌面开始进入屏保状态
	SCRSTATE_STILL_IN_SCREEN_SAVER,		// 用户桌面仍然处于屏保状态
	SCRSTATE_OVER_SCREEN_SAVER			// 用户桌面离开屏保状态
};

struct tag_ScreenSaverDetectInfo
{
	DWORD dwSessionID;		//SessionID
	DWORD dwScrState;		//屏保状态标志
};
typedef tag_ScreenSaverDetectInfo SCREENSAVERDETECTINFO,*LP_SCREENSAVERDETECTINFO;

#include <poppack.h>
#endif // end of __SCREEN_SAVER_DECTECTER__H_