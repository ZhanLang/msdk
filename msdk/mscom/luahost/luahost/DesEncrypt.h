#pragma once
class CDesEncrypt
{
public:
	CDesEncrypt(void);
	~CDesEncrypt(void);

	static BOOL BindToLua(ILuaVM* pLuaVm)
	{
		RASSERT(pLuaVm, FALSE);
		GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("Des")
			+mluabind::Function("Encrypt", &CDesEncrypt::Encrypt)
			+mluabind::Function("Decrypt", &CDesEncrypt::Decrypt)
			);

		return TRUE;
	}

	static std::string Encrypt(LPCSTR lpszContext, LPSTR lpszKey);
	static std::string Decrypt(LPCSTR lpszContext, LPSTR lpszKey);
};

