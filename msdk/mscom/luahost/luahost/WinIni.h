#ifndef WinIni_h__
#define WinIni_h__

#include <vector>

#include "luahost.h"
namespace winfunc
{
	typedef std::basic_string<TCHAR> TString;
	typedef std::vector<TString> TStringS;

	class CWinIni
	{
	public:
		CWinIni() { }

		static BOOL BindToLua(ILuaVM* pLuaVM)
		{
			GET_LUAHOST(pLuaVM)->Insert(mluabind::Declare("ini")
				+mluabind::Function("GetIniString", &CWinIni::GetIniString)
				+mluabind::Function("SetIniString", &CWinIni::SetIniString)
				);

			return TRUE;
		}
		static TString GetIniString(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName);
		static bool    SetIniString(LPCTSTR lszFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpszValue);

	};
}

#endif // WinIni_h__
