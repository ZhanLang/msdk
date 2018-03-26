#include "7Zip/7Zip.h"
#include "7Zip/7ZipLua.h"
#include <string>
#include "luahost/luahost.h"
using namespace mluabind;

LZMA_HANDLE LZMA_Lua_OpenFile(std::string lpszFile)
{
	return LZMA_OpenFileA(lpszFile.c_str());
}



LZMA_HANDLE LZMA_Lua_OpenMem(LPVOID pMem, DWORD dwLength)
{
	return LZMA_OpenMem(pMem, dwLength);
}


VOID		LZMA_Lua_Close(LZMA_HANDLE _handle)
{
	return LZMA_Close(_handle);
}


DWORD		LZMA_Lua_GetFileCount(LZMA_HANDLE _handle)
{
	return LZMA_GetFileCount(_handle);
}

std::string		LZMA_Lua_GetFileName(LZMA_HANDLE _handle, DWORD dwIndex)
{
	CHAR szFileName[MAX_PATH] = {0};
	LZMA_GetFileNameA(_handle, dwIndex, szFileName, MAX_PATH);
	return szFileName;
}



DWORD		LZMA_Lua_Extract(LZMA_HANDLE _handle, DWORD dwIndex, std::string  lpszFile)
{
	return LZMA_ExtractA(_handle, dwIndex, lpszFile.c_str());
}


bool Bind7ZipLuaLibrary(void* pLuaHost)
{
	if (!pLuaHost){
		return false;
	}

	CHost* host = CHost::GetFromLua((lua_State*)pLuaHost);
	
 	//std::locale::global(std::locale(""));
	host->Insert(Declare("lzma")
		+Function("LZMA_Lua_OpenFile", LZMA_Lua_OpenFile)
		+Function("LZMA_Lua_OpenMem", LZMA_Lua_OpenMem)
		+Function("LZMA_Lua_Close", LZMA_Lua_Close)
		+Function("LZMA_Lua_GetFileCount", LZMA_Lua_GetFileCount)
		+Function("LZMA_Lua_GetFileName", LZMA_Lua_GetFileName)
		+Function("LZMA_Lua_Extract", LZMA_Lua_Extract)
		);

	return true;
}