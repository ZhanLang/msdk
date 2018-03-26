#pragma once

namespace msdk{;

class CVerCmp
{
public:
	CVerCmp(LPCSTR lpszVer)
	{
		llVer = 0;
		MakeVersion(lpszVer);
	}
	
	CVerCmp(LPCWSTR lpszVer)
	{
		llVer = 0;
		MakeVersion(lpszVer);
	}

	//获取小版本号
	ULONG GetMinVersion()
	{
		return ulVer;
	}


	//获取主版本号
	ULONG GetMaxVersion()
	{
		return uhVer;
	}


	//获取版本号
	ULONGLONG GetVersion()
	{
		return llVer;
	}


	//比较版本
	static INT Compare(CVerCmp& _1, CVerCmp& _2)
	{
		if (_1.llVer == _2.llVer)
			return 0;
		
		return _1.llVer >_2.llVer ? 1 : -1;
	}

	static INT CompareStr(LPCSTR lpszV_1, LPCSTR lpszV_2)
	{
		CVerCmp _1(lpszV_1);
		CVerCmp _2(lpszV_2);
		return Compare(_1, _2);
	}

	static INT CompareStr(LPCWSTR lpszV_1, LPCWSTR lpszV_2)
	{
		CVerCmp _1(lpszV_1);
		CVerCmp _2(lpszV_2);
		return Compare(_1, _2);
	}

	//比较主版本
	static INT CompareMax(CVerCmp& _1, CVerCmp& _2)
	{
		if (_1.uhVer == _2.uhVer)
			return 0;

		return _1.uhVer >_2.uhVer ? 1 : -1;
	}

	static INT CompareStrMax(LPCSTR lpszV_1, LPCSTR lpszV_2)
	{
		CVerCmp _1(lpszV_1);
		CVerCmp _2(lpszV_2);
		return CompareMax(_1, _2);
	}

	//比较小版本
	static INT CompareMin(CVerCmp& _1, CVerCmp& _2)
	{
		if (_1.ulVer == _2.ulVer)
			return 0;

		return _1.ulVer >_2.ulVer ? 1 : -1;
	}

	static INT CompareStrMin(LPCSTR lpszV_1, LPCSTR lpszV_2)
	{
		CVerCmp _1(lpszV_1);
		CVerCmp _2(lpszV_2);
		return CompareMin(_1, _2);
	}

	union
	{
		ULONGLONG llVer;
		struct{
			union
			{
				ULONG ulVer;
				struct
				{
					USHORT sRevisionVer;
					USHORT sBuildVer;
				};
			};
			union
			{
				ULONG uhVer;
				struct
				{
					USHORT sMinorVer;
					USHORT sMajorVer;
				};
			};
		};
	} ;

private:
	BOOL MakeVersion(LPCSTR lpszVer)
	{
		CHAR strVer[MAX_PATH] = { 0 };
		strcpy_s(strVer, MAX_PATH, lpszVer);
		CHAR *next_token = NULL;
		CHAR *szCmd = strtok_s(strVer, ".,", &next_token);

		ULONGLONG uVer = 0;
		INT nPos = 3;
		while (szCmd != NULL && strlen(szCmd))
		{
			ULONGLONG nVer = atoi(szCmd);
			uVer |= nVer << (nPos * 16);
			if ( nPos == 0 )
				break;
			
			nPos--;
			szCmd = strtok_s(NULL, ".,", &next_token);
		}

		llVer = uVer;
		return TRUE;
	}


	BOOL MakeVersion(LPCWSTR lpszVer)
	{
		WCHAR strVer[MAX_PATH] = { 0 };
		wcscpy_s(strVer, MAX_PATH, lpszVer);
		WCHAR *next_token = NULL;
		WCHAR *szCmd = wcstok_s(strVer, L".,", &next_token);

		ULONGLONG uVer = 0;
		INT nPos = 3;
		while (szCmd != NULL && wcslen(szCmd))
		{
			ULONGLONG nVer = _wtoi(szCmd);
			uVer |= nVer << (nPos * 16);
			if ( nPos == 0 )
				break;

			nPos--;
			szCmd = wcstok_s(NULL, L".,", &next_token);
		}

		llVer = uVer;
		return TRUE;
	}
};

};//namespace msdk