#include "stdafx.h"
#include "dsFontMgr.h"

dsFontMgr* dsFontMgr::m_dsFontMgr = NULL;
DSUI_API IdsFontMgr * IdsGet_FontMgr()
{
	return dsFontMgr::getInstance();
}

HFONT CreatePointFontIndirect( const LOGFONT* lpLogFont, HDC hDC /*= NULL*/ )
{
	HDC hDC1 = (hDC != NULL) ? hDC : ::GetDC(NULL);

	// convert nPointSize to logical units based on hDC
	LOGFONT logFont = *lpLogFont;
#ifndef _WIN32_WCE
	POINT pt = { 0, 0 };
	pt.y = ::MulDiv(::GetDeviceCaps(hDC1, LOGPIXELSY), logFont.lfHeight, 720);   // 72 points/inch, 10 decipoints/point
	::DPtoLP(hDC1, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC1, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);
#else // CE specific
	// DP and LP are always the same on CE
	logFont.lfHeight = -abs(::MulDiv(::GetDeviceCaps(hDC1, LOGPIXELSY), logFont.lfHeight, 720));   // 72 points/inch, 10 decipoints/point
#endif // _WIN32_WCE

	if(hDC == NULL)
		::ReleaseDC(NULL, hDC1);

	return CreateFontIndirect(&logFont);
}

HFONT CreatePointFont( int nPointSize, LPCTSTR lpszFaceName, HDC hDC /*= NULL*/, bool bBold /*= false*/, bool bItalic /*= false*/, bool bUnderline /*= false*/ )
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nPointSize;
	SecureHelper::strncpy_x(logFont.lfFaceName, _countof(logFont.lfFaceName), lpszFaceName, _TRUNCATE);

	if(bBold)
		logFont.lfWeight = FW_BOLD;
	if(bItalic)
		logFont.lfItalic = (BYTE)TRUE;

	logFont.lfUnderline = (BYTE)bUnderline;

	return CreatePointFontIndirect(&logFont, hDC);
}