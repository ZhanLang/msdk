// CDbghelp.h

#if !defined(_INCLUDE_CDBGHELP_H__)
#define _INCLUDE_CDBGHELP_H__

#include <windows.h> // dbghelp.h depend on this.
#include <dbghelp.h> // Provides stack walking and symbol handling services.

#include <util/CallApi.h>
using namespace msdk;

class CMoudleDbghelp : public tImpModuleMid<CMoudleDbghelp>
{
public:	
	BOOL (__stdcall *SymInitialize)(HANDLE, PCSTR, BOOL);
	DWORD (__stdcall *SymSetOptions)(DWORD);
	DWORD (__stdcall *SymGetOptions)();
	BOOL (__stdcall *SymGetModuleInfo)(HANDLE, DWORD, PIMAGEHLP_MODULE);
	BOOL (__stdcall *SymGetModuleInfoW)(HANDLE, DWORD, PIMAGEHLP_MODULEW);
	BOOL (__stdcall *SymGetSymFromAddr)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);
	BOOL (__stdcall *SymGetLineFromAddr)(HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE);	
	BOOL (__stdcall *StackWalk)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, PVOID, 
								PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE, 
								PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
	DWORD (__stdcall *UnDecorateSymbolName)(PCSTR, PSTR, DWORD, DWORD);	
	PVOID (__stdcall *SymFunctionTableAccess)(HANDLE, DWORD);
	DWORD (__stdcall *SymGetModuleBase)(HANDLE, DWORD);
	BOOL (__stdcall *StackWalk64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64,
                                PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
	PVOID (__stdcall *SymFunctionTableAccess64)(HANDLE, DWORD64);
	DWORD64 (__stdcall *SymGetModuleBase64)(HANDLE, DWORD64);
	BOOL (__stdcall *SymCleanup)(HANDLE);

	DECLARE_FUN_BEGIN(CMoudleDbghelp, "dbghelp.DLL")
		DECLARE_FUN(SymInitialize)
		DECLARE_FUN(SymSetOptions)
		DECLARE_FUN(SymGetOptions)
		DECLARE_FUN(SymGetModuleInfo)
		DECLARE_FUN(SymGetModuleInfoW)
		DECLARE_FUN(SymGetSymFromAddr)
		DECLARE_FUN(SymGetLineFromAddr)		
		DECLARE_FUN(StackWalk)
		DECLARE_FUN(UnDecorateSymbolName)		
		DECLARE_FUN(SymFunctionTableAccess)
		DECLARE_FUN(SymGetModuleBase)
		DECLARE_FUN(StackWalk64)
		DECLARE_FUN(SymFunctionTableAccess64)
		DECLARE_FUN(SymGetModuleBase64)
		DECLARE_FUN(SymCleanup)		
	DECLARE_FUN_END()
};


#endif // !defined(_INCLUDE_CDBGHELP_H__)
