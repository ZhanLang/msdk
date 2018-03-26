#pragma once
#include <string>
#include <map>
#include <vector>


#include "luahost/ILuaHost.h"
#include "luahost/luahost.h"


#ifndef CmdSeps
#	define CmdSeps	   "|"
#endif

#ifndef CmdSepsArg
#	define CmdSepsArg "="
#endif

class CParseCommandToLua
{
public:

	static BOOL BindToLua(ILuaVM* pLuaVm);

	CParseCommandToLua(bool bParse = false)
	{
		if (bParse)
		{
			ParseCmd();
		}
	}

	bool ParseCmd(LPCSTR lpszCmd)
	{
		RASSERT(lpszCmd, false);
		std::string strCmd = lpszCmd;

		strCmd.erase(0,strCmd.find('\"') + 1);
		m_str_filepath = strCmd.substr(0,strCmd.find('\"'));
		strCmd.erase(0, strCmd.find('\"') + 1);

		m_commandMap.clear();

		char seps[]   = CmdSeps;
		char *next_token = NULL;

		char *szCmd = strtok_s((LPSTR)strCmd.c_str(), seps, &next_token);
		RASSERT(szCmd, false);

		//for vs2003
		std::vector<std::string> argv;
		while( szCmd != NULL )
		{
			if(strlen(szCmd) > 0)
				argv.insert(argv.end(),std::string(szCmd));

			szCmd = strtok_s( NULL, seps, &next_token );
		}

		std::vector<std::string>::iterator it= argv.begin();
		while(it != argv.end())
		{
			ParseArg(it->c_str());
			it = argv.erase(it);
		}
		return true;
	}

	bool IsExist(LPCSTR lpszCmd)
	{
		RASSERT(lpszCmd, FALSE);
		return m_commandMap.find(lpszCmd) != m_commandMap.end();
	}

	DWORD GetArgCount()
	{
		return static_cast<DWORD>(m_commandMap.size());
	}

	LPCSTR GetArg(LPCSTR strName)
	{
		return GetArgDef(strName, "");
	}

	LPCSTR GetArgDef(LPCSTR strName , LPCSTR szDefault)
	{
		CCommandMap::iterator it = m_commandMap.find(strName);
		RASSERT(it != m_commandMap.end(), szDefault);
		return it->second.c_str();
	}

	LPCSTR GetExeFilePath()
	{
		return m_str_filepath.c_str();
	}

	INT		GetArgIntDef(LPCSTR strName , INT nDefault)
	{
		LPCSTR szArg = GetArgDef(strName, NULL);
		if (szArg)
		{
			return atoi(szArg);
		}
		return nDefault;
	}

	INT		GetArgInt(LPCSTR strName)
	{
		return GetArgIntDef(strName, 0);
	}

	//构造命令行

	VOID SetArg(LPCSTR lpszName, LPCSTR lpszValue = NULL)
	{
		if (lpszName)
		{
			m_commandMap.insert(m_commandMap.end(), CCommandMap::value_type(lpszName, lpszValue ? lpszValue : ""));
		}
	}

	VOID SetIntArg(LPCSTR lpszName, INT nValue = 0)
	{
		if (lpszName)
		{
			char szFormat[MAX_PATH] = {0};
			sprintf_s(szFormat, MAX_PATH, "%d", nValue); 
			m_commandMap.insert(m_commandMap.end(), CCommandMap::value_type(lpszName, szFormat ));
		}
	}


	//获取格式化后的命令行
	std::string GetCmdLine()
	{
		std::string strCmd;

		if (!m_str_filepath.empty())
		{
			strCmd += "\"";
			strCmd += m_str_filepath;
			strCmd += "\" ";
		}
		
		CCommandMap::iterator it = m_commandMap.begin();
		for (; it != m_commandMap.end(); ++it)
		{
			strCmd += it->first;
			strCmd += "=";
			strCmd += it->second;
			strCmd += "|";
		}

		return strCmd;
	}

	VOID Clear()
	{
		m_commandMap.clear();
	}
private:
	bool ParseCmd()
	{
		std::string strCmd = GetCommandLineA();

		//strCmd.
		return ParseCmd(strCmd.c_str());
	}

	void ParseArg(LPCSTR lpszAag)
	{
		RASSERTV(lpszAag);
		std::string szArg = lpszAag;
		std::string value;
		LPSTR lpszTemp =(LPSTR)strstr(szArg.c_str(), CmdSepsArg);
		if(lpszTemp)
		{
			lpszTemp[0] = 0;
			value = lpszTemp+1;
		}

		m_commandMap.insert(m_commandMap.end(), CCommandMap::value_type(remove_non_lws(szArg.c_str(), ' '), value ));
	}


	std::string remove_non_lws( const char* s ,char c)
	{
		std::string strResult = s;
		std::string::size_type i = std::string::npos;
		while( ( i = strResult.find(c) ) != std::string::npos )
			strResult.erase( i, 1 );

		return strResult;
	}

	struct less_str_nocase{
		bool operator() (const std::string& l,const std::string& r) const{
			return stricmp(l.c_str() , r.c_str()) < 0;
		}
	};

	typedef std::map<std::string, std::string,less_str_nocase> CCommandMap;
	std::string m_str_filepath;
	CCommandMap m_commandMap;
};