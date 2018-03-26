
#pragma once
#include "stdafx.h"
#include <string>

class CShellLinkInfo
{
	typedef struct _LNKHEAD
	{
		DWORD		dwID;
		DWORD		dwGUID[4];
		DWORD		dwFlags;
		DWORD		dwFileAttributes;
		FILETIME	dwCreationTime;
		FILETIME	dwModificationTime;
		FILETIME	dwLastaccessTime;
		DWORD		dwFileLen;
		DWORD		dwIconNum;
		DWORD		dwWinStyle;
		DWORD		dwHotkey;
		DWORD		dwReserved1;
		DWORD		dwReserved2;
	}LNKHEAD, *PLNKHEAD;

	typedef struct _FILELOCATIONINFO
	{
		DWORD		dwSize;
		DWORD		dwFirstOffset;
		DWORD		dwFlags;
		DWORD		dwOffsetOfVolume;
		DWORD		dwOffsetOfBasePath;
		DWORD		dwOffsetOfNetworkVolume;
		DWORD		dwOffsetOfRemainingPath;
	}FILELOCATIONINFO, *PFILELOCATIONINFO;

	typedef struct _LOCALVOLUMETAB
	{
		DWORD		dwSize;
		DWORD		dwTypeOfVolume;
		DWORD		dwVolumeSerialNumber;
		DWORD		dwOffsetOfVolumeName;
		char		strVolumeName[0];
	}LOCALVOLUMETAB, *PLOCALVOLUMETAB;

	typedef struct _NETWORKVOLUMETAB
	{
		DWORD		dwSize;
		DWORD		dwUnknown1;
		DWORD		dwOffsetOfNetShareName;
		DWORD		dwUnknown2;
		DWORD		dwUnknown3;
		char		strNetShareName[0];
	}NETWORKVOLUMETAB, *PNETWORKVOLUMETAB;

#define LNK_HASIDLIST	0x1
#define LNK_FILEDIR		0x2
#define LNK_HASDES		0x4
#define LNK_HASPATH		0x8
#define LNK_HASWORKDIR	0x10
#define LNK_HASCMD		0x20

#define LNK_LOCALVOLUME	0x1
#define LNK_NETSHARE	0x2

	BOOL   m_bIsDir;
	std::string m_strRealFilePath;
public: 
	CShellLinkInfo():m_bIsDir(FALSE)
	{

	}
	~CShellLinkInfo()
	{
	}

	LPCSTR GetRealFilePath()
	{
		return m_strRealFilePath.c_str();
	}

	BOOL IsDir()
	{
		return m_bIsDir;
	}

	BOOL Open(LPCTSTR pFileName)
	{
		FILE			*file = 0;
		unsigned short	usLenTemp;
		int				iSize;
		LNKHEAD			head;
		FILELOCATIONINFO	fileLocationInfo;
		char			szDescription[1024]; //快捷方式所指向的文件描述
		char			szFilePath[1024] = {0};    //快捷方式所指向的文件
		char			szCommand[1024];
		WCHAR			wszTemp[512];
		char*			pDest;
		DWORD			dwFlags;
		int				p;


		if((file =  _wfopen(pFileName, L"rb")) == NULL)
		{
			return FALSE;
		}

		// head
		iSize = sizeof(LNKHEAD);
		if (fread(&head, 1, iSize, file) != iSize)
		{
			fclose(file);
			return FALSE;
			//goto errorExit;
		}

		dwFlags = head.dwFlags;

		if(dwFlags & LNK_HASIDLIST)
		{
			// The Shell Item Id List
			if(fread(&usLenTemp, 2, 1, file) != 1)
			{
				fclose(file);
				return FALSE;
				//goto errorExit;
			}
			//fread(&szCommand, usLenTemp, 1, file);
			fseek(file, usLenTemp, SEEK_CUR);
		}


		p = ftell(file);
		// file location info
		if(fread(&fileLocationInfo, sizeof(fileLocationInfo), 1, file) != 1)
		{
			fclose(file);
			return FALSE;
			//goto errorExit;
		}

		//fread(&szCommand, fileLocationInfo.dwSize - sizeof(fileLocationInfo), 1, file);

		fseek(file, fileLocationInfo.dwOffsetOfBasePath +  p, SEEK_SET);


		if(fileLocationInfo.dwFlags & LNK_NETSHARE)
		{
			iSize = fileLocationInfo.dwOffsetOfNetworkVolume - fileLocationInfo.dwOffsetOfBasePath;
		}
		else
		{
			iSize = fileLocationInfo.dwOffsetOfRemainingPath - fileLocationInfo.dwOffsetOfBasePath;
		}



		if(fread(&szCommand, 1, iSize, file) != iSize)
		{
			fclose(file);
			return FALSE;
			//goto errorExit;
		}


		/*
		pDest = strupr(szCommand);
		if(NULL == strstr(pDest, ".EXE"))
		{
			fclose(file);
			return FALSE;
			//goto errorExit;
		}
		*/

		m_strRealFilePath = szCommand;

		fclose(file);
		return TRUE;
		sprintf(szFilePath, "\"%s\"", szCommand);

		fseek(file, fileLocationInfo.dwSize +  p, SEEK_SET);

		if(dwFlags & LNK_HASDES)
		{
			// skip Description string
			if(fread(&usLenTemp, 2, 1, file) != 1)
			{
				return FALSE;
				//goto errorExit;
			}
			if(fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				return FALSE;
				//goto errorExit;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte( CP_ACP, 0, wszTemp, -1,
				szDescription, 512, NULL, NULL );
			//fseek(file, usLenTemp*2, SEEK_CUR);
		}

		if(dwFlags & LNK_HASPATH)
		{
			// skip Relative path
			if(fread(&usLenTemp, 2, 1, file) != 1)
			{
				return FALSE;
				//goto errorExit;
			}

			if(fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				return FALSE;
				//goto errorExit;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte( CP_ACP, 0, wszTemp, -1,
				szCommand, 512, NULL, NULL );
			//fseek(file, usLenTemp*2, SEEK_CUR);
		}

		if(dwFlags & LNK_HASWORKDIR)
		{
			// skip Working directory
			if(fread(&usLenTemp, 2, 1, file) != 1)
			{
				return FALSE;
			}

			if(fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				return FALSE;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte( CP_ACP, 0, wszTemp, -1,
				szCommand, 512, NULL, NULL );
			//fseek(file, usLenTemp*2, SEEK_CUR);
		}

		if(dwFlags & LNK_HASCMD)
		{
			// Command line arguments
			if(fread(&usLenTemp, 2, 1, file) != 1)
			{
				return FALSE;
			}

			if(fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				return FALSE;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte( CP_ACP, 0, wszTemp, -1,
				szCommand, 512, NULL, NULL );
			strcat(szFilePath, " ");
			strcat(szFilePath, szCommand);

			
		}

		return TRUE;
	}
};