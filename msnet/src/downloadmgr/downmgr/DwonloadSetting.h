#pragma once
#include <map>
#include "dwonload.h"
#include "util/xbuffer.hpp"

//…Ë÷√¿‡
class CDwonloadSetting:
	public IDwonloadSetting
{
public:
	CDwonloadSetting(void);
	~CDwonloadSetting(void);
public:
	HRESULT SetOption(DWORD option,LPVOID lpdata,DWORD dwLen);
	HRESULT GetOption(DWORD option, OUT LPVOID* lpData, OUT DWORD&dwLen);
public:
	DWORD GetMaxTaskNum();
	DWORD GetMsgIdMask();
	const IAT_ProxyInfo* GetProxyInfo() const;
	
private:
	DWORD			m_dwTotalTaskNum;
	IAT_ProxyInfo   m_IAT_ProxyInfo;
	DWORD			m_dwNotifyMsgIdMask;

	typedef std::map<DWORD,XBuffer> CDLOptionMap;
	CDLOptionMap   m_optionMap;
};
