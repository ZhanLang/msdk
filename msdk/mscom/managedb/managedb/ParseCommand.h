#pragma once
#include <map>
#include <util/utility.h>
#include <vector>
#include <string>
#ifndef fstring
#	ifdef UNICODE
#		define fstring std::wstring
#	else
#		define fstring std::string
#	endif//ifdef UNICODE
#endif//#ifndef fstring

#ifndef CmdSeps
#	define CmdSeps	   _T("|;")
#endif

#ifndef CmdSepsArg
#	define CmdSepsArg _T("=")
#endif

//说明：统一的解析参数帮助类
//在使用参数的时候使用 “|”作为参数分割符,“=”作为值分割符
//例如：rul=www.baidu.com | savepath=c:\download\dl.exe | mode=http
//优点：参数之间没有顺序关系，使用起来方便
//缺点：不支参数的统一处理，限定于现有模式，但也满足基本需求

class CParseCommand
{
public:
    CParseCommand(BOOL bParse = FALSE)
    {
		if (bParse)
		{
			ParseCmd();
		}
    }

    BOOL ParseCmd(LPCTSTR lpszCmd)
    {
        RASSERT(lpszCmd, FALSE);
		fstring strCmd = lpszCmd;

		strCmd.erase(0,strCmd.find('\"') + 1);
		m_str_filepath = strCmd.substr(0,strCmd.find('\"'));
		strCmd.erase(0, strCmd.find('\"') + 1);

        m_commandMap.clear();
        
        TCHAR seps[]   = CmdSeps;
        TCHAR *next_token = NULL;

        TCHAR *szCmd = _tcstok_s((LPTSTR)strCmd.c_str(), seps, &next_token);
        RASSERT(szCmd, FALSE);
		
		//for vs2003
		std::vector<fstring> argv;
        while( szCmd != NULL )
        {
            if(_tcslen(szCmd) > 0)
				argv.insert(argv.end(),fstring(szCmd));
            
            szCmd = _tcstok_s( NULL, seps, &next_token );
        }

		std::vector<fstring>::iterator it= argv.begin();
		while(it != argv.end())
		{
			ParseArg(it->c_str());
			it = argv.erase(it);
		}
        return TRUE;
    }

    BOOL IsExist(LPCTSTR lpszCmd)
    {
        RASSERT(lpszCmd, FALSE);
        return m_commandMap.find(lpszCmd) != m_commandMap.end();
    }

    DWORD GetArgCount()
    {
        return static_cast<DWORD>(m_commandMap.size());
    }

    LPCTSTR GetArg(LPCTSTR strName , LPCTSTR szDefault = _T(""))
    {
        CCommandMap::iterator it = m_commandMap.find(strName);
        RASSERT(it != m_commandMap.end(), szDefault);
        return it->second.c_str();
    }
	
	LPCTSTR GetExeFilePath()
	{
		return m_str_filepath.c_str();
	}

    INT		GetArgInt(LPCTSTR strName , INT nDefault = 0)
    {
        LPCTSTR szArg = GetArg(strName, NULL);
        if (szArg)
        {
            return _ttoi(szArg);
        }
        return nDefault;
    }

	//构造命令行

	VOID SetArg(LPCTSTR lpszName, LPCTSTR lpszValue = NULL)
	{
		if (lpszName)
		{
			m_commandMap.insert(m_commandMap.end(), CCommandMap::value_type(lpszName, lpszValue ? lpszValue : _T("")));
		}
	}

	VOID SetIntArg(LPCTSTR lpszName, INT nValue = 0)
	{
		if (lpszName)
		{
			TCHAR szFormat[MAX_PATH] = {0};
			_stprintf_s(szFormat, MAX_PATH,_T("%d"), nValue); 
			m_commandMap.insert(m_commandMap.end(), CCommandMap::value_type(lpszName, szFormat ));
		}
	}


	//获取格式化后的命令行
	fstring GetCmdLine()
	{
		fstring strCmd;
		CCommandMap::iterator it = m_commandMap.begin();
		for (; it != m_commandMap.end(); ++it)
		{
			strCmd += it->first;
			strCmd += _T("=");
			strCmd += it->second;
			strCmd += _T("|");
		}

		return strCmd;
	}

	VOID Clear()
	{
		m_commandMap.clear();
	}
private:
    BOOL ParseCmd()
    {
        fstring strCmd = GetCommandLine();
       
		//strCmd.
        return ParseCmd(strCmd.c_str());
    }

    VOID ParseArg(LPCTSTR lpszAag)
    {
        RASSERTV(lpszAag);
        fstring szArg = lpszAag;
        TCHAR seps[]   = CmdSepsArg;
        TCHAR *next_token = NULL;
        TCHAR *szCmd = _tcstok_s((LPTSTR)szArg.c_str(), seps, &next_token);
        RASSERTV(szCmd);
		fstring sa = remove_non_lws(szCmd,' ');
		RASSERTV(sa.length());
        TCHAR *lpArg = _tcstok_s( NULL, seps, &next_token );
        m_commandMap.insert(m_commandMap.end(), CCommandMap::value_type(sa, lpArg ? lpArg : _T("")));
    }

	 
	fstring remove_non_lws( const TCHAR* s ,TCHAR c)
	{
		fstring strResult = s;
		fstring::size_type i = fstring::npos;
		while( ( i = strResult.find(c) ) != fstring::npos )
			strResult.erase( i, 1 );

		return strResult;
	}
	
	struct less_str_nocase{
		bool operator() (const fstring& l,const fstring& r) const{
			return _tcsicmp(l.c_str() , r.c_str()) < 0;
		}
	};

    typedef std::map<fstring, fstring,less_str_nocase> CCommandMap;
	fstring m_str_filepath;
    CCommandMap m_commandMap;
};