#pragma once

#include <Tlhelp32.h>

#include <string>
#include <map>
#include <vector>
#include <util/str_encode_embed.h>
namespace msdk{;
namespace msapi{;
//枚举安全软件
enum //序号和软件扫描引擎中的一致
{
	Antivirus_Rav	  = 1,	//瑞星杀毒
	Antivirus_King	  = 8,	//金山杀毒

	Antivirus_360Safe = 1242,	//360安全卫士
	Antivirus_360Sd	  = 1243,	//360杀毒
	Antivirus_Biadu	  = 1244,	//百度卫士
	Antivirus_BaiduSd = 1245,	//百度杀毒
	Antivirus_QQMgr	  = 1246,	//腾讯电脑管家

	Antivirus_HipsDaemon = 1247,//火绒
	Antivirus_2345Safe = 1248	//2345安全卫士
};

class CEnumAntivirus
{
public:
	std::map<int, std::wstring> m_ProcessMap;
	CEnumAntivirus()
	{
		m_ProcessMap[Antivirus_Rav]			= ustr_encode_embed<14,'r','a','v','m','o','n','d','.','e','x','e'>();
		m_ProcessMap[Antivirus_King]		= ustr_encode_embed<14,'k','x','e','s','c','o','r','e','.','e','x','e'>();
		m_ProcessMap[Antivirus_360Safe]		= ustr_encode_embed<12,'Z','h','u','D','o','n','g','F','a','n','g','Y','U','.','e','x','e'>();
		m_ProcessMap[Antivirus_360Sd]		= ustr_encode_embed<13,'3','6','0','s','d','u','p','d','.','e','x','e'>();
		m_ProcessMap[Antivirus_Biadu]		= ustr_encode_embed<13,'B','a','i','d','u','A','n','S','v','c','.','e','x','e'>();
		m_ProcessMap[Antivirus_BaiduSd]		= ustr_encode_embed<13,'B','a','i','d','u','S','d','S','v','c','.','e','x','e'>();
		m_ProcessMap[Antivirus_QQMgr]		= ustr_encode_embed<11,'Q','Q','P','C','R','T','P','.','e','x','e'>();//QQPCRTP.exe
		m_ProcessMap[Antivirus_HipsDaemon]	= ustr_encode_embed<11,'H','i','p','s','D','a','e','m','o','n','.','e','x','e'>();//HipsDaemon.exe
		m_ProcessMap[Antivirus_2345Safe]	= ustr_encode_embed<11,'2','3','4','5','S','a','f','e','S','v','c','.','e','x','e'>();//2345SafeSvc.exe
	}

	BOOL Enum( std::vector<int>& antivirus)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (INVALID_HANDLE_VALUE == hSnapshot) 
			return FALSE;

		PROCESSENTRY32W pe = { sizeof(PROCESSENTRY32W) };
		for (BOOL fOk = Process32FirstW(hSnapshot, &pe); fOk; fOk = Process32NextW(hSnapshot, &pe)) 
		{
			std::map<int, std::wstring>::iterator it = m_ProcessMap.begin();
			for (  ; it != m_ProcessMap.end() ; it++ )
			{
				if ( 0 == wcsicmp(pe.szExeFile, it->second.c_str()) )
					antivirus.push_back(it->first);
			}
		}
		CloseHandle(hSnapshot);

		return TRUE;
	}

	//以;号隔开
	std::basic_string<TCHAR> Enum( )
	{
		std::basic_string<TCHAR> sRet;
		std::vector<int> antivirus;
		if ( Enum( antivirus ) )
		{
			std::vector<int>::iterator it = antivirus.begin();
			for ( ; it != antivirus.end() ; it++)
			{
				TCHAR sFormat[ MAX_PATH ] = { 0 };
				_stprintf(sFormat, _T("%d;"), *it);
				sRet.append( sFormat );
			}
		}

		return sRet;
	}

	BOOL Has( INT antivirus) 
	{
		if ( m_ProcessMap.find( antivirus ) == m_ProcessMap.end() )
			return FALSE;

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (INVALID_HANDLE_VALUE == hSnapshot) 
			return FALSE;

		BOOL bFind = FALSE;

		std::wstring s = m_ProcessMap.find(antivirus)->second;
		PROCESSENTRY32W pe = { sizeof(PROCESSENTRY32W) };
		for (BOOL fOk = Process32FirstW(hSnapshot, &pe); fOk; fOk = Process32NextW(hSnapshot, &pe)) 
		{
			if ( 0 == wcsicmp(pe.szExeFile, s.c_str()) )
			{
				bFind = TRUE;
				break;
			}
		}
		CloseHandle(hSnapshot);

		return bFind;
	}
};};};