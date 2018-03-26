#pragma once

#include <assert.h>

#include <regex> 
using namespace std::tr1;

#ifdef _UNICODE
typedef wcmatch	t_match;
typedef wregex	t_regex;
typedef wsmatch	t_wsmatch;
#else
typedef cmatch	t_match;
typedef regex	t_regex;
typedef smatch	t_wsmatch;
#endif

namespace match_strings
{



//直接通过正则表达式进行匹配，并且立即获取统计结果
//szText		需要匹配的文本
//szRegExpr		ECMAScript正则表达式表达式
//pdwTotal		输出数组地址
//dwSize		数组大小
//bNoCase		大小写不敏感
DWORD GetMatchCount( LPCTSTR szText, LPCTSTR szRegExpr, LPDWORD pdwTotal, DWORD dwSize, BOOL bNoCase = FALSE);

//多个正则表达式的内容匹配，用于统计多个表达式的匹配次数
class CRegexMatchStrings;


// 以下是具体实现

DWORD GetMatchCount( LPCTSTR szText, LPCTSTR szRegExpr, LPDWORD pdwTotal, DWORD dwSize, BOOL bNoCase)
{		
	assert(szText);
	assert(szRegExpr);
	assert(pdwTotal);

	t_match mr; 
	t_regex rx(szRegExpr, 
		bNoCase?(regex_constants::ECMAScript|regex_constants::icase):(regex_constants::ECMAScript)); 
	regex_constants::match_flag_type ft = regex_constants::match_default; 

	DWORD dwCount=0;
	LPCTSTR it = szText;
	LPCTSTR end = szText+lstrlen(szText);
	while(regex_search(it, end, mr, rx, ft))
	{
// #ifdef _DEBUG
// 		assert(dwSize<=mr.size());
// 		TRACE("size = %d\n", mr.size());
// 		USES_CONVERSION;
// 		TRACE("[%d] find = %s, \t next = %s\n", dwCount, T2A(mr[0].first), T2A(mr[0].second));
// #endif
		for(DWORD index = 0; index<dwSize && (index+1)<mr.size(); index++)
		{
			if(mr[index+1].matched)//下标从1开始的
				++(pdwTotal[index]);
		}			

		dwCount++;
		it = mr[0].second;			
	}		

	return dwCount;
}


//多个正则表达式的内容匹配
class CRegexMatchStrings
{
public:
	CRegexMatchStrings()
	{
		Clear();
	}

	//清零
	void Clear()
	{
		m_strRules = _T("");
		m_arTotals.RemoveAll();
	}

	//加入一条正则表达式，返回一个索引，可以通过这个索引对统计结果进行检索
	DWORD AddRegExpr(LPCTSTR szRegExpr)
	{
		//"(a)|(b)|(c)"
		DWORD dwCount = m_arTotals.GetSize();
		if(dwCount==0)
			m_strRules = szRegExpr;
		else if(dwCount==1)
		{
			CString str1 = m_strRules;
			m_strRules.Format(_T("(%s)|(%s)"), str1, szRegExpr);
		}
		else
		{
			m_strRules += _T("|(");
			m_strRules += szRegExpr;
			m_strRules += _T(")");
		}
		m_arTotals.Add(0);
		return m_arTotals.GetSize();
	}

	//匹配完成，并且获取匹配的结果总规则条数
	DWORD GetMatchResult(LPCTSTR szText)
	{
		GetMatchResultEx(szText, m_arTotals.GetData(), m_arTotals.GetSize());
		return  m_arTotals.GetSize();
	}

	//方便直接获取索引对应的统计频率数
	const DWORD& operator[] (int nIndex) const
	{
		return m_arTotals[nIndex];
	}

	//一个快速获取匹配，并立即获得结果的方法
	//注意:
	//		1、后面是填希望获取的多个规则的数组
	//		2、和 GetMatchResult 返回值得含义不同！这个是返回的所有匹配的频率总和
	DWORD GetMatchResultEx(LPCTSTR szText, LPDWORD pdwTotal, DWORD dwSize)
	{
		assert(dwSize<= (DWORD)(m_arTotals.GetSize()));
		return GetMatchCount(szText, m_strRules, pdwTotal, dwSize);
	}
public:
	CString m_strRules;
	CSimpleArray<DWORD> m_arTotals;
};


/* demo

//case 1:

DWORD dwTotals[3] = {0};
dwTotals[2] = -1;
match_strings::GetMatchCount(_T("abcdaabcdaaab"), _T("(ab)|(cd)"), dwTotals, sizeof(dwTotals)/sizeof(dwTotals[0]));
//ms.GetMatchCount(_T("abcdaabcdaaab"), _T("(a)|(b)|(c)"), dwTotals, sizeof(dwTotals)/sizeof(dwTotals[0]));

//case 2:

match_strings::CRegexMatchStrings ms;
ms.AddRegExpr(_T("ab"));
ms.AddRegExpr(_T("cd"));
int iRet = ms.GetMatchResult(_T("abcdaabcdaaab"));
TRACE("being txt = %s\n", _T("abcdaabcdaaab"));
for(int i=0; i<iRet; i++)
{
	TRACE("[%d] find = %d\n", i, ms[i]);
}
*/


}// namespace match_strings