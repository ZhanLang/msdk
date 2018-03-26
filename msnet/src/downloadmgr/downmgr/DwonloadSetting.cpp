#include "StdAfx.h"
#include "DwonloadSetting.h"

CDwonloadSetting::CDwonloadSetting(void)
{
	m_dwTotalTaskNum = 5;
	m_IAT_ProxyInfo.accessType= IAT_NO_PROXY;
	m_IAT_ProxyInfo.szProxyName[0] = '\0';
	m_IAT_ProxyInfo.szProxyPwd[0] = '\0';
	m_IAT_ProxyInfo.szProxyUser[0] = '\0';
	m_dwNotifyMsgIdMask = DefaultNotifyMsgMask;
}

CDwonloadSetting::~CDwonloadSetting(void)
{
	m_optionMap.clear();
}

HRESULT CDwonloadSetting::SetOption(DWORD option,LPVOID lpdata,DWORD dwLen)
{
	switch(option)
	{
	case DLO_MaxTaskNum_Option:
		{
			if (lpdata && dwLen == sizeof(DWORD))
			{
				m_dwTotalTaskNum = *((DWORD*)(lpdata));
				return S_OK;
			}
			break;
		}
	case DLO_NotifyMsgMask:
		{
			if (lpdata && dwLen == sizeof(DWORD))
			{
				m_dwNotifyMsgIdMask = *((DWORD*)(lpdata));
				return S_OK;
			}
			break;
		}
	case DLO_IAT_Option:
		{
			if (lpdata&& dwLen == sizeof(m_IAT_ProxyInfo))
			{
				memcpy(&m_IAT_ProxyInfo,lpdata,dwLen);
			}
			break;
		}
	
	default:
		{
			XBuffer buf;
			buf.SetBuffer(static_cast<unsigned char*>(lpdata),dwLen);
			m_optionMap[option] = buf;
		}
	}

	return E_FAIL;
}

HRESULT CDwonloadSetting::GetOption(DWORD option, OUT LPVOID* lpData, OUT DWORD&dwLen)
{
	RASSERT(lpData,E_INVALIDARG);

	switch(option)
	{
	case DLO_MaxTaskNum_Option:
		{
			LPDWORD* lppDw = (LPDWORD*)lpData;
			*lpData= (&m_dwTotalTaskNum);
			dwLen = sizeof(DWORD);
			return S_OK;
		}
	case DLO_NotifyMsgMask:
		{
			LPDWORD* lppDw = (LPDWORD*)lpData;
			*lpData= (&m_dwNotifyMsgIdMask);
			dwLen = sizeof(DWORD);
			break;
		}
	case DLO_IAT_Option:
		{
			*lpData = &m_IAT_ProxyInfo;
			dwLen = sizeof(m_IAT_ProxyInfo);
			return S_OK;
		}
	default:
		{
			CDLOptionMap::iterator it = m_optionMap.find(option);
			RASSERT(it != m_optionMap.end(),E_NOTIMPL);
			return it->second.GetRawBuffer((UCHAR**)(lpData),&dwLen) ? S_OK:E_FAIL;
		}
	}
	return S_OK;
}

DWORD CDwonloadSetting::GetMaxTaskNum()
{
	return m_dwTotalTaskNum; 
}

DWORD CDwonloadSetting::GetMsgIdMask()
{
	return m_dwNotifyMsgIdMask;
}

const IAT_ProxyInfo* CDwonloadSetting::GetProxyInfo() const 
{
	return &m_IAT_ProxyInfo;
}
