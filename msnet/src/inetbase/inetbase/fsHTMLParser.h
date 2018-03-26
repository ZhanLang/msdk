/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSHTMLPARSER_H__AE1402E9_034A_41A0_8BD2_74430D5938AB__INCLUDED_)
#define AFX_FSHTMLPARSER_H__AE1402E9_034A_41A0_8BD2_74430D5938AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <windows.h>
#include <fsString.h>
#include "../list.h"

enum fsLinkRelType
{
	LRT_STYLESHEET,	
	LRT_UNKNOWN		
};

enum fsMetaHttpEquivType
{
	MHET_REFRESH,	
	MHET_UNKNOWN,	
};

class fsHTMLParser  
{
public:
	void ReplaceFrameUrl(int iIndex, LPCSTR pszNewUrl);
	LPCSTR GetFrameUrl  (int iIndex);
	int GetFrameUrlCount();
	void RemoveBaseTag();
	LPCSTR Get_BaseURL();
	
	void ParseHTML (LPSTR pszHTML);
	
	void SetKillDupes (BOOL bKill);
	
	
	void ReplaceLinkUrl(int iIndex, LPCSTR pszNewUrl);
	
	fsLinkRelType GetLinkUrlRelType (int iIndex);
	
	LPCSTR GetLinkUrl (int iIndex);
	
	int GetLinkUrlCount();
	
	void ReplaceImage (int iIndex, LPCSTR pszNewImg);
	
	void ReplaceUrl (int iIndex, LPCSTR pszNewUrl);
	
	int GetImageLinkTo (int iImage);
	
	LPCSTR GetImage (int iIndex);
	
	int GetImageCount();
	
	int GetUrlCount();
	
	LPCSTR GetUrl (int iIndex);
	
	UINT GetHTMLLength ();

	fsHTMLParser();
	virtual ~fsHTMLParser();

protected:
	LPCSTR ParseTag_Frame_Src(LPCSTR pszTag, LPCSTR pszAddUrlEnds = NULL);
	static LPCSTR ParseTag_Frame (LPCSTR pszTag, fsHTMLParser* pThis);
	LPCSTR ParseTag_Base_Href(LPCSTR pszTag, LPCSTR pszAddUrlEnds = NULL);
	static LPCSTR ParseTag_Base (LPCSTR pszTag, fsHTMLParser *pThis);
	
	
	
	LPCSTR ParseTag (LPCSTR pszTag);
	LPCSTR ParseTag_Meta_Content (LPCSTR pszTag, fsMetaHttpEquivType mhet);
	LPCSTR ParseTag_Meta_HttpEquiv (LPCSTR pszTag, fsMetaHttpEquivType* mhet);
	static LPCSTR ParseTag_Meta (LPCSTR pszTag, fsHTMLParser *pThis);
	LPCSTR ParseTag_Link_Href (LPCSTR pszTag);
	LPCSTR ParseTag_Link_Rel (LPCSTR pszTag, fsLinkRelType *lrt);
	static LPCSTR ParseTag_Link (LPCSTR pszTag, fsHTMLParser* parser);
	LPCSTR ParseTag_Img_Src (LPCSTR pszTag);
	LPCSTR ParseTag_A_Href (LPCSTR pszTag, LPCSTR pszAddUrlEnds = NULL);
	LPCSTR ParseTag_Unknown (LPCSTR pszTag);
	static LPCSTR ParseTag_Img (LPCSTR pszTag, fsHTMLParser *pThis);
	static LPCSTR ParseTag_A (LPCSTR pszTag, fsHTMLParser *pThis);

	
	
	void CorrectRegions (int nStart, int nHole);
	
	
	struct fsTextRegion
	{
		int nStart;		
		int nEnd;		
	};

	
	
	int ReplaceString (LPCSTR pszNewVal, fsTextRegion &rgn);

	
	void ParseHTML ();
	
	
	
	
	
	LPCSTR Parse_HTMLGiving (LPCSTR pszGiving, LPSTR* ppszValue, LPCSTR pszAddEnds = NULL);
	
	LPSTR m_pszHTML;	
	fs::list <fsString> m_vUrls, m_vImgs, m_vLinkUrls, m_vFrameUrls; 
	fs::list <fsTextRegion> m_vUrlsRgns, m_vImgsRgns, m_vLinkUrlsRgns, m_vFrameRgns;	
	fs::list <fsLinkRelType> m_vLRTs;	
	fs::list <int> m_vImgsLinksTo;		
	int m_htmlLen;					
	BOOL m_bKillDupes;				
	fsString m_strBaseURL;
	fsTextRegion m_BaseURLPosition;
private:
	int m_iTagAOpened;				
									
};

#endif 
