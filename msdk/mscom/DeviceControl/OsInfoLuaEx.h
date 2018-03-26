#pragma once
#include "luahost/ILuaHost.h"
class COsInfoLuaEx:
	public ILuaExtend,
	public CUnknownImp
{
public:
	UNKNOWN_IMP1_(ILuaExtend);

	HRESULT init_class(IMSBase* pRot, IMSBase* pOuter)
	{
		RASSERT(pRot, E_FAIL);
		m_pRot = pRot;
		return S_OK;
	}

	COsInfoLuaEx(void);
	~COsInfoLuaEx(void);

protected:
	STDMETHOD(RegisterLuaEx)(ILuaVM* pLuaVm);

	//获取操作系统版本号
	static int GetOsVersion();

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

private:
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;
};

