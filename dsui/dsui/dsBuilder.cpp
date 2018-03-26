#include "stdafx.h"
#include "dsdef.h"

CIdsBuilder::CIdsBuilder() 
{
	m_nBorderLeft = 1;
	m_nBorderTop = 1;
	m_nBorderRight = 1;
	m_nBorderBottom = 1;
	m_nBorderCaption = 40;
	m_pactDef = IdsNew_Activity();
}

void CIdsBuilder::Attach(HWND hWnd)
{
	m_wndAttached = hWnd;
}

CIdsBuilder::~CIdsBuilder()
{
	IdsDel_Activity(m_pactDef);
}

void CIdsBuilder::AssignXMLPath(LPCTSTR szFilePath)
{
	m_strXMLPath = szFilePath;
}

void CIdsBuilder::SetBorderPadding(int l, int t, int r, int b, int c)
{
	m_nBorderLeft = l;
	m_nBorderBottom = t;
	m_nBorderRight = r;
	m_nBorderBottom = b;
	m_nBorderCaption = c;
}

void CIdsBuilder::AddClientHitRect(DWORD dwMark, LPRECT lprc)
{
	if(NULL == lprc)
		return;

	MAP_MARKEDRECT::iterator it = m_mapClientHitRect.find(dwMark);

	if(it != m_mapClientHitRect.end())
		it->second.CopyRect(lprc);
	else
		m_mapClientHitRect.insert(make_pair(dwMark, lprc));

}

IdsUIBase* CIdsBuilder::GetDsuiElement(LPARAM lParam, LPVOID pUserInfo)
{
	return m_pactDef->GetDsuiElement(lParam, pUserInfo);
}

IdsUIBase* CIdsBuilder::GetDsuiElement(WORD wUIID)
{
	return m_pactDef->GetDsuiElement(wUIID);
}

void CIdsBuilder::OnSize(UINT nType, CSize size)
{	
	MAP_MARKEDRECT::iterator it = m_mapClientHitRect.begin();
	for( ;it != m_mapClientHitRect.end(); it++ )
	{
		IdsUIBase *pui = m_pactDef->GetDsuiElement(it->first);
		if(NULL != pui)
		{
			CRect rcUI;
			pui->GetRect(rcUI);
			AddClientHitRect(it->first, rcUI);
		}
	}
}

UINT CIdsBuilder::OnNcHitTest(CPoint point)
{
	m_wndAttached.ScreenToClient(&point);

	CRect rcWnd;
	m_wndAttached.GetWindowRect(rcWnd);
	m_wndAttached.ScreenToClient(rcWnd);

	MAP_MARKEDRECT::iterator it = m_mapClientHitRect.begin();

	while(it != m_mapClientHitRect.end())
	{
		if(it->second.PtInRect(point))
			return HTCLIENT;
		it++;
	}

	UINT uHitFlag = HTNOWHERE;
	UINT uVertFlag = VERTFLAG_NULL;
	if(0 <= point.y && point.y <= m_nBorderTop)
		uVertFlag = VERTFLAG_TOP;
	else if(m_nBorderTop < point.y && point.y < rcWnd.Height() - m_nBorderBottom)
		uVertFlag = VERTFLAG_CENTER;
	else if(rcWnd.Height() - m_nBorderBottom < point.y && point.y < rcWnd.Height())
		uVertFlag = VERTFLAG_BOTTOM;

	if(VERTFLAG_NULL == uVertFlag)
		return uHitFlag;

	if(0 <= point.x && point.x <= rcWnd.Width())
	{
		if(VERTFLAG_TOP == uVertFlag)
			uHitFlag = HTCAPTION;
		else if(VERTFLAG_CENTER == uVertFlag)
		{
			uHitFlag = HTCAPTION;

			if(point.y > m_nBorderTop + m_nBorderCaption)
				uHitFlag = HTCLIENT;
		}
		else if(VERTFLAG_BOTTOM == uVertFlag)
			uHitFlag = HTCLIENT;
	}

	return uHitFlag;
}

BOOL CIdsBuilder::OnNcActivate(BOOL bActive)
{
	if (bActive)
	{
		static DWORD _dwSysVersion = GetSysVer(NULL);
		if (_dwSysVersion < 6)
		{
			::DefWindowProc(m_wndAttached, WM_NCACTIVATE, (WPARAM)TRUE, 0);			
		}
	}
	return TRUE;
}

LRESULT CIdsBuilder::OnNcCalcSize( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
{
	NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;
	if((BOOL)wParam)
	{
		lpncsp->rgrc[2] = lpncsp->rgrc[1];
		lpncsp->rgrc[1] = lpncsp->rgrc[0];
	}
	return TRUE;
}

BOOL CIdsBuilder::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

DWORD CIdsBuilder::GetSysVer(PDWORD dwMinorVersion /*= NULL*/)
{
	OSVERSIONINFO osvi; 
	memset(&osvi, 0, sizeof(OSVERSIONINFO)); 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
	GetVersionEx(&osvi); 

	if (dwMinorVersion)
		*dwMinorVersion = osvi.dwMinorVersion;

	return osvi.dwMajorVersion;
}

void CIdsBuilder::builder()
{
	InitFont();
	InitImage();
	InitSkin();
	InitUI();
	InitWindow();
}

void CIdsBuilder::InitWindow()
{
	USES_CONVERSION;
	TiXmlDocument *pDoc = new TiXmlDocument();
	if(pDoc->LoadFile(T2A(m_strXMLPath)) )	
	{
		TiXmlElement* pRootElement = pDoc->RootElement();
		if( pRootElement )
		{
			int nWidth = 0;
			BOOL bWidthRet = GetElementAttrIntValue( pRootElement, UI_XML_ATTR_WIDTH, nWidth);

			int nHeight = 0;
			BOOL bHeightRet = GetElementAttrIntValue( pRootElement, UI_XML_ATTR_HEIGHT, nHeight);

			COLORREF crBack = RGB(255,255,255);
			BOOL bBackColorRet = GetElementAttrColorValue( pRootElement, UI_XML_ATTR_BACKCOLOR, crBack);
	
			int nFont = 0;
			BOOL bFontRet = GetElementAttrIntValue( pRootElement, UI_XML_ATTR_TIPFONT, nFont);

			COLORREF crText = RGB(0,0,0);
			BOOL bTextColorRet = GetElementAttrColorValue( pRootElement, UI_XML_ATTR_TIPTEXTCOLOR, crText);

			CRect rcMargin;
			BOOL bMarginRet = GetElementAttrRectValue( pRootElement, UI_XML_ATTR_TIPMARGIN, rcMargin);

			COLORREF crTipBack = RGB(255,255,255);
			BOOL bTipBackColorRet = GetElementAttrColorValue( pRootElement, UI_XML_ATTR_TIPBACKCOLOR, crTipBack);

			if( nWidth > 0 && nHeight > 0 )
			{
				if( bWidthRet && bHeightRet ) 
				{
					m_wndAttached.MoveWindow(0,0,nWidth,nHeight, FALSE);
				}
				m_wndAttached.CenterWindow();
				if( !m_wndAttached.IsWindowVisible() ) 
				{
					m_wndAttached.ShowWindow(SW_SHOW);
				}
			}
			if( bBackColorRet ) m_pactDef->SetBkColor(crBack);
			if( bFontRet ) m_pactDef->SetTipFont(nFont);
			if( bMarginRet) m_pactDef->SetTipMargin(rcMargin);
			if( bTipBackColorRet ) m_pactDef->SetTipBkColor(crTipBack);
			if( bTextColorRet ) m_pactDef->SetTipTextColor(crText);
			m_pactDef->LinkWindow(m_wndAttached);
		}
		delete pDoc;
		pDoc = NULL;
	}
}

void CIdsBuilder::InitSkin()
{
	USES_CONVERSION;
	TiXmlDocument *pDoc = new TiXmlDocument();
	if(pDoc->LoadFile(T2A(m_strXMLPath)) )	
	{
		TiXmlElement* pRootElement = pDoc->RootElement();
		if( pRootElement )
		{
			// 皮肤
			TiXmlElement* pSkinElement = pRootElement->FirstChildElement(UI_XML_NODE_SKIN);
			if ( pSkinElement )
			{
				// 边框
				TiXmlElement* pBorderElement = pSkinElement->FirstChildElement(UI_XML_NODE_BORDER);
				if( pBorderElement )
				{
					int nImageID = -1;
					BOOL bImageRet = GetElementAttrIntValue(pBorderElement, UI_XML_ATTR_IMAGEID, nImageID);

					COLORREF crBack;
					BOOL bBackColorRet = GetElementAttrColorValue(pBorderElement, UI_XML_ATTR_BACKCOLOR, crBack);

					int nBorderSize = 1;
					BOOL bBorderSizeRet = GetElementAttrIntValue(pBorderElement, UI_XML_ATTR_SIZE, nBorderSize);

					CRect rcSquire9;
					BOOL bSquire9Ret = GetElementAttrRectValue(pBorderElement, UI_XML_ATTR_SQUIRE9, rcSquire9);

					IdsPaintMission* ppm = IdsNew_PaintMission();
					if( ppm )
					{
						if( bImageRet )
						{
							ppm->SetImageID(nImageID);
							ppm->SetPaintMode(DSUI_PAINTMODE_STRETCH);
							if( bSquire9Ret ) 
							{
								// 绘制整个边框
								ppm->SetSquares9(rcSquire9);
								ppm->SetDstRect(CRect(0,0,0,0));
								ppm->SetDstCoordMark(CRect(0,0,1,1));
								m_pactDef->AddSkinPaintMission(ppm);

								IdsDel_PaintMission(ppm);
							}
						}
						else if( bBackColorRet ) 
						{
							ppm->SetColor(crBack);
						}

						if( !bImageRet || !bSquire9Ret )
						{
							// 左边框
							ppm->SetDstRect(CRect(0,0,nBorderSize,0));
							ppm->SetDstCoordMark(CRect(0,0,0,1));
							m_pactDef->AddSkinPaintMission(ppm);

							// 上边框
							ppm->SetDstRect(CRect(0,0,0,nBorderSize));
							ppm->SetDstCoordMark(CRect(0,0,1,0));
							m_pactDef->AddSkinPaintMission(ppm);

							// 右边框
							ppm->SetDstRect(CRect(nBorderSize,0,0,0));
							ppm->SetDstCoordMark(CRect(1,0,1,1));
							m_pactDef->AddSkinPaintMission(ppm);

							// 下边框
							ppm->SetDstRect(CRect(0,nBorderSize,0,0));
							ppm->SetDstCoordMark(CRect(0,1,1,1));
							m_pactDef->AddSkinPaintMission(ppm);

							IdsDel_PaintMission(ppm);
						}
					}
				}

				// 标题栏
				TiXmlElement*  pHeaderElement = pSkinElement->FirstChildElement(UI_XML_NODE_HEADER);
				if( pHeaderElement )
				{
					int nImageID = -1;
					BOOL bImageRet = GetElementAttrIntValue(pHeaderElement, UI_XML_ATTR_IMAGEID, nImageID);

					COLORREF crBack;
					BOOL bBackColorRet = GetElementAttrColorValue(pHeaderElement, UI_XML_ATTR_BACKCOLOR, crBack);

					CRect rcPos;
					BOOL bPosRet = GetElementAttrRectValue(pHeaderElement, UI_XML_ATTR_DEST, rcPos);

					CRect rcMark;
					BOOL bMarkRet = GetElementAttrRectValue(pHeaderElement, UI_XML_ATTR_DESTMARK, rcMark);					
					
					CRect rcSquire9;
					BOOL bSquire9Ret = GetElementAttrRectValue(pHeaderElement, UI_XML_ATTR_SQUIRE9, rcSquire9);

					IdsPaintMission* ppm = IdsNew_PaintMission();
					if( ppm )
					{
						if( bImageRet )
						{
							ppm->SetImageID(nImageID);
							ppm->SetPaintMode(DSUI_PAINTMODE_STRETCH);
							if( bSquire9Ret ) ppm->SetSquares9(rcSquire9);
						}
						else if( bBackColorRet ) 
						{
							ppm->SetColor(crBack);
						}							
						if( bPosRet ) ppm->SetDstRect(rcPos);
						if( bMarkRet ) ppm->SetDstCoordMark(rcMark);
						m_pactDef->AddSkinPaintMission(ppm);
						IdsDel_PaintMission(ppm);
					}
					
					// 图标
					TiXmlElement*  pIconElement = pHeaderElement->FirstChildElement(UI_XML_NODE_ICON);
					if( pIconElement )
					{
						int nImageID = -1;
						BOOL bImageRet = GetElementAttrIntValue(pIconElement, UI_XML_ATTR_IMAGEID, nImageID);

						CRect rcPos;
						BOOL bPosRet = GetElementAttrRectValue(pIconElement, UI_XML_ATTR_DEST, rcPos);

						CRect rcMark;
						BOOL bMarkRet = GetElementAttrRectValue(pIconElement, UI_XML_ATTR_DESTMARK, rcMark);

						IdsPaintMission* ppm = IdsNew_PaintMission();
						if( ppm )
						{
							ppm->SetImageID(nImageID);
							ppm->SetPaintMode(DSUI_PAINTMODE_STRETCH);
							if( bPosRet ) ppm->SetDstRect(rcPos);
							if( bMarkRet ) ppm->SetDstCoordMark(rcMark);

							m_pactDef->AddSkinPaintMission(ppm);
							IdsDel_PaintMission(ppm);
						}
					}

					// 标题
					TiXmlElement*  pTitleElement = pHeaderElement->FirstChildElement(UI_XML_NODE_TITLE);
					if( pTitleElement ) 
					{
						TCHAR szText[MAX_PATH] = {0};
						BOOL bTextRet = GetElementAttrValue(pTitleElement, UI_XML_ATTR_NAME, szText, MAX_PATH-1);

						CRect rcPos;
						BOOL bPosRet = GetElementAttrRectValue(pTitleElement, UI_XML_ATTR_DEST, rcPos);

						CRect rcMark;
						BOOL bMarkRet = GetElementAttrRectValue(pTitleElement, UI_XML_ATTR_DESTMARK, rcMark);

						int nFontID = 0;
						BOOL bFontRet = GetElementAttrIntValue(pTitleElement, UI_XML_ATTR_FONTID, nFontID);

						COLORREF crText;
						BOOL bTextColorRet = GetElementAttrColorValue(pTitleElement, UI_XML_ATTR_TEXTCOLOR, crText);

						IdsPaintMission* ppm = IdsNew_PaintMission();
						if( ppm )
						{
							ppm->SetText(szText);
							if( bFontRet ) ppm->SetFont(nFontID);
							if( bTextColorRet ) ppm->SetColor(crText);
							ppm->SetTextDrawFlag(DT_LEFT|DT_VCENTER|DT_SINGLELINE);
							if( bPosRet ) ppm->SetDstRect(rcPos);
							if( bMarkRet ) ppm->SetDstCoordMark(rcMark);

							m_pactDef->AddSkinPaintMission(ppm);
							IdsDel_PaintMission(ppm);
						}
					}
				}
			}
		}
		delete pDoc;
		pDoc = NULL;
	}
}

void CIdsBuilder::InitUI()
{
	USES_CONVERSION;
	TiXmlDocument *pDoc = new TiXmlDocument();
	if(pDoc->LoadFile(T2A(m_strXMLPath)) )	
	{
		TiXmlElement* pRootElement = pDoc->RootElement();
		InitElement(pRootElement);

		delete pDoc;
		pDoc = NULL;
	}
}

void CIdsBuilder::InitElement(TiXmlElement* pElement, IdsUIBase* pParentBase)
{
	if( NULL == pElement )
	{
		return;
	}

	TiXmlElement* pChildUIElement = pElement->FirstChildElement(UI_XML_NODE_UICONTROL);
	while ( pChildUIElement )
	{
		TCHAR szClass[MAX_PATH] = {0};
		GetElementAttrValue(pChildUIElement, UI_XML_ATTR_CLASS, szClass, MAX_PATH-1);
		if( _tcslen(szClass) == 0 ) 
		{
			_tcscpy_s(szClass, MAX_PATH-1, L"dsBase");
		}

		TCHAR szName[MAX_PATH] = {0};
		GetElementAttrValue(pChildUIElement, UI_XML_ATTR_NAME, szName, MAX_PATH-1);

		int dwID = 0;
		GetElementAttrIntValue(pChildUIElement, UI_XML_ATTR_ID, dwID);

		CRect rcPos;
		GetElementAttrRectValue(pChildUIElement, UI_XML_ATTR_DEST, rcPos);

		CRect rcMark;
		GetElementAttrRectValue(pChildUIElement, UI_XML_ATTR_DESTMARK, rcMark);

		TCHAR szToolTip[MAX_PATH] = {0};
		GetElementAttrValue(pChildUIElement, UI_XML_ATTR_TOOLTIP, szToolTip, MAX_PATH-1);
		
		BOOL bShowHand = FALSE;
		GetElementAttrBooleanValue(pChildUIElement, UI_XML_ATTR_SHOWHAND, bShowHand);

		BOOL bEmbedHeader = FALSE;
		GetElementAttrBooleanValue(pChildUIElement, UI_XML_ATTR_EMBEDHEADER, bEmbedHeader);

		BOOL bVisable = TRUE;
		GetElementAttrBooleanValue(pChildUIElement, UI_XML_ATTR_VISABLE, bVisable);

		BOOL bEnable = TRUE;
		GetElementAttrBooleanValue(pChildUIElement, UI_XML_ATTR_ENABLE, bEnable);

		IdsUIBase* pbase = NULL;
		if( pParentBase )
		{
			pbase = pParentBase->CreateDsuiElement(szClass, szName, dwID, rcPos, rcMark, FALSE);
		}
		else
		{
			pbase = m_pactDef->CreateDsuiElement(szClass, szName, dwID, rcPos, rcMark, FALSE);
		}

		if( NULL == pbase )
		{
			continue;
		}

		pbase->SetTooltipText(szToolTip);
		if( bShowHand ) pbase->SetSystemCursor(32649);
		if( bEmbedHeader ) 
		{
			AddClientHitRect(dwID, CRect(0,0,0,0));
		}
		if( !bVisable ) 
		{
			pbase->SetVisible(FALSE);
		}
		if( !bEnable ) pbase->EnableUI(FALSE);

		// 绘制任务
		TiXmlElement* pPaintMissionElement = pChildUIElement->FirstChildElement(UI_XML_NODE_PAINTMISIION);
		while ( pPaintMissionElement )
		{
			BOOL bSuperText = FALSE;
			GetElementAttrBooleanValue(pPaintMissionElement, UI_XML_ATTR_SUPERTEXT, bSuperText);
			if( bSuperText )
			{
				CRect rcDest;
				BOOL bDestRet = GetElementAttrRectValue(pPaintMissionElement, UI_XML_ATTR_DEST, rcDest);

				CRect rcDestMark;
				BOOL bDestMarkRet = GetElementAttrRectValue(pPaintMissionElement, UI_XML_ATTR_DESTMARK, rcDestMark);

				int nParam = 0;
				BOOL bParamRet = GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_PARAM, nParam);

				int nHorAlign = 0;
				GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_TEXTHORALIGN, nHorAlign);

				int nVerAlign = 0;
				GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_TEXTVERALIGN, nVerAlign);

				BOOL bSigLine = TRUE;
				GetElementAttrBooleanValue(pPaintMissionElement, UI_XML_ATTR_SIGLINE, bSigLine);

				IdsPaintMission* ppm = IdsNew_PaintMission();
				if( ppm )
				{
					if(bDestRet) ppm->SetDstRect(rcDest);
					if(bDestMarkRet) ppm->SetDstCoordMark(rcDestMark);
					if(bParamRet) ppm->SetParam(nParam);

					UINT uDrawFlag = 0;
					if( nHorAlign == 0 ) uDrawFlag |= DT_LEFT;
					else if( nHorAlign == 1 ) uDrawFlag |= DT_CENTER;
					else uDrawFlag |= DT_RIGHT;

					if( nVerAlign == 0 ) uDrawFlag |= DT_TOP;
					else if( nVerAlign == 1 ) uDrawFlag |= DT_VCENTER;
					else uDrawFlag |= DT_BOTTOM;

					if( bSigLine ) 
					{
						uDrawFlag |= DT_SINGLELINE;
					}
					else
					{
						uDrawFlag |= DT_WORDBREAK;
						uDrawFlag |= DT_NOCLIP;
					}
					ppm->SetTextDrawFlag(uDrawFlag);

					TiXmlElement* pSuperTextPaintMissionElement = pPaintMissionElement->FirstChildElement(UI_XML_NODE_SUPERTEXT);
					while ( pSuperTextPaintMissionElement )
					{
						TCHAR szText[MAX_PATH] = {0};
						GetElementAttrValue(pSuperTextPaintMissionElement, UI_XML_ATTR_NAME, szText, MAX_PATH-1);

						COLORREF crText;
						GetElementAttrColorValue(pSuperTextPaintMissionElement, UI_XML_ATTR_TEXTCOLOR, crText);

						int nFontID = 0;
						GetElementAttrIntValue(pSuperTextPaintMissionElement, UI_XML_ATTR_FONTID, nFontID);

						ppm->AddSuperTextElement(szText, crText, nFontID);
						pSuperTextPaintMissionElement = pSuperTextPaintMissionElement->NextSiblingElement(UI_XML_NODE_SUPERTEXT);
					}

					pbase->AddPaintMisson(ppm);
					IdsDel_PaintMission(ppm);
				}
			}
			else
			{
				TCHAR szText[MAX_PATH] = {0};
				BOOL bTextRet = GetElementAttrValue(pPaintMissionElement, UI_XML_ATTR_NAME, szText, MAX_PATH-1);

				int nImageID = -1;
				BOOL bImageRet = GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_IMAGEID, nImageID);

				BOOL bStretch = FALSE;
				BOOL bStretchRet = GetElementAttrBooleanValue(pPaintMissionElement, UI_XML_ATTR_STRETCH, bStretch);
				
				CRect rcSquire9;
				BOOL bSquire9Ret = GetElementAttrRectValue(pPaintMissionElement, UI_XML_ATTR_SQUIRE9, rcSquire9);

				CRect rcSrc;
				BOOL bSrcRet = GetElementAttrRectValue(pPaintMissionElement, UI_XML_ATTR_SRC, rcSrc);

				CRect rcDest;
				BOOL bDestRet = GetElementAttrRectValue(pPaintMissionElement, UI_XML_ATTR_DEST, rcDest);

				CRect rcDestMark;
				BOOL bDestMarkRet = GetElementAttrRectValue(pPaintMissionElement, UI_XML_ATTR_DESTMARK, rcDestMark);

				int nState = 0;
				BOOL bStateRet = GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_STATE, nState);

				int nHorAlign = 0;
				GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_TEXTHORALIGN, nHorAlign);

				int nVerAlign = 0;
				GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_TEXTVERALIGN, nVerAlign);

				BOOL bSigLine = TRUE;
				GetElementAttrBooleanValue(pPaintMissionElement, UI_XML_ATTR_SIGLINE, bSigLine);

				COLORREF crText;
				BOOL bTextColorRet = GetElementAttrColorValue(pPaintMissionElement, UI_XML_ATTR_TEXTCOLOR, crText);

				int nFontID = -1;
				BOOL bFontRet = GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_FONTID, nFontID);

				int nParam = 0;
				BOOL bParamRet = GetElementAttrIntValue(pPaintMissionElement, UI_XML_ATTR_PARAM, nParam);

				COLORREF crBKColor;
				BOOL bBkColorRet = GetElementAttrColorValue(pPaintMissionElement, UI_XML_ATTR_BACKCOLOR, crBKColor);

				IdsPaintMission* ppm = IdsNew_PaintMission();
				if( ppm )
				{
					if( bImageRet )
					{	
						ppm->SetImageID(nImageID);
						if( bStretchRet && bStretch ) 
						{
							ppm->SetPaintMode(DSUI_PAINTMODE_STRETCH);
						}
						if( bSrcRet ) ppm->SetSrcRect(rcSrc);
						if( bSquire9Ret ) ppm->SetSquares9(rcSquire9);
					}
					else if( bTextRet )
					{
						ppm->SetText(szText);
						if(bFontRet) ppm->SetFont(nFontID);
						if(bTextColorRet) ppm->SetColor(crText);

						UINT uDrawFlag = 0;
						if( nHorAlign == 0 ) uDrawFlag |= DT_LEFT;
						else if( nHorAlign == 1 ) uDrawFlag |= DT_CENTER;
						else uDrawFlag |= DT_RIGHT;

						if( nVerAlign == 0 ) uDrawFlag |= DT_TOP;
						else if( nVerAlign == 1 ) uDrawFlag |= DT_VCENTER;
						else uDrawFlag |= DT_BOTTOM;

						if( bSigLine ) 
						{
							uDrawFlag |= DT_SINGLELINE;
						}
						else
						{
							uDrawFlag |= DT_WORDBREAK;
							uDrawFlag |= DT_NOCLIP;
						}
						ppm->SetTextDrawFlag(uDrawFlag);
					}
					else if( bBkColorRet )
					{
						ppm->SetColor(crBKColor);
					}
					if(bDestRet) ppm->SetDstRect(rcDest);
					if(bDestMarkRet) ppm->SetDstCoordMark(rcDestMark);	
					if( bStateRet ) ppm->SetState(nState);
					if( bParamRet ) ppm->SetParam(nParam);	
					pbase->AddPaintMisson(ppm);

					IdsDel_PaintMission(ppm);
				}
			}
			pPaintMissionElement = pPaintMissionElement->NextSiblingElement(UI_XML_NODE_PAINTMISIION);
		}

		int nGifParam = 0;
		BOOL bMarkAsGifRet = GetElementAttrIntValue(pChildUIElement, UI_XML_ATTR_MARKASGIF, nGifParam);
		if( bMarkAsGifRet )
		{
			IdsPaintMission* pMission = pbase->GetPaintMisson(nGifParam);
			if( pMission )  pMission->MarkAsGif((DWORD)pbase);
		}

		// 子控件
		InitElement(pChildUIElement, pbase);
		pChildUIElement = pChildUIElement->NextSiblingElement(UI_XML_NODE_UICONTROL);
	}
}

void CIdsBuilder::InitFont()
{
	USES_CONVERSION;
	TiXmlDocument *pDoc = new TiXmlDocument();
	if(pDoc->LoadFile(T2A(m_strXMLPath)) )	
	{
		TiXmlElement* pRootElement = pDoc->RootElement();
		if( pRootElement )
		{
			TiXmlElement* pFontElement = pRootElement->FirstChildElement(UI_XML_NODE_FONT);
			while ( pFontElement )
			{
				TCHAR szFontName[MAX_PATH] = {0};
				GetElementAttrValue( pFontElement, UI_XML_ATTR_NAME, szFontName, MAX_PATH-1 );

				int nFontSize = 100;
				GetElementAttrIntValue( pFontElement, UI_XML_ATTR_SIZE, nFontSize);

				TCHAR szFaceName[MAX_PATH] = {0};
				GetElementAttrValue( pFontElement, UI_XML_ATTR_FACENAME, szFaceName, MAX_PATH-1 );

				BOOL bBold = FALSE;
				GetElementAttrBooleanValue( pFontElement, UI_XML_ATTR_BOLD, bBold );

				BOOL bItalic = FALSE;
				GetElementAttrBooleanValue( pFontElement, UI_XML_ATTR_ITALIC, bItalic );
				
				IdsFontMgr * pFontMgr = IdsGet_FontMgr();
				if( pFontMgr ) 
				{
					int nFontID = pFontMgr->NewFont(szFontName, nFontSize, szFaceName, bBold, bItalic, false);
					if( nFontID == -1 )
					{
						assert(FALSE);
					}
				}
				pFontElement = pFontElement->NextSiblingElement(UI_XML_NODE_FONT);
			}
		}
		delete pDoc;
		pDoc = NULL;
	}
}

void CIdsBuilder::InitImage()
{
	TCHAR szFilePath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szFilePath, MAX_PATH-1);
	CString strModulePath = szFilePath;
	int nFind = strModulePath.ReverseFind(L'\\');
	if( -1 != nFind ) strModulePath = strModulePath.Left(nFind+1);

	USES_CONVERSION;
	TiXmlDocument *pDoc = new TiXmlDocument();
	if(pDoc->LoadFile(T2A(m_strXMLPath)) )	
	{
		TiXmlElement* pRootElement = pDoc->RootElement();
		if( pRootElement )
		{
			TiXmlElement* pImageElement = pRootElement->FirstChildElement(UI_XML_NODE_IMAGE);
			while ( pImageElement )
			{
				TCHAR szImagePath[MAX_PATH] = {0};
				GetElementAttrValue( pImageElement, UI_XML_ATTR_IMAGEPATH, szImagePath, MAX_PATH-1 );

				IdsImageMgr * pImageMgr = IdsGet_ImageMgr();
				if( pImageMgr ) 
				{
					CString strFilePath = strModulePath + szImagePath;
					int nImageid = pImageMgr->NewImage(strFilePath, NULL, false);
					if( nImageid == -1 )
					{
						assert(FALSE);
					}
				}
				pImageElement = pImageElement->NextSiblingElement(UI_XML_NODE_IMAGE);
			}
		}
		delete pDoc;
		pDoc = NULL;
	}
}

void CIdsBuilder::ConvertUTF8toGBK(LPCSTR lptcsValue,LPTSTR lptcsBuffer, int nBufferLen )
{
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lptcsValue, -1, NULL,0);
	TCHAR* tcsUTF8= new TCHAR[len+1];
	ZeroMemory( tcsUTF8, sizeof(TCHAR)*(len+1) );
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lptcsValue, -1, tcsUTF8, len);
	_tcscpy_s( lptcsBuffer, nBufferLen, tcsUTF8 );
	delete []tcsUTF8;
}

void CIdsBuilder::SplitString(CString strSrc, TCHAR cSplit, vector<CString> &vecDest)
{
	CString strLeft = strSrc;
	while(TRUE)
	{	
		int nFind = strLeft.Find(cSplit);
		if( -1 == nFind )
		{
			strLeft.Trim();
			if(!strLeft.IsEmpty())
			{
				vecDest.push_back(strLeft);
			}
			break;
		}
		else
		{
			CString strPre = strLeft.Left(nFind);
			strPre.Trim();
			if(!strPre.IsEmpty())
			{
				vecDest.push_back(strPre);
			}
			strLeft = strLeft.Right(strLeft.GetLength()-nFind-1);
		}
	}

}

void CIdsBuilder::ConvertGBKtoUTF8( LPCSTR lptcsValue, LPSTR lptcsBuffer, int nBufferLen )
{
	if(lptcsValue == NULL)
		return ;

	int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lptcsValue, -1, NULL,0);
	len += 1;
	WCHAR * wszUtf8 = new WCHAR[len];
	if(wszUtf8)
	{
		memset(wszUtf8,0,len*sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lptcsValue, -1, wszUtf8, len);

		len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
		if(len > nBufferLen)
		{
			len = nBufferLen;
		}

		WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, lptcsBuffer, len, NULL, NULL);

		delete [] wszUtf8;
	}
}

BOOL CIdsBuilder::GetElementAttrValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, LPTSTR lptcsAttrValue, int nBufferLen )
{
	BOOL bOk = FALSE;
	if( !pElement || !lptcsNodeAttr || !lptcsAttrValue || nBufferLen<= 0 )
	{
		return bOk;
	}
	if( pElement->Attribute( lptcsNodeAttr ) )
	{
		ConvertUTF8toGBK( pElement->Attribute(lptcsNodeAttr), lptcsAttrValue, nBufferLen );
		bOk = TRUE;
	}

	return bOk;
}

BOOL CIdsBuilder::GetElementAttrIntValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, int &nValue )
{
	BOOL bOk = FALSE;
	if( !pElement || !lptcsNodeAttr || !lptcsNodeAttr )
	{
		return bOk;
	}
	if( pElement->Attribute( lptcsNodeAttr ) )
	{
		nValue = atoi( pElement->Attribute( lptcsNodeAttr ) );
		bOk = TRUE;
	}

	return bOk;
}

BOOL CIdsBuilder::GetElementAttrBooleanValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, BOOL &bValue )
{
	BOOL bOk = FALSE;
	if( !pElement || !lptcsNodeAttr || !lptcsNodeAttr )
	{
		return bOk;
	}

	const char *lptcsNodeAttrValue = pElement->Attribute( lptcsNodeAttr );
	if( lptcsNodeAttrValue )
	{
		char szNodeAttrValue[MAX_PATH] = {0};
		strcpy_s(szNodeAttrValue, MAX_PATH-1, lptcsNodeAttrValue );
		_strupr_s(szNodeAttrValue, MAX_PATH);
		if( stricmp(szNodeAttrValue, "TRUE") == 0 )
		{
			bValue = TRUE;
		}
		else
		{
			bValue = FALSE;
		}
		bOk = TRUE;
	}
	return bOk;
}

BOOL CIdsBuilder::GetElementAttrRectValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, LPRECT lprcPos )
{
	USES_CONVERSION;

	BOOL bOk = FALSE;
	if( !pElement || !lptcsNodeAttr || !lprcPos )
	{
		return bOk;
	}

	if( pElement->Attribute( lptcsNodeAttr ) )
	{
		string strAttr = pElement->Attribute(lptcsNodeAttr);
		CString strPos = A2T(strAttr.c_str());
		strPos.Replace(L"CRect(", L"");
		strPos.Remove(L')');
		strPos.Remove(L' ');

		vector<CString> vecRect;
		SplitString(strPos, L',', vecRect);
		if( vecRect.size() > 0 ) lprcPos->left = _ttoi(vecRect[0]);
		if( vecRect.size() > 1 ) lprcPos->top = _ttoi(vecRect[1]);
		if( vecRect.size() > 2 ) lprcPos->right = _ttoi(vecRect[2]);
		if( vecRect.size() > 3 ) lprcPos->bottom = _ttoi(vecRect[3]);

		bOk = TRUE;
	}
	return bOk;
}

BOOL CIdsBuilder::GetElementAttrColorValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, DWORD &dwValue )
{
	USES_CONVERSION;

	BOOL bOk = FALSE;
	if( !pElement || !lptcsNodeAttr || !lptcsNodeAttr )
	{
		return bOk;
	}
	if( pElement->Attribute( lptcsNodeAttr ) )
	{
		string strAttr = pElement->Attribute(lptcsNodeAttr);
		CString strColor( A2T(strAttr.c_str()));
		strColor.Replace(L"RGB(", L"");
		strColor.Remove(L')');
		strColor.Remove(L' ');

		int nR=0, nG=0, nB=0;

		vector<CString> vecColor;
		SplitString(strColor, L',', vecColor);
		if( vecColor.size() > 0 ) nR = _ttoi(vecColor[0]);
		if( vecColor.size() > 1 ) nG = _ttoi(vecColor[1]);
		if( vecColor.size() > 2 ) nB = _ttoi(vecColor[2]);

		dwValue = RGB( nR, nG, nB );
		bOk = TRUE;
	}
	return bOk;
}