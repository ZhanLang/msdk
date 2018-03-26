
#pragma once

#include "arith/encrypt/RC4.h"

//LUAÊµÏÖRC4¼ÓÃÜ
class CRc4LuaEx
{
public:

	static BOOL BindToLua(ILuaVM* pLuaVm)
	{
		RASSERT(pLuaVm, FALSE);
		GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("rc4")
			
			+mluabind::Function("EncryptBase64", &CRc4LuaEx::EncryptBase64)
			+mluabind::Function("DecryptBase64", &CRc4LuaEx::DecryptBase64)
			);

		return TRUE;
	}

	static std::string EncryptBase64(LPCSTR lpKey, LPCSTR lpszContext)
	{
		CRC4EncryptBase64 Encrypt((unsigned char*)lpKey, strlen(lpKey));
		Encrypt.Encrypt((unsigned char*)lpszContext, strlen(lpszContext) +1);
		return Encrypt.GetResult();
	}

	static std::string DecryptBase64(LPCSTR lpKey, LPCSTR lpszContext)
	{
		CRC4DecryptBase64 Decrypt((unsigned char*)lpKey, strlen(lpKey));
		Decrypt.Decrypt(lpszContext);
		return (const char* )Decrypt.GetResult();
	}
};