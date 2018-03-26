/***************************************************************************************
* AUTHOR : sudami [sudami@163.com]
* TIME   : 2009/03/02 [2:3:2009 - 15:28]
* MODULE : F:\Tmp\磁盘操作\code\ntfs-3g-1.0\libntfs-3g\debug.c 
*
* Description:
*   Debugging output functions. Originated from the Linux-NTFS project.
*                        
*
***
* Copyright (c) 2008 - 2010 sudami.
* Freely distributable in source or binary for noncommercial purposes.
* TAKE IT EASY,JUST FOR FUN.
*
****************************************************************************************/



#include <errno.h>


#include "types.h"
#include "runlist.h"
#include "debug.h"
#include "logging.h"

//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

void 
ntfs_debug_runlist_dump(
	IN const runlist_element *rl
	)
/*++

Author: sudami [sudami@163.com]
Time  : 2009/03/02 [2:3:2009 - 15:26]

Routine Description:
  将该数据运行中的内容按照一定的格式打印出来
    
--*/
{
	int i = 0;
	const char *lcn_str[5] = { 
		"LCN_HOLE         ", 
		"LCN_RL_NOT_MAPPED",
		"LCN_ENOENT       ", "LCN_EINVAL       ",
		"LCN_unknown      " 
	};

	ntfs_log_debug("NTFS-fs DEBUG: Dumping runlist (values in hex):\n");
	if ( !rl ) { // 参数合法性检测
		ntfs_log_debug("Run list not present.\n");
		return;
	}

	ntfs_log_debug("VCN              LCN               Run length\n");
	do {
		LCN lcn = (rl + i)->lcn;

		if ( lcn < (LCN)0 ) {
			int idx = -lcn - 1;

			if (idx > -LCN_EINVAL - 1)
				idx = 4;
			ntfs_log_debug("%-16llx %s %-16llx%s\n", rl[i].vcn, lcn_str[idx], rl[i].length, rl[i].length ? "" : " (runlist end)");
		} else
			ntfs_log_debug("%-16llx %-16llx  %-16llx%s\n", rl[i].vcn, rl[i].lcn, rl[i].length, rl[i].length ? "" : " (runlist end)");

	} while (rl[i++].length);

	return ;
}

#endif
