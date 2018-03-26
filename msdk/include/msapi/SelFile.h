#pragma once

#include <string>


/*
msdk::msapi::SEL_FILTER Filter[] = 
	{  
	{ _T("ISO文件"),         L"*.iso" },
	{ _T("NRG文件"),         L"*.nrg" },
	{ _T("MDS文件"),         L"*.mds" },
	{ _T("MDF文件"),         L"*.mdf" },
	{ _T("IMG文件"),         L"*.img" },
	{ _T("CUE文件"),         L"*.cue" },
	{ _T("CCD文件"),         L"*.ccd" },
	{ _T("所有支持的文件"), NULL},
	};

TCHAR szDocPath[MAX_PATH] = { 0 };
SHGetSpecialFolderPath(NULL, szDocPath, CSIDL_MYDOCUMENTS ,FALSE);


msdk::msapi::CSelFile selFile;
selFile.SelPath(Filter, _countof(Filter) ,szDocPath);
selFile.GetPath();
*/
namespace msdk{;
namespace msapi{;

typedef struct SEL_FILTER
{
	const TCHAR *name;
	const TCHAR *filter;
}SEL_FILTER; 


class CSelFile
{
public:
	CSelFile()
	{
		memset(m_SelFile, 0, sizeof(m_SelFile));
	}

	LPCTSTR GetPath()
	{
		return m_SelFile;
	}

	BOOL SelPath(SEL_FILTER* filter, DWORD dwCount, LPCTSTR lpszDefPath , HWND hWnd = NULL)
	{
		std::basic_string<TCHAR> fileFilter;

		BOOL bAll = FALSE;
		std::basic_string<TCHAR> strAll;
		for ( DWORD dwLoop = 0 ; dwLoop < dwCount ; dwLoop++)
		{

			if ( filter[dwLoop].filter )
			{
				strAll.append(filter[dwLoop].filter);
				strAll.append(_T(";"));
			}
			else
			{
				fileFilter = filter[dwLoop].name;
				bAll = TRUE;
				break;
			}
		}

		if ( bAll )
		{
			fileFilter.append(_T("\1"));
			fileFilter.append(strAll);
			fileFilter.append(_T("\1"));
		}


		for ( DWORD dwLoop = 0 ; dwLoop < dwCount ; dwLoop++)
		{
			if ( filter[dwLoop].filter && filter[dwLoop].name)
			{
				fileFilter.append(filter[dwLoop].name);
				fileFilter.append(_T("("));
				fileFilter.append(filter[dwLoop].filter);
				fileFilter.append(_T(")"));
				fileFilter.append(_T("\1"));
				fileFilter.append(filter[dwLoop].filter);
				fileFilter.append(_T("\1"));
			}
		}

		
		fileFilter = replace<TCHAR>(fileFilter, '\1', '\0');

		TCHAR szBuffer[MAX_PATH] = {0};   
		OPENFILENAME ofn= {0};

		ofn.lStructSize = sizeof(ofn);   
		ofn.hwndOwner = hWnd;   
		ofn.lpstrFilter = fileFilter.c_str();
		ofn.lpstrInitialDir = lpszDefPath;	//默认的文件路径   
		ofn.lpstrFile = szBuffer;			//存放文件的缓冲区   
		ofn.nMaxFile = sizeof(szBuffer)/sizeof(*szBuffer);   
		ofn.nFilterIndex = 0;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ; //标志如果是多选要加上OFN_ALLOWMULTISELECT  

		if ( GetOpenFileName(&ofn) )
		{
			_tcscpy_s(m_SelFile, _countof(m_SelFile), szBuffer);
			return TRUE;
		}
		
		return FALSE;
	}


public:
	TCHAR m_SelFile[MAX_PATH];


private:

	template< class E >
	std::basic_string<E> replace(const std::basic_string<E>& str, E _o, E _n) 
	{
		std::basic_string<E> strResult = str;
		std::basic_string<E>::size_type i = std::basic_string<E>::npos;

		while( ( i = strResult.find(_o) ) != std::basic_string<E>::npos )
			strResult[i] = _n;

		return strResult;
	}
};

};};