//
// 项目名称:
//
// 实现功能: 作为调试模块必须声明的头文件
//
// 文件名称: dbg.h
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
// 1. 一般的信息打印.
//
//     Assert( (f == 5) );
//     AssertMsg( (f == 5), ("F needs to be %d here!\n", 5) );
//     AssertFunc( (f == 5), BadFunc() );
//     AssertEquals( f, 5 );
//     AssertFloatEquals( f, 5.0f, 1e-3 );
//
//     Warning("Oh I feel so %s all over\n", "yummy");
//
//	   Msg( "Isn't this exciting %d?", 5 );
//
//	   Error( "I'm just thrilled" );
//
//     GrpWarning( "group", level, "Oh I feel even yummier!\n" );
//
//     DbgOutputFunc( OutputFunc );
//
//
// 2. 只运行在DEBUG版下的代码
//
//   DBG_CODE(
//				{
//					int x = 5;
//					++x;
//				}
//           );
//
//
//   DBG_GRPCODE( "group", level,
//              { int x = 5; ++x; }
//            );
//
// 3. 异常进入调试器的断点
//
//   DBG_BREAK();
//
//	 DebuggerBreak();
//=============================================================================


#ifndef _DBG_INCLUDE_H_
#define _DBG_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif

#include "mslog/Syslay/platform.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>


#ifdef _WIN32
	// 代码注释， 例如：#pragma reminder(fix it later)
	#define chSTR2(x)		#x
	#define chSTR(x)		chSTR2(x)
	#define reminder(desc)	message(__FILE__ "(" chSTR(__LINE__) "):" #desc)

	// 文件名的宽字符定义
	#define WIDEN2(x) L ## x
	#define WIDEN(x) WIDEN2(x)
	#define __WFILE__ WIDEN(__FILE__)

	#if defined(_UNICODE) || defined(UNICODE)
		#define __TFILE__	__WFILE__
	#else
		#define __TFILE__	__FILE__
	#endif

#endif

//-----------------------------------------------------------------------------
// 导出的定义
//-----------------------------------------------------------------------------

#ifdef SYSLAY_DLL_EXPORT
	#define DBG_INTERFACE	DLL_EXPORT
	#define DBG_OVERLOAD	DLL_GLOBAL_EXPORT
	#define DBG_CLASS		DLL_CLASS_EXPORT
#else
	#define DBG_INTERFACE	DLL_IMPORT
	#define DBG_OVERLOAD	DLL_GLOBAL_IMPORT
	#define DBG_CLASS		DLL_CLASS_IMPORT
#endif


#define DEBUG_MESSAGE_BUFLEN_MAX		2048
//-----------------------------------------------------------------------------
// 调试信息的类型
enum DbgType_t
{
	DBG_MESSAGE = 0,
	DBG_WARNING,
	DBG_ASSERT,
	DBG_ERROR,
	DBG_LOG,
	DBG_REPORT,

	DBG_TYPE_COUNT
};

enum Msg_Level
{
	MsgLevel_Error		=0,	//1
	MsgLevel_Warning	=1,	//3
	MsgLevel_Msg		=2,	//5	
	MsgLevel_Log		=3,	//5	
	MsgLevel_Notify		=4	//10
};

enum DbgRetval_t
{
	DBG_RETURNED_DEBUGGER = 0,
	DBG_RETURNED_CONTINUES,
	DBG_RETURNED_ABORT
};

// 打印调试信息的函数原型
typedef DbgRetval_t (*DbgOutputFunc_tA)( DbgType_t spewType, char const *pMsg );
typedef DbgRetval_t (*DbgOutputFunc_tW)( DbgType_t spewType, wchar_t const *pMsg );

// 重定义新的打印调试信息的函数
DBG_INTERFACE void   DbgOutputFuncA( DbgOutputFunc_tA func );
DBG_INTERFACE void   DbgOutputFuncW( DbgOutputFunc_tW func );

// 获取老的函数
DBG_INTERFACE DbgOutputFunc_tA GetDbgOutputFuncA( void );
DBG_INTERFACE DbgOutputFunc_tW GetDbgOutputFuncW( void );

#if defined(_UNICODE) || defined(UNICODE)

#define DbgOutputFunc_t				DbgOutputFunc_tW
#define DbgOutputFunc				DbgOutputFuncW
#define GetDbgOutputFunc			GetDbgOutputFuncW

#else

#define DbgOutputFunc_t				DbgOutputFunc_tA
#define DbgOutputFunc				DbgOutputFuncA
#define GetDbgOutputFunc			GetDbgOutputFuncA

#endif

//-----------------------------------------------------------------------------
// 用于管理开发组，并定义各自打印调试信息的输出级别
// pGroupName - 每个人或者每个组最好要固定的名字，打印时调用Grp*簇的接口
//			例如后面定义了一个简化了的APP_GROUP
// level - 日志级别 0：表示保留；1：最重要的日志级别；2：次重要的类似<=2级才输出；3：以此类推4、5、。。。；
//
// 说明：	因此需要根据模块所在的调用逻辑层次，合理定义输出级别
//-----------------------------------------------------------------------------
DBG_INTERFACE void   DbgActivateA( char const* pGroupName, int level );
DBG_INTERFACE void   DbgActivateW( wchar_t const* pGroupName, int level );
DBG_INTERFACE bool   IsDbgActiveA( char const* pGroupName, int level );
DBG_INTERFACE bool   IsDbgActiveW( wchar_t const* pGroupName, int level );

#if defined(_UNICODE) || defined(UNICODE)

#define DbgActivate				DbgActivateW
#define IsDbgActive				IsDbgActiveW

#else

#define DbgActivate				DbgActivateA
#define IsDbgActive				IsDbgActiveA

#endif

//-----------------------------------------------------------------------------
// 一般不建议直接调用
// 打印信息（内部使用）
DBG_INTERFACE void			_DbgInfoA( DbgType_t type, char const* pFile, int line );
DBG_INTERFACE DbgRetval_t   _DbgMessageA( char const* pMsg, ... );
DBG_INTERFACE DbgRetval_t   _DDbgMessageA( char const *pGroupName, int level, char const* pMsg, ... );
DBG_OVERLOAD DbgRetval_t	_DbgMessageA( DbgType_t DebugType, char const* pMsgFormat, va_list args ,const char*szGrpName = NULL);
DBG_OVERLOAD DbgRetval_t	_DbgMessageModuleA( char const* pModuleName, DbgType_t DebugType, char const* pMsgFormat, va_list args );

DBG_INTERFACE void			_DbgInfoW( DbgType_t type, wchar_t const* pFile, int line );
DBG_INTERFACE DbgRetval_t   _DbgMessageW( wchar_t const* pMsg, ... );
DBG_INTERFACE DbgRetval_t   _DDbgMessageW( wchar_t const *pGroupName, int level, wchar_t const* pMsg, ... );
DBG_OVERLOAD DbgRetval_t	_DbgMessageW( DbgType_t DebugType, wchar_t const* pMsgFormat, va_list args ,const wchar_t*szGrpName = NULL);
DBG_OVERLOAD DbgRetval_t	_DbgMessageModuleW( wchar_t const* pModuleName, DbgType_t DebugType, wchar_t const* pMsgFormat, va_list args );

DBG_INTERFACE void			_ReportA( unsigned char * pBuffer, int iLen );
DBG_INTERFACE void			_ReportW( unsigned char * pBuffer, int iLen );

#if defined(_UNICODE) || defined(UNICODE)

#define _DbgInfo				_DbgInfoW
#define _DbgMessage				_DbgMessageW
#define _DDbgMessage			_DDbgMessageW
#define _DbgMessageModule		_DbgMessageModuleW

#else

#define _DbgInfo				_DbgInfoA
#define _DbgMessage				_DbgMessageA
#define _DDbgMessage			_DDbgMessageA
#define _DbgMessageModule		_DbgMessageModuleA

#endif

// 宏定义（内部使用）
#define  _AssertA( _exp )			do {															\
									if (!(_exp)) 													\
									{ 																\
										_DbgInfoA( DBG_ASSERT, __FILE__, __LINE__ );				\
										if (_DbgMessageA("Assertion Failed: " #_exp) == DBG_RETURNED_DEBUGGER)		\
										{															\
											DebuggerBreak();										\
										}															\
									}																\
									} while (0)

#define  _AssertMsgA( _exp, _msg )	do {															\
									if (!(_exp)) 													\
									{ 																\
										_DbgInfoA( DBG_ASSERT, __FILE__, __LINE__ );				\
										if (_DbgMessageA(_msg) == DBG_RETURNED_DEBUGGER)					\
										{															\
											DebuggerBreak();										\
										}															\
									}																\
									} while (0)

#define  _AssertFuncA( _exp, _f )	do {															\
									if (!(_exp)) 													\
									{ 																\
										_DbgInfoA( DBG_ASSERT, __FILE__, __LINE__ );				\
										DbgRetval_t ret = _DbgMessageA("Assertion Failed!" #_exp);	\
										_f;															\
										if (ret == DBG_RETURNED_DEBUGGER)									\
										{															\
											DebuggerBreak();										\
										}															\
									}																\
									} while (0)

#define  _AssertEqualsA( _exp, _expectedValue ) \
									do {															\
									if ((_exp) != (_expectedValue)) 								\
									{ 																\
										_DbgInfoA( DBG_ASSERT, __FILE__, __LINE__ );				\
										DbgRetval_t ret = _DbgMessageA("Expected %d but got %d!", (_expectedValue), (_exp));	\
										if (ret == DBG_RETURNED_DEBUGGER)									\
										{															\
											DebuggerBreak();										\
										}															\
									}																\
									} while (0)

#define  _AssertFloatEqualsA( _exp, _expectedValue, _tol ) \
									do {															\
									if (fabs((_exp) - (_expectedValue)) > (_tol))					\
									{ 																\
										_DbgInfoA( DBG_ASSERT, __FILE__, __LINE__ );				\
										DbgRetval_t ret = _DbgMessageA("Expected %f but got %f!", (_expectedValue), (_exp));	\
										if (ret == DBG_RETURNED_DEBUGGER)									\
										{															\
											DebuggerBreak();										\
										}															\
									}																\
									} while (0)


// 宏定义（内部使用）
#define  _AssertW( _exp )			do {															\
	if (!(_exp)) 													\
{ 																\
	_DbgInfoW( DBG_ASSERT, __WFILE__, __LINE__ );				\
	if (_DbgMessageW(L"Assertion Failed: " L#_exp) == DBG_RETURNED_DEBUGGER)		\
{															\
	DebuggerBreak();										\
}															\
}																\
} while (0)

#define  _AssertMsgW( _exp, _msg )	do {															\
	if (!(_exp)) 													\
{ 																\
	_DbgInfoW( DBG_ASSERT, __WFILE__, __LINE__ );				\
	if (_DbgMessageW(_msg) == DBG_RETURNED_DEBUGGER)					\
{															\
	DebuggerBreak();										\
}															\
}																\
} while (0)

#define  _AssertFuncW( _exp, _f )	do {															\
	if (!(_exp)) 													\
{ 																\
	_DbgInfoW( DBG_ASSERT, __WFILE__, __LINE__ );				\
	DbgRetval_t ret = _DbgMessageW(L"Assertion Failed!" L#_exp);	\
	_f;															\
	if (ret == DBG_RETURNED_DEBUGGER)									\
{															\
	DebuggerBreak();										\
}															\
}																\
} while (0)

#define  _AssertEqualsW( _exp, _expectedValue ) \
	do {															\
	if ((_exp) != (_expectedValue)) 								\
{ 																\
	_DbgInfoW( DBG_ASSERT, __WFILE__, __LINE__ );				\
	DbgRetval_t ret = _DbgMessageW(L"Expected %d but got %d!", (_expectedValue), (_exp));	\
	if (ret == DBG_RETURNED_DEBUGGER)									\
{															\
	DebuggerBreak();										\
}															\
}																\
	} while (0)

#define  _AssertFloatEqualsW( _exp, _expectedValue, _tol ) \
	do {															\
	if (fabs((_exp) - (_expectedValue)) > (_tol))					\
{ 																\
	_DbgInfoW( DBG_ASSERT, __WFILE__, __LINE__ );				\
	DbgRetval_t ret = _DbgMessageW(L"Expected %f but got %f!", (_expectedValue), (_exp));	\
	if (ret == DBG_RETURNED_DEBUGGER)									\
{															\
	DebuggerBreak();										\
}															\
}																\
	} while (0)


//-----------------------------------------------------------------------------
// 下面是我们要使用的各种东东:-)，同MFC用法相似，Release版时被忽略的
//-----------------------------------------------------------------------------
#ifdef _DEBUG

#define  AssertA( _exp )           _AssertA( _exp )
#define  AssertMsgA( _exp, _msg )  _AssertMsgA( _exp, _msg )
#define  AssertFuncA( _exp, _f )   _AssertFuncA( _exp, _f )
#define  AssertEqualsA( _exp, _expectedValue )              _AssertEqualsA( _exp, _expectedValue )
#define  AssertFloatEqualsA( _exp, _expectedValue, _tol )   _AssertFloatEqualsA( _exp, _expectedValue, _tol )

#define  AssertW( _exp )           _AssertW( _exp )
#define  AssertMsgW( _exp, _msg )  _AssertMsgW( _exp, _msg )
#define  AssertFuncW( _exp, _f )   _AssertFuncW( _exp, _f )
#define  AssertEqualsW( _exp, _expectedValue )              _AssertEqualsW( _exp, _expectedValue )
#define  AssertFloatEqualsW( _exp, _expectedValue, _tol )   _AssertFloatEqualsW( _exp, _expectedValue, _tol )

#define  AssertMsg1( _exp, _msg, a1 )									_AssertMsg( _exp, CDbgFmtMsg( _msg, a1 ) )
#define  AssertMsg2( _exp, _msg, a1, a2 )								_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2 ) )
#define  AssertMsg3( _exp, _msg, a1, a2, a3 )							_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3 ) )
#define  AssertMsg4( _exp, _msg, a1, a2, a3, a4 )						_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4 ) )
#define  AssertMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5 ) )
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6 ) )
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6 ) )
#define  AssertMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7 ) )
#define  AssertMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7, a8 ) )
#define  AssertMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) )

#if defined(_UNICODE) || defined(UNICODE)

#define Assert					AssertW
#define AssertMsg				AssertMsgW
#define AssertFunc				AssertFuncW
#define AssertEquals			AssertEqualsW
#define AssertFloatEquals		AssertFloatEqualsW

#else

#define Assert					AssertA
#define AssertMsg				AssertMsgA
#define AssertFunc				AssertFuncA
#define AssertEquals			AssertEqualsA
#define AssertFloatEquals		AssertFloatEqualsA

#endif

#else /* Not _DEBUG */

#define  Assert( _exp )           ((void)0)
#define  AssertMsg( _exp, _msg )  ((void)0)
#define  AssertFunc( _exp, _f )   ((void)0)
#define  AssertEquals( _exp, _expectedValue )              ((void)0)
#define  AssertFloatEquals( _exp, _expectedValue, _tol )   ((void)0)

#define  AssertMsg1( _exp, _msg, a1 )									((void)0)
#define  AssertMsg2( _exp, _msg, a1, a2 )								((void)0)
#define  AssertMsg3( _exp, _msg, a1, a2, a3 )							((void)0)
#define  AssertMsg4( _exp, _msg, a1, a2, a3, a4 )						((void)0)
#define  AssertMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					((void)0)
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			((void)0)
#define  AssertMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		((void)0)
#define  AssertMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	((void)0)

#endif   /* _DEBUG */


//_exp总会执行的
#if defined(_UNICODE) || defined(UNICODE)
#define  Verify( _exp )           _AssertW( _exp )
#else
#define  Verify( _exp )           _AssertA( _exp )
#endif

//-----------------------------------------------------------------------------
// 目的 : 我们的标准日志入口
// 输入 : *pGroupName - 不分大小写的组名
//		  level - 级别
//		  *pMsg - 待输出的调试信息
// 输出 : void -
// 说明 :	1、Grp* 簇接口用于支持分组打印；
//			2、GrpError 不进行任何组与级别检查，与 Error 等价；
//			3、Msg 使用 DBG_MESSAGE 类，用于普通信息，例如程序流程；
//			4、Log 使用 DBG_LOG 类，用于程序中里程碑式的运行信息；保留作为将来写入系统的事件数据库中！
//			5、Warning 使用 DBG_WARNING 类，用于告警信息，但不影响程序运行；
//			6、Error 使用 DBG_ERROR 类，说明出现致命错误，毫无疑问需要立即退出；
//			7、Report 使用 DBG_REPORT类，用于输出一些二进制的特殊信息；保留作为将来写入系统的事件数据库中！
//-----------------------------------------------------------------------------
DBG_INTERFACE void MsgA( char const* pMsg, ... );
DBG_INTERFACE void MsgW( wchar_t const* pMsg, ... );
DBG_INTERFACE void GrpMsgA( char const *pGroupName, int level, char const *pMsg, ... );
DBG_INTERFACE void GrpMsgW( wchar_t const *pGroupName, int level, wchar_t const *pMsg, ... );

DBG_INTERFACE void LogA( char const *pMsg, ... );
DBG_INTERFACE void LogW( wchar_t const *pMsg, ... );
DBG_INTERFACE void GrpLogA( char const *pGroupName, int level, char const *pMsg, ... );
DBG_INTERFACE void GrpLogW( wchar_t const *pGroupName, int level, wchar_t const *pMsg, ... );

DBG_INTERFACE void WarningA( char const *pMsg, ... );
DBG_INTERFACE void WarningW( wchar_t const *pMsg, ... );
DBG_INTERFACE void GrpWarningA( char const *pGroupName, int level, char const *pMsg, ... );
DBG_INTERFACE void GrpWarningW( wchar_t const *pGroupName, int level, wchar_t const *pMsg, ... );

DBG_INTERFACE void ErrorA( char const *pMsg, ... );
DBG_INTERFACE void ErrorW( wchar_t const *pMsg, ... );
DBG_INTERFACE void GrpErrorA( char const *pGroupName, int level, char const *pMsg, ... );
DBG_INTERFACE void GrpErrorW( wchar_t const *pGroupName, int level, wchar_t const *pMsg, ... );

DBG_INTERFACE void ReportA( unsigned char * pBuffer, int iLen );
DBG_INTERFACE void ReportW( unsigned char * pBuffer, int iLen );
DBG_INTERFACE void GrpReportA( char const *pGroupName, int level, unsigned char * pBuffer, int iLen );
DBG_INTERFACE void GrpReportW( wchar_t const *pGroupName, int level, unsigned char * pBuffer, int iLen );


#if defined(_UNICODE) || defined(UNICODE)

#define Msg			MsgW
#define GrpMsg		GrpMsgW
#define Log			LogW
#define GrpLog		GrpLogW
#define Warning		WarningW
#define GrpWarning	GrpWarningW
#define Error		ErrorW
#define GrpError	GrpErrorW
#define Report		ReportW
#define GrpReport	GrpReportW

#else

#define Msg			MsgA
#define GrpMsg		GrpMsgA
#define Log			LogA
#define GrpLog		GrpLogA
#define Warning		WarningA
#define GrpWarning	GrpWarningA
#define Error		ErrorA
#define GrpError	GrpErrorA
#define Report		ReportA
#define GrpReport	GrpReportA

#endif

// 致命错误的中断，例如 ErrorIfNot( bCondition, ("a b c %d %d %d", 1, 2, 3) );
#define ErrorIfNot( condition, msg ) \
	if ( (condition) )		\
		;					\
	else 					\
	{						\
		Error msg;			\
	}


//-----------------------------------------------------------------------------
// 目的 : 方便每人使用自己的分级调试信息，例如：
//			1、可以自定义使用一个 name 组，大家也可公用一个名字，由自己决定
//			2、默认输出级别为 level 级
//			3、 当name##Error没有级别判定的原因是，错误绝对必须被处理，因此等于大家都
//				调用Error()也可
//			4、例如 DECLARE_DEBUG_GROUP(Module1, 2) 表示Module1的默认调试级定义为2
//				，这样 Module1Msg(3, "OnlyU") 将不被输出；Module1Msg("Only you")
//				可被输出（默认为2）；Module1Msg(2, "OnlyU")可被输出（显示指定了2）
// 输入 : name - 定义使用自己的组
//		  level - 定义默认级别为 level 级
// 输出 : 自动产生8个inline类型的函数
//-----------------------------------------------------------------------------
#define CAT_DEBUG_REPORT(name, level) \
	{if( !IsDbgActive( #name, level ) ) return; \
		_Report( pBuffer, iLen ); \
	}

#define CAT_DEBUG_PRINT(name, type, level) \
	{if( !IsDbgActive( #name, level ) ) return; \
		va_list args; \
		va_start( args, pMsgFormat ); \
		_DbgMessageModule( #name, type, pMsgFormat, args ); \
		va_end(args); \
	}

#define CAT_DEBUG_ERROR(name, type) \
	{ \
		va_list args; \
		va_start( args, pMsgFormat ); \
		_DbgMessageModule( #name, type, pMsgFormat, args ); \
		va_end(args); \
	}

#define DECLARE_DEBUG_GROUP(name, defaultlevel) \
	inline void name##Msg( int level, char const* pMsgFormat, ... ) \
	CAT_DEBUG_PRINT(name, DBG_MESSAGE, level)\
	inline void name##Warning( int level, char const* pMsgFormat, ... ) \
	CAT_DEBUG_PRINT(name, DBG_WARNING, level)\
	inline void name##Log( int level, char const* pMsgFormat, ... ) \
	CAT_DEBUG_PRINT(name, DBG_LOG, level)\
	inline void name##Report( int level, unsigned char * pBuffer, int iLen ) \
	CAT_DEBUG_REPORT(name, level) \
	inline void name##Msg( char const *pMsgFormat, ... ) \
	CAT_DEBUG_PRINT(name, DBG_MESSAGE, defaultlevel)\
	inline void name##Warning( char const *pMsgFormat, ... ) \
	CAT_DEBUG_PRINT(name, DBG_WARNING, defaultlevel)\
	inline void name##Log( char const *pMsgFormat, ... ) \
	CAT_DEBUG_PRINT(name, DBG_LOG, defaultlevel) \
	inline void name##Report( unsigned char * pBuffer, int iLen ) \
	CAT_DEBUG_REPORT(name, defaultlevel) \
	inline void name##Error( char const *pMsgFormat, ... ) \
	CAT_DEBUG_ERROR(name, DBG_ERROR)


// 调试使用
#ifdef _DEBUG

	#define DBG_CODE( _code )				if (0) ; else { _code }
	#define DBG_GRPCODE( _g, _l, _code )	if (IsDbgActive( _g, _l )) { _code } else {}
	#define DBG_BREAK()						DebuggerBreak()	/* defined in platform.h */

#else /* not _DEBUG */

	#define DBG_CODE( _code )				((void)0)
	#define DBG_GRPCODE( _g, _l, _code )		((void)0)
	#define DBG_BREAK()						((void)0)

#endif /* _DEBUG */



//-----------------------------------------------------------------------------
// 检查指针的合法性
//-----------------------------------------------------------------------------
DBG_INTERFACE void _AssertValidReadPtr( void* ptr, int count = 1 );
DBG_INTERFACE void _AssertValidWritePtr( void* ptr, int count = 1 );
DBG_INTERFACE void _AssertValidReadWritePtr( void* ptr, int count = 1 );

DBG_INTERFACE  void AssertValidStringPtr( const char* ptr, int maxchar = 0xFFFFFF );
template<class T> inline void AssertValidReadPtr( T* ptr, int count = 1 )		     { _AssertValidReadPtr( (void*)ptr, count ); }
template<class T> inline void AssertValidWritePtr( T* ptr, int count = 1 )		     { _AssertValidWritePtr( (void*)ptr, count ); }
template<class T> inline void AssertValidReadWritePtr( T* ptr, int count = 1 )	     { _AssertValidReadWritePtr( (void*)ptr, count ); }

#define AssertValidThis() AssertValidReadWritePtr(this,sizeof(*this))

//-----------------------------------------------------------------------------
// 保护当前函数不被递归等方式重入
//-----------------------------------------------------------------------------
#ifdef _DEBUG
class CReentryGuard
{
public:
	CReentryGuard(int *pSemaphore)
	 : m_pSemaphore(pSemaphore)
	{
		++(*m_pSemaphore);
	}

	~CReentryGuard()
	{
		--(*m_pSemaphore);
	}

private:
	int *m_pSemaphore;
};

#define ASSERT_NO_REENTRY() \
	static int fSemaphore##__LINE__; \
	Assert( !fSemaphore##__LINE__ ); \
	CReentryGuard ReentryGuard##__LINE__( &fSemaphore##__LINE__ )
#else
#define ASSERT_NO_REENTRY()
#endif

//-----------------------------------------------------------------------------
// 格式化调试信息
//-----------------------------------------------------------------------------
class CDbgFmtMsg
{
public:
#if defined(_UNICODE) || defined(UNICODE)
	CDbgFmtMsg(const wchar_t *pszFormat, ...)
#else
	CDbgFmtMsg(const char *pszFormat, ...)
#endif
	{
		va_list arg_ptr;

		va_start(arg_ptr, pszFormat);
#if defined(_UNICODE) || defined(UNICODE)
		_vsnwprintf_s(m_szBuf, sizeof(m_szBuf), sizeof(m_szBuf)-1, pszFormat, arg_ptr);
#else
		_vsnprintf_s(m_szBuf, sizeof(m_szBuf), sizeof(m_szBuf)-1, pszFormat, arg_ptr);
#endif
		va_end(arg_ptr);

		m_szBuf[sizeof(m_szBuf)-1] = 0;
	}
#if defined(_UNICODE) || defined(UNICODE)
	operator const wchar_t *() const
#else
	operator const char *() const
#endif
	{
		return m_szBuf;
	}

private:

#if defined(_UNICODE) || defined(UNICODE)
	wchar_t m_szBuf[1024];
#else
	char m_szBuf[1024];
#endif

};

// //-----------------------------------------------------------------------------
// // 目的： 调试版时在目标文件中嵌入编译时间信息
// //-----------------------------------------------------------------------------
// #ifdef _WIN32
//
// 	#ifdef _DEBUG
// 		#pragma comment(compiler)
// 		#pragma comment(exestr,"Debug Version, Last Compile Time: " __DATE__ ", " __TIME__ " ***")
// 	#endif
//
// #endif


#endif // _DBG_INCLUDE_H_
