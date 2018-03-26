#include "StdAfx.h"
#include "DuiSkinFile.h"

#include <DuiCore/IDuiStream.h>
#include <DuiCore/IDuiImage.h>
#include <DuiCore/IDuiLang.h>
//DEFINE_OBJECT(CDuiSkinFile, OBJECT_CORE_SKIN_FILE)
CDuiSkinFile::CDuiSkinFile()
:m_pCore(NULL),
m_pDuiLangSet(NULL)
{
	m_bOpen = FALSE;
}


CDuiSkinFile::~CDuiSkinFile(void)
{
	Close();
}


BOOL CDuiSkinFile::Open(LPVOID lpMem, DWORD dwSize, LPVOID lpParam/* = 0*/, DWORD dwParamSize /*= 0*/)
{
	if ( !(m_pCore && m_strName.Length()) )
	{
//		GrpError(GroupName, MsgLevel_Error, L"打开皮肤文件失败，必须设置 SetCore 和 SetName");
		return FALSE;
	}

	if ( !m_pDuiLangSet)
	{
		m_pDuiLangSet = (IDuiLangSet*)CreateObject(OBJECT_CORE_LANG_SET)->QueryInterface(IIDuiLangSet);
		m_pDuiLangSet->DoCreate(this, m_pCore,NULL);
	}
	

	RASSERT( lpMem && dwSize && wcslen((WCHAR*)lpMem) > 3, FALSE);
	m_strPath = (WCHAR*)lpMem;
	
	RASSERT(GetFileAttributes(m_strPath) != -1, FALSE);

	if (m_strPath[m_strPath.Length() - 1] != '\\')
	{
		m_strPath.Append('\\');
	}
	
	m_bOpen = TRUE;
	return TRUE;
}

BOOL CDuiSkinFile::Close()
{
	if ( m_pCore )
	{
		m_pCore->RemoveSkin(GetName());
	}

	{
		AUTOLOCK_CS(m_skinStreamMap);
		for (CSkinStreamMap::Iterator it = m_skinStreamMap.Min() ; it ; it++)
		{
			IDuiByteStream* pStream = it->Value;
			pStream->DeleteThis();
		}
		m_skinStreamMap.Clear();
	}
	
	{
		AUTOLOCK_CS(m_imageMap);
		for ( CImageMap::Iterator itImage = m_imageMap.Min() ; itImage ; itImage++)
		{
			IDuiImage* tImage = itImage->Value;
			tImage->DeleteThis();
		}
		m_imageMap.Clear();
	}

	{
		AUTOLOCK_CS(m_FontMap);
		for (CDuiFontMap::Iterator it = m_FontMap.Min(); it ; it++)
		{
			IDuiFont* tFont = it->Value;
			if ( tFont )
			{
				tFont->DeleteThis();
			}
		}
		m_FontMap.Clear();
	}
	

	
	
	m_bOpen = FALSE;
	return TRUE;
}

DuiKit::SkinType CDuiSkinFile::GetSkinType()
{
	return SkinFile;
}


IDuiByteStream* CDuiSkinFile::GetStream(LPCWSTR lpszFileName, BOOL bCache/* = TRUE*/)
{
	if ( !(lpszFileName && wcslen(lpszFileName)) )
	{
		return NULL;
	}

	
	{//查询现有的
		AUTOLOCK_CS(m_skinStreamMap);
		CSkinStreamMap::Iterator it = m_skinStreamMap.Find(lpszFileName);
		if ( it )
		{
			return it->Value;
		}
	}


	CDuiString sFilePath = m_strPath;
	sFilePath += lpszFileName;

	//新建一个
	HANDLE hFile = CreateFile(sFilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);

	IDuiByteStream* tStream  = (IDuiByteStream*)CreateObject(OBJECT_CORE_BYTE_STREAM);
	if ( !tStream )
	{
		CloseHandle(hFile);
		return NULL;
	}

	tStream->Reserve(dwFileSize);

	/*
	while(TRUE)
	{
		BYTE read[MAX_PATH] = { 0 };
		DWORD dwRead = 0;
		ReadFile(hFile, read, MAX_PATH, &dwRead, NULL);
		if ( dwRead == 0)
		{
			break;
		}

		tStream->AddTail(read, dwRead);
	}
	*/

	DWORD dwRead = 0;
	ReadFile(hFile, tStream->GetData(), dwFileSize, &dwRead, NULL);
	tStream->SetBufSize(dwRead);

	if ( bCache)
	{
		AUTOLOCK_CS(m_skinStreamMap);
		m_skinStreamMap.Insert(lpszFileName, tStream);
	}
	
	CloseHandle(hFile);
	return tStream;
}

VOID CDuiSkinFile::SetName(LPCWSTR lpszName)
{
	if ( lpszName)
	{
		m_strName = lpszName;
	}
}

LPCWSTR CDuiSkinFile::GetName()
{
	return m_strName.GetBuffer();
}

VOID CDuiSkinFile::SetCore(IDuiCore* Core)
{
	m_pCore = Core;
	m_pCore->AddSkin(GetName(), this);
}

IDuiCore* CDuiSkinFile::GetCore()
{
	return m_pCore;
}


IDuiImage* CDuiSkinFile::GetImage(LPCWSTR lpszImage)
{
	if ( lpszImage && wcslen(lpszImage) )
	{
		AUTOLOCK_CS(m_imageMap);
		CImageMap::Iterator itImage = m_imageMap.Find(lpszImage);
		if ( itImage )
		{
			if ( itImage->Value && !itImage->Value->GetHandle())
			{
				itImage->Value->Create();
			}
			return itImage->Value;
		}
	}

	return NULL;
}

VOID CDuiSkinFile::AddImage(IDuiImage* Image)
{
	if ( Image && wcslen(Image->GetName()) )
	{
		AUTOLOCK_CS(m_imageMap);
		CImageMap::Iterator itImage = m_imageMap.Find(Image->GetName());
		if ( itImage )
		{
			m_imageMap.Remove(Image->GetName());
		}

		m_imageMap.Insert(Image->GetName(), Image);
	}


}

VOID CDuiSkinFile::RemoveImage(LPCWSTR lpszName)
{
	if ( lpszName && wcslen(lpszName))
	{
		m_imageMap.Remove(lpszName);
	}
}

VOID CDuiSkinFile::AddFont(LPCWSTR lpszFontName, IDuiFont* pFont)
{
	if ( lpszFontName && wcslen(lpszFontName) && pFont)
	{
		AUTOLOCK_CS(m_FontMap);
		CDuiFontMap::Iterator itFont = m_FontMap.Find(lpszFontName);
		if ( itFont && itFont->Value )
		{
//			GrpMsg(GroupName, MsgLevel_Msg, _T("FILE: [%s] LINE: [%d] The Font:%s is exist , it will be replace." ), __TFILE__, __LINE__, lpszFontName);
			itFont->Value->DeleteThis();
			m_FontMap.Remove(lpszFontName);
		}


		m_FontMap.Insert(lpszFontName, pFont);
	}
}

VOID CDuiSkinFile::RemoveFont(LPCWSTR lpszFontName)
{
	if ( lpszFontName && wcslen(lpszFontName))
	{
		AUTOLOCK_CS(m_FontMap);
		CDuiFontMap::Iterator it = m_FontMap.Find(lpszFontName);
		if ( it )
		{
			IDuiFont* tFont = it->Value;
			if ( tFont )
			{
				tFont->DeleteThis();
			}
		}

		m_FontMap.Remove(lpszFontName);
	}
}

IDuiFont* CDuiSkinFile::GetFont(LPCWSTR lpszFontName)
{
	AUTOLOCK_CS(m_FontMap);
	RASSERT(m_FontMap.Count(), NULL);
	CDuiFontMap::ValueType v = m_FontMap.Find(lpszFontName);
	if ( v )
	{
		return v->Value;
	}

	return m_FontMap.Max()->Value;
}

IDuiLangSet* CDuiSkinFile::GetLangSet()
{
	return m_pDuiLangSet;
}