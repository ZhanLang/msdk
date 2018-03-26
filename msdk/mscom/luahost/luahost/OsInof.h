#pragma once

#include "luahost/luahost.h"
//lua  扩展
class COsInof
{
public:
	COsInof();
	~COsInof(void);


	static BOOL Bind(CLuaHost* pLuaHost);


	//获取操作系统信息
	static std::string GetOsInfo();

	//获取系统盘总大小
	static std::string GetDiskSize();

	//获取系统盘剩余空间大小
	static std::string GetDiskFreeSize();

	//获取用户名称
	static std::string GetUserName();

	//获取电脑名称
	static std::string GetComputerName();

	static bool IsWow64();

	static bool IsVM();
};

