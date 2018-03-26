#ifndef _RE_UTILITY_EXTEND_H_
#define _RE_UTILITY_EXTEND_H_


#include "utility.h"

namespace UTILEX { ;

#if defined(PLATFORM_TYPE_POSIX)

struct hmodule_sentry
{
	static HMODULE default_value() { return 0; }
	static bool equal_to(HMODULE l, HMODULE r) { return l == r; }
	static void destroy(HMODULE h) { if(h) dlclose(h); }
};

#else

// ##### sentry traits extend ##################################################
struct hmodule_sentry
{
	static HMODULE default_value() { return 0; }
	static bool equal_to(HMODULE l, HMODULE r) { return l == r; }
	static void destroy(HMODULE h) { if(h) ::FreeLibrary(h); }
};

struct handle_sentry
{
	static HANDLE default_value() { return 0; }
	static bool equal_to(HANDLE l, HANDLE r) { return l == r; }
	static void destroy(HANDLE h) { if(h) ::CloseHandle(h); }
};

struct file_handle_sentry
{
	static HANDLE default_value() { return INVALID_HANDLE_VALUE; }
	static bool equal_to(HANDLE l, HANDLE r) { return l == r; }
	static void destroy(HANDLE h) { if(INVALID_HANDLE_VALUE != h) ::CloseHandle(h); }
};

struct find_handle_sentry
{
	static HANDLE default_value() { return INVALID_HANDLE_VALUE; }
	static bool equal_to(HANDLE l, HANDLE r) { return l == r; }
	static void destroy(HANDLE h) { if(INVALID_HANDLE_VALUE != h) ::FindClose(h); }
};

struct heap_mem_sentry
{
	HANDLE m_hHeap;
	heap_mem_sentry(HANDLE h) : m_hHeap(h) {}
	static void* default_value() { return 0; }
	static bool equal_to(void *l, void *r) { return l == r; }
	void destroy(void *p) { if(p) ::HeapFree(m_hHeap, 0, p); }
};

struct virtual_mem_sentry
{
	static void* default_value() { return 0; }
	static bool equal_to(void *l, void *r) { return l == r; }
	static void destroy(void *p) { if(p) ::VirtualFree(p, 0, MEM_RELEASE); }
};

// added by yechao 2008-03-06
struct view_of_map_sentry
{
    static void* default_value() { return 0; }
    static bool equal_to(void *l, void *r) { return l == r; }
    static void destroy(void *p) { if(p) ::UnmapViewOfFile(p); }
};

/*
struct co_task_mem_sentry
{
	static void* default_value() { return 0; }
	static bool equal_to(void *l, void *r) { return l == r; }
	static void destroy(void *p) { if(p) ::CoTaskMemFree(p); }
};
*/
// ################################################## sentry traits extend #####

#endif //PLATFORM_TYPE_POSIX

} // namespace UTILEX



#endif // duplicate inclusion protection
