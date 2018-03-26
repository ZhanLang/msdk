#pragma once

#include "stdafx.h"
#include "LogTool.h"
#include <fcntl.h>
#include <io.h>

#pragma warning(disable:4996)

#ifndef fstring
#	ifdef _UNICODE
#		define fstring std::wstring
#	else
#		define fstring std::string
#	endif
#endif


#define MSLUMESSAGE_MAX 1024
#define MSLUMESSAGE_LEN 1024 - 40

#define _NSYN_OBJ() _NCMsSync _obj_NCMsSync(this)
#define _NRANGE(xValue, xMin, xMax) \
	((xValue) = min(max((xValue), (xMin)), (xMax)))
/////////////////////////////////////////////////////////////////////////////
_NCMsMutex::_NCMsMutex()
{
    InitializeCriticalSection(&m_csSync);
}

/////////////////////////////////////////////////////////////////////////////
_NCMsMutex::~_NCMsMutex()
{
    DeleteCriticalSection(&m_csSync);
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsMutex::Acquire()
{
    EnterCriticalSection(&m_csSync);
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsMutex::Release()
{
    LeaveCriticalSection(&m_csSync);
}

/////////////////////////////////////////////////////////////////////////////
_NCMsSync::_NCMsSync(_NCMsMutex *pObj)
    : m_pObj(pObj)
{
    //	_ASSERT(NULL != pObj);
    m_pObj->Acquire();
}

/////////////////////////////////////////////////////////////////////////////
_NCMsSync::~_NCMsSync()
{
    m_pObj->Release();
}


/////////////////////////////////////////////////////////////////////////////
_NCMsModuleLog::_NCMsModuleLog(DWORD dwModuleID, DWORD dwLevel, DWORD dwMaxFileSize)
    : m_hFile(INVALID_HANDLE_VALUE),
      m_dwModuleID(dwModuleID),
      m_dwLevel(dwLevel),
      m_dwMaxFileSize(dwMaxFileSize)
{
    GetLogDirectory(m_lpszFilePath);

    if(_tcscmp(m_lpszFilePath, _T("")) == 0)
        return;

    TCHAR *pFind = _tcsrchr(m_lpszFilePath, _T('.'));
    if (NULL != pFind)
    {
        *pFind = _T('\0');
    }
    _tcsncat(m_lpszFilePath, _T(".log"), MAX_PATH - 1);
}

/////////////////////////////////////////////////////////////////////////////
_NCMsModuleLog::~_NCMsModuleLog()
{
    CloseFile();
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::Trace(DWORD dwLevel, LPCTSTR lpszFormat, va_list arglist)
{
    _NSYN_OBJ();
    BOOL bWriteFile = FALSE;
    if(_tcscmp(m_lpszFilePath, _T("")) == 0)
    {
#ifdef _DEBUG
        goto Flag;
#endif
        return;
    }

    bWriteFile = TRUE;
    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        m_hFile = CreateFile(m_lpszFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE != m_hFile)
        {
            SetFilePointer(m_hFile, 0, NULL, FILE_END);
        }
        else
        {
            return;
        }
    }

    DWORD dwFileSize = GetFileSize(m_hFile, NULL);
    if (dwFileSize > m_dwMaxFileSize)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        TCHAR lpszBackFilePath[MAX_PATH];
        _tcsncpy(lpszBackFilePath, m_lpszFilePath, MAX_PATH - 1);
        TCHAR *pFind = _tcsrchr(lpszBackFilePath, _T('.'));
        if (NULL != pFind)
        {
            *pFind = _T('\0');
        }
        _tcsncat(lpszBackFilePath, _T(".wtx"), MAX_PATH - 1);

        CopyFile(m_lpszFilePath, lpszBackFilePath, FALSE);
        DeleteFile(m_lpszFilePath);
        m_hFile = CreateFile(m_lpszFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE != m_hFile)
        {
            SetFilePointer(m_hFile, 0, NULL, FILE_END);
        }
        else
        {
            return;
        }
    }
#ifdef _DEBUG
Flag:
#endif
    FormatLevelPrefix(dwLevel);

    TCHAR lpszMessage[MSLUMESSAGE_LEN] = { 0 };
    _vsntprintf(lpszMessage, MSLUMESSAGE_LEN - 1, lpszFormat, arglist);
    lpszMessage[MSLUMESSAGE_LEN - 1] = _T('\0');

    TCHAR lpszBuffer[MSLUMESSAGE_MAX];
    DWORD dwCurrentProcessID = GetCurrentProcessId();
    DWORD dwCurrentThreadID  = GetCurrentThreadId();
    SYSTEMTIME systemTime = { 0 };
    GetLocalTime(&systemTime);
    INT iLen = _sntprintf(lpszBuffer,
                          MSLUMESSAGE_MAX - 3 * sizeof(TCHAR),
                          _T("[%4d]%4d-%02d-%02d %02d:%02d:%02d:%03d: [%4d][%s]%s\r\n"),
                          dwCurrentProcessID,
                          systemTime.wYear,
                          systemTime.wMonth,
                          systemTime.wDay,
                          systemTime.wHour,
                          systemTime.wMinute,
                          systemTime.wSecond,
                          systemTime.wMilliseconds,
                          dwCurrentThreadID,
                          m_lpszLevelPrefix,
                          lpszMessage);
    if (iLen == MSLUMESSAGE_MAX - 2)
    {
        iLen += 1;
        lpszBuffer[MSLUMESSAGE_MAX] = _T('\0');
    }
    else if (iLen < 0)
    {
        lpszBuffer[MSLUMESSAGE_MAX - 3] = _T('\r');
        lpszBuffer[MSLUMESSAGE_MAX - 2] = _T('\n');
        lpszBuffer[MSLUMESSAGE_MAX - 1] = _T('\0');
        iLen = MSLUMESSAGE_MAX;
    }

    DWORD dwBytesWritten;

    char szBuffer[MSLUMESSAGE_MAX] = {0};
#ifdef _UNICODE
    WideCharToMultiByte( CP_ACP, 0, lpszBuffer, -1, szBuffer, MSLUMESSAGE_MAX, NULL, NULL );
#else
    memcpy( szBuffer, lpszBuffer, MSLUMESSAGE_MAX );
#endif

    if(bWriteFile)
    {
        WriteFile(m_hFile, szBuffer, strlen(szBuffer), &dwBytesWritten, NULL);
    }

    //控制台输出，不限制编译类型。
    OutputDebugString(lpszBuffer);
#ifdef _DEBUG

    //std::cout <<szBuffer;
#endif

}

/////////////////////////////////////////////////////////////////////////////
DWORD _NCMsModuleLog::GetModuleID()
{
    return m_dwModuleID;
}

/////////////////////////////////////////////////////////////////////////////
DWORD _NCMsModuleLog::GetLevel()
{
    return m_dwLevel;
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::SetLevel(DWORD dwLevel)
{
    m_dwLevel = dwLevel;
}

/////////////////////////////////////////////////////////////////////////////
DWORD _NCMsModuleLog::GetMaxFileSize()
{
    return m_dwMaxFileSize;
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::SetMaxFileSize(DWORD dwMaxFileSize)
{
    m_dwMaxFileSize = dwMaxFileSize;
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::SetFilePath(LPCTSTR lpszFilePath)
{
    _NSYN_OBJ();
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    _tcsncpy(m_lpszFilePath, lpszFilePath, MAX_PATH - 1);
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::GetFilePath(TCHAR lpszFilePath[MAX_PATH])
{
    _NSYN_OBJ();
    _tcsncpy(lpszFilePath, m_lpszFilePath, MAX_PATH - 1);
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::FlushFile()
{
    _NSYN_OBJ();
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        FlushFileBuffers(m_hFile);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::CloseFile()
{
    _NSYN_OBJ();
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}
/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::GetLogDirectory(TCHAR lpszDirectoryName[MAX_PATH])
{

    TCHAR lpszModulePath[MAX_PATH];
    GetModulePath(lpszModulePath);
    fstring strConfigFile = lpszModulePath;
    strConfigFile += CFG_FILE_NAME;

    lpszDirectoryName[0] = _T('\0');
    ::GetPrivateProfileString(_T("Log"), _T("OutFilePath"), _T(""), lpszDirectoryName, MAX_PATH, strConfigFile.c_str());
    if(_tcscmp(lpszDirectoryName, _T("")) == 0)
    {
#ifdef LOG_OUTPUT_DIR
        fstring strDefaultPath = LOG_OUTPUT_DIR;
#else
        fstring strDefaultPath = lpszModulePath;
#endif

        strDefaultPath += LOG_FILE_NAME;
        _tcsncpy(lpszDirectoryName, strDefaultPath.c_str(), MAX_PATH);
    }
    else if(_tcscmp(lpszDirectoryName, _T("none")) == 0)//没有日志文件输出
    {
        lpszDirectoryName[0] = _T('\0');
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::GetModuleName(TCHAR lpszModuleName[MAX_PATH])
{
    TCHAR lpszTempModuleName[MAX_PATH];
    if(::GetModuleFileName(NULL, lpszTempModuleName, MAX_PATH) > 0)
    {
        TCHAR *pFind = _tcsrchr(lpszTempModuleName, _T('\\'));
        if (NULL == pFind)
        {
            pFind = lpszTempModuleName;
        }
        else
        {
            pFind++;
        }

        _tcsncpy(lpszModuleName, pFind, MAX_PATH);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::GetModulePath(TCHAR lpszModulePath[MAX_PATH])
{
    TCHAR lpszTempModuleName[MAX_PATH];
    if(::GetModuleFileName(NULL, lpszTempModuleName, MAX_PATH) > 0)
    {
        TCHAR *pFind = _tcsrchr(lpszTempModuleName, _T('\\'));
        if (NULL == pFind)
        {
            pFind = lpszTempModuleName;
        }
        else
        {
            pFind++;
        }

        *pFind = _T('\0');
        _tcsncpy(lpszModulePath, lpszTempModuleName, MAX_PATH);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsModuleLog::FormatLevelPrefix(DWORD dwLevel)
{
    switch (dwLevel)
    {
    case LEVEL_ERROR:
        _tcscpy(m_lpszLevelPrefix, _T("ERR"));
        break;
    case LEVEL_WARNING:
        _tcscpy(m_lpszLevelPrefix, _T("WAR"));
        break;
    case LEVEL_INFO:
        _tcscpy(m_lpszLevelPrefix, _T("INF"));
        break;
    case LEVEL_DEBUG:
        _tcscpy(m_lpszLevelPrefix, _T("DBG"));
        break;
    default:
        _sntprintf(m_lpszLevelPrefix, 3, _T("%d"), m_dwLevel);
        m_lpszLevelPrefix[3] = _T('\0');
    }
}

/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtilityProxy::_NCMsLogUtilityProxy()
{
#ifdef _WINDLL
    m_bOnlyMyself = FALSE;
#else
    _NCMsLogUtility::GetInstance();
#endif
}

/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtilityProxy::~_NCMsLogUtilityProxy()
{
#ifdef _WINDLL
    if (m_bOnlyMyself)
    {
        _NCMsLogUtility::DestroyInstance();
    }
#else
    _NCMsLogUtility::DestroyInstance();
#endif
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _WINDLL
VOID _NCMsLogUtilityProxy::SetOnlyMyself()
{
    m_bOnlyMyself = TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtilityProxy g_objMsLogUtilityProxy;


/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtility *_NCMsLogUtility::s_this = NULL;

/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtility *_NCMsLogUtility::GetInstance()
{
    if (NULL == s_this)
    {
        //在DllMain函数中为了保险起见不推荐调用本类打日志，是不是会出问题跟操
        //作系统的装载DLL的顺序有关，具体得根据不同得操作系统调试一下
#ifndef _WINDLL
        s_this = new _NCMsLogUtility();
        if (NULL != s_this)
        {
            s_this->Initialize();
        }
#else
        typedef _NCMsLogUtility* (_stdcall * GETRSLOGUTILITYINSTANCE)();
        GETRSLOGUTILITYINSTANCE pfnGetMsLogUtilityInstance = NULL;
        pfnGetMsLogUtilityInstance = (GETRSLOGUTILITYINSTANCE)GetProcAddress(GetModuleHandle(NULL), "?GetInstance@_NCMsLogUtility@@SGPAV1@XZ");
        if (NULL != pfnGetMsLogUtilityInstance)
        {
            s_this = pfnGetMsLogUtilityInstance();
        }
        else
        {
            s_this = new _NCMsLogUtility();
            if (NULL != s_this)
            {
                g_objMsLogUtilityProxy.SetOnlyMyself();
            }
        }
#endif
    }

    return s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::DestroyInstance()
{
    delete s_this;
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::Error(LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(MODULE_CURRENTPROCESS);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_ERROR)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_ERROR, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::Warning(LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(MODULE_CURRENTPROCESS);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_WARNING)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_WARNING, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::Info(LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(MODULE_CURRENTPROCESS);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_INFO)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_INFO, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::Debug(LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(MODULE_CURRENTPROCESS);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_DEBUG)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_DEBUG, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::Trace(DWORD dwLevel, LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(MODULE_CURRENTPROCESS);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= dwLevel)
    {
        _NRANGE(dwLevel, LEVEL_NONE, LEVEL_MAX);
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(dwLevel, lpszFormat, arglist);
        va_end(arglist);
    }
}


/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::ErrorEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_ERROR)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_ERROR, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::WarningEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_WARNING)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_WARNING, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::InfoEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_INFO)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_INFO, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::DebugEx(DWORD dwModuleID, LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= LEVEL_DEBUG)
    {
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(LEVEL_DEBUG, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::TraceEx(DWORD dwModuleID, DWORD dwLevel, LPCTSTR lpszFormat, ...)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL == pMsModuleLog)
    {
        return;
    }

    if (pMsModuleLog->GetLevel() >= dwLevel)
    {
        _NRANGE(dwLevel, LEVEL_NONE, LEVEL_MAX);
        va_list arglist;
        va_start(arglist, lpszFormat);
        pMsModuleLog->Trace(dwLevel, lpszFormat, arglist);
        va_end(arglist);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::SetLevelEx(DWORD dwModuleID, DWORD dwLevel)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        _NRANGE(dwLevel, LEVEL_NONE, LEVEL_MAX);
        pMsModuleLog->SetLevel(dwLevel);
    }
}

/////////////////////////////////////////////////////////////////////////////
DWORD _NCMsLogUtility::GetLevelEx(DWORD dwModuleID)
{
    if (NULL == this)
    {
        return 0;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        return pMsModuleLog->GetLevel();
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::SetMaxFileSizeEx(DWORD dwModuleID, DWORD dwMaxFileSize)
{
    if (NULL == this)
    {
        return;
    }


    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        _NRANGE(dwMaxFileSize, MAXFILESIZE_MIN, MAXFILESIZE_MAX);
        pMsModuleLog->SetMaxFileSize(dwMaxFileSize);
    }
}

/////////////////////////////////////////////////////////////////////////////
DWORD _NCMsLogUtility::GetMaxFileSizeEx(DWORD dwModuleID)
{
    if (NULL == this)
    {
        return 0;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        return pMsModuleLog->GetMaxFileSize();
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::SetFilePathEx(DWORD dwModuleID, LPCTSTR lpszFilePath)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        pMsModuleLog->SetFilePath(lpszFilePath);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::GetFilePathEx(DWORD dwModuleID, TCHAR lpszFilePath[MAX_PATH])
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        pMsModuleLog->GetFilePath(lpszFilePath);
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::FlushFileEx(DWORD dwModuleID)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        pMsModuleLog->FlushFile();
    }
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::CloseFileEx(DWORD dwModuleID)
{
    if (NULL == this)
    {
        return;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(dwModuleID);
    if (NULL != pMsModuleLog)
    {
        pMsModuleLog->CloseFile();
    }
}

/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtility::_NCMsLogUtility()
    : m_dwDefaultLevel(LEVEL_DEFAULT),
      m_dwDefaultMaxFileSize(MAXFILESIZE_DEFAULT)
{
    TCHAR lpszModulePath[MAX_PATH];
    _NCMsModuleLog::GetModulePath(lpszModulePath);
    fstring strConfigPath = lpszModulePath;
    strConfigPath += CFG_FILE_NAME;

    m_dwDefaultLevel = ::GetPrivateProfileInt(_T("Log"), _T("LogLevel"), LEVEL_DEFAULT, strConfigPath.c_str());
    m_dwDefaultMaxFileSize = ::GetPrivateProfileInt(_T("Log"), _T("MaxFileSize"), MAXFILESIZE_DEFAULT, strConfigPath.c_str());
    _NCMsModuleLog *pMsModuleLog = new _NCMsModuleLog(MODULE_CURRENTPROCESS, m_dwDefaultLevel, m_dwDefaultMaxFileSize);
    if (NULL != pMsModuleLog)
    {
        m_listModule.push_back(pMsModuleLog);
    }
}

/////////////////////////////////////////////////////////////////////////////
_NCMsLogUtility::~_NCMsLogUtility()
{
    _NSYN_OBJ();
    std::list<_NCMsModuleLog *>::iterator it;
    for (it = m_listModule.begin(); it != m_listModule.end(); it++)
    {
        (*it)->CloseFile();
        delete (*it);
    }
    m_listModule.clear();

    s_this = NULL;
}

/////////////////////////////////////////////////////////////////////////////
_NCMsModuleLog *_NCMsLogUtility::FindModule(DWORD dwModuleID)
{
    _NSYN_OBJ();
    std::list<_NCMsModuleLog *>::iterator it;
    for (it = m_listModule.begin(); it != m_listModule.end(); it++)
    {
        if ((*it)->GetModuleID() == dwModuleID)
        {
            return (*it);
        }
    }

    _NCMsModuleLog *pMsModuleLog = new _NCMsModuleLog(dwModuleID, m_dwDefaultLevel, m_dwDefaultMaxFileSize);
    if (NULL != pMsModuleLog)
    {
        m_listModule.push_back(pMsModuleLog);
    }

    return pMsModuleLog;
}

/////////////////////////////////////////////////////////////////////////////
VOID _NCMsLogUtility::Initialize()
{

    //注释掉这些，免得级别功能不能生效
    // #ifdef _DEBUG
    // 	m_dwDefaultLevel = LEVEL_DEBUG;
    // #else
    // 	m_dwDefaultLevel = LEVEL_DEFAULT;
    // #endif

    DWORD dwValue = m_dwDefaultLevel;

    {
        _NRANGE(dwValue, LEVEL_NONE, LEVEL_MAX);
        m_dwDefaultLevel = dwValue;
    }

#ifdef _DEBUG
    m_dwDefaultMaxFileSize = MAXFILESIZE_DEFAULT * 2;
#else
   // m_dwDefaultMaxFileSize = MAXFILESIZE_DEFAULT;
#endif

    dwValue = m_dwDefaultMaxFileSize;

    {
        _NRANGE(dwValue, MAXFILESIZE_MIN, MAXFILESIZE_MAX);
        m_dwDefaultMaxFileSize = dwValue;
    }

    _NCMsModuleLog *pMsModuleLog = FindModule(MODULE_CURRENTPROCESS);
    if (NULL != pMsModuleLog)
    {
        pMsModuleLog->SetLevel(m_dwDefaultLevel);
        pMsModuleLog->SetMaxFileSize(m_dwDefaultMaxFileSize);
    }
}

/////////////////////////////////////////////////////////////////////////////
_NCMsMethod::_NCMsMethod(LPCTSTR lpszMethod)
    : m_dwModuleID(0),
      m_lpszMethod(lpszMethod)
{
    //	_ASSERT(NULL != m_lpszMethod);
    NMsDebug(_T("%s() Enter<<<<<<<<<<<<<<<<<<"), m_lpszMethod);
}

/////////////////////////////////////////////////////////////////////////////
_NCMsMethod::_NCMsMethod(DWORD dwModuleID, LPCTSTR lpszMethod)
    : m_dwModuleID(dwModuleID),
      m_lpszMethod(lpszMethod)
{
    //	_ASSERT(NULL != lpszMethod);
    NMsDebugEx(m_dwModuleID, _T("%s() Enter<<<<<<<<<<<<<<<<<<"), m_lpszMethod);
}

/////////////////////////////////////////////////////////////////////////////
_NCMsMethod::~_NCMsMethod()
{
    //	_ASSERT(NULL != m_lpszMethod);
    if (0 == m_dwModuleID)
    {
        NMsDebug(_T("%s() Leave>>>>>>>>>>>>>>>>>>"), m_lpszMethod);
    }
    else
    {
        NMsDebugEx(m_dwModuleID, _T("%s() Leave>>>>>>>>>>>>>>>>>>"), m_lpszMethod);
    }
}
