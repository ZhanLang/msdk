/*
 *   Copyright (C) =USTC= Fu Li
 *
 *   Author   :  Fu Li
 *   Create   :  2005-3-11
 *   Home     :  www.crazy-bit.com
 *   Mail     :  crazybit@263.net
 *   History  :  
 */
#pragma once
#include "ctrl_list_item.h"

class FCLayout_HandlerBase ;
    class FCLayout_FixLineCount ;

class FCLayout_AutoItemSize ;

//-------------------------------------------------------------------------------------
/**
    Layout list items.
*/
class FCLayout_HandlerBase
{
public:
    virtual ~FCLayout_HandlerBase() {}

    /**
        Layout all items in itemList \n
        item_list - user must set item's position on canvas.
    */
    virtual void Layout_ListItem (CWnd* pWnd, std::deque<FCListItem_Base*>& item_list) =0 ;

protected:
    /// Set window's vertical scrollbar range and pagezie.
    static void SetVScrollRange (CWnd* pWnd, int nCanvasHeight)
    {
        BOOL    bShow = pWnd->GetStyle() & WS_VSCROLL ;
        CRect   rcClient ;
        pWnd->GetClientRect(rcClient) ;
        if (nCanvasHeight > rcClient.Height())
        {
            if (!bShow)
            {
                pWnd->ModifyStyle (0, WS_VSCROLL, SWP_FRAMECHANGED) ;
                pWnd->SetScrollPos (SB_VERT, 0, FALSE) ;
            }

            SCROLLINFO   si = {0} ;
            si.cbSize = sizeof(SCROLLINFO) ;
            si.nMin = 0 ;
            si.nMax = nCanvasHeight - 1 ; // notice -1
            si.nPage = rcClient.Height() ;
            si.fMask = SIF_RANGE | SIF_PAGE ;
            pWnd->SetScrollInfo (SB_VERT, &si, TRUE) ;
        }
        else
        {
            if (bShow)
            {
                pWnd->ModifyStyle (WS_VSCROLL, 0, SWP_FRAMECHANGED) ;
                pWnd->SetScrollPos (SB_VERT, 0, FALSE) ;
            }
        }
    }
};

//-------------------------------------------------------------------------------------
/**
    Vertical array items.
*/
class FCLayout_FixLineCount : public FCLayout_HandlerBase
{
    int   m_nXNum ;
    int   m_height ;
public:
    /**
        Constructor \n
        nXNum - item count per line (item's width depend on this param) \n
        nHeightPixel - item's height in pixel
    */
    FCLayout_FixLineCount (int nXNum, int nHeightPixel)
    {
        SetLineItemCount(nXNum) ;
        SetItemHeight(nHeightPixel) ;
    }

    void SetLineItemCount (int nXNum) { m_nXNum = __max(1,nXNum); }
    void SetItemHeight (int nHeight) { m_height = __max(1,nHeight); }

    virtual void Layout_ListItem (CWnd* pWnd, std::deque<FCListItem_Base*>& itemList)
    {
        if (itemList.empty())
        {
            pWnd->ModifyStyle (WS_VSCROLL, 0, SWP_FRAMECHANGED) ;
            pWnd->SetScrollPos (SB_VERT, 0, FALSE) ;
            return ;
        }

        CRect     rcClient ;
        pWnd->GetClientRect(rcClient) ;

        // if need scroll-bar, deflate client area
        BOOL    bSBShow = pWnd->GetStyle() & WS_VSCROLL ;
        int     nCanvasHeight = (int)ceil(itemList.size()/(double)m_nXNum) * m_height ;
        if ((nCanvasHeight > rcClient.Height()) && !bSBShow)
        {
            rcClient.right -= GetSystemMetrics(SM_CXVSCROLL) ;
        }
        if ((nCanvasHeight <= rcClient.Height()) && bSBShow)
        {
            rcClient.right += GetSystemMetrics(SM_CXVSCROLL) ;
        }

        // set item's position
        int     nIW = rcClient.Width() / m_nXNum ;
        CRect   rcBound (0,0,0,0) ;
        for (int i=0 ; i < (int)itemList.size() ; i++)
        {
            int     nX = (i % m_nXNum) * nIW,
                    nY = (i / m_nXNum) * m_height ;
            CRect   rcItem (CPoint(nX,nY), CSize(nIW,m_height)) ;
            itemList[i]->SetRectOnCanvas (rcItem) ;
            rcBound.UnionRect (rcBound, rcItem) ;
        }

        SetVScrollRange (pWnd, rcBound.Height()) ;
    }
};

//-------------------------------------------------------------------------------------
/// Dynamic item size.
class FCLayout_AutoItemSize : public FCLayout_HandlerBase
{
    int   m_min_size ;
    FCLayout_FixLineCount   m_proxy ;
public:
    FCLayout_AutoItemSize (int nMinSize) : m_proxy(1,1)
    {
        m_min_size = __max(nMinSize,1) ;
    }

private:
    virtual void Layout_ListItem (CWnd* pWnd, std::deque<FCListItem_Base*>& itemList)
    {
        CRect   rc ;
        pWnd->GetClientRect (rc) ;

        int   nx = rc.Width() / m_min_size ;
        m_proxy.SetLineItemCount (nx) ;
        int   ny = __max(1, rc.Height() / m_min_size) ;
        m_proxy.SetItemHeight (rc.Height() / ny) ;

        m_proxy.Layout_ListItem (pWnd, itemList) ;
    }
};
