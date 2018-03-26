/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,17th,2014

Module Name:

	dsScroll.h 

Abstract: scroll 实类


*/
/************************************************************************/

#ifndef __DSSCROLL_H__
#define __DSSCROLL_H__


#include "timer.h"


#define FLAG_NULL -1
#define FLAG_UP	0
#define FLAG_DOWN	1
#define FLAG_PAGEUP	2
#define FLAG_PAGEDOWN	3


class dsScroll:public IdsScroll,public dsUIBase,public dsTimer
{
public:
	SCROLLINFO m_si;
	LPARAM m_lParamForegnd;
	BOOL m_bPaintSwitch;
	BOOL m_bVert;

	CPoint m_ptDown;
	int m_nPosDown;

	int m_nThumbState;

	int m_nFlag; 

		
public:



	static dsUIBase* CALLBACK NewDsuiElesInst(){
		return new dsScroll();
	}
	static BOOL GetClassInfo(__out PDSUICLASSINFO pci){
		if(NULL == pci)
			return FALSE;

		pci->procNewUI = NewDsuiElesInst;
		pci->strClassName = L"dsScroll";
		return TRUE;
	}

	dsScroll():m_lParamForegnd(-1), m_bPaintSwitch(FALSE),m_bVert(TRUE),m_nThumbState(DSUI_STATE_NORMAL),m_nFlag(FLAG_NULL)
	{
		m_si.nMin = 0;
		m_si.nMax = 100;
		m_si.nPos = 0;
		m_si.nPage = m_si.nMax - m_si.nMin;
	}


	static void CALLBACK TimerProc(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
	{
		dsScroll* pThis = (dsScroll*)dwUser;

		BOOL bEffect = FALSE;

		int nNotify = SB_THUMBPOSITION;

		switch(pThis->m_nFlag)
		{
		case FLAG_PAGEUP:
			pThis->SetPos(pThis->m_si.nPos - pThis->m_si.nPage);
			nNotify = SB_PAGEUP;
			bEffect = TRUE;
			break;

		case FLAG_PAGEDOWN:
			pThis->SetPos(pThis->m_si.nPos + pThis->m_si.nPage);
			nNotify = SB_PAGEDOWN;
			bEffect = TRUE;
			break;

		default:

			break;
		}

		if(bEffect)
		{
			pThis->RedrawUI();
			//RedrawWindow(pThis->m_hwnd, pThis->m_rcUI, NULL, 0);


			int uScrollMsg = pThis->m_bVert ? WM_VSCROLL:WM_HSCROLL;

			::SendMessage(pThis->m_hwnd, uScrollMsg, MAKEWPARAM(nNotify, pThis->m_si.nTrackPos), (LPARAM)pThis);
		}

		

	}



public:
	virtual int SetScrollInfo(__in LPSCROLLINFO lpsi){
		

		bool bMyRedraw = FALSE;

		//if (SIF_TRACKPOS & lpsi->fMask)
		//{
		//	m_si.nTrackPos = lpsi->nTrackPos;
		//}

		if ((SIF_RANGE & lpsi->fMask) && (lpsi->nMax > lpsi->nMin))
		{
			int nOldMin(m_si.nMin), nOldMax(m_si.nMax);
			m_si.nMin = lpsi->nMin;
			m_si.nMax = lpsi->nMax;
			if(nOldMin != m_si.nMin  || nOldMax != m_si.nMax) bMyRedraw = TRUE;

		}

		if ((SIF_PAGE & lpsi->fMask) && (m_si.nPage != lpsi->nPage))
		{
			if(lpsi->nPage > 0 && lpsi->nPage <= (m_si.nMax - m_si.nMin))
			{
				m_si.nPage = lpsi->nPage;
				bMyRedraw = TRUE;
			}
		}

		if (SIF_POS & lpsi->fMask)
		{
			int nOldPos = m_si.nPos;

			m_si.nPos = lpsi->nPos;
			m_si.nPos = m_si.nTrackPos = min(max(m_si.nMin, m_si.nPos), (int)(m_si.nMax - m_si.nPage/* + 1*/));

			if (nOldPos != m_si.nPos) bMyRedraw = TRUE;
		}

		if (TRUE)
		{
			dsuiInvalidateRect(m_hwnd, m_rcUI, TRUE);;
		}

		return m_si.nPos;
	}
	virtual BOOL GetScrollInfo(__inout LPSCROLLINFO lpsi){

		if (SIF_POS & lpsi->fMask)
		{
			lpsi->nPos = m_si.nPos;
		}

		if (SIF_TRACKPOS & lpsi->fMask)
		{
			lpsi->nTrackPos = m_si.nTrackPos;
		}

		if (SIF_RANGE & lpsi->fMask)
		{
			lpsi->nMin = m_si.nMin;
			lpsi->nMax = m_si.nMax;
		}

		if (SIF_PAGE & lpsi->fMask)
		{
			lpsi->nPage = m_si.nPage;
		}

		return TRUE;
	}

	virtual int SetPos(int nPos)
	{
		SCROLLINFO si;
		si.cbSize = sizeof SCROLLINFO;
		si.fMask = SIF_POS;
		si.nPos = nPos;
		return SetScrollInfo(&si);
	}

	virtual int GetPos()
	{
		SCROLLINFO si;
		si.cbSize = sizeof SCROLLINFO;
		si.fMask = SIF_POS;

		if(!GetScrollInfo(&si))
			return -1;

		return si.nPos;
	}

	virtual void SetForegndPaint(LPARAM lParamForFind){
		m_lParamForegnd = lParamForFind;
	}

	virtual BOOL PaintSwitch(BOOL bPaint){
		return dsTools::Exchange2Objects(m_bPaintSwitch, bPaint);
	}


	virtual void DealPaintMisson( dsPaintMission *pdspm, CDCHandle dc, LPPAINTSTRUCT lpps )
	{
		if(NULL == pdspm || NULL == lpps || !m_bPaintSwitch)
			return;

		CRect rcDst;

		if (pdspm->m_lParam == m_lParamForegnd)
		{
			GetThumbRect(rcDst);
		}
		else
			pdspm->GetRealDstRect(m_rcUI, rcDst);

		CRect rcSrc;
		pdspm->GetRealSrcRect(rcSrc);

		CRect rcPaint(lpps->rcPaint);

		Image *pimg = pdspm->GetImagePtr();
		if(NULL != pimg){

			Graphics grah(dc);

			NSST nsst;

			if(DSUI_PAINTMODE_STRETCH == pdspm->m_nPaintMode){

				if(ds9SquaresStretch::Analysis(rcDst, rcSrc, pdspm->m_rcSquares9, &nsst)){

					for (int i=0; i<9; i++)
					{
						DrawImage(dc, &grah, pimg, &nsst.tables[i].rcDst, &nsst.tables[i].rcSrc, rcPaint, nsst.tables[i].nPaintMode);
					}
				}
				else{
					DrawImage(dc, &grah, pimg, rcDst, rcSrc, rcPaint, DSUI_PAINTMODE_STRETCH);
				}

			}
			else{
				DrawImage(dc, &grah, pimg, rcDst, rcSrc, rcPaint, DSUI_PAINTMODE_NORMAL);
			}

		}
		CString &strText = pdspm->GetTextStrQuote();
		if(strText.GetLength() > 0){
			dc.SetBkMode(TRANSPARENT);

			dc.DrawText(strText, strText.GetLength(), rcDst, DT_LEFT | DT_WORDBREAK);
		}
	}

	void GetThumbRect(__out LPRECT lprcThumb)
	{

		if (NULL == lprcThumb)
			return;

		if (m_bVert)
		{
			float yRatio_Height_Range = (float)m_rcUI.Height()/(m_si.nMax - m_si.nMin);

			::CopyRect(lprcThumb, m_rcUI);

			lprcThumb->top = m_rcUI.top + (m_si.nPos - m_si.nMin) * yRatio_Height_Range;
			lprcThumb->bottom = lprcThumb->top + m_si.nPage * yRatio_Height_Range;

			if(lprcThumb->bottom - lprcThumb->top < 20)//20为thumb条的最小宽度或者高度
				lprcThumb->bottom = lprcThumb->top + 20;

		}
		else
		{
			float xRatio_Height_Range = (float)m_rcUI.Width()/(m_si.nMax - m_si.nMin);

			::CopyRect(lprcThumb, m_rcUI);

			lprcThumb->left = m_rcUI.left + (m_si.nPos - m_si.nMin) * xRatio_Height_Range;
			lprcThumb->right = lprcThumb->left + m_si.nPage * xRatio_Height_Range;

			if(lprcThumb->right - lprcThumb->left < 20)
				lprcThumb->right = lprcThumb->left + 20;
		}

	}


	virtual BOOL ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		

		BOOL bRedraw = FALSE;

		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			{

				CRect rcThumb;
				GetThumbRect(rcThumb);

				m_ptDown.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

				dsGlobalStateMgr::getInstance()->SetCapture(this);

				if (rcThumb.PtInRect(m_ptDown))
				{
					m_nThumbState = DSUI_STATE_PUSHED;


					m_nPosDown = m_si.nPos;

					DestroyTimer();

					m_nFlag = FLAG_NULL;

				}
				else if(FLAG_NULL == m_nFlag)
				{
					if(m_bVert)
					{
						if(m_ptDown.y < rcThumb.top)
						{
							m_nFlag = FLAG_PAGEUP;
						}
						else if(m_ptDown.y > rcThumb.bottom)
						{
							m_nFlag = FLAG_PAGEDOWN;
						}
					}
					else{

						if(m_ptDown.x < rcThumb.left)
						{
							m_nFlag = FLAG_PAGEUP;
						}
						else if(m_ptDown.x > rcThumb.bottom)
						{
							m_nFlag = FLAG_PAGEDOWN;
						}
					}

					CreateTimer(2, 10, (DWORD_PTR)this , TimerProc);
				}

				

				
			}

		case WM_MOUSEMOVE:
			{

				if (!m_bPaintSwitch && (DSUI_STATE_NORMAL & m_nThumbState) )
				{
					m_bPaintSwitch = TRUE;
					bRedraw = TRUE;
				}

				if (DSUI_STATE_PUSHED & m_nThumbState)
				{
					bRedraw = TRUE;

					CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

					if (m_bVert)
					{
						int yOffset = GET_Y_LPARAM(lParam) - m_ptDown.y;

						float yRatio_Range_Height = (float)(m_si.nMax - m_si.nMin) / m_rcUI.Height();
						m_si.nTrackPos = m_nPosDown + yRatio_Range_Height * yOffset;

					}
					else
					{
						int xOffset = GET_X_LPARAM(lParam) - m_ptDown.x;

						float xRatio_Range_Width = (float)(m_si.nMax - m_si.nMin) / m_rcUI.Width();
						m_si.nTrackPos = m_nPosDown + xRatio_Range_Width * xOffset;

					}

					m_si.nPos = m_si.nTrackPos = min(max(m_si.nMin, m_si.nTrackPos), (int)(m_si.nMax - m_si.nMin - m_si.nPage/* + 1*/));
					
					int uScrollMsg = m_bVert ? WM_VSCROLL:WM_HSCROLL;

					::SendMessage(hwnd, uScrollMsg, MAKEWPARAM(SB_THUMBPOSITION, m_si.nTrackPos), (LPARAM)this);


				}


			
			}


			break;

		case WM_MOUSELEAVE:
			{
				m_nThumbState = DSUI_STATE_NORMAL;
			}

			break;

		case WM_LBUTTONUP:
			{
				dsGlobalStateMgr::getInstance()->ReleaseCapture();

				m_nThumbState = DSUI_STATE_NORMAL;

				DestroyTimer();

				m_nFlag = FLAG_NULL;

				bRedraw = TRUE;

			}
			break;


		case WM_MOUSEWHEEL:
			{
				short wForwardCount = (short)HIWORD(wParam) / -120;

				m_si.nPos += (short)wForwardCount * (m_si.nPage / 4);
				m_si.nPos = min(max(m_si.nMin, m_si.nPos), (int)(m_si.nMax - m_si.nMin - m_si.nPage/* + 1*/));

				m_si.nTrackPos = m_si.nPos;

				int uScrollMsg = m_bVert ? WM_VSCROLL:WM_HSCROLL;

				::SendMessage(hwnd, uScrollMsg, MAKEWPARAM(SB_THUMBPOSITION, m_si.nTrackPos), (LPARAM)this);

				bRedraw = TRUE;
			}

		default:

			break;
		}

		if(bRedraw)
		{
			dsuiInvalidateRect(hwnd, m_rcUI, TRUE);
		}

		return FALSE;
	}
};



#endif /*__DSSCROLL_H__*/