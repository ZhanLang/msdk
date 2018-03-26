/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,17th,2014

Module Name:

	dsProgress.h 

Abstract: progress ÊµÀà


*/
/************************************************************************/

#ifndef __DSPROGRESS_H__
#define __DSPROGRESS_H__

#include "dsGifAnimateMgr.h"


class dsProgress:public IdsProgress,public dsUIBase
{
public:
	int m_nMin,m_nMax,m_nPos;
	LPARAM m_lParamForegnd;

public:

	static dsUIBase* CALLBACK NewDsuiElesInst(){
		return new dsProgress();
	}
	static BOOL GetClassInfo(__out PDSUICLASSINFO pci){
		if(NULL == pci)
			return FALSE;
		
		pci->procNewUI = NewDsuiElesInst;
		pci->strClassName = L"dsProgress";
		return TRUE;
	}

	dsProgress():m_nMin(0), m_nMax(100), m_nPos(0), m_lParamForegnd(-1)
	{}
public:
	virtual void SetRange(int nMin, int nMax){
		if(nMin == m_nMin && nMax == m_nMax)
			return;

		m_nMin = nMin;
		m_nMax = nMax;

		m_nPos = max(m_nMin, min(m_nMax, m_nPos));

		dsuiInvalidateRect(m_hwnd, m_rcUI, FALSE);
	}
	virtual BOOL GetRange(__out LPINT lpnMin, __out LPINT lpnMax){
		if(NULL == lpnMin || NULL == lpnMax)
			return FALSE;

		*lpnMin = m_nMin;
		*lpnMax = m_nMax;

		return TRUE;
	}
	virtual int SetPos(int nPos){
		if(m_nMin <= nPos && nPos <= m_nMax){
			m_nPos = nPos;
			dsuiInvalidateRect(m_hwnd, m_rcUI, FALSE);
		}
		return m_nPos;
	}
	virtual int GetPos()
	{
		return m_nPos;
	}
	virtual void SetForegndPaint(LPARAM lParamForFind)
	{
		m_lParamForegnd = lParamForFind;
	}

	virtual void DealPaintMisson( dsPaintMission *pdspm, CDCHandle dc, LPPAINTSTRUCT lpps )
	{

		if(pdspm->m_lParam != m_lParamForegnd){
			dsUIBase::DealPaintMisson(pdspm, dc, lpps);
			return;
		}

		if(NULL == pdspm || NULL == lpps)
			return;

		CRect rcDst;

		if (pdspm->m_lParam == m_lParamForegnd)
		{

			if(m_nPos == m_nMin)
				return;

			rcDst.CopyRect(m_rcUI);
			rcDst.right = rcDst.left + rcDst.Width() * (float)(m_nPos - m_nMin)/(float)(m_nMax - m_nMin);

			CRect rcOld(rcDst);
			pdspm->GetRealDstRect(rcOld, rcDst);
		}
		else
			pdspm->GetRealDstRect(m_rcUI, rcDst);

		CRect rcSrc;
		pdspm->GetRealSrcRect(rcSrc);

		CRect rcPaint(lpps->rcPaint);

		int nDrawCount = 0;

		Image *pimg = pdspm->GetImagePtr();
		if(NULL != pimg){

			dsGifAnimateMgr *pgam = dsGifAnimateMgr::getInstance();

			//if(0 != pdspm->m_dwOldGifKey){
			//	pgam->DestroyAnimate(pdspm->m_dwOldGifKey);
			//	pdspm->m_dwOldGifKey = 0;
			//}

			if(0 != pdspm->m_dwGifKey)
			{
				GUID   pageGuid = FrameDimensionTime;



				if(!pgam->CheckGifAnimate(pdspm->m_dwGifKey))
					pgam->StartAnimate(this, pdspm->m_dwGifKey, pimg);
				pimg->SelectActiveFrame(&pageGuid, pgam->GetImgFrmPos(pdspm->m_dwGifKey));
			}

			nDrawCount ++;

			Graphics grah(dc);

			NSST nsst;

			if(DSUI_PAINTMODE_STRETCH == pdspm->m_nPaintMode){

				if(ds9SquaresStretch::Analysis(rcDst, rcSrc, pdspm->m_rcSquares9, &nsst)){

					for (int i=0; i<9; i++)
					{
						DrawImage(dc, &grah, pimg, &nsst.tables[i].rcDst, &nsst.tables[i].rcSrc, rcPaint, nsst.tables[i].nPaintMode, pdspm->m_bMarkAsIcon);
					}
				}
				else{
					DrawImage(dc, &grah, pimg, rcDst, rcSrc, rcPaint, DSUI_PAINTMODE_STRETCH, pdspm->m_bMarkAsIcon);
				}

			}
			else{
				DrawImage(dc, &grah, pimg, rcDst, rcSrc, rcPaint, DSUI_PAINTMODE_NORMAL, pdspm->m_bMarkAsIcon);
			}

		}

		HICON hIcon = pdspm->GetIcon();
		if(NULL != hIcon){

			nDrawCount ++;

			CRgn rgn;
			rgn.CreateRectRgnIndirect(rcPaint);
			::ExtSelectClipRgn(dc, rgn, RGN_AND);

			DrawIconEx(dc, rcDst.left, rcDst.top, hIcon, rcDst.Width(), rcDst.Height(), 0, NULL, DI_NORMAL);

			dc.SelectClipRgn(NULL);

		}

		CString &strText = pdspm->GetTextStrQuote();

		int nCountOfSuperText = pdspm->m_vecSuperText.size();

		BOOL bSuperText = nCountOfSuperText > 0;

		if(bSuperText)
		{
			strText = m_strText;

			CSize sizeOffset;

			CRgn rgn;
			rgn.CreateRectRgnIndirect(rcPaint);
			::ExtSelectClipRgn(dc, rgn, RGN_AND);

			dc.SetBkMode(TRANSPARENT);



			for (int i=0; i<nCountOfSuperText; i++)
			{

				CRect rcDraw(rcDst);

				SUPERTEXT &st = pdspm->m_vecSuperText[i];

				strText = st.strText;

				if(strText.IsEmpty() && NULL == pimg)
					strText = m_strText;

				if(strText.GetLength() > 0){

					nDrawCount ++;

					HFONT hFontOld = dc.SelectFont(st.hFontText);
					COLORREF crOld = dc.SetTextColor(st.crText);

					rcDraw.OffsetRect(sizeOffset);

					dc.DrawText(strText, strText.GetLength(), rcDraw, pdspm->m_uFormat);

					CSize sizeTemp;
					dc.GetTextExtent(strText, strText.GetLength(), &sizeTemp);

					if('\n'  == strText.Find(strText.GetLength() - 1) ||
						('\n' == strText.Find(strText.GetLength() - 2) && '\r' == strText.Find(strText.GetLength() - 1)))
					{
						sizeOffset.cx = 0;
						sizeOffset.cy += sizeTemp.cy + pdspm->m_sizeSuperTextScale.cy;
					}
					else
					{
						sizeOffset.cx += sizeTemp.cx + pdspm->m_sizeSuperTextScale.cx;
					}

					dc.SelectFont(hFontOld);
					dc.SetTextColor(crOld);	
				}

			}

			dc.SelectClipRgn(NULL);


		}
		else
		{
			if(strText.IsEmpty() && NULL == pimg && pdspm->m_bUseUIText)
				strText = m_strText;

			if(strText.GetLength() > 0){

				nDrawCount ++;

				CRgn rgn;
				rgn.CreateRectRgnIndirect(rcPaint);
				::ExtSelectClipRgn(dc, rgn, RGN_AND);

				dc.SetBkMode(TRANSPARENT);


				HFONT hFontOld = dc.SelectFont(pdspm->m_hFont);
				COLORREF crOld = dc.SetTextColor(pdspm->m_crText);

				dc.DrawText(strText, strText.GetLength(), rcDst, pdspm->m_uFormat);

				dc.SelectFont(hFontOld);
				dc.SetTextColor(crOld);

				dc.SelectClipRgn(NULL);
			}
		}

		if (0 == nDrawCount && NULL == pdspm->m_hFont)
		{
			CRgn rgn;
			rgn.CreateRectRgnIndirect(rcPaint);
			::ExtSelectClipRgn(dc, rgn, RGN_AND);

			dc.FillSolidRect(rcDst, pdspm->m_crText);

			dc.SelectClipRgn(NULL);
		}

	}
};


#endif /*__DSPROGRESS_H__*/