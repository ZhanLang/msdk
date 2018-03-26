#ifndef _MSC_H_
#define _MSC_H_

/////////////////////////////////////////////////////////////////////////////
#ifdef USE_COMMONDLL
#	ifdef MSC_EXPORTS
#		define MSCAPI __declspec(dllexport)
#	else
#		define MSCAPI __declspec(dllimport)
#	endif
#else
#	define MSCAPI 
#endif

#include "XMSCProxy.h"
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////
//初始化，宿主服务进程为各个子系统提供产品名称和安装路径
MSCAPI BOOL WINAPI MSC_Initialize(LPCTSTR lpszProduct, LPCTSTR lpszInstallPath);
//反初始化，给各个子系统一个清理机会
MSCAPI VOID WINAPI MSC_Uninitialize();
//启动，要求各个子系统自己开线程运行，不能阻塞

//?没检查init
MSCAPI BOOL WINAPI MSC_Start();
//停止，要求各个子系统停止自己的运行
MSCAPI VOID WINAPI MSC_Stop();
//创建MSCProxy对象
MSCAPI BOOL WINAPI MSC_CreateMSCProxy(XMSCProxy** lppXMSCProxy);
/////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif