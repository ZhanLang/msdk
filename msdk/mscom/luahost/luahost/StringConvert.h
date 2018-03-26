
#include "luahost/ILuaHost.h"
class CStringConvert
{
public:
	CStringConvert(void);
	~CStringConvert(void);

	static BOOL BindToLua(ILuaVM* pLuaVM);

	static std::string wstring_to_sting(std::wstring s);
	static std::wstring string_to_wstring(std::string s);

	static std::string wstring_to_string_utf8(const WCHAR* SourceStr);
	static std::wstring utf8_string_to_wstring(const char* SourceStr);
};
