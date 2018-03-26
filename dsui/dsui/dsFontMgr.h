/************************************************************************/
/* 
Author:

	lourking. (languang).All rights reserved.

Create Time:

	1,121th,2014

Module Name:

	IdsFontMgr.h 

Abstract: ◊÷ÃÂ µ¿‡


*/
/************************************************************************/

#ifndef __DSFONTMGR_H__
#define __DSFONTMGR_H__

#include <map>
using namespace std;

HFONT CreatePointFontIndirect(const LOGFONT* lpLogFont, HDC hDC = NULL);


HFONT CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, HDC hDC = NULL, bool bBold = false, bool bItalic = false, bool bUnderline = false);




#define INVALID_FONTID	-1
#define FONTRES_EXIST -2

class dsFontMgr:public IdsFontMgr
{
public:


	typedef map<int,HFONT> MAP_FONT;
	typedef map<CString,int> MAP_FONTMARK;//here


public:
	static dsFontMgr* m_dsFontMgr;
	MAP_FONT m_mapFont;
	MAP_FONTMARK m_mapFontMark;

public:

	~dsFontMgr()
	{
		ReleaseAllFont();
	}

	static dsFontMgr *getInstance()
	{
		if( !m_dsFontMgr )
		{
			m_dsFontMgr = new dsFontMgr;
		}
		return m_dsFontMgr;
	}

	virtual int NewFont(__in LPCWSTR lpszFontKeyName, HFONT hFont, BOOL bForceReplace){

		if(NULL == lpszFontKeyName || NULL == hFont)
			return INVALID_FONTID;

		int nNewFontID = GetFontID(lpszFontKeyName);

		if(INVALID_FONTID == nNewFontID)
			nNewFontID = CreateNewFontID();
		else if(bForceReplace)
			ReleaseFont(nNewFontID);
		else
			return FONTRES_EXIST;

		if(INVALID_FONTID == nNewFontID)
			return INVALID_FONTID;


		MAP_FONT::_Pairib pib = m_mapFont.insert(make_pair(nNewFontID, hFont));

		if(pib.second)
		{
			MAP_FONTMARK::_Pairib pibMark = m_mapFontMark.insert(make_pair(lpszFontKeyName, nNewFontID));

			if (!pibMark.second)
			{
				m_mapFont.erase(pib.first);
				::DeleteObject(hFont);

				_ASSERT(FALSE);
			}
			else
				return nNewFontID;
		}
		else{
			::DeleteObject(hFont);
			_ASSERT(FALSE);
		}

		return INVALID_FONTID;
	}

	virtual int NewFont(__in LPCWSTR lpszFontKeyName, int nPointSize, __in LPCWSTR lpszFaceName,bool bBold, bool bItalic, bool bForceReplace){
	
		return NewFont(lpszFontKeyName, nPointSize, lpszFaceName, bBold, bItalic, FALSE, bForceReplace);
	}

	virtual int NewFont(__in LPCWSTR lpszFontKeyName, int nPointSize, __in LPCWSTR lpszFaceName,bool bBold, bool bItalic, bool bUnderline, bool bForceReplace){
		if(NULL == lpszFontKeyName)
			return INVALID_FONTID;

		int nNewFontID = GetFontID(lpszFontKeyName);

		if(INVALID_FONTID == nNewFontID)
			nNewFontID = CreateNewFontID();
		else if(bForceReplace)
			ReleaseFont(nNewFontID);
		else
			return FONTRES_EXIST;

		if(INVALID_FONTID == nNewFontID)
			return INVALID_FONTID;

		nPointSize = nPointSize * 10;

		HFONT hFont = CreatePointFont(nPointSize, lpszFaceName, NULL, bBold, bItalic, bUnderline);


		if(NULL != hFont){

			MAP_FONT::_Pairib pib = m_mapFont.insert(make_pair(nNewFontID, hFont));

			if(pib.second)
			{
				MAP_FONTMARK::_Pairib pibMark = m_mapFontMark.insert(make_pair(lpszFontKeyName, nNewFontID));

				if (!pibMark.second)
				{
					m_mapFont.erase(pib.first);
					::DeleteObject(hFont);

					_ASSERT(FALSE);
				}
				else
					return nNewFontID;
			}
			else{
				::DeleteObject(hFont);
				_ASSERT(FALSE);
			}
		}

		return INVALID_FONTID;
	}
	virtual int GetFontID(__in LPCWSTR lpszFontKeyName){
		if(NULL == lpszFontKeyName)
			return INVALID_FONTID;

		MAP_FONTMARK::iterator it = m_mapFontMark.find(lpszFontKeyName);

		if(it != m_mapFontMark.end())
		{
			return it->second;
		}
		return INVALID_FONTID;
	}


	inline HFONT GetFont(__in LPCWSTR lpszFontKeyName){
		return GetFont(GetFontID(lpszFontKeyName));
	}

	virtual HFONT GetFont(__in int nFontID){
		MAP_FONT::iterator it = m_mapFont.find(nFontID);

		if (it != m_mapFont.end())
		{
			return it->second;
		}

		return NULL;
	}

	inline BOOL ReleaseFont(LPCWSTR lpszFontKeyName){
			return ReleaseFont(GetFontID(lpszFontKeyName));
	}


	BOOL ReleaseFont(int nFontID){
		if(nFontID < 0)
			return FALSE;

		MAP_FONT::iterator it = m_mapFont.find(nFontID);
		if(it != m_mapFont.end())
		{
			MAP_FONTMARK::iterator itMark = m_mapFontMark.begin();
			for( ; itMark != m_mapFontMark.end(); itMark++ )
			{
				if( itMark->second == nFontID )
				{
					m_mapFontMark.erase(itMark);
					break;
				}
			}

			::DeleteObject(it->second);
			m_mapFont.erase(it);
			return TRUE;
		}

		return FALSE;
	}

	void ReleaseAllFont(){
		m_mapFontMark.clear();

		MAP_FONT::iterator it = m_mapFont.begin();
		while (it != m_mapFont.end())
		{
			::DeleteObject(it->second);
			it ++;
		}

		m_mapFont.clear();
	}


private:

	inline int CreateNewFontID(){
		return m_mapFontMark.size();
	}



};

#endif /*__DSFONTMGR_H__*/