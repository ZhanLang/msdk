/*
    Copyright (C) =USTC= Fu Li

    Author   :  Fu Li
    Create   :  2008-6-27
    Home     :  http://www.phoxo.com
    Mail     :  crazybitwps@hotmail.com

    This file is part of UIStone

    The code distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Redistribution and use the source code, with or without modification,
    must retain the above copyright.
*/
#pragma once
#include <shlwapi.h>
#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "Version.lib")

//-------------------------------------------------------------------------------------
/**
    File helper.
*/
class FCFileEx
{
public:
    /**
        Get folder that module locate, with backslash('\') append \n
        hModule - NULL to get current process folder.
    */
    static CString GetModuleFolder (HMODULE hModule=NULL)
    {
        TCHAR   s[MAX_PATH] = {0} ;
        GetModuleFileName (hModule, s, MAX_PATH) ;
        PathRemoveFileSpec (s) ;
        PathAddBackslash (s) ;
        return s ;
    }

    /// Get folder that file locate, with backslash('\') append.
    static CString GetFileFolder (LPCTSTR strFilename)
    {
        TCHAR   s[MAX_PATH] = {0} ;
        lstrcpyn (s, strFilename, MAX_PATH) ;
        PathRemoveFileSpec (s) ;
        PathAddBackslash (s) ;
        return s ;
    }

    /// Get file's size.
    static INT64 GetSize (LPCTSTR strFilename)
    {
        WIN32_FILE_ATTRIBUTE_DATA   fd ;
        if (GetFileAttributesEx (strFilename, GetFileExInfoStandard, &fd))
        {
            LARGE_INTEGER   n ;
            n.LowPart = fd.nFileSizeLow ;
            n.HighPart = (LONG)fd.nFileSizeHigh ;
            return n.QuadPart ;
        }
        return 0 ;
    }

    /// Get version of PE file.
    static CString GetFileVersion (LPCTSTR sFilename)
    {
        CString   sVer ;

        DWORD   dwSize = GetFileVersionInfoSize(sFilename, 0) ;
        if (dwSize)
        {
            std::vector<BYTE>   buf (dwSize) ;
            GetFileVersionInfo (sFilename, 0, dwSize, &buf[0]) ;

            VS_FIXEDFILEINFO   * pInfo ;
            UINT   nInfoLen ;

            if (VerQueryValue(&buf[0], _T("\\"), (void**)&pInfo, &nInfoLen))
            {
                sVer.Format(_T("%d.%d.%d.%d"), (int)HIWORD(pInfo->dwFileVersionMS), (int)LOWORD(pInfo->dwFileVersionMS),
                                               (int)HIWORD(pInfo->dwFileVersionLS), (int)LOWORD(pInfo->dwFileVersionLS)) ;
            }
        }
        return sVer ;
    }

    /// Get file's extension name, not including leading period.
    static CString GetExtName (LPCTSTR strFilename)
    {
        CString   s(strFilename) ;
        int   n = s.ReverseFind('.') ;
        if (n != -1)
        {
            return s.Mid(n+1) ;
        }
        return _T("") ;
    }

    /// Read file to memory.
    static BOOL Read (LPCTSTR strFilename, std::vector<BYTE>& file_data)
    {
        file_data.clear() ;

        BOOL     b = FALSE ;
        HANDLE   f = CreateFile (strFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) ;
        if (f != INVALID_HANDLE_VALUE)
        {
            DWORD   nLength = ::GetFileSize(f, NULL) ;
            if (nLength)
            {
                file_data.resize (nLength) ;

                DWORD   dwRead ;
                ::ReadFile (f, &file_data[0], nLength, &dwRead, NULL) ;
                b = (dwRead == nLength) ;
            }
            else
            {
                b = TRUE ;
            }
            CloseHandle (f) ;
        }
        assert(b) ;
        return b ;
    }

    /// Write buffer to file, if file already exist, it will be delete before write.
    static BOOL Write (LPCTSTR strFilename, const void* p, int nLength)
    {
        SetFileAttributes (strFilename, FILE_ATTRIBUTE_NORMAL) ;

        BOOL     b = FALSE ;
        HANDLE   f = CreateFile (strFilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;
        if (f != INVALID_HANDLE_VALUE)
        {
            DWORD   nWrite ;
            ::WriteFile (f, p, nLength, &nWrite, NULL) ;
            b = ((int)nWrite == nLength) ;
            CloseHandle (f) ;
        }
        assert(b) ;
        return b ;
    }

    /// Get a temp file path.
    static CString QueryTempFilePath()
    {
        TCHAR   szTmpPath[MAX_PATH],
                szFilename[MAX_PATH] ;
        ::GetTempPath (MAX_PATH, szTmpPath) ;
        ::GetTempFileName (szTmpPath, _T("t"), 0, szFilename) ;
        return szFilename ;
    }

    /// @name Read/Write INI file.
    //@{
    /// Read string key from ini file, return FALSE if key doesn't exist.
    static BOOL INIRead (LPCTSTR strFilename, LPCTSTR strKey, CString& s, LPCTSTR strSection=_T("app"))
    {
        TCHAR   b[512] ;
        DWORD   dwWrite = GetPrivateProfileString (strSection, strKey, _T("\n"), b, 512, strFilename) ;
        if ((b[0] == '\n') && (b[1] == 0))
            return FALSE ;

        if (dwWrite > (512 - 8))
        {
            std::vector<TCHAR>   buf (4096) ;
            GetPrivateProfileString (strSection, strKey, _T("\n"), &buf[0], (DWORD)buf.size(), strFilename) ;
            s = &buf[0] ;
        }
        else
        {
            s = b ;
        }
        return TRUE ;
    }

    /// Read int key from ini file, return FALSE if key doesn't exist.
    static BOOL INIRead (LPCTSTR strFilename, LPCTSTR strKey, INT64& n, LPCTSTR strSection=_T("app"))
    {
        TCHAR   b[32] ;
        GetPrivateProfileString (strSection, strKey, _T("\n"), b, 32, strFilename) ;
        if ((b[0] == '\n') && (b[1] == 0))
            return FALSE ;

        n = _ttoi64(b) ;
        return TRUE ;
    }

    /// Write a int key to ini file.
    static BOOL INIWrite (LPCTSTR strFilename, LPCTSTR strKey, INT64 n, LPCTSTR strSection=_T("app"))
    {
        CString   s ;
        s.Format(_T("%I64d"), n) ;
        return INIWrite (strFilename, strKey, s, strSection) ;
    }

    /// Write a string key to ini file.
    static BOOL INIWrite (LPCTSTR strFilename, LPCTSTR strKey, LPCTSTR strValue, LPCTSTR strSection=_T("app"))
    {
        return WritePrivateProfileString (strSection, strKey, strValue, strFilename) ;
    }
    //@}
};
