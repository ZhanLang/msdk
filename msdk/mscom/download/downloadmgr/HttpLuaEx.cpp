#include "StdAfx.h"
#include "HttpLuaEx.h"
#include "luahost/luahost.h"
#include "script/http.h"
#include <map>
using namespace std;

CHttpLuaEx::CHttpLuaEx(void)
{
}


CHttpLuaEx::~CHttpLuaEx(void)
{
}


STDMETHODIMP CHttpLuaEx::RegisterLuaEx( ILuaVM* pLuaVm )
{
	RASSERT(pLuaVm, E_INVALIDARG);
	
	CLuaHost* pHost = static_cast<CLuaHost*>(pLuaVm->GetContext());

	pHost->Insert(mluabind::Declare("http")
			
		+mluabind::Function("http_request", &http::http_request)
		+mluabind::Function("http_post", &http::http_post)
		+mluabind::Function("encode_url", &http::encode_url)
		+mluabind::Function("decode_url", &http::decode_url)
		+mluabind::Function("http_download", &http::http_download)
		);
	
		
	return S_OK;
}
