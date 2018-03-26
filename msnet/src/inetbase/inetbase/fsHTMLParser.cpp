/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/
#include "StdAfx.h"
#include "fsHTMLParser.h"
#include "strparsing.h"
#include "system.h"

#pragma warning (disable : 4127)
#pragma warning (disable : 4710)

fsHTMLParser::fsHTMLParser()
{
	m_bKillDupes = TRUE;
}

fsHTMLParser::~fsHTMLParser()
{

}

void fsHTMLParser::ParseHTML(LPSTR pszHTML)
{
	m_pszHTML = pszHTML;
	m_htmlLen = strlen (m_pszHTML);

	m_strBaseURL = "";

	ParseHTML ();
}

LPCSTR fsHTMLParser::GetUrl(int iIndex)
{
	return m_vUrls [iIndex];
}

void fsHTMLParser::ParseHTML()
{
	LPCSTR pszTagB = m_pszHTML;

	m_vUrls.clear ();
	m_vUrlsRgns.clear ();
	m_vImgs.clear ();
	m_vImgsRgns.clear ();
	m_vImgsLinksTo.clear ();
	m_vLinkUrls.clear ();
	m_vLinkUrlsRgns.clear ();
	m_vLRTs.clear ();

	m_iTagAOpened = -1;

	do
	{
		
		pszTagB = strchr (pszTagB, '<');
		if (pszTagB == NULL) break;

		
		pszTagB = ParseTag (pszTagB);
	}
	while (pszTagB);
}

LPCSTR fsHTMLParser::ParseTag_A(LPCSTR pszTag, fsHTMLParser *pThis)
{
	do
	{
		
		if (strnicmp (pszTag, "href", 4) == 0)
		{
			
			if (fsStrIsDivider (pszTag [4]) || pszTag [4] == '=')
			{
				
				pszTag += 4; 
				pszTag = pThis->ParseTag_A_Href (pszTag); 
				if (pszTag == NULL) break;
				pszTag = strchr (pszTag, '>');
				if (pszTag == NULL) break;
			}
		}

		
		if (*pszTag == '>')
		{
			pszTag++;
			break;
		}

		pszTag++; 
	}
	while (*pszTag); 

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Img(LPCSTR pszTag, fsHTMLParser *pThis)
{
	do
	{
		if (strnicmp (pszTag, "src", 3) == 0)
		{
			if (fsStrIsDivider (pszTag [3]) || pszTag [3] == '=')
			{
				pszTag = pThis->ParseTag_Img_Src (pszTag);
				if (pszTag == NULL) break;
				pszTag = strchr (pszTag, '>');
				if (pszTag == NULL) break;
			}
		}
		
		if (*pszTag == '>')
		{
			pszTag++;
			break;
		}

		pszTag++;
	}
	while (*pszTag);

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag(LPCSTR pszTag)
{
	typedef LPCSTR (*fntTagParser) (LPCSTR pszTag, fsHTMLParser *parser);

	
	LPCSTR apszTags [] = { "a", "img", "link", "area", "meta", "base", "frame" };
	
	fntTagParser apfnTagParsers [] = { ParseTag_A, ParseTag_Img, ParseTag_Link, ParseTag_A,
		ParseTag_Meta, ParseTag_Base, ParseTag_Frame };

	
	pszTag++; 

	if (*pszTag == '/')  
	{
		
		if (strnicmp (pszTag, "/a>", 3) == 0 || strnicmp (pszTag, "/a ", 3) == 0)
			m_iTagAOpened = -1; 

		
		pszTag = strchr (pszTag, '>');
		if (pszTag) pszTag++;
		return pszTag;
	}

	
	int i = 0;
	for (i = 0; i < sizeof (apszTags) / sizeof (LPCSTR); i++)
	{
		int taglen = strlen (apszTags [i]);
		if (strnicmp (pszTag, apszTags [i], taglen) == 0 && fsStrIsDivider (pszTag [taglen]))
		{
			
			pszTag += taglen;
			pszTag = apfnTagParsers [i] (pszTag, this); 
			break;
		}
	}

	if (i == sizeof (apszTags) / sizeof (LPCSTR)) 
		pszTag = ParseTag_Unknown (pszTag);	

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Unknown(LPCSTR pszTag)
{

	
	pszTag = strchr (pszTag, '>');
	if (pszTag) pszTag ++;
	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_A_Href(LPCSTR pszTag, LPCSTR pszAddUrlEnds)
{
	LPSTR pszUrl;

	
	
	pszTag = Parse_HTMLGiving (pszTag, &pszUrl, pszAddUrlEnds);

	if (pszUrl)
	{
		int iUrlFound = -1;
		if (m_bKillDupes) 
		{

			
			for (int i = 0; i < m_vUrls.size (); i++) 
			{
				if (stricmp (m_vUrls [i], pszUrl) == 0)
				{
					iUrlFound = i;
					break;
				}
			}

		}

		if (iUrlFound == -1)
		{
			

			fsTextRegion rgn;	
			rgn.nStart = pszTag - strlen (pszUrl) - m_pszHTML;
			rgn.nEnd = pszTag - m_pszHTML;
			
			if (pszTag [-1] == '"' || pszTag [-1] == '\'') 
			{
				rgn.nStart --;
				rgn.nEnd --;
			}
			m_vUrlsRgns.add (rgn);
			m_vUrls.add (pszUrl);
			iUrlFound = m_vUrls.size () - 1;
		}

		m_iTagAOpened = iUrlFound; 

		delete [] pszUrl;
	}

	
	return pszTag;
}

LPCSTR fsHTMLParser::Parse_HTMLGiving(LPCSTR pszGiving, LPSTR *ppszValue, LPCSTR pszAddEnds)
{

	
	pszGiving = fsStrSkipDividers (pszGiving);

	*ppszValue = NULL;

	
	if (*pszGiving != '=')
		return pszGiving;

	pszGiving ++;	

	pszGiving = fsStrSkipDividers (pszGiving);

	
	LPCSTR pszEnd = " >";
	
	if (*pszGiving == '"' || *pszGiving == '\'')
	{
		if (*pszGiving == '"')
			pszEnd = "\""; 
		else
			pszEnd = "'";

		pszGiving ++;
	}

	char szEnd [100];
	strcpy (szEnd, pszEnd);
	if (pszAddEnds)	
		strcat (szEnd, pszAddEnds);

	
	pszGiving = fsStrGetStrUpToChar (pszGiving, szEnd, ppszValue);

	if (pszGiving == NULL)
		return NULL;

	
	if (*(pszGiving - 1) == '>' || *(pszGiving - 1) == ' ')
		pszGiving --;

	return pszGiving;
}

LPCSTR fsHTMLParser::ParseTag_Img_Src(LPCSTR pszTag)
{
	pszTag += 3; 

	LPSTR pszUrl;
	pszTag = Parse_HTMLGiving (pszTag, &pszUrl);

	if (pszUrl)
	{
		fsTextRegion rgn;
		rgn.nStart = pszTag - strlen (pszUrl) - m_pszHTML;
		rgn.nEnd = pszTag - m_pszHTML;
		if (pszTag [-1] == '"' || pszTag [-1] == '\'') 
		{
			rgn.nStart --;
			rgn.nEnd --;
		}
		m_vImgsRgns.add (rgn);
		m_vImgs.add (pszUrl);
		m_vImgsLinksTo.add (m_iTagAOpened); 
		delete [] pszUrl;
	}
	
	return pszTag;
}

int fsHTMLParser::GetUrlCount()
{
	return m_vUrls.size ();
}

int fsHTMLParser::GetImageCount()
{
	return m_vImgs.size ();
}

LPCSTR fsHTMLParser::GetImage(int iIndex)
{
	return m_vImgs [iIndex];
}

int fsHTMLParser::GetImageLinkTo(int iImage)
{
	return m_vImgsLinksTo [iImage];
}

void fsHTMLParser::SetKillDupes(BOOL bKill)
{
	m_bKillDupes = bKill;
}

void fsHTMLParser::ReplaceUrl(int iIndex, LPCSTR pszNewUrl)
{
	if (strcmp (m_vUrls [iIndex], pszNewUrl) == 0)
		return;

	fsTextRegion &rgn = m_vUrlsRgns [iIndex];
	int nHole = ReplaceString (pszNewUrl, rgn); 

	
	

	
	rgn.nEnd -= nHole;	
	CorrectRegions (rgn.nStart, nHole);
}

int fsHTMLParser::ReplaceString(LPCSTR pszNewVal, fsTextRegion &rgn)
{
	int newlen = strlen (pszNewVal);

	
	if (newlen > rgn.nEnd - rgn.nStart)
	{
		
		

		
		LPSTR pszBuf = NULL;
		fsnew (pszBuf, char, m_htmlLen - rgn.nEnd + 1);
		
		CopyMemory (pszBuf,  m_pszHTML + rgn.nEnd, m_htmlLen - rgn.nEnd + 1);
		
		
		CopyMemory (m_pszHTML + rgn.nStart + newlen, pszBuf, m_htmlLen - rgn.nEnd + 1);
		delete [] pszBuf;
	}
	else
	{
		
		CopyMemory (m_pszHTML + rgn.nStart + newlen, m_pszHTML + rgn.nEnd, m_htmlLen - rgn.nEnd + 1);
	}

	
	CopyMemory (m_pszHTML + rgn.nStart, pszNewVal, newlen);
	
	
	m_htmlLen -= (int) rgn.nEnd - (int) rgn.nStart - newlen;

	return (int) rgn.nEnd - (int) rgn.nStart - newlen;
}

void fsHTMLParser::ReplaceImage(int iIndex, LPCSTR pszNewImg)
{
	if (strcmp (m_vImgs [iIndex], pszNewImg) == 0)
		return;

	fsTextRegion &rgn = m_vImgsRgns [iIndex];
	int nHole = ReplaceString (pszNewImg, rgn);

	rgn.nEnd -= nHole;

	CorrectRegions (rgn.nStart, nHole);
}

UINT fsHTMLParser::GetHTMLLength()
{
	return m_htmlLen;
}

void fsHTMLParser::CorrectRegions(int nStart, int nHole)
{
	

	
	
	int i = 0;
	for (i = 0; i < m_vUrlsRgns.size (); i++)
	{
		fsTextRegion &rgn = m_vUrlsRgns [i];
		if (rgn.nStart > nStart)
		{
			rgn.nStart -= nHole;
			rgn.nEnd -= nHole;
		}
	}

	for (i = 0; i < m_vImgsRgns.size (); i++)
	{
		fsTextRegion &rgn = m_vImgsRgns [i];
		if (rgn.nStart > nStart)
		{
			rgn.nStart -= nHole;
			rgn.nEnd -= nHole;
		}
	}

	for (i = 0; i < m_vLinkUrlsRgns.size (); i++)
	{
		fsTextRegion &rgn = m_vLinkUrlsRgns [i];
		if (rgn.nStart > nStart)
		{
			rgn.nStart -= nHole;
			rgn.nEnd -= nHole;
		}
	}

	for (i = 0; i < m_vFrameRgns.size (); i++)
	{
		fsTextRegion &rgn = m_vFrameRgns [i];
		if (rgn.nStart > nStart)
		{
			rgn.nStart -= nHole;
			rgn.nEnd -= nHole;
		}
	}
}

LPCSTR fsHTMLParser::ParseTag_Link(LPCSTR pszTag, fsHTMLParser *pThis)
{
	fsLinkRelType lrt = LRT_UNKNOWN;
	BOOL bLinkAdded = FALSE;

	do
	{
		if (strnicmp (pszTag, "rel", 3) == 0)
		{
			if (fsStrIsDivider (pszTag [3]) || pszTag [3] == '=')
			{
				
				pszTag = pThis->ParseTag_Link_Rel (pszTag, &lrt);
				if (pszTag == NULL) break;
			}
		}

		if (strnicmp (pszTag, "href", 4) == 0 && bLinkAdded == FALSE)
		{
			if (fsStrIsDivider (pszTag [4]) || pszTag [4] == '=')
			{
				pszTag = pThis->ParseTag_Link_Href (pszTag);
				if (pszTag == NULL) break;
				bLinkAdded = TRUE;
			}
		}
		
		if (*pszTag == '>')
		{
			pszTag++;
			break;
		}

		pszTag++;
	}
	while (*pszTag);

	if (bLinkAdded)
		pThis->m_vLRTs.add (lrt);

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Link_Rel(LPCSTR pszTag, fsLinkRelType *lrt)
{
	pszTag += 3; 

	LPSTR pszType;
	pszTag = Parse_HTMLGiving (pszTag, &pszType);

	*lrt = LRT_UNKNOWN;

	if (pszType)
	{
		if (stricmp (pszType, "stylesheet") == 0)
			*lrt = LRT_STYLESHEET;

		delete [] pszType;
	}

	
	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Link_Href(LPCSTR pszTag)
{
	

	pszTag += 4; 

	LPSTR pszUrl;
	pszTag = Parse_HTMLGiving (pszTag, &pszUrl);

	if (pszUrl)
	{
		int iUrlFound = -1;
		if (m_bKillDupes)
		{
			for (int i = 0; i < m_vLinkUrls.size (); i++)
				if (stricmp (m_vLinkUrls [i], pszUrl) == 0)
				{
					iUrlFound = i;
					break;
				}
		}

		if (iUrlFound == -1)
		{
			fsTextRegion rgn;
			rgn.nStart = pszTag - strlen (pszUrl) - m_pszHTML;
			rgn.nEnd = pszTag - m_pszHTML;
			if (pszTag [-1] == '"' || pszTag [-1] == '\'') 
			{
				rgn.nStart --;
				rgn.nEnd --;
			}
			m_vLinkUrlsRgns.add (rgn);
			m_vLinkUrls.add (pszUrl);
		}

		delete [] pszUrl;
	}

	
	return pszTag;	
}

int fsHTMLParser::GetLinkUrlCount()
{
	return m_vLinkUrls.size ();
}

LPCSTR fsHTMLParser::GetLinkUrl(int iIndex)
{
	return m_vLinkUrls [iIndex];
}

fsLinkRelType fsHTMLParser::GetLinkUrlRelType(int iIndex)
{
	return m_vLRTs [iIndex];
}

void fsHTMLParser::ReplaceLinkUrl(int iIndex, LPCSTR pszNewUrl)
{
	if (strcmp (m_vLinkUrls [iIndex], pszNewUrl) == 0)
		return;

	fsTextRegion &rgn = m_vLinkUrlsRgns [iIndex];
	int nHole = ReplaceString (pszNewUrl, rgn);

	rgn.nEnd -= nHole;

	CorrectRegions (rgn.nStart, nHole);
}

LPCSTR fsHTMLParser::ParseTag_Meta(LPCSTR pszTag, fsHTMLParser *pThis)
{
	fsMetaHttpEquivType mhet = MHET_UNKNOWN;

	do
	{
		if (strnicmp (pszTag, "http-equiv", 10) == 0)
		{
			if (fsStrIsDivider (pszTag [10]) || pszTag [10] == '=')
			{
				
				pszTag = pThis->ParseTag_Meta_HttpEquiv (pszTag, &mhet);
				if (pszTag == NULL) break;
			}
		}

		if (strnicmp (pszTag, "content", 7) == 0)
		{
			if (fsStrIsDivider (pszTag [7]) || pszTag [7] == '=')
			{
				pszTag = pThis->ParseTag_Meta_Content (pszTag, mhet);
				if (pszTag == NULL) break;
				pszTag = strchr (pszTag, '>');
				if (pszTag == NULL) break;
			}
		}
		
		if (*pszTag == '>')
		{
			pszTag++;
			break;
		}

		pszTag++;
	}
	while (*pszTag);

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Meta_HttpEquiv(LPCSTR pszTag, fsMetaHttpEquivType *mhet)
{
	pszTag += 10; 

	LPSTR pszType;
	pszTag = Parse_HTMLGiving (pszTag, &pszType);

	*mhet = MHET_UNKNOWN;

	if (pszType)
	{
		if (stricmp (pszType, "refresh") == 0)
			*mhet = MHET_REFRESH;

		delete [] pszType;
	}

	
	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Meta_Content(LPCSTR pszTag, fsMetaHttpEquivType mhet)
{
	pszTag += 7;

	switch (mhet)
	{
		
		
	case MHET_REFRESH:

		pszTag = fsStrSkipDividers (pszTag);
		if (*pszTag != '=')
		{

			return pszTag;
		}
		pszTag++;

		BOOL bA;
		bA = *pszTag == '\'' || *pszTag == '"';
		if (bA) pszTag++;

		pszTag = fsStrSkipDividers (pszTag);

		while (fsStrIsDivider (*pszTag) == FALSE && *pszTag != ';') pszTag++;

		if (*pszTag == ';') pszTag++;

		pszTag = fsStrSkipDividers (pszTag);

		if (strnicmp (pszTag, "url", 3) || (fsStrIsDivider (pszTag [3]) == FALSE && pszTag [3] != '='))
		{

			return pszTag;
		}

		pszTag += 3;
		pszTag = ParseTag_A_Href (pszTag, bA ? "\"\'" : NULL);

		

		break;

	default:

		LPSTR pszContent;
		pszTag = Parse_HTMLGiving (pszTag, &pszContent);
		if (pszContent)
			delete [] pszContent;
	}

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Base(LPCSTR pszTag, fsHTMLParser *pThis)
{
	pThis->m_BaseURLPosition.nStart = pszTag-4-1 - pThis->m_pszHTML;

	do
	{
		
		if (strnicmp (pszTag, "href", 4) == 0)
		{
			
			if (fsStrIsDivider (pszTag [4]) || pszTag [4] == '=')
			{
				
				pszTag += 4; 
				pszTag = pThis->ParseTag_Base_Href (pszTag); 
				if (pszTag == NULL) break;
				pszTag = strchr (pszTag, '>');
				if (pszTag == NULL) break;
			}
		}
		
		
		if (*pszTag == '>')
		{
			pszTag++;
			break;
		}

		pszTag++; 
	}
	while (*pszTag); 

	pThis->m_BaseURLPosition.nEnd = pszTag - pThis->m_pszHTML;

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Base_Href(LPCSTR pszTag, LPCSTR pszAddUrlEnds)
{
	LPSTR pszUrl;

	
	
	pszTag = Parse_HTMLGiving (pszTag, &pszUrl, pszAddUrlEnds);

	if (pszUrl)
	{
		m_strBaseURL = pszUrl;
		delete [] pszUrl;
	}
	
	return pszTag;
}

LPCSTR fsHTMLParser::Get_BaseURL()
{
	return m_strBaseURL;
}

void fsHTMLParser::RemoveBaseTag()
{
	int nHole = ReplaceString ("", m_BaseURLPosition);

	m_BaseURLPosition.nEnd -= nHole;

	CorrectRegions (m_BaseURLPosition.nStart, nHole);
}

LPCSTR fsHTMLParser::ParseTag_Frame(LPCSTR pszTag, fsHTMLParser *pThis)
{
	do
	{
		
		if (strnicmp (pszTag, "src", 3) == 0)
		{
			if (fsStrIsDivider (pszTag [3]) || pszTag [3] == '=')
			{
				
				pszTag += 3; 
				pszTag = pThis->ParseTag_Frame_Src (pszTag); 
				if (pszTag == NULL) break;
				pszTag = strchr (pszTag, '>');
				if (pszTag == NULL) break;
			}
		}
		
		
		if (*pszTag == '>')
		{
			pszTag++;
			break;
		}

		pszTag++; 
	}
	while (*pszTag); 

	return pszTag;
}

LPCSTR fsHTMLParser::ParseTag_Frame_Src(LPCSTR pszTag, LPCSTR )
{
	LPSTR pszUrl;
	pszTag = Parse_HTMLGiving (pszTag, &pszUrl);

	if (pszUrl)
	{
		fsTextRegion rgn;
		rgn.nStart = pszTag - lstrlen (pszUrl) - m_pszHTML;
		rgn.nEnd = pszTag - m_pszHTML;
		if (pszTag [-1] == '"' || pszTag [-1] == '\'') 
		{
			rgn.nStart --;
			rgn.nEnd --;
		}
		m_vFrameRgns.add (rgn);
		m_vFrameUrls.add (pszUrl);
		delete [] pszUrl;
	}
	
	return pszTag;
}

int fsHTMLParser::GetFrameUrlCount()
{
	return m_vFrameUrls.size ();
}

LPCSTR fsHTMLParser::GetFrameUrl(int iIndex)
{
	return m_vFrameUrls [iIndex];
}

void fsHTMLParser::ReplaceFrameUrl(int iIndex, LPCSTR pszNewUrl)
{
	if (strcmp (m_vFrameUrls [iIndex], pszNewUrl) == 0)
		return;
	fsTextRegion &rgn = m_vFrameRgns [iIndex];
	int nHole = ReplaceString (pszNewUrl, rgn);

	rgn.nEnd -= nHole;

	CorrectRegions (rgn.nStart, nHole);
}
