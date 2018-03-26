#pragma once
#include "luahost/ILuaHost.h"
class CHttpLuaEx:
	public ILuaExtend,
	public CUnknownImp
{
public:
	UNKNOWN_IMP1_(ILuaExtend);
	CHttpLuaEx(void);
	~CHttpLuaEx(void);

	STDMETHOD(RegisterLuaEx)(ILuaVM* pLuaVm);

	
};




