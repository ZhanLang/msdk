#pragma once




#include <atltime.h>
using ATL::CTime;
using ATL::CTimeSpan;

#include <vector>

#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) x
#endif
#define UNUSED_ALWAYS(x) x

#include "luahost/ILuaHost.h"
#include "luahost.h"
namespace winfunc
{
	typedef std::basic_string<TCHAR> TString;
	typedef std::vector<TString> TStringS;

	class CWinFileSystems
	{
	public:
		CWinFileSystems();		
		~CWinFileSystems();


		static BOOL BindToLua(ILuaVM* pLuaVm)
		{
			GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("winapi")
				+mluabind::Function("CreatePath", &CWinFileSystems::CreatePath)
				+mluabind::Function("CopyFile", &CWinFileSystems::CopyFile)
				+mluabind::Function("DeletePath",					&CWinFileSystems::DeletePath)

				//static bool DeletePathEx(LPCTSTR lpszPath, bool bRecursive = true, bool bOnlyDelDir = false, bool bErrContinue = true);
				+mluabind::Function("DeletePathEx",					&CWinFileSystems::DeletePathEx)
				+mluabind::Function("CopyPath", &CWinFileSystems::CopyPath)
				+mluabind::Function("GetFileSize", &CWinFileSystems::GetFileSize)
				+mluabind::Function("GetDiskFreeSpace", &CWinFileSystems::GetDiskFreeSpace)
				+mluabind::Function("DelFileEx", &CWinFileSystems::DelFileEx)
				+mluabind::Function("MoveFile", &CWinFileSystems::MoveFile)
				+mluabind::Function("MoveFileEx", &CWinFileSystems::MoveFileEx)
				
				+mluabind::Function("PathFileExists", &CWinFileSystems::PathFileExists)
				+mluabind::Function("RsLog", &CWinFileSystems::RsLog)
			
				// 		+mluabind::Class<Loki::SmartPtr<CWinFileSystems> >()
				// 		.SmartPtr<CWinFileSystems>()

				);

			return TRUE;
		}

		static bool CreatePath(LPCTSTR lpFileName);
		static bool CopyFile(
				LPCTSTR lpExistingFileName, // name of an existing file
				LPCTSTR lpNewFileName,      // name of new file
				bool bFailIfExists,   // operation if file exists
				bool bCreateDir = true,	  //  Create directory default
				DWORD dwFileAttributes = 0L//	new file's attributes
				);		
		// 删除路径(传入文件删除文件)
		static bool InternalDeletePath(LPCTSTR lpszPath, bool bRecursive = true, bool bOnlyDelDir = false, bool bErrContinue = true, bool bMoveAnyway = false);
		static bool DeletePath(LPCTSTR lpszPath, bool bRecursive = true, bool bOnlyDelDir = false, bool bErrContinue = true);
		static bool DeletePathEx(LPCTSTR lpszPath, bool bRecursive = true, bool bOnlyDelDir = false, bool bErrContinue = true);
		static bool CopyPath(LPCTSTR lpszSrcPath, LPCTSTR lpszDestPath, DWORD dwDestFileAttributes = 0L);		
		static ULONG GetFileSize(LPCTSTR lpszFileName);		
		static bool GetDiskFreeSpaceEx(LPCTSTR lpszDriver, ULONGLONG& ullFreeSize);
		static HRESULT DelFileEx(LPCTSTR lpszFileName); // 支持通配符

		static bool MoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);	
		static bool MoveFileEx(
			LPCTSTR lpExistingFileName,
			LPCTSTR lpNewFileName,
			DWORD dwFlags
			);
		
		//for lua
		static ULONGLONG GetDiskFreeSpace(LPCTSTR lpszDriver);
		
		static bool PathFileExists(LPCTSTR lpszPath);
		static void RsLog(DWORD dwLevel, LPCTSTR lpszScriptFileName, LPTSTR lpszInfo);
	};

	class CWinComDll
	{
	public:
		static BOOL BindToLua(ILuaVM* pLuaVM);
		static bool InstallCom(LPCTSTR lpszFileName, bool bInstall);
	};

////////////////////////////////////////////////////////////////////////////
// Local file searches



	class CWinFileFind
	{
	public:
		CWinFileFind();
		virtual ~CWinFileFind();

		static BOOL BindToLua(ILuaVM* pLuaVm)
		{
			GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("winfunc")
				+mluabind::Class<winfunc::CWinFileFind>("WinFileFind")
				.Constructor()
				.Method("GetLength", (ULONGLONG (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetLength)
				.Method("GetFileName", (TString (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetFileName)
				.Method("GetFilePath", (TString (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetFilePath)
				.Method("GetFileTitle", (TString (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetFileTitle)
				.Method("GetFileExt", (TString (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetFileExt)

				.Method("GetFileURL", (TString (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetFileURL)
				.Method("GetRoot", (TString (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::GetRoot)

				.Method("MatchesMask",	(bool (winfunc::CWinFileFind::*) (DWORD) )&winfunc::CWinFileFind::MatchesMask)
				.Method("IsDots",		(bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsDots)
				.Method("IsReadOnly",	(bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsReadOnly)
				.Method("IsDirectory", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsDirectory)
				.Method("IsCompressed", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsCompressed)
				.Method("IsSystem", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsSystem)
				.Method("IsHidden", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsHidden)
				.Method("IsTemporary", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsTemporary)
				.Method("IsNormal", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::IsNormal)

				.Method("Close", (VOID (winfunc::CWinFileFind::*) (LPCTSTR, DWORD) )&winfunc::CWinFileFind::Close)
				.Method("FindFile", (bool (winfunc::CWinFileFind::*) (LPCTSTR, DWORD) )&winfunc::CWinFileFind::FindFile)
				.Method("FindNextFile", (bool (winfunc::CWinFileFind::*) () )&winfunc::CWinFileFind::FindNextFile)
				);	

			return TRUE;
		}

		// Attributes
	public:
		ULONGLONG GetLength() const;
		virtual TString GetFileName() const;
		virtual TString GetFilePath() const;
		virtual TString GetFileTitle() const;
		virtual TString GetFileExt()const;
		virtual TString GetFileURL() const;
		virtual TString GetRoot() const;

		virtual bool GetLastWriteTime(FILETIME* pTimeStamp) const;
		virtual bool GetLastAccessTime(FILETIME* pTimeStamp) const;
		virtual bool GetCreationTime(FILETIME* pTimeStamp) const;
		virtual bool GetLastWriteTime(CTime& refTime) const;
		virtual bool GetLastAccessTime(CTime& refTime) const;
		virtual bool GetCreationTime(CTime& refTime) const;

		virtual bool MatchesMask(DWORD dwMask) const;

		virtual bool IsDots() const;
		// these aren't virtual because they all use MatchesMask(), which is
		bool IsReadOnly() const;
		bool IsDirectory() const;
		bool IsCompressed() const;
		bool IsSystem() const;
		bool IsHidden() const;
		bool IsTemporary() const;
		bool IsNormal() const;
		bool IsArchived() const;

		// Operations
		void Close();
		virtual bool FindFile(LPCTSTR pstrName = NULL, DWORD dwUnused = 0);
		virtual bool FindNextFile();

	protected:
		virtual void CloseContext();

		// Implementation
	protected:
		void* m_pFoundInfo;
		void* m_pNextInfo;
		HANDLE m_hContext;
		TString m_strRoot;
		TCHAR m_chDirSeparator;     // not '\\' for Internet classes
	};



};//namespace winfunc