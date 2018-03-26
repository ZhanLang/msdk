#include "stdafx.h"
#include "BaseDownloaderSetting.h"
#include "msupgradecode.h"


// ------------------------------------------------------------
// struct P2PHash_t


// ------------------------------------------------------------
// class CP2PClientTool
void CP2PClientTool::Encrypt(const char* src, std::string& dst)
{
	//此函数的实现用起来别扭，在算需要的Buffer长度时，需要先传个非空Buffer
	char lpDumpBuf[4] = {0};
	int iLen = MsUpgradeEncrypt(lpDumpBuf, 0, src);
	if (0 == iLen)
	{
		return;
	}

	_ASSERT(iLen < 0);
	iLen = -iLen;
	dst.resize(iLen);
	MsUpgradeEncrypt(dst.c_str(), iLen, src);
}

void CP2PClientTool::Decrypt(const char* src, std::string & dst)
{
	//此函数的实现用起来别扭，在算需要的Buffer长度时，需要先传个非空Buffer
	char lpDumpBuf[4];
	INT iLen = MsUpgradeDecrypt(lpDumpBuf, 0, src);
	if (0 == iLen)
	{
		return;
	}

	_ASSERT(iLen < 0);
	iLen = -iLen;
	dst.resize(iLen);
	MsUpgradeDecrypt(dst.c_str(), iLen, src);
}

// version ...
bool CP2PClientTool::GetSelfModuleVersion(std::wstring & strVersion)
{
	TCHAR strFile[MAX_PATH] = {0};

	extern BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID);
	GetModuleFileName(GetModuleFromAddr(DllMain), strFile, sizeof(strFile));
	assert(_tcslen(strFile) > 0);
	return GetFileVersion(strFile, strVersion);
}

bool CP2PClientTool::GetFileVersion(LPCTSTR strFile, std::wstring & strVersion)    
{
	DWORD dwVerSize = GetFileVersionInfoSize(strFile, 0);
	if(dwVerSize == 0)
		return false;

	std::wstring szVersionBuffer(dwVerSize, 0);
	if(GetFileVersionInfo(strFile, 0, dwVerSize, &szVersionBuffer[0]))
	{
		VS_FIXEDFILEINFO * pInfo;
		unsigned int nInfoLen;

		if(VerQueryValue(szVersionBuffer.c_str(), _T("\\"), (void**)&pInfo, &nInfoLen)) 
		{
			TCHAR tmp[32] ={0};
			swprintf_s(tmp, SIZEOFV(tmp), _T("%d.%d.%d.%d"), 
				HIWORD(pInfo->dwFileVersionMS), LOWORD(pInfo->dwFileVersionMS), 
				HIWORD(pInfo->dwFileVersionLS), LOWORD(pInfo->dwFileVersionLS));
			strVersion = tmp;
			return true;
		}
	}

	return false;
}

HMODULE CP2PClientTool::GetModuleFromAddr(PVOID p)
{
	MEMORY_BASIC_INFORMATION m = {0};
	VirtualQuery(p, &m, sizeof(MEMORY_BASIC_INFORMATION));
	return (HMODULE)m.AllocationBase;
}


// -------------------------------------------------------------
// class CMsBaseDownloaderSetting
int CMsBaseDownloaderSetting::SetOpt(int opt, void* data, size_t len)
{
	VariantType_t vt(data, len);
	switch(opt)
	{
	case Opt_P2PTimeOut:
		m_timeout = vt.ival;
		break;

	case Opt_P2PTolerate:
		m_limitsz = vt.ival > 0 ? (vt.ival * MIN_TOLERATE_DOWN_TIME) : -1;
		break;

	case Opt_P2PSavePath:
		m_savepath = vt.sval;
		if(m_savepath.size() > 0 && L'\\' != m_savepath[m_savepath.size() - 1])
		{
			m_savepath += L"\\";
		}
		break;

	case Opt_P2PMsgInterval:
		m_interval = vt.ival;
		return 0;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsBaseDownloaderSetting::SetOpt()," \
			"Unknown Setting ID (%d).", opt);
		break;
	}

	return 0;
}

int CMsBaseDownloaderSetting::GetOpt(int opt, void* data, size_t len)
{
	switch(opt)
	{
	case Opt_P2PTimeOut:
		*(int*)data = m_timeout;
		break;

	case Opt_P2PTolerate:
		*(int*)data = m_limitsz;
		break;

	case Opt_P2PSavePath:
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_savepath.c_str());
		break;

	case Opt_P2PMsgInterval:
		*(int*)data = m_interval;
		return 0;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsBaseDownloaderSetting::GetOpt()," \
			"Unknown Setting ID (%d).", opt);
		break;
	}

	return 0;
}
