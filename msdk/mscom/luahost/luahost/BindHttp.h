#pragma once

#include <network/HttpImplement.h>
class CBindHttp
{
public:

	static BOOL BindToLua(ILuaVM* pLuaVm)
	{
		RASSERT(pLuaVm, FALSE);
		GET_LUAHOST(pLuaVm)->Insert(mluabind::Declare("http")
			+mluabind::Class<network::CHttpImplement>("CHttpImpl")
				.Constructor()
				.Method<DWORD,LPCSTR , WORD , LPCSTR , LPCSTR >("GetRequest", &network::CHttpImplement::GetRequest)
				.Method<DWORD,LPCSTR,WORD,LPCSTR,LPCSTR>("PostRequest", &network::CHttpImplement::PostRequest)
				.Method("JSonRequest", &network::CHttpImplement::JSonRequest)
				.Method("TextRequest", &network::CHttpImplement::TextRequest)
				.Method("GetRequestCode", &network::CHttpImplement::GetRequestCode)
				.Method("GetRequestData", &network::CHttpImplement::GetRequestData)
				.Method("GetRequestLength", &network::CHttpImplement::GetRequestLength)

			+mluabind::Function("encode_url", &network::CHttpImplement::encode_url)
			+mluabind::Function("decode_url", &network::CHttpImplement::decode_url)
			);

		return TRUE;
	}

};

