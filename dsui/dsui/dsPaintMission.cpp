#include "stdafx.h"
#include "dsFontMgr.h"
#include "dsPaintMission.h"

#include "dsGifAnimateMgr.h"


dsPaintMission::dsPaintMission() :
m_nState(DSUI_STATE_NORMAL),
m_nPaintMode(DSUI_PAINTMODE_NORMAL),
m_nImageID(INVALID_IMAGEID),
m_crText(0),
m_lParam(0),
m_uFormat(DT_LEFT),
m_hFont(NULL),
m_bEnabled(TRUE),
m_bMarkAsIcon(FALSE),
m_bMarkAsGif(FALSE),
m_dwGifKey(0),
m_dwOldGifKey(0),
m_bUseUIText(FALSE)
{
	SetRectForAllDraw(m_rcDst);
	SetRectForAllDraw(m_rcSrc);
}

int dsPaintMission::GetState()
{
	return m_nState;
}

Image * dsPaintMission::GetImagePtr()
{
	return dsImageMgr::getInstance()->GetImagePtr(m_nImageID);
}



HICON dsPaintMission::GetIcon()
{
	return dsImageMgr::getInstance()->GetIcon(m_nImageID);
}
LPCWSTR dsPaintMission::GetTextPtr()
{
	return m_strText;
}

CString & dsPaintMission::GetTextStrQuote()
{
	return m_strText;
}

BOOL dsPaintMission::GetRealDstRect( __in LPRECT lprcUI, __out LPRECT lprcDst )
{
	
	if(NULL == lprcUI && NULL == lprcDst)
		return FALSE;

	if (IsRectForAllDraw(m_rcDst)){
		::CopyRect(lprcDst, lprcUI);
		return TRUE;
	}
	else{
		::CopyRect(lprcDst, m_rcDst);
		//::OffsetRect(lprcDst, lprcUI->left, lprcUI->top);
	}

	CRect rcDst(lprcDst),rcUI(lprcUI);
	dsTools::UpdateLayoutRect(rcUI, rcDst, m_rcDstCoordMark, lprcDst);

	//::OffsetRect(lprcDst, lprcUI->left, lprcUI->top);

	return TRUE;
}

BOOL dsPaintMission::GetRealSrcRect( __out LPRECT lprcSrc )
{
	if (NULL == lprcSrc)
		return FALSE;


	if(IsRectForAllDraw(m_rcSrc))
	{
		Image* pimg = dsImageMgr::getInstance()->GetImagePtr(m_nImageID);
		if (NULL == pimg)
			return FALSE;

		::SetRect(lprcSrc, 0, 0, pimg->GetWidth(), pimg->GetHeight());
	}
	else
		::CopyRect(lprcSrc, m_rcSrc);

	return TRUE;
}

BOOL dsPaintMission::Get9SquaresStretchTable( __in LPRECT lprcUI, __out LPNSST lpnsst )
{
	if(NULL == lpnsst)
		return FALSE;
	else{

		if(NULL ==  lprcUI)
			return FALSE;

		CRect rcDst;
		GetRealDstRect(lprcUI, rcDst);

		CRect rcSrc;
		GetRealSrcRect(rcSrc);

		return ds9SquaresStretch::Analysis(rcDst, rcSrc, m_rcSquares9, lpnsst);
	}
}

BOOL dsPaintMission::IsRectForAllDraw( __in LPRECT lprc )
{
	return (-1 == lprc->left && -1 == lprc->top && -1 == lprc->right && -1 == lprc->bottom);
}

BOOL dsPaintMission::SetRectForAllDraw( __inout LPRECT lprc )
{
	if(NULL == lprc)
		return FALSE;
	lprc->left = lprc->top = lprc->right = lprc->bottom = -1;
	return TRUE;
}

void dsPaintMission::Reset()
{
	m_nState = DSUI_STATE_NORMAL;
	SetRectForAllDraw(m_rcDst);
	m_rcDstCoordMark.SetRectEmpty();
	m_nPaintMode = DSUI_PAINTMODE_NORMAL;
	m_nImageID = INVALID_IMAGEID;
	SetRectForAllDraw(m_rcSrc);
	m_rcSquares9.SetRectEmpty();
	m_strText = L"";
	m_crText = 0;
	m_lParam = 0;
	m_hFont = NULL;
	m_uFormat = 0;
	m_vecSuperText.clear();
}

void dsPaintMission::SetState( int nState )
{
	m_nState = nState;
}
BOOL dsPaintMission::SetDstRect( __in LPRECT lprc )
{
	if(NULL == lprc)
		return FALSE;

	m_rcDst.CopyRect(lprc);
	return TRUE;
}

BOOL dsPaintMission::SetDstCoordMark( __in LPRECT lprc )
{
	if(NULL == lprc)
		return FALSE;
	m_rcDstCoordMark.CopyRect(lprc);
	return TRUE;
}

void dsPaintMission::SetPaintMode( int nPaintMode )
{
	m_nPaintMode = nPaintMode;
}

void dsPaintMission::SetImageID( int nImageID )
{
	m_nImageID = nImageID;

	MarkAsGif(FALSE);
}

BOOL dsPaintMission::SetSrcRect( __in LPRECT lprc )
{
	if(NULL == lprc)
		return FALSE;
	m_rcSrc.CopyRect(lprc);
	return TRUE;
}

BOOL dsPaintMission::SetSquares9( __in LPRECT lprc )
{
	if(NULL == lprc)
		return FALSE;
	m_rcSquares9.CopyRect(lprc);
	return TRUE;
}

BOOL dsPaintMission::SetText( __in LPCWSTR lpszText )
{
	if(NULL == lpszText)
		return FALSE;

	m_strText = lpszText;
	return TRUE;
}

void dsPaintMission::SetTextDrawFlag( UINT uFormat )
{
	m_uFormat = uFormat;
}
void dsPaintMission::SetColor( COLORREF cr )
{
	m_crText = cr;
}

void dsPaintMission::SetParam( LPARAM lParam )
{
	m_lParam = lParam;
}

void dsPaintMission::SetFont( int nFontID )
{
	m_hFont = dsFontMgr::getInstance()->GetFont(nFontID);
}

BOOL dsPaintMission::Enable( BOOL bEnable )
{
	return dsTools::Exchange2Objects(m_bEnabled, bEnable);
}

void dsPaintMission::MarkAsIcon( BOOL bEnable )
{
	m_bMarkAsIcon = bEnable;
}

BOOL dsPaintMission::AddSuperTextElement( LPCWSTR lpszText, COLORREF crText, int nFontID )
{

	if(NULL == lpszText)
		return FALSE;


	SUPERTEXT st;
	st.strText = lpszText;
	st.crText = crText;
	st.hFontText = dsFontMgr::getInstance()->GetFont(nFontID);

	m_vecSuperText.push_back(st);

	return TRUE;
}

void dsPaintMission::ResetSuperText()
{
	m_vecSuperText.clear();
}

BOOL dsPaintMission::SetSuperTextElement( int nPos, LPCWSTR lpszText, COLORREF crText, int nFontID )
{
	if(nPos < 0 || nPos > m_vecSuperText.size() -1)
		return FALSE;

	m_vecSuperText[nPos].strText = lpszText;
	m_vecSuperText[nPos].crText = crText;
	m_vecSuperText[nPos].hFontText = dsFontMgr::getInstance()->GetFont(nFontID);

	return TRUE;
}

//BOOL dsPaintMission::MarkAsGif( BOOL bMark )
//{
//	//if(!bMark)
//	//	m_gif.StopAnimate();
//
//	if(m_bMarkAsGif)	
//	{
//		return m_bMarkAsGif = bMark;
//	}
//
//	Image *pgif = (Image*)dsImageMgr::getInstance()->GetImagePtr(m_nImageID);
//
//	if(NULL == pgif)
//		return FALSE;
//
//	if(!m_bMarkAsGif)
//		dsGifAnimateMgr::getInstance()->DestroyAnimate(m_dwGifKey);
//
//	//if(pgif->IsAnimatedGIF())
//	//{
//	//	m_gif.Attatch(pgif);
//	//}
//	//else
//	//	return m_bMarkAsGif = FALSE;
//
//	//if(!bMark)
//	//	m_gif.StopAnimate();
//
//	return m_bMarkAsGif = bMark;
//}

BOOL dsPaintMission::MarkAsGif( DWORD dwKey )
{
	if(0 != m_dwOldGifKey)
		dsGifAnimateMgr::getInstance()->DestroyAnimate(m_dwOldGifKey);


	m_dwOldGifKey = m_dwGifKey;
	m_dwGifKey = dwKey;

	return TRUE;
}

dsPaintMission::~dsPaintMission()
{
	dsGifAnimateMgr::getInstance()->DestroyAnimate(m_dwGifKey);

}

BOOL dsPaintMission::UseUIText( BOOL bUse )
{
	return m_bUseUIText = bUse;
}
DSUI_API IdsPaintMission * IdsNew_PaintMission()
{
	return new dsPaintMission();
}


DSUI_API void IdsDel_PaintMission( IdsPaintMission *p )
{
	if(NULL != p)
		delete p;
}