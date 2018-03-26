#include "StdAfx.h"
#include "DuiFont.h"
#include <DuiCore/IDuiSkin.h>

CDuiFont::CDuiFont():
	m_nSize(0),
	m_bBold(FALSE),
	m_bUnderLine(FALSE),
	m_bItalic(FALSE),
	m_bDefault(FALSE),
	m_pSkin(NULL),
	m_hFont(NULL)
{
}


CDuiFont::~CDuiFont(void)
{
}

VOID CDuiFont::SetDuiSkin(IDuiSkin* Skin)
{
	m_pSkin = Skin;
}

IDuiSkin* CDuiFont::GetDuiSkin()
{
	return m_pSkin;
}

HFONT CDuiFont::GetHandle()
{
	if ( m_hFont )
	{
		return m_hFont;
	}

	return CreateFont();
}

VOID CDuiFont::Attach(HFONT hFont)
{
	Detach();
	m_hFont = hFont;
}

VOID CDuiFont::Detach()
{
	if( m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

HFONT CDuiFont::CreateFont()
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy(lf.lfFaceName, m_strName, LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -m_nSize;
	if( m_bBold ) lf.lfWeight += FW_BOLD;
	if( m_bUnderLine ) lf.lfUnderline = TRUE;
	if( m_bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if( hFont == NULL ) return NULL;


	return hFont;
}

VOID CDuiFont::SetTypeFace(LPCWSTR lpszTypeFace)
{
	m_strTypeFace = lpszTypeFace;
}

LPCWSTR CDuiFont::GetTypeFace()
{
	return m_strTypeFace;
}

VOID CDuiFont::SetSize(INT nSize)
{
	m_nSize = nSize;
}

INT CDuiFont::GetSize()
{
	return m_nSize;
}

VOID CDuiFont::SetBold(BOOL bBold)
{
	m_bBold = bBold;
}

BOOL CDuiFont::GetBold()
{
	return m_bBold;
}

VOID CDuiFont::SetUnderLine(BOOL bUnderline)
{
	m_bUnderLine = bUnderline;
}

BOOL CDuiFont::GetUnderLine()
{
	return m_bUnderLine;
}

VOID CDuiFont::SetItalic(BOOL bItalic)
{
	 m_bItalic = bItalic;
}

BOOL CDuiFont::GetItalic()
{
	return m_bItalic;
}

HRESULT CDuiFont::DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
{
	IDuiSkin* tSkin = (IDuiSkin*)pParent->QueryInterface(IIDuiSkin);
	if ( tSkin )
	{
		m_pSkin = tSkin;
	}

	if ( !(m_pSkin && m_strName.Length()))
	{
//		GrpError(GroupName, MsgLevel_Error, _T(" Create DuiFont object faild. The param (m_pSkin) is null."));
		return FALSE;
	}
	m_pSkin->AddFont(GetName(), this);
	return S_OK;
}

LPCWSTR CDuiFont::GetName()
{
	return m_strName;
}

VOID CDuiFont::SetName(LPCWSTR lpszName)
{
	if ( lpszName && wcslen(lpszName))
	{
		m_strName = lpszName;
	}
}
