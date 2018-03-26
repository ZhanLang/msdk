
// 项目名称:
//
// 实现功能: 作为调试模块必须声明的头文件
//
// 文件名称: stdlog.h
//
// 创建作者:
//
// 创建日期:
//
// 修改作者:
//
// 修改日期:
//
// （如有新的修改人请在这里添加）
//
// 使用说明:
//		如果在当前进程比如A.EXE的目录中有stdlog.ini内容如下:
//	[CmdLine]
//	A.EXE = -pid -tid
//	B.EXE...
//	...
//
//	那么我们会忽略进程自己的命令行，使用stdlog.ini中指定的此进程的命令行参数，此功能主要用于比如服务程序的日志输出调整
//
// 已经实现地 命令行
// -dbgstr	输出到OutputDebugString，一般在调试器中可以看到，或者用工具DebugView.exe也可以。
// -file [c:\a.txt]	输出到日志文件中，默认为XXX.exe.log，XXX为这个exe的名字，可以选择指定一个其他的文件名。
// -cons			输出到控制台窗口stdout，也就是使用printf的输出。
// -pipe [sss]		输出到某个管道，名字为前缀\\.\pipe\XXX，其中XXX默认定义为DebugWindow，可以指定为一个其他的。
// -pid				输出的每一行日志包含当前进程id，如[4527]
// -tid				输出的每一行日志包含当前线程id，如[5279]
// -time			输出的每一行日志包含当前时间，如2010-01-20 17:48:22
// -group rsplugdll 2 onlyu 3	可以初始化指定调试打印组的日志输出级别，特别的-group * 1标识默认日志级别为1级
// -mt				当进程为多线程运行时，为了防止日志输出多线程混乱，因此可以设置此参数，但是效率肯定不乐观。
//
// 尚未实现
// -pdb [c:\pdb;]	当Debug版调试遇到Assert错误时，即使不在调试器跑，也可以输出调用的堆栈以及参数值，并且可以选择pdb符号路径
// -wmcpy					允许采用WM_COPYDATA传递日志
// -tcp [193.168.19.66:800]	允许采用Tcp方式传递日志

//=============================================================================
#pragma once
#include "mslog/syslay/dbg.h"

//在别的项目中直接用发布版的吧
#ifdef STDLOG_LINK_STATIC
	#define STDLOG_API
#else

	#ifdef STDLOG_EXPORTS
		#define STDLOG_API __declspec(dllexport)
	#else
		#define STDLOG_API __declspec(dllimport)
	#endif
#endif


extern STDLOG_API int nstdlog;

extern"C" STDLOG_API  void  InitDumpLib(int iReserved = 0, HMODULE hModule = 0);
extern"C" STDLOG_API  void	TermDumpLib();

