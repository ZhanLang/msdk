
//	internal.h

#ifndef _____cloudstore_internal_h____
#define _____cloudstore_internal_h____

#include <string>
using namespace std;


#pragma comment(lib, "Version.lib")

//////////////////////////////////////////////////////////////////////////

#ifndef tstring

#if defined(UNICODE) || defined(_UNICODE)
#define tstring	std::wstring
#else
#define tstring	std::string
#endif

#endif

static BOOL GetFileVerAndCompany(LPCTSTR path, tstring& ver, tstring& company)
{
	tstring ret;
	DWORD dwHandle = 0;
	DWORD dwLen = GetFileVersionInfoSize(path, &dwHandle);
	if (dwLen <= 0)
		return FALSE;
	BYTE *pVersionInfo = new BYTE[dwLen];
	if (!::GetFileVersionInfo(path, dwHandle, dwLen, pVersionInfo))
	{
		delete[] pVersionInfo;
		return FALSE;
	}

	VS_FIXEDFILEINFO *pFixed = NULL;
	UINT uLen = 0;
	if (!VerQueryValue(pVersionInfo, _T("\\"), (LPVOID*)&pFixed, &uLen))
	{
		delete[] pVersionInfo;
		return FALSE;
	}
	TCHAR szVer[MAX_PATH] = _T("");
	StringCchPrintf(szVer, MAX_PATH, _T("%u.%u.%u.%u"),
		HIWORD(pFixed->dwFileVersionMS), LOWORD(pFixed->dwFileVersionMS),
		HIWORD(pFixed->dwFileVersionLS), LOWORD(pFixed->dwFileVersionLS));
	ver = szVer;

	// Retrieve the first language and character-set identifier  
	UINT nQuerySize;  
	DWORD* pTransTable;  
	if ( ::VerQueryValue(pVersionInfo, _T("\\VarFileInfo\\Translation"),  
		(void **)&pTransTable, &nQuerySize) )  
	{
		DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0])); 

		// Query version information value  
		UINT nQuerySize;  
		LPVOID lpData;

		TCHAR szBlockName[64] = _T("");
		StringCchPrintf(szBlockName, _countof(szBlockName), _T("\\StringFileInfo\\%08lx\\CompanyName"),
			dwLangCharset);

		if ( ::VerQueryValue(pVersionInfo, szBlockName,   
			&lpData, &nQuerySize) )
		{
			company = (LPCTSTR)lpData; 
		}
	}

	delete[] pVersionInfo;
	return TRUE;
}

#endif