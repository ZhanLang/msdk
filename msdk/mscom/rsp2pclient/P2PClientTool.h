#pragma once
#ifndef __P2PClientTool_H__
#define __P2PClientTool_H__


#include <map>
#include <vector>
#include <string>
#include <p2pclient/IP2PClient.h>


#define P2PLPHash           unsigned char*
#define P2PStdString        std::wstring
#define P2PStringSize(len)	((len)/sizeof(wchar_t))
#define HttpUrlHash         AnyUrlHash


typedef struct P2PHash_t
{
	unsigned char hash[P2P_HASH_SIZE];
}P2PHash_t;
inline bool operator < (const P2PHash_t & h1, const P2PHash_t & h2)
{
	return memcmp(h1.hash, h2.hash, P2P_HASH_SIZE) < 0;
}

inline bool operator == (const P2PHash_t & h1, const P2PHash_t & h2)
{
	return memcmp(h1.hash, h2.hash, P2P_HASH_SIZE) == 0;
}

inline bool operator != (const P2PHash_t & h1, const P2PHash_t & h2)
{
	return memcmp(h1.hash, h2.hash, P2P_HASH_SIZE) != 0;
}

inline bool IsAnyHash(P2PHash_t &hash)
{
	return (hash == *(P2PHash_t*)AnyUrlHash);
}

inline bool IsValidHash(P2PHash_t &hash)
{
	P2PHash_t invalid = {0};
	return !(hash == invalid) && !IsAnyHash(hash);
}


class CP2PClientTool
{
public:
	static void Encrypt(const char* src, std::string& dst);
	static void Decrypt(const char* src, std::string & dst);

	static bool GetSelfModuleVersion(std::wstring & strVersion);
	static bool GetFileVersion(LPCTSTR strFile, std::wstring & strVersion);
	static HMODULE GetModuleFromAddr(PVOID p);

	static bool FindFileW(const wchar_t* file)
	{
		HANDLE hfind = NULL;
		WIN32_FIND_DATAW wfd = {0};
		if(INVALID_HANDLE_VALUE != (hfind = FindFirstFileW(file, &wfd)))
		{
			FindClose(hfind);
			return true;
		}

		return false;
	}

	static bool FindFileA(const char* file)
	{
		HANDLE hfind = NULL;
		WIN32_FIND_DATAA wfd = {0};
		if(INVALID_HANDLE_VALUE != (hfind = FindFirstFileA(file, &wfd)))
		{
			FindClose(hfind);
			return true;
		}

		return false;
	}

	static int GetFileSize(const wchar_t* file)
	{
		int len = 0;
		FILE* f = NULL;
		if(0 == _wfopen_s(&f, file, L"rb") && f != NULL)
		{
			fseek(f, 0, SEEK_END);
			len = ftell(f);
			fclose(f);
		}

		return len;
	}

	static char* GetModuleDirectoryA()
	{
		static char dir[MAX_PATH] = {0};
		if(0 == dir[0] && 0 != ::GetModuleFileNameA(NULL, dir, sizeof(dir)))
		{
			char* p = strrchr(dir, '\\');
			if(0 != p) p[1] = 0;
		}
		
		return dir;
	}

	static wchar_t* GetModuleDirectoryW()
	{
		static wchar_t dir[MAX_PATH] = {0};
		if(0 == dir[0] && 0 != ::GetModuleFileNameW(NULL, dir, sizeof(dir)))
		{
			wchar_t* p = wcsrchr(dir, '\\');
			if(0 != p) p[1] = 0;
		}

		return dir;
	}

	static const wchar_t* Atoix(int i)
	{
		static wchar_t tmp[64] = {0};
		swprintf_s(tmp, sizeof(tmp)/sizeof(wchar_t), L"%d", i);
		return tmp;
	}

	static void WideToMulti(const wchar_t* wide, std::string &multi)
	{
		int len = WideCharToMultiByte(CP_ACP, 0, wide, (int)wcslen(wide), NULL, 0, NULL, NULL);
		if(len > 0)		// std::string
		{
			multi.resize(len);
			WideCharToMultiByte(CP_ACP, 0, wide, -1, &multi[0], len, NULL, NULL);
		}
	}

	static void MultiToWide(const char* multi, std::wstring &wide)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, multi, (int)strlen(multi), NULL, 0);
		if(len > 0)		// std::wstring 
		{
			wide.resize(len);
			MultiByteToWideChar(CP_ACP, 0, multi, -1, &wide[0], len);
		}
	}

	static void ToHex(unsigned char const *in, size_t len, char* out)
	{
		static const unsigned char hex_chars[] = "0123456789abcdef";
		for (unsigned char const* end = in + len; in < end; ++in)
		{
			*out++ = hex_chars[(*in) >> 4];
			*out++ = hex_chars[(*in) & 0xf];
		}

		*out = '\0';
	}

	static bool FromHex(char const *in, size_t len, unsigned char* out)
	{
		for (char const* end = in + len; in < end; ++in, ++out)
		{
			int t = HexToInt(*in);
			if (t == -1) return false;
			*out = t << 4;
			++in;
			t = HexToInt(*in);
			if (t == -1) return false;
			*out |= t & 15;
		}
		return true;
	}

	static const char* HashToString(const P2PHash_t & h)
	{
		static char buffer[41] ={0};
		ToHex(h.hash, 20, buffer);

		return buffer;
	}


private:
	static int HexToInt(char in)
	{
		if (in >= '0' && in <= '9') return int(in) - '0';
		if (in >= 'A' && in <= 'F') return int(in) - 'A' + 10;
		if (in >= 'a' && in <= 'f') return int(in) - 'a' + 10;
		return -1;
	}
};


class CLockObject
{
public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};


class CMutexObject : public CLockObject
{
public:
	CMutexObject()
	{
		m_lock = CreateMutex(0, false, NULL);
		assert(m_lock != NULL);
	}

	~CMutexObject()
	{
		CloseHandle(m_lock);
	}

	void Lock()
	{
		WaitForSingleObject(m_lock, INFINITE);
	}

	void Unlock()
	{
		ReleaseMutex(m_lock);
	}

private:
	HANDLE           m_lock;
};


class CCriticalSetionObject : public CLockObject
{
public:
	CCriticalSetionObject()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CCriticalSetionObject()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}


private:
	CRITICAL_SECTION m_cs;
};


class CBaseLockHandler
{
public:
	CBaseLockHandler(CLockObject & lock)
		: m_lock(lock)
	{
		m_lock.Lock();
	}


	virtual ~CBaseLockHandler()
	{
		m_lock.Unlock();
	}

private:
	CLockObject & m_lock;
};


class CAutoLockHandler
{
public:
	CAutoLockHandler(CLockObject & lock, bool islock)
		: m_lock(lock)
		, m_islock(islock)
	{
		if(m_islock) m_lock.Lock();
	}


	virtual ~CAutoLockHandler()
	{
		if(m_islock) m_lock.Unlock();
	}

private:
	CLockObject & m_lock;
	bool          m_islock;
};


class CVectorString : public std::vector<std::wstring>
{
public:
	void push_back(std::wstring str)
	{
		for(const_iterator it = begin(); it != end(); it++)
		{
			if(wcscmp(str.c_str(), it->c_str()) == 0)
			{
				return;
			}
		}

		std::vector<std::wstring>::push_back(str);
	}
};


#endif
