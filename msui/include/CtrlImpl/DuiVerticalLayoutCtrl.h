#pragma once

#include <DuiCtrl/IDuiVerticalLayoutCtrl.h>
#include "DuiContainerCtrl.h"
namespace DuiKit{;
class CDuiVerticalLayoutCtrl : 
	public IDuiVerticalLayoutCtrl,
	public CDuiContainerCtrl
{
public:
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiVerticalLayoutCtrl, CTRL_VERTICALLAYOUT)
		DUI_DEFINE_INTERFACE(IDuiVerticalLayoutCtrl, IIDuiVerticalLayoutCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiVerticalLayoutCtrl)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiContainerCtrl);
	DUI_END_SETATTRIBUTE;
	

	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiContainerCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	virtual VOID SetPos(const CDuiRect& pos)
	{
		if ( pos.IsNull() )
		{
			return ;
		}

		CDuiControlCtrl::SetPos(pos);

		if ( !m_ChildControlArray.Size())
		{
			return ;
		}

		CDuiRect ThisPos  = m_pos;
		ThisPos.left += m_InsetRect.left;
		ThisPos.top  += m_InsetRect.top;
		ThisPos.right -= m_InsetRect.right;
		ThisPos.bottom -= m_InsetRect.bottom;

	

		INT cyFixed = 0;		//已被占用的Y轴高度
		INT nFixedNum = 0;		//使用固定高度的控件个数
		INT nValidCtrlNum = 0;
		//平均高度，如果控件的最大高度小于平均值，则是用平均值
	
		//首先算出垂直布局中固定高度的控件所占的像素
		for( int it1 = 0; it1 < m_ChildControlArray.Size(); it1++ )
		{
			CDuiControlCtrl* pControl = static_cast<CDuiControlCtrl*>(m_ChildControlArray[it1]);
			if( !pControl->GetVisable() ) 
				continue;

			nValidCtrlNum ++ ;
			DWORD nCtrlHeight = pControl->GetFixedHeight();
			if ( nCtrlHeight )
			{
				if (pControl->GetMaxHeight() && nCtrlHeight > pControl->GetMaxHeight())  
					nCtrlHeight = pControl->GetMaxHeight();

				if ( nCtrlHeight < pControl->GetMinHeight() )
					nCtrlHeight = pControl->GetMinHeight();
			}

			if (  nCtrlHeight )
			{
				cyFixed += nCtrlHeight;
				nFixedNum++;
			}
		}

		//剩余的控件平均值
		INT nSurplusAverage = 0;
		if (nValidCtrlNum - nFixedNum )
		{
			nSurplusAverage = (ThisPos.GetHeight() - cyFixed) / (nValidCtrlNum - nFixedNum);
		}



		cyFixed = ThisPos.top;
		for( int it1 = 0; it1 < m_ChildControlArray.Size(); it1++ )
		{
			CDuiControlCtrl* pControl = static_cast<CDuiControlCtrl*>(m_ChildControlArray[it1]);
			if( !pControl->GetVisable() ) 
				continue;

			DWORD nCtrlWidth =  pControl->GetFixedWidth();
			DWORD nCtrlHeight = pControl->GetFixedHeight();

			if ( nCtrlWidth == 0) 
				nCtrlWidth = ThisPos.GetWidth();

			if ( nCtrlHeight == 0 ) 
				nCtrlHeight = nSurplusAverage;

			if (pControl->GetMaxWidth() && nCtrlWidth > pControl->GetMaxWidth()) 
				nCtrlWidth = pControl->GetMaxWidth();

			if ( nCtrlWidth < pControl->GetMinWidth() )
				nCtrlWidth = pControl->GetMinWidth();


			//确定高度
			if (pControl->GetMaxHeight() && nCtrlHeight > pControl->GetMaxHeight()) 
				nCtrlWidth = pControl->GetMaxHeight();

			if ( nCtrlHeight < pControl->GetMinHeight() )
				nCtrlWidth = pControl->GetMinHeight();

			CDuiRect itemPos(ThisPos.left, cyFixed,  ThisPos.left + nCtrlWidth , cyFixed + nCtrlHeight);

			Halign align = pControl->GetHalign();
			if ( align == HalignCenter)
			{
				DWORD dwCenter = (ThisPos.GetWidth() - nCtrlWidth) / 2;
				itemPos.left = ThisPos.left + dwCenter;
				itemPos.right = itemPos.left + nCtrlWidth;
			}
			else if (align == HalignRight)
			{
				itemPos.left = ThisPos.left + (ThisPos.GetWidth() - nCtrlWidth);
				itemPos.right = ThisPos.right;
			}

			pControl->SetPos(itemPos);
			cyFixed += itemPos.GetHeight();
		}
	}
};

};