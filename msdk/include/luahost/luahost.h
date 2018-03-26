
#pragma once


#define LOKI_STATIC
#include "Loki/EmptyType.h"
#include "Loki/SmartPtr.h"
#include "mluabind/mluabind.h"

namespace msdk{;
namespace mscom{;




#define GET_LUAHOST(pLuaVm) ((CLuaHost*)pLuaVm->GetContext())

static int LuaDummyHandler(lua_State* L)
{
	return 1;
}

class CLuaException : public std::exception
{
public:
	CLuaException(const char *s) : std::exception(s) {
		
		/*
		char pTempBuffer[65536] = {0};

		DWORD dwCurrentProcessID = GetCurrentProcessId();
		DWORD dwCurrentThreadID  = GetCurrentThreadId();

		SYSTEMTIME systemTime = { 0 };
		GetLocalTime(&systemTime);

		int len = sprintf_s(pTempBuffer,
			65536 ,
			"[%4d]%4d-%02d-%02d %02d:%02d:%02d:%03d: [%4d][%s][%s]:%s",
			dwCurrentProcessID,
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wMilliseconds,
			dwCurrentThreadID,
			"DBG",
			"luahost",
			s);
			*/
		GrpErrorA("luahost", MsgLevel_Error, s);
	};
};

// static void* operator new(size_t size) 
// {
// 	static HANDLE hProcessHeap = GetProcessHeap();
// 	return HeapAlloc(hProcessHeap, 0, size);
// }
// 
// static void operator delete(void *pUserData)
// {
// 	static HANDLE hProcessHeap = GetProcessHeap();
// 	HeapFree(hProcessHeap, HEAP_NO_SERIALIZE, pUserData);
// }

class CLuaHost : public mluabind::CHost
{
public:
	virtual void Error(const char *_format, ...)
	{
		char pTempBuffer[65536] = {0};

		/*
		DWORD dwCurrentProcessID = GetCurrentProcessId();
		DWORD dwCurrentThreadID  = GetCurrentThreadId();

		SYSTEMTIME systemTime = { 0 };
		GetLocalTime(&systemTime);

		int len = sprintf_s(pTempBuffer,
			65536 ,
			"[%4d]%4d-%02d-%02d %02d:%02d:%02d:%03d: [%4d][%s][%s]:",
			dwCurrentProcessID,
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wMilliseconds,
			dwCurrentThreadID,
			"DBG",
			"luahost");
			*/
		va_list args;
		va_start( args, _format );
		vsprintf_s ( pTempBuffer, 65536, _format, args );
		va_end( args );

		GrpErrorA("luahost", MsgLevel_Error, pTempBuffer);
	}

	CLuaHost(lua_State*L) : CHost(L){}

	


	bool DoString(const char *_str)
	{
		try{
			_DoString(_str);
			return true;
		}catch(CLuaException&){}
		return false;
	}

	bool DoFile(const char* _file)
	{
		try{
			_DoFile(_file);
			return true;
		}catch(CLuaException&){}
		return false;
	}

	template<class _T>
	_T GetGlobal(const char* _name )
	{
		  lua_getglobal(GetLua(), _name);
	}
	
	void CallLuaFunctionEx(const std::string &_name)
	{
		try{
			 CallLuaFunction<void>(_name);
		}catch(CLuaException&){}
	}

	template <class _P1>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1)
	{
		try{
			CallLuaFunction<void>(_name, _p1);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4, class _P5>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4, _p5);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4, class _P5, class _P6>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4, _p5, _p6);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
		}catch(CLuaException&){}
	}

	template < class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9, class _P10>
	void CallLuaFunctionEx(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9, _P10 _p10)
	{
		try{
			CallLuaFunction<void>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
		}catch(CLuaException&){}
	}


	template <class _Ret, _Ret _Def>
	_Ret CallLuaFunctionRet(const std::string &_name)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name);
		}catch(CLuaException&){}
		
		return ret;
	}



	template <class _Ret, _Ret _Def, class _P1>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1);
		}catch(CLuaException&){}

		return ret;
	}


	template <class _Ret, _Ret _Def, class _P1, class _P2>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3);

		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4, class _P5>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4, _p5);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4, _p5, _p6);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4, _p5, _p6, _p7);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4, _p5, _p6, _p7, _p8);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4, _p5, _p6, _p7, _p8, _p9);
		}catch(CLuaException&){}

		return ret;
	}

	template <class _Ret, _Ret _Def, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9, class _P10>
	_Ret CallLuaFunctionRet(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9, _P10 _p10)
	{
		_Ret ret = _Def;
		try{
			return CallLuaFunction<_Ret>(_name, _p1, _p2,_p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
		}catch(CLuaException&){}

		return ret;
	}



	template <class _Ret>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def)
	{
		try{
			return CallLuaFunction<_Ret>(_name);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1)
	{
		try{
			return CallLuaFunction<_Ret>(_name, _p1);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3, _p4);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4, class _P5>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3, _p4, _p5);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4, class _P5, class _P6>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3, _p4, _p5, _p6);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8)
	{
		try{
			return CallLuaFunction<std::string>(_name,  _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
		}catch(CLuaException&){}
		return _def;
	}

	template < class _Ret,class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9,class _P10>
	_Ret CallLuaFunctionDef(const std::string &_name, _Ret _def, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9, _P10 _p10)
	{
		try{
			return CallLuaFunction<std::string>(_name, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
		}catch(CLuaException&){}
		return _def;
	}

private:
	void _DoFile(const char *_str)
	{
		lua_pushcclosure(GetLua(), &LuaDummyHandler, 0);

		if (luaL_loadfile(GetLua(), _str))
		{
			std::string err(lua_tostring(GetLua(), -1));
			lua_pop(GetLua(), 2);
			throw CLuaException(err.c_str());
		}

		if (lua_pcall(GetLua(), 0, 0, -2))
		{
			std::string err(lua_tostring(GetLua(), -1));
			lua_pop(GetLua(), 2);
			throw CLuaException(err.c_str());
		}

		lua_pop(GetLua(), 1);
	};

	void _DoString(const char *_str)
	{
		lua_pushcclosure(GetLua(), &LuaDummyHandler, 0);

		if (luaL_loadbuffer(GetLua(), _str, strlen(_str), _str))
		{
			std::string err(lua_tostring(GetLua(), -1));
			lua_pop(GetLua(), 2);
			throw CLuaException(err.c_str());
		}

		if (lua_pcall(GetLua(), 0, 0, -2))
		{
			std::string err(lua_tostring(GetLua(), -1));
			lua_pop(GetLua(), 2);
			throw CLuaException(err.c_str());
		}

		lua_pop(GetLua(), 1);
	};
};


#define LUA_TRY_BEGIN try{

#define LUA_TRY_END }catch(CLuaException ){}
};};