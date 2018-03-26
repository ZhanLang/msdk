// luahost_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mscom/mscominc.h"
#include "luahost/luahost.h"
#include "luahost/ILuaHost.h"
using namespace mscom;

class CBindClass
{
public:
	CBindClass()
	{

	}
	VOID TestPrint(LPCTSTR lpszTest)
	{
		OutputDebugString(lpszTest);
	}
};
int _tmain(int argc, _TCHAR* argv[])
{

#ifdef DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//_CrtSetBreakAlloc(6066);
	CMSComLoader luahostLib;
	BOOL bRet = luahostLib.LoadMSCom(_T(".\\common file\\mscom\\luahost.dll"));

	UTIL::com_ptr<ILuaVM> pLuaVM;
	luahostLib.CreateInstance(CLSID_LuaVM, NULL, NULL, __uuidof(ILuaVM), (void**)&pLuaVM.m_p);
	RASSERT(pLuaVM, 0);
	pLuaVM->OpenVM();

	CLuaHost* pHost = static_cast<CLuaHost*>(pLuaVM->GetContext());


	pHost->Insert(mluabind::Declare("LuaHost")
		+mluabind::Class<CBindClass>("CBindClass")
		.Constructor()
		.Method("TestPrint", &CBindClass::TestPrint)
		);

	HRESULT hr = pLuaVM->DoFile(_T("test.lua"));

	//pHost->CallLuaFunction<VOID>("test_print");

	pLuaVM->ClosetVM();

	if (pLuaVM)
	{
		pLuaVM = INULL;
	}
	return 0;
}

