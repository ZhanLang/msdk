
#ifndef _MEMMAPFILE_INCLUDE_H_
#define _MEMMAPFILE_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif

#include <windows.h>
#include <stdlib.h>	// for _MAX_PATH
#include "util/globalevent.h"
#include "util/listsessions.h"
#include <time.h>//for time
namespace msdk {

	class CMemMapFile
	{
	public:
		CMemMapFile(DWORD dwGrowSize = 0)
		{
			m_hFile = INVALID_HANDLE_VALUE;
			m_hMapping = NULL;
			m_bReadOnly = TRUE;
			m_bAppendNull = FALSE;
			m_lpData = NULL;
			m_bOpen = FALSE;	
			m_dwLength = 0;
			m_dwGrowSize = dwGrowSize;
			m_bReset = FALSE;

			ZeroMemory(m_szMappingName, sizeof(m_szMappingName));
			ZeroMemory(m_szMappingNameVista, sizeof(m_szMappingNameVista));	

			//m_IsNt = !(GetVersion() & 0x80000000);
			DWORD dwVersion					= GetVersion();
			DWORD dwWindowsMajorVersion		=  (DWORD)(LOBYTE(LOWORD(dwVersion)));

			m_bIs9X							= (dwVersion & 0x80000000)?TRUE:FALSE;
			m_IsNt							= (!m_bIs9X && dwWindowsMajorVersion<5)?TRUE:FALSE;
			m_IsVista = IsVista();	
		}
		~CMemMapFile()
		{
			UnMap();
		}

		//-----------------------------------------------------------------------------
		// 目的 : 考虑到将来会出现多个进程写入同一个日志文件的情况，那么需要在这里做互斥
		//			现在暂时没必要做 ;-)
		// 输入 : szFilename - 
		//		  bReadOnly - 
		//		  dwReservedSize - 
		//		  bNamed - 
		//		  dwShareMode - 
		//		  bAppendNull - 
		//		  bGrowable - 
		// 输出 : 返回TRUE是成功，FALSE是失败 - 
		//-----------------------------------------------------------------------------
		BOOL    MapFile(LPCTSTR szFilename, BOOL bReadOnly = FALSE, 
			DWORD dwReservedSize = 0, BOOL bNamed = FALSE, 
			DWORD dwShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE, 
			BOOL bAppendNull = FALSE, BOOL bGrowable = FALSE )
		{
			DWORD dwFileSizeHigh=0;

			// 默认为增量写模式
			m_bReset = FALSE;

			// 只读检查
			m_bReadOnly = bReadOnly;
			DWORD dwDesiredFileAccess = GENERIC_READ;
			if (!m_bReadOnly)
				dwDesiredFileAccess |= GENERIC_WRITE;

			// 是否需要在文件结尾部分加 2字节的0（为了支持宽字符的0）
			m_bAppendNull = bAppendNull;

			// 打开真正的文件
			m_hFile = CreateFile(szFilename, dwDesiredFileAccess, dwShareMode, NULL, 
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (m_hFile == INVALID_HANDLE_VALUE)
			{
				ErrorMsg(TEXT("Failed in call to CreateFile, GetLastError returned %d\n"), GetLastError());		
				UnMap();
				return FALSE;
			}

			// 允许增长文件，目前不支持
			if (bGrowable)
			{
			}

			// 获取文件尺寸	
			m_dwLength = GetFileSize(m_hFile, &dwFileSizeHigh);
			if (m_dwLength == 0xFFFFFFFF)
			{		
				ErrorMsg(TEXT("Failed in call to GetFileSize, GetLastError returned %d\n"), GetLastError());		
				UnMap();
				return FALSE;
			}

			// 4G文件太大了我们不支持
			if (dwFileSizeHigh)
			{
				ErrorMsg(TEXT("File size is greater than 4GB, Memory mapping this file size will not work until CMemMapFile supports Win64 !!\n"));		
				UnMap();
				return FALSE;
			}

			// 如果不是只读的打开模式
			if(!m_bReadOnly && m_dwLength!=dwReservedSize)
			{		
				m_dwLength = dwReservedSize;
			}

			/*
			if(!m_bReadOnly && m_dwLength!=dwReservedSize)
			{
			if(0!=m_dwLength)
			{			
			if(!BackupFile(szFilename))
			{
			UnMap();
			return FALSE;
			}
			CloseHandle(m_hFile);

			m_hFile = CreateFile(szFilename, dwDesiredFileAccess, dwShareMode, NULL, 
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (m_hFile == INVALID_HANDLE_VALUE)
			{
			ErrorMsg(TEXT("Failed in call to ReCreateFile, GetLastError returned %d\n"), GetLastError());
			UnMap();
			return FALSE;
			}
			m_bReset = TRUE;
			}		
			m_dwLength = dwReservedSize;
			}
			*/

			// 长度为零时 CreateFileMapping 在95/98上有问题
			if (dwFileSizeHigh == 0 && m_dwLength == 0)
			{
				ErrorMsg(TEXT("File size is 0, not attempting to memory map the file\n"));	
				UnMap();
				return FALSE;
			}

			// 创建并记录文件名
			CreateMappingName(szFilename, bNamed);


			// 开始映射到内存
			return MapHandle(m_hFile);
		}

		BOOL    MapMemory(LPCTSTR sName, DWORD dwBytes, BOOL bReadOnly = FALSE)
		{
			m_dwLength = dwBytes;
			m_bReadOnly = bReadOnly;	
			m_dwShareMode = (!m_bReadOnly) ? FILE_MAP_WRITE : FILE_MAP_READ;
			m_bAppendNull = FALSE;
			CreateMappingName(sName, TRUE);

			if(m_IsVista)
			{		
				// 		TCHAR	szMappingName[_MAX_PATH]={0};		
				// 		for(int i=0;i<16;i++)//暂定遍历16个session
				// 		{
				// 			wsprintf(szMappingName, "Session\\%d\\%s", i, m_szMappingNameVista);
				// 			m_hMapping = OpenFileMapping(m_dwShareMode,	0, szMappingName);
				// 			if (m_hMapping != NULL)
				// 			{				
				// 				m_lpData = MapViewOfFile(m_hMapping, m_dwShareMode, 0, 0, m_dwLength);
				// 				if(m_lpData != NULL)
				// 					return TRUE;
				// 			}
				// 		}

				TCHAR	szMappingName[_MAX_PATH]={0};
				m_EnumSessions.ListSessionID();
				for(DWORD i=0;i<m_EnumSessions.m_listSid.size();i++)
				{
					_stprintf_s (szMappingName, _T("Session\\%d\\%s"), m_EnumSessions.m_listSid[i], m_szMappingNameVista);
					m_hMapping = OpenFileMapping(m_dwShareMode,	0, szMappingName);
					if (m_hMapping != NULL)
					{				
						m_lpData = MapViewOfFile(m_hMapping, m_dwShareMode, 0, 0, m_dwLength);
						if(m_lpData != NULL)
							return TRUE;
					}
				}		
			}
			//先试验一下能不能直接创建,0xFFFFFFFF(INVALID_HANDLE_VALUE)作为句柄，表示没有真正的文件存在
			if(MapHandle(INVALID_HANDLE_VALUE)) return TRUE;

			//一般只有vista才能走到这里来
			//都失败了,那就在当前session下创建一个吧
			lstrcpy(m_szMappingName, m_szMappingNameVista);
			return MapHandle(INVALID_HANDLE_VALUE);
		}

		BOOL	MapExistingMemory(LPCTSTR sName, DWORD dwBytes, BOOL bReadOnly = FALSE)
		{
			m_dwLength = dwBytes;
			m_bReadOnly = bReadOnly;

			m_dwShareMode = (!bReadOnly) ? FILE_MAP_WRITE : FILE_MAP_READ;
			CreateMappingName(sName, TRUE);
			m_hMapping = OpenFileMapping(m_dwShareMode,	0, m_szMappingName);
			if (m_hMapping == NULL)
			{		
				UnMap();
				return FALSE;
			}

			// 映射文件
			m_lpData = MapViewOfFile(m_hMapping, m_dwShareMode, 0, 0, m_dwLength);

			return (m_lpData != NULL);
		}

		BOOL	MapExistingMemoryVista(int iTryCount=1);
		VOID    UnMap()
		{

			if (m_lpData != NULL)
			{
				FlushViewOfFile(m_lpData, 0);
				UnmapViewOfFile(m_lpData);
				ReleaseMappedBuffer(m_lpData);
				m_lpData = NULL;
			}

			// 关闭文件映射句柄
			if (m_hMapping != NULL)
			{
				CloseHandle(m_hMapping);
				m_hMapping = NULL;
			}

			// 关闭打开的文件
			if (m_hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
			}
		}
		VOID    CloseMap()
		{
			ReleaseMappedBuffer(m_lpData);
			m_lpData = NULL;
		}
		BOOL	Flush()
		{
			if (m_lpData == NULL)
				return FALSE;

			return FlushViewOfFile(m_lpData, 0);
		}

		inline	LPVOID GetMappedBuffer ()
		{
			return MapViewOfFile (m_hMapping, m_dwShareMode, 0, 0, 0);
		}
		inline	void ReleaseMappedBuffer (LPVOID pBuffer)
		{
			UnmapViewOfFile (pBuffer);
		}

		BOOL	BackupFile(LPCTSTR szBackupName)
		{
			return TRUE;
		}

		BOOL Reserve(DWORD dwNewSize)
		{
			this->UnMap();
			HANDLE hFile = CreateFile(this->m_szFileName, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			RASSERT(hFile != INVALID_HANDLE_VALUE, FALSE);
			LARGE_INTEGER liDistanceToMove;
			liDistanceToMove.QuadPart = dwNewSize; //设置成这个大，单位字节
			RASSERT(SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_BEGIN), FALSE);
			RASSERT(SetEndOfFile(hFile),FALSE);
			CloseHandle(hFile);
			return this->MapFile(this->m_szFileName, FALSE, dwNewSize);
		}
		//Accessors
		inline	LPCTSTR	GetFileName() { return m_szFileName; };
		inline	LPCTSTR	GetMappingName() { return m_szMappingName; };
		inline	HANDLE  GetFileHandle() const { return m_hFile; };
		inline	HANDLE  GetFileMappingHandle() const { return m_hMapping; };
		inline	DWORD   GetLength() const { return m_dwLength; };
		inline	LPVOID	GetBuffer() { return m_lpData; };
		inline	BOOL    IsReset() const { return m_bReset; };

	protected:  

		BOOL	MapHandle(HANDLE hHandle)
		{
			// 检查只读
			DWORD flProtect = (!m_bReadOnly) ? PAGE_READWRITE : PAGE_READONLY;

			// 修正长度
			DWORD dwLength = m_dwLength;
			if (m_bAppendNull)
				dwLength += 2;

			SECURITY_DESCRIPTOR sd;
			PSID sid;
			PACL acl;
			SECURITY_ATTRIBUTES sa = {0};
			LPSECURITY_ATTRIBUTES lpSecurityAttrib = NULL;
			if (Init_sd_to_everyone(sd, sid, acl))
			{
				sa.nLength = sizeof(sa);
				sa.bInheritHandle = FALSE;
				sa.lpSecurityDescriptor = &sd;
				lpSecurityAttrib = &sa;
			}
			/*
			//不仅加入Everyone组还加入了BUILTIN\Administrators组

			DWORD   dwRes;   
			PSID   pEveryoneSID   =   NULL,   pAdminSID   =   NULL;   
			PACL   pACL   =   NULL;   
			PSECURITY_DESCRIPTOR   pSD   =   NULL;   
			EXPLICIT_ACCESS   ea[2];   
			SID_IDENTIFIER_AUTHORITY   SIDAuthWorld   =   SECURITY_WORLD_SID_AUTHORITY;   
			SID_IDENTIFIER_AUTHORITY   SIDAuthNT   =   SECURITY_NT_AUTHORITY;   

			SECURITY_ATTRIBUTES   sa;   

			//   Create   a   well-known   SID   for   the   Everyone   group.   
			char   ErrorInfo[1024];   
			if(!AllocateAndInitializeSid(&SIDAuthWorld,   1,   
			SECURITY_WORLD_RID,   
			0,   0,   0,   0,   0,   0,   0,   
			&pEveryoneSID))   
			{   
			wsprintf(ErrorInfo,"AllocateAndInitializeSid   Error   %u\n",   GetLastError());   
			ErrorMsg(ErrorInfo);   
			return FALSE;  
			}   

			//   Initialize   an   EXPLICIT_ACCESS   structure   for   an   ACE.   
			//   The   ACE   will   allow   Everyone   read   access   to   the   key.   
			ZeroMemory(&ea,   2   *   sizeof(EXPLICIT_ACCESS));   
			ea[0].grfAccessPermissions   =   KEY_ALL_ACCESS;   
			ea[0].grfAccessMode   =   SET_ACCESS;   
			ea[0].grfInheritance=   NO_INHERITANCE;   
			ea[0].Trustee.TrusteeForm   =   TRUSTEE_IS_SID;   
			ea[0].Trustee.TrusteeType   =   TRUSTEE_IS_WELL_KNOWN_GROUP;   
			ea[0].Trustee.ptstrName     =   (LPTSTR)   pEveryoneSID;   

			//   Create   a   SID   for   the   BUILTIN\Administrators   group.   
			if(!   AllocateAndInitializeSid(&SIDAuthNT,   2,   
			SECURITY_BUILTIN_DOMAIN_RID,   
			DOMAIN_ALIAS_RID_ADMINS,   
			0,   0,   0,   0,   0,   0,   
			&pAdminSID))     
			{   
			wsprintf(ErrorInfo,"AllocateAndInitializeSid   Error   %u\n",   GetLastError());   
			ErrorMsg(ErrorInfo);   
			FreeSid(pEveryoneSID);   
			return FALSE;
			}   

			//   Initialize   an   EXPLICIT_ACCESS   structure   for   an   ACE.   
			//   The   ACE   will   allow   the   Administrators   group   full   access   to   the   key.   
			ea[1].grfAccessPermissions   =   KEY_ALL_ACCESS;   
			ea[1].grfAccessMode   =   SET_ACCESS;   
			ea[1].grfInheritance=   NO_INHERITANCE;   
			ea[1].Trustee.TrusteeForm   =   TRUSTEE_IS_SID;   
			ea[1].Trustee.TrusteeType   =   TRUSTEE_IS_GROUP;   
			ea[1].Trustee.ptstrName     =   (LPTSTR)   pAdminSID;   

			//   Create   a   new   ACL   that   contains   the   new   ACEs.   
			dwRes   =   SetEntriesInAcl(2,   ea,   NULL,   &pACL);   
			if   (ERROR_SUCCESS   !=   dwRes)     
			{   
			wsprintf(ErrorInfo,"SetEntriesInAcl   Error   %u\n",   GetLastError());   
			ErrorMsg(ErrorInfo);   
			FreeSid(pEveryoneSID);   
			FreeSid(pAdminSID);   
			LocalFree(pACL);   
			return FALSE;
			}   

			//   Initialize   a   security   descriptor.       
			pSD   =   (PSECURITY_DESCRIPTOR)   LocalAlloc(LPTR,     
			SECURITY_DESCRIPTOR_MIN_LENGTH);     
			if   (NULL   ==   pSD)     
			{     
			wsprintf(ErrorInfo,"LocalAlloc   Error   %u\n",   GetLastError());   
			ErrorMsg(ErrorInfo);   
			FreeSid(pEveryoneSID);   
			FreeSid(pAdminSID);   
			LocalFree(pACL);   
			LocalFree(pSD);   
			return FALSE;
			}     

			if   (!InitializeSecurityDescriptor(pSD,   SECURITY_DESCRIPTOR_REVISION))     
			{       
			wsprintf(ErrorInfo,"InitializeSecurityDescriptor   Error   %u\n",   
			GetLastError());   
			ErrorMsg(ErrorInfo);   
			FreeSid(pEveryoneSID);   
			FreeSid(pAdminSID);   
			LocalFree(pACL);   
			LocalFree(pSD);   
			return FALSE;
			}     

			//   Add   the   ACL   to   the   security   descriptor.     
			if   (!SetSecurityDescriptorDacl(pSD,     
			TRUE,           //   bDaclPresent   flag         
			pACL,     
			FALSE))       //   not   a   default   DACL     
			{       
			wsprintf(ErrorInfo,"SetSecurityDescriptorDacl   Error   %u\n",   GetLastError());   
			ErrorMsg(ErrorInfo);   
			FreeSid(pEveryoneSID);   
			FreeSid(pAdminSID);   
			LocalFree(pACL);   
			LocalFree(pSD);   
			return FALSE;
			}     

			//   Initialize   a   security   attributes   structure.   
			sa.nLength   =   sizeof   (SECURITY_ATTRIBUTES);   
			sa.lpSecurityDescriptor   =   pSD;   
			sa.bInheritHandle   =   FALSE;   
			*/
			m_hMapping = ::CreateFileMapping(hHandle, lpSecurityAttrib , flProtect, 0, dwLength, m_szMappingName);	
			if (m_hMapping == NULL)
			{
				ErrorMsg(TEXT("Failed in call to CreateFileMapping, GetLastError returned %d\n"), GetLastError());
				UnMap();
				if(sid || acl)
				{
					Free_sid_acl(sid, acl);
					sid = NULL;
					acl = NULL;
				}

				return FALSE;
			}

			// 映射文件获得内存指针
			m_dwShareMode = (!m_bReadOnly) ? FILE_MAP_WRITE : FILE_MAP_READ;
			m_lpData = MapViewOfFile(m_hMapping, m_dwShareMode, 0, 0, 0);

			// 末尾是否加0
			if (m_bAppendNull && m_lpData)
			{		
				BYTE* lpData = (BYTE*) m_lpData;
				lpData[m_dwLength] = 0;
				lpData[m_dwLength+1] = 0;
			}

			if(sid || acl)
			{
				Free_sid_acl(sid, acl);
				sid = NULL;
				acl = NULL;
			}

			return (m_lpData != NULL);
		}

		LPCTSTR	CreateMappingName(LPCTSTR sName, BOOL bNamed)
		{

#if _MSC_VER >= 1400
			_tcscpy_s(m_szFileName, _MAX_PATH, sName);
#else
			_tcscpy(m_szFileName, sName);
#endif
			if (bNamed)
			{
				TCHAR	szMappingName[_MAX_PATH];
#if _MSC_VER >= 1400
				_tcscpy_s(szMappingName, _MAX_PATH, sName);
#else
				_tcscpy(szMappingName, sName);
#endif
				for (UINT i=0; i<_tcslen(sName); i++)
				{
					if(szMappingName[i] == _T('\\') ||
						szMappingName[i] == _T(':') || 
						szMappingName[i] == _T('/') ||
						szMappingName[i] == _T(' '))
						szMappingName[i] = _T('_');			
				}
#if _MSC_VER >= 1400
				_tcsupr_s(szMappingName, _MAX_PATH );
#else
				_tcsupr(szMappingName);
#endif
				//if (m_IsNt)
				if( !m_bIs9X && !m_IsNt )
				{
					if(m_IsVista)//vista标准权限下用Global\会失败
					{
						lstrcpy(m_szMappingNameVista, szMappingName);
					}

					lstrcpy(m_szMappingName, _T("Global\\SHMem"));
					lstrcat(m_szMappingName, szMappingName);
				}
				else 
					lstrcpy(m_szMappingName, szMappingName);		
			}
			else
				m_szMappingName[0] = 0;
			return m_szMappingName;

		}
		LPCSTR	CreateMutexName();

		HANDLE  m_hFile;
		HANDLE  m_hMapping;
		BOOL    m_bReadOnly;
		BOOL    m_bAppendNull;
		LPVOID  m_lpData;
		TCHAR	m_szFileName[_MAX_PATH];
		TCHAR	m_szMappingName[_MAX_PATH];
		TCHAR	m_szMappingNameVista[_MAX_PATH];
		BOOL    m_bOpen;
		DWORD   m_dwLength;
		DWORD	m_dwShareMode;
		DWORD	m_dwGrowSize;
		BOOL	m_bReset;
		BOOL	m_IsNt;
		BOOL    m_bIs9X;
		BOOL	m_IsVista;

		CEnumSessions m_EnumSessions;

	private:
		void ErrorMsg(const TCHAR * msgDescriptor, ... )
		{
			TCHAR szBuf[1024];

			time_t t = time(NULL);
			va_list marker;
			va_start( marker, msgDescriptor );

#if _MSC_VER >= 1400
			_vstprintf_s( szBuf, 1024, msgDescriptor, marker );
#else
			_vstprintf( szBuf, msgDescriptor, marker );
#endif

			va_end( marker );

#if _MSC_VER >= 1400
			FILE *f;
			_tfopen_s(&f, _T("c:\\ms_error.txt"), _T("a+t"));
#else
			FILE *f=_tfopen("c:\\ms_error.txt", "a+t");
#endif
			if(f)
			{
				char tmptime[128];
				char tmpdate[128];
#if _MSC_VER >= 1400
				_strtime_s( tmptime, 128 );
				_strdate_s( tmpdate, 128 );
#else
#endif

				fprintf(f, "[%s %s] %s", tmpdate, tmptime, szBuf);
				//fputs(szBuf, f);
				fclose(f);
			}
			::OutputDebugString(szBuf);
		}

		static BOOL IsVista()
		{
			OSVERSIONINFO osver;

			osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

			if (	::GetVersionEx( &osver ) && 
				osver.dwPlatformId == VER_PLATFORM_WIN32_NT && 
				(osver.dwMajorVersion >= 6 ) )
				return TRUE;

			return FALSE;
		}
	};

} //namespace msdk

#endif // _MEMMAPFILE_INCLUDE_H_
