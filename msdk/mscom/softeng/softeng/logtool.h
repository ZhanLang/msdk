#ifndef __RAVNET_LOGUTILS_H__
#define __RAVNET_LOGUTILS_H__ 

#pragma once

#include <Windows.h>
#include <list>

//////////////////////////////////////////////////////////////////////////
// 在使用之前请先定义:【CFG_FILE_NAME，LOG_FILE_NAME】这两个宏。
//						例如:#define CFG_FILE_NAME _T("logtool.ini")
//						     #define LOG_FILE_NAME _T("logtool.log")
//						
//配置文件格式:【logtool.ini】
//	[Log]
//	LogLevel = 1              日志级别 
//	MaxFileSize = 25          日志大小
//	OutFilePath = c:\logg.log 日志输出位置
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//注意，将 (CFG_FILE_NAME) (LOG_FILE_NAME) 宏定义到 #include "logtool.h"之前
#ifndef CFG_FILE_NAME
#	define CFG_FILE_NAME _T("logtool.ini")
#endif //CFG_FILE_NAME

#ifndef LOG_FILE_NAME
#	define LOG_FILE_NAME _T("logtool.log")
#endif //LOG_FILE_NAME


//////////////////////////////////////////////////////////////////////////
//日志输出目录--------注意以 \ 结尾哦
//#define LOG_OUTPUT_DIR _T("C:\\") //将日志输出到C:\下

enum
{	
#
#ifdef _DEBUG
	LEVEL_DEFAULT = 9,  //调试版打印所有日志
#else
	LEVEL_DEFAULT = 3,  //发布版最大化，只打印错误日志
#endif // _DEBUG
	LEVEL_NONE    = 0,
	LEVEL_ERROR   = 1,
	LEVEL_WARNING = 2,
	LEVEL_INFO    = 3,
	LEVEL_DEBUG   = 4,
	//LEVEL_XXXXX   = X,
	LEVEL_MAX     = 9
};

#define NMsError                  _NCMsLogUtility::GetInstance()->Error
#define NMsWarning                _NCMsLogUtility::GetInstance()->Warning
#define NMsInfo                   _NCMsLogUtility::GetInstance()->Info
#define NMsDebug                  _NCMsLogUtility::GetInstance()->Debug
#define NMsTrace                  _NCMsLogUtility::GetInstance()->Trace
//用于跟踪函数的进出
#define NMsMethod(X)              _NCMsMethod _nMsMethod_obj(X)//_NCMsMethod _nMsMethod_obj(#X)


#define NMsErrorEx                _NCMsLogUtility::GetInstance()->ErrorEx
#define NMsWarningEx              _NCMsLogUtility::GetInstance()->WarningEx
#define NMsInfoEx                 _NCMsLogUtility::GetInstance()->InfoEx
#define NMsDebugEx                _NCMsLogUtility::GetInstance()->DebugEx
#define NMsTraceEx                _NCMsLogUtility::GetInstance()->TraceEx
#define NMsMethodEx(X, Y)         _NCMsMethod _nMsMethod_obj(X, Y)

#define NMsDestory()              _NCMsLogUtility::DestroyInstance()

#define NMsSetLevel(X)            _NCMsLogUtility::GetInstance()->SetLevelEx(MODULE_CURRENTPROCESS, X)
#define NMsGetLevel()             _NCMsLogUtility::GetInstance()->GetLevelEx(MODULE_CURRENTPROCESS)
#define NMsSetMaxFileSize(X)      _NCMsLogUtility::GetInstance()->SetMaxFileSizeEx(MODULE_CURRENTPROCESS, X)
#define NMsGetMaxFileSize()       _NCMsLogUtility::GetInstance()->GetMaxFileSizeEx(MODULE_CURRENTPROCESS)
#define NMsSetFilePath(X)         _NCMsLogUtility::GetInstance()->SetFilePathEx(MODULE_CURRENTPROCESS, X)
#define NMsGetFilePath(X)         _NCMsLogUtility::GetInstance()->GetFilePathEx(MODULE_CURRENTPROCESS, X)
#define NMsFlushFile()            _NCMsLogUtility::GetInstance()->FlushFileEx(MODULE_CURRENTPROCESS)
#define NMsCloseFile()            _NCMsLogUtility::GetInstance()->CloseFileEx(MODULE_CURRENTPROCESS)

#define NMsSetLevelEx(X, Y)       _NCMsLogUtility::GetInstance()->SetLevelEx(X, Y)
#define NMsGetLevelEx(X)          _NCMsLogUtility::GetInstance()->GetLevelEx(X)
#define NMsSetMaxFileSizeEx(X, Y) _NCMsLogUtility::GetInstance()->SetMaxFileSizeEx(X, Y)
#define NMsGetMaxFileSizeEX(X)    _NCMsLogUtility::GetInstance()->GetMaxFileSizeEx(X)
#define NMsSetFilePathEx(X, Y)    _NCMsLogUtility::GetInstance()->SetFilePathEx(X, Y)
#define NMsGetFilePathEx(X, Y)    _NCMsLogUtility::GetInstance()->GetFilePathEx(X, Y)
#define NMsFlushFileEx(X)         _NCMsLogUtility::GetInstance()->FlushFileEx(X)
#define NMsCloseFileEx(X)         _NCMsLogUtility::GetInstance()->CloseFileEx(X)

#define NMs_ASSERT(x)\
do \
{\
	if (!(x))\
	{\
		char buffer[256] = { 0 }; \
		sprintf_s(buffer, 256,"File=(%s) Line=(%d) <%s>", __FILE__, __LINE__, #x); \
		OutputDebugStringA(buffer); \
	}\
} while (0);
enum
{
	MODULE_CURRENTPROCESS = 0,
	MODULE_COMMUNICATE    = 1,
    MODULE_DLCENTER       = 2,
	MODULE_STRATEGY       = 3,
	MODULE_AGENTSPECIAL   = 4,
};

/////////////////////////////////////////////////////////////////////////////
class _NCMsMutex
{
public:
	_NCMsMutex();
	virtual ~_NCMsMutex();

	VOID Acquire();
	VOID Release();
protected:
	CRITICAL_SECTION m_csSync;
};

/////////////////////////////////////////////////////////////////////////////
class _NCMsSync
{
public:
	_NCMsSync(_NCMsMutex* pObj);
	~_NCMsSync();

private:
	private:
	_NCMsMutex* m_pObj;
};

/////////////////////////////////////////////////////////////////////////////
class _NCMsModuleLog :public _NCMsMutex
{
public:
	_NCMsModuleLog(DWORD dwModuleID, DWORD dwLevel, DWORD dwMaxFileSize);
	~_NCMsModuleLog();

	VOID   Trace(DWORD dwLevel, LPCTSTR lpszFormat, va_list arglist);
	DWORD  GetModuleID();
	DWORD  GetLevel();
	VOID   SetLevel(DWORD dwLevel);
	DWORD  GetMaxFileSize();
	VOID   SetMaxFileSize(DWORD dwMaxFileSize);
	VOID   SetFilePath(LPCTSTR lpszFilePath);
	VOID   GetFilePath(TCHAR lpszFilePath[MAX_PATH]);
	VOID   FlushFile();
	VOID   CloseFile();

	static VOID GetModulePath(TCHAR lpszModulePath[MAX_PATH]);

private:
	static VOID GetModuleName(TCHAR lpszModuleName[MAX_PATH]);
	static VOID GetLogDirectory(TCHAR lpszDirectoryName[MAX_PATH]);
	VOID   FormatLevelPrefix(DWORD dwLevel);
	HANDLE m_hFile;
	DWORD  m_dwModuleID;
	DWORD  m_dwLevel;
	DWORD  m_dwMaxFileSize;
	TCHAR  m_lpszFilePath[MAX_PATH];
	TCHAR  m_lpszLevelPrefix[4];
};

/////////////////////////////////////////////////////////////////////////////
class _NCMsLogUtility :public _NCMsMutex
{
public:
	enum
	{
		MAXFILESIZE_DEFAULT = 5 * 1024 * 1024,
		MAXFILESIZE_MIN     = 100 * 1024,
		MAXFILESIZE_MAX     =  10 * 1024 * 1024,
	};

	static _NCMsLogUtility* _stdcall GetInstance();
	static VOID DestroyInstance();

	VOID  _cdecl Error(LPCTSTR lpszFormat, ...);
	VOID  _cdecl Warning(LPCTSTR lpszFormat, ...);
	VOID  _cdecl Info(LPCTSTR lpszFormat, ...);
	VOID  _cdecl Debug(LPCTSTR lpszFormat, ...);
	VOID  _cdecl Trace(DWORD dwLevel, LPCTSTR lpszFormat, ...);

	
	VOID  _cdecl ErrorEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...);
	VOID  _cdecl WarningEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...);
	VOID  _cdecl InfoEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...);
	VOID  _cdecl DebugEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...);
	VOID  _cdecl TraceEx(DWORD dwModuleID, DWORD dwLevel, LPCTSTR lpszFormat, ...);
	VOID  SetLevelEx(DWORD dwModuleID, DWORD dwLevel);
	DWORD GetLevelEx(DWORD dwModuleID);
	VOID  SetMaxFileSizeEx(DWORD dwModuleID, DWORD dwMaxFileSize);
	DWORD GetMaxFileSizeEx(DWORD dwModuleID);
	VOID  SetFilePathEx(DWORD dwModuleID, LPCTSTR lpszFilePath);
	VOID  GetFilePathEx(DWORD dwModuleID, TCHAR lpszFilePath[MAX_PATH]);
	VOID  FlushFileEx(DWORD dwModuleID);
	VOID  CloseFileEx(DWORD dwModuleID);

private:
	_NCMsLogUtility();
	~_NCMsLogUtility();
	_NCMsModuleLog* FindModule(DWORD dwModuleID);
	VOID  Initialize();

	static _NCMsLogUtility* s_this;
	std::list<_NCMsModuleLog*> m_listModule;
	DWORD m_dwDefaultLevel;
	DWORD m_dwDefaultMaxFileSize;

};

/////////////////////////////////////////////////////////////////////////////
class _NCMsLogUtilityProxy
{
public:
	_NCMsLogUtilityProxy();
	~_NCMsLogUtilityProxy();

	//为了处理第三方EXE使用我们的DLL时,EXE中没有日志实例句柄的问题
	//本日志类无法解决第三方EXE使用二个我们的DLL的问题
#ifdef _WINDLL
	VOID SetOnlyMyself();
	BOOL m_bOnlyMyself;
#endif
};

/////////////////////////////////////////////////////////////////////////////
class _NCMsMethod
{
public:
	_NCMsMethod(LPCTSTR lpszMethod);
	_NCMsMethod(DWORD dwModuleID, LPCTSTR lpszMethod);
	~_NCMsMethod();
	
private:
	DWORD   m_dwModuleID;
	LPCTSTR m_lpszMethod;
};
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_MESSAGE_BUFLEN_MAX
#	define DEBUG_MESSAGE_BUFLEN_MAX		1024
#endif

static VOID ZM1_GrpDbg( TCHAR const *pGroupName, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX] = {0};
	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[%-8s]:"), pGroupName);
	va_list args;
	va_start( args, pMsg );
	_vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );
	NMsDebug(pTempBuffer);
	va_end( args );
}

static VOID ZM1_GrpDbgOutput( TCHAR const *pGroupName, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX] = {0};

	DWORD dwCurrentProcessID = GetCurrentProcessId();
	DWORD dwCurrentThreadID  = GetCurrentThreadId();

	SYSTEMTIME systemTime = { 0 };
	GetLocalTime(&systemTime);
	
	//int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[%-8s]:"), pGroupName);

	int len = _stprintf_s(pTempBuffer,
		DEBUG_MESSAGE_BUFLEN_MAX ,
		_T("[%4d]%4d-%02d-%02d %02d:%02d:%02d:%03d: [%4d][%s][%s]:"),
		dwCurrentProcessID,
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute,
		systemTime.wSecond,
		systemTime.wMilliseconds,
		dwCurrentThreadID,
		_T("DBG"),
		pGroupName);

	va_list args;
	va_start( args, pMsg );
	len += _vstprintf_s ( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );
	len += _stprintf_s ( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, _T("\r\n"));
	::OutputDebugString(pTempBuffer);

	va_end( args );
}

static VOID ZM1_GrpError( TCHAR const *pGroupName, TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX] = {0};
	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[%-8s]:"), pGroupName );
	va_list args;
	va_start( args, pMsg );
	 _vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );
	NMsError(pTempBuffer);
	va_end( args );
}

static VOID ZM1_GrpWarn( TCHAR const *pGroupName,TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX] = {0};
	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[%-8s]:"), pGroupName );
	va_list args;
	va_start( args, pMsg );
	 _vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );
	NMsWarning(pTempBuffer);
	va_end( args );
}
static VOID ZM1_GrpInfo( TCHAR const *pGroupName,TCHAR const *pMsg, ... )
{
	TCHAR pTempBuffer[DEBUG_MESSAGE_BUFLEN_MAX] = {0};
	int len = _stprintf_s( pTempBuffer, DEBUG_MESSAGE_BUFLEN_MAX, _T("[%-8s]:"), pGroupName );
	va_list args;
	va_start( args, pMsg );
	_vstprintf_s( pTempBuffer+len, DEBUG_MESSAGE_BUFLEN_MAX-len, pMsg, args );
	NMsInfo(pTempBuffer);
	va_end( args );
}

#endif