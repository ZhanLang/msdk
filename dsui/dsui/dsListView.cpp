#include "stdafx.h"
#include "dsListView.h"
#include "dsUIClassMgr.h"
#include "dsActivity.h"


dsListView::dsListView():
m_strItemClass(L"dsUIBase")
,m_nFirstVisualItem(0)
,m_nVisualItemCount(0)
,m_nMaxMayShowItemCount(0)
,m_sizeItem(100,30)
,m_xyDisOfItem(0)
,m_bVert(TRUE)
,m_procExtLayout(NULL)
{

}

dsUIBase* CALLBACK dsListView::NewDsuiElesInst()
{
	return new dsListView();
}

BOOL dsListView::GetClassInfo( __out PDSUICLASSINFO pci )
{
	if(NULL == pci)
		return FALSE;

	pci->procNewUI = NewDsuiElesInst;
	pci->strClassName = L"dsListView";

	return TRUE;
}

BOOL dsListView::ProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	



	return TRUE;
}

BOOL dsListView::SetDefaultItemClass( __in LPCWSTR lpszClass )
{
	if(NULL == lpszClass)
		return FALSE;

	if(!dsUIClassMgr::getInstance()->IsClassExist(lpszClass))
		return FALSE;

	m_strItemClass = lpszClass;

	return TRUE;
}

IdsUIBase * dsListView::InsertItem( __in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, int nIndex, LPARAM lParam, BOOL bUpdateLayout)
{

	if (NULL == lpszClass)
		lpszClass = m_strItemClass;

	dsUIBase *pbaseRet = InsertDsuiElement(lpszClass, lpszUIName, nIndex, lParam);
	//dsUIBase *pbaseRet = (dsUIBase*) CreateDsuiElement(lpszClass, lpszUIName, 0, NULL, NULL, FALSE);

	ULI uli;

	uli.nFlag = UPDATE_LAYOUT_INSERT;
	uli.pbaseParent = (dsUIBase*)this;
	uli.nItemFirst = nIndex;

	if (bUpdateLayout)
		UpdateItemLayout(&uli);
	
	return pbaseRet;
}

BOOL dsListView::SetItemLayoutInfo( __in LPSIZE lpsizeItem, __in LPRECT lprcPadding, int xyDisOfItem, BOOL bVert )
{

	if(NULL != lpsizeItem && lpsizeItem->cx >= 5 && lpsizeItem->cy >= 5)
		m_sizeItem.SetSize(lpsizeItem->cx, lpsizeItem->cy);

	if(NULL != lprcPadding && lprcPadding->left >= 0 && lprcPadding->top >=0 && lprcPadding->right >= 0 && lprcPadding->bottom >= 0)
	{
		m_rcPadding.CopyRect(lprcPadding);
	}

	m_bVert = bVert;

	if (m_bVert)
	{
		if(xyDisOfItem < m_sizeItem.cy)
			m_xyDisOfItem = xyDisOfItem;
	}
	else if(xyDisOfItem < m_sizeItem.cx)
		m_xyDisOfItem = xyDisOfItem;

	ULI uli;
	
	uli.nFlag = UPDATE_LAYOUT_REFRESH;
	uli.pbaseParent = (dsUIBase*)this;
	UpdateItemLayout(&uli);


	return TRUE;

}

void dsListView::UpdateItemLayout( PULI puli )
{
	if (NULL == puli)
		return;



	puli->pbaseParent = (IdsUIBase*)this;
	puli->ptPrevBottomRight.x = m_rcPadding.left + m_sizeViewOffset.cx;
	puli->ptPrevBottomRight.y = m_rcPadding.top + m_sizeViewOffset.cy;

	m_rcTotalDraw.left = m_sizeViewOffset.cx;
	m_rcTotalDraw.top = m_sizeViewOffset.cy;

	//m_rcTotalDraw.left = puli->ptPrevBottomRight.x;
	//m_rcTotalDraw.top = puli->ptPrevBottomRight.y;

	switch(puli->nFlag)
	{
	case UPDATE_LAYOUT_REFRESH:

	case UPDATE_LAYOUT_INSERT:

	case UPDATE_LAYOUT_DELETE:

	case UPDATE_LAYOUT_EXCHANGE:

		UpdateLayoutBase();

		EnumUIElements(EnumProc_UpdateItemLayout, (LPARAM)puli);

		break;

	default:

		break;
	}

	if(m_bVert)
	{
		if(m_rcUI.Height() < m_rcTotalDraw.Height() && m_sizeViewOffset.cy + m_rcUI.Height() > m_rcTotalDraw.Height())
		{
			m_sizeViewOffset.cy = m_rcTotalDraw.Height() - m_rcUI.Height();
		}
	}
	else
	{
		if(m_rcUI.Width() < m_rcTotalDraw.Width() && m_sizeViewOffset.cx + m_rcUI.Width() > m_rcTotalDraw.Width())
		{
			m_sizeViewOffset.cy = m_rcTotalDraw.Width() - m_rcUI.Width();
		}
	}
	

}

void dsListView::UpdateLayoutBase()
{
	if(m_bVert)
	{
		m_nMaxMayShowItemCount = (m_rcUI.Height() - m_rcPadding.top - m_rcPadding.bottom) / (m_sizeItem.cy + m_xyDisOfItem);//这里需要修改
	}
	else
	{
		m_nMaxMayShowItemCount = (m_rcUI.Width() - m_rcPadding.left - m_rcPadding.right) / (m_sizeItem.cx + m_xyDisOfItem);//这里需要修改
	}

	m_nVisualItemCount = min(m_listUIBase.size(), m_nMaxMayShowItemCount);

}

BOOL CALLBACK dsListView::EnumProc_UpdateItemLayout( dsUIBase *pbase, LPARAM lParam, int nIndex )
{
	if(NULL == pbase)
		return FALSE;

	PULI puli = (PULI)lParam;

	if(NULL == puli)
		return FALSE;

	dsListView *pthis = (dsListView*)(dsUIBase*)puli->pbaseParent;

	if(NULL == pthis)
		return FALSE;

	if(NULL != pthis->m_procExtLayout){
		

		PULI puli = (PULI)lParam;
		puli->sizeItem = pthis->m_sizeItem;

		pthis->m_procExtLayout(pbase, lParam, nIndex);

		if(pthis->m_bVert)
		{
			pbase->m_rcUI.SetRect(pthis->m_rcPadding.left, puli->ptPrevBottomRight.y + pthis->m_xyDisOfItem, pthis->m_rcPadding.left + pthis->m_sizeItem.cx, 0);
			pbase->m_rcUI.bottom = pbase->m_rcUI.top + puli->sizeItem.cy;

		}
		else
		{
			pbase->m_rcUI.SetRect(puli->ptPrevBottomRight.x + pthis->m_xyDisOfItem, pthis->m_rcPadding.top, 0, pthis->m_rcPadding.top + pthis->m_sizeItem.cy);
			pbase->m_rcUI.right = pbase->m_rcUI.left + puli->sizeItem.cx;
		}

		puli->ptPrevBottomRight.SetPoint(pbase->m_rcUI.right, pbase->m_rcUI.bottom);

		pbase->m_rcUISrc.CopyRect(pbase->m_rcUI);
		pbase->m_rcUI.OffsetRect(pthis->m_rcUI.TopLeft());

		pthis->m_rcTotalDraw.right = puli->ptPrevBottomRight.x;
		pthis->m_rcTotalDraw.bottom = puli->ptPrevBottomRight.y;
		
	}
	else
		pthis->UpdateItemRect(pbase, nIndex);

	DSLAYOUTINFO dsli;
	dsli.rcWnd = pbase->m_rcUI;

	pbase->EnumUIElements(dsActivity::EnumUIProc_UpdateLayout, (LPARAM)&dsli, nIndex);

	return TRUE;
}

BOOL dsListView::SetExtLayoutProc( __in ENUMUIPROC_USER prcExtLayout )
{
	if(NULL == prcExtLayout)
		return FALSE;

	m_procExtLayout = prcExtLayout;

	return TRUE;
}

void dsListView::UpdateItemRect( __in dsUIBase *pbase, int nIndex )
{
	if(nIndex < m_nFirstVisualItem || nIndex >= m_nFirstVisualItem + m_nVisualItemCount)
		pbase->m_rcUI.SetRectEmpty();

	int nItemOffset = nIndex - m_nFirstVisualItem;

	if(m_bVert)
	{
		pbase->m_rcUI.SetRect(m_rcPadding.left, nItemOffset*(m_sizeItem.cy + m_xyDisOfItem) + m_rcPadding.top, m_rcPadding.left + m_sizeItem.cx, 0);
		pbase->m_rcUI.bottom = pbase->m_rcUI.top + m_sizeItem.cy;

	}
	else
	{
		pbase->m_rcUI.SetRect(nItemOffset*(m_sizeItem.cx + m_xyDisOfItem) + m_rcPadding.left, m_rcPadding.top, 0, m_rcPadding.top + m_sizeItem.cy);
		pbase->m_rcUI.right = pbase->m_rcUI.left + m_sizeItem.cx;
	}

	pbase->m_rcUISrc.CopyRect(pbase->m_rcUI);
	pbase->m_rcUI.OffsetRect(m_rcUI.TopLeft());
}

void dsListView::SetVisualOffset( int x, int y, BOOL bLayout )
{
	m_sizeViewOffset.SetSize(x, y);

	if(bLayout)
	{
		ULI uli;
		UpdateItemLayout(&uli);
		RedrawUI();
	}
}

CSize dsListView::GetVisualOffset()
{
	return m_sizeViewOffset;
}
//IdsPaintMission * dsListView::GetPaintMisson( LPARAM lParamForFind )
//{
//	return dsUIBase::GetPaintMisson(lParamForFind);
//}
//
//BOOL dsListView::AddPaintMisson( IdsPaintMission *pdspm )
//{
//	return dsUIBase::AddPaintMisson(pdspm);
//}
//
//BOOL dsListView::CopyPaint( IdsUIBase *psrc )
//{
//	return dsListView::CopyPaint(psrc);
//}
CSize dsListView::GetTotalDrawSize()
{
	return m_rcTotalDraw.Size();
}

void dsListView::DeleteAllItem()
{
	DeleteAllUIElement();

	ReInitializeData();
}

void dsListView::ReInitializeData()
{
	m_nFirstVisualItem = 0;
	
	UpdateLayoutBase();

	m_sizeViewOffset.SetSize(0, 0);
	m_rcTotalDraw.SetRectEmpty();

}
