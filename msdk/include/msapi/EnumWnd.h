
#pragma once

#include <string>



namespace msdk{;
namespace msapi{;


enum 
{
	EnumWnd_Class = 0x01,
	EnumWnd_Name  = 0x02,
	EnumWnd_All	  = 0x11,
};
class CEnumWnd
{
public:
	CEnumWnd(int e, HWND hParentWnd = NULL)
	{
		m_nEnumType = e;
		m_hParentWnd = hParentWnd;
		m_hWnd = NULL;
	}

	HWND Enum(LPCTSTR lpszClassName, LPCTSTR lpszText = NULL)
	{
		if ( lpszClassName )
			m_strClassName = lpszClassName;
		
		if ( lpszText )
			m_strText = lpszText;
		
		EnumChildWindows(m_hParentWnd, _EnumChildWindows_, (LPARAM)this);
		return m_hWnd;
	}



private:

	BOOL _EnumChildWindows(HWND hwnd)
	{
		BOOL bCaseClass = FALSE;
		BOOL bCaseName  = FALSE;

		if ( m_nEnumType & EnumWnd_Name)
		{
			TCHAR szText[ 1024 ] = { 0 };
			INT nLen = GetWindowText(hwnd, szText, 1024);
			if ( _tcsicmp(szText, m_strText.c_str()) == 0 )
			{
				m_hWnd = hwnd;
				bCaseName = TRUE;
			}
		}

		if ( m_nEnumType & EnumWnd_Class )
		{
			TCHAR szClassName[ MAX_PATH ] = { 0 };
			GetClassName(hwnd, szClassName, MAX_PATH);
			if ( _tcsicmp(szClassName, m_strClassName.c_str()) == 0)
			{
				m_hWnd = hwnd;
				bCaseClass = TRUE;
			}
		}

		
		if( m_nEnumType & EnumWnd_All )
		{
			if ( bCaseClass && bCaseName)
				return TRUE;
			else
				return FALSE;
		}
		
		return  bCaseName |  bCaseClass;
	}
	static BOOL CALLBACK  _EnumChildWindows_(HWND hwnd,LPARAM lParam)
	{
		CEnumWnd* Param = (CEnumWnd*)lParam;
		if ( Param->_EnumChildWindows(hwnd) )
			return FALSE;
		
		EnumChildWindows(hwnd, _EnumChildWindows_, lParam);
		return TRUE;
	}

	HWND m_hParentWnd;
	HWND m_hWnd;
	INT  m_nEnumType;
	std::basic_string<TCHAR> m_strClassName;
	std::basic_string<TCHAR> m_strText;
};


//class CEnumWnd

};};