#pragma once

/*!使用方法
//解压缩文件
function extract_7z(filepath)
	local _handle = lzma.LZMA_Lua_OpenFile(filepath);
	local _file_count = lzma.LZMA_Lua_GetFileCount(_handle);

	for loop = 0,  _file_count -1 , 1 do
		local _name = lzma.LZMA_Lua_GetFileName(_handle, loop);
		local temp = "G:\\TEST\\" + _name;
		lzma.LZMA_Lua_Extract(_handle, loop, temp);
	end
	lzma.LZMA_Lua_Close(_handle);
	return true;
end

//解压缩内存
function extract_7z_mem(mem, lenght)
	local _handle = lzma.LZMA_Lua_OpenMem(mem, lenght);
	local _file_count = lzma.LZMA_Lua_GetFileCount(_handle);

	for loop = 0,  _file_count -1 , 1 do
		local _name = lzma.LZMA_Lua_GetFileName(_handle, loop);
		local temp = "G:\\TEST\\" + _name;
		lzma.LZMA_Lua_Extract(_handle, loop, temp);
	end
	lzma.LZMA_Lua_Close(_handle);
	return true;
end

*/
bool Bind7ZipLuaLibrary(void* pLuaHost);

