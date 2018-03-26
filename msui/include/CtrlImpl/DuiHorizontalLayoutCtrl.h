#pragma once

#include <DuiCtrl/IDuiHorizontalLayoutCtrl.h>

namespace DuiKit{;
class CDuiHorizontalLayoutCtrl : 
	public IDuiHorizontalLayoutCtrl,
	public CDuiContainerCtrl
{
public:
	CDuiHorizontalLayoutCtrl()
	{
	}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiVerticalLayoutCtrl, CTRL_HORIZONTALLAYOUT)
		DUI_DEFINE_INTERFACE(IDuiVerticalLayoutCtrl, IIDuiHorizontalLayoutCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
		DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiHorizontalLayoutCtrl)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiContainerCtrl);
	DUI_END_SETATTRIBUTE;

public:
	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		//SetValign(ValignCenter);
		if ( CDuiContainerCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	virtual VOID SetPos(const CDuiRect& pos)
	{
		RASSERTV(!pos.IsNull());
		CDuiControlCtrl::SetPos(pos);
		RASSERTV(m_ChildControlArray.Size());

		CDuiRect ThisPos  = m_pos;
		ThisPos.left += m_InsetRect.left;
		ThisPos.top  += m_InsetRect.top;
		ThisPos.right -= m_InsetRect.right;
		ThisPos.bottom -= m_InsetRect.bottom;


		INT cxFixed = 0;		//已被占用的Y轴高度
		INT nFixedNum = 0;		//使用固定高度的控件个数
		INT nValidCtrlNum = 0;

		//首先算出垂直布局中固定高度的控件所占的像素
		for( INT nLoop = 0; nLoop < m_ChildControlArray.Size(); nLoop++ )
		{
			IDuiControlCtrl* pControl = m_ChildControlArray[nLoop];
			if( !pControl->GetVisable() ) 
				continue;
			

			nValidCtrlNum++;


			DWORD nCtrlWidth = pControl->GetFixedWidth();
			if ( nCtrlWidth )
			{
				if (pControl->GetMaxWidth() && nCtrlWidth > pControl->GetMaxWidth())  nCtrlWidth = pControl->GetMaxWidth();
				if ( nCtrlWidth < pControl->GetMinWidth() ) nCtrlWidth = pControl->GetMinWidth();
			}

			if (  nCtrlWidth )
			{

				cxFixed += nCtrlWidth;
				nFixedNum++;
			}
		}

		//剩余的控件平均值
		INT nSurplusAverage = 0;
		if (nValidCtrlNum - nFixedNum )
		{
			nSurplusAverage = (ThisPos.GetWidth() - cxFixed) / (nValidCtrlNum - nFixedNum);
		}



		cxFixed = ThisPos.left;
		for( int it1 = 0; it1 < m_ChildControlArray.Size(); it1++ )
		{
			CDuiControlCtrl* pControl = static_cast<CDuiControlCtrl*>(m_ChildControlArray[it1]);
			if( !pControl->GetVisable() ) 
				continue;

			DWORD nCtrlWidth = pControl->GetFixedWidth();
			DWORD nCtrlHeight = pControl->GetFixedHeight();
			
			if( nCtrlHeight == 0) nCtrlHeight = ThisPos.GetHeight();
			if(nCtrlWidth == 0 ) nCtrlWidth = nSurplusAverage;

			if (pControl->GetMaxWidth() && nCtrlWidth > pControl->GetMaxWidth()) 
				nCtrlWidth = pControl->GetMaxWidth();

			if ( nCtrlWidth < pControl->GetMinWidth() )
				nCtrlWidth = pControl->GetMinWidth();

		
			//确定高度
			if (pControl->GetMaxHeight() && nCtrlHeight > pControl->GetMaxHeight()) 
				nCtrlWidth = pControl->GetMaxHeight();

			if ( nCtrlHeight < pControl->GetMinHeight() )
				nCtrlWidth = pControl->GetMinHeight();

			//
			CDuiRect itemPos(cxFixed , ThisPos.top,  cxFixed + nCtrlWidth  , ThisPos.top + nCtrlHeight);
			Valign align = pControl->GetValign();
			if ( align == ValignCenter)
			{
				DWORD dwCenter = (ThisPos.GetHeight() - nCtrlHeight) / 2;
				itemPos.top = ThisPos.top + dwCenter;
				itemPos.bottom = itemPos.top + nCtrlHeight;
			}
			else if (align == ValignBottom)
			{
				itemPos.top = ThisPos.top + (ThisPos.GetHeight() - nCtrlHeight);
				itemPos.bottom = ThisPos.bottom;
			}

			pControl->SetPos(itemPos);

			cxFixed += itemPos.GetWidth();
		}
	}
};

};