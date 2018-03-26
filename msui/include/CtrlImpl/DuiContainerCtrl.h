#pragma once

#include <DuiImpl/DuiVector.h>
#include <DuiImpl/DuiCriticalsection.h>
#include <DuiCtrl/IDuiContainerCtrl.h>
#include <DuiCtrl/IDuiOptionCtrl.h>
#include "DuiControlCtrl.h"
#include <DuiImpl/DuiRenderClip.h>
namespace DuiKit{;


class CDuiContainerCtrl : 
	public IDuiContainerCtrl,
	public CDuiControlCtrl
{
public:
	typedef CDuiVector<IDuiControlCtrl*> CChildControlArray;
	CChildControlArray m_ChildControlArray;
	DECLARE_AUTOLOCK_CS(m_ChildControlArray);

public:
	CDuiRect m_InsetRect;
	CDuiString m_strGroupName;
public:

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiContainerCtrl, CTRL_CONTAINER)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiContainerCtrl)
		DUI_SETATTRIBUTE_RECT_FUNCTION(inset, SetInsetSize);
		DUI_SETATTRIBUTE_STRING_FUNCTION(group, SetGroup)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiControlCtrl)
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiContainerCtrl)
		DUI_MESSAGE_HANDLER(DuiMsg_Z_OrderChanged, OnZ_OrderChanged)
		DUI_MESSAGE_HANDLER(DuiMsg_Initialize, OnInitialize)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseMove, OnMouseMove)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDown, OnLButtonDown)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonUp, OnLButtonUp)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDblClk, OnLButtonDblClk)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseEnter, OnMouseEnter)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseLeave, OnMouseLeave)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseHover, OnMouseHover)
	DUI_END_MSG_MAP();

	DUI_BEGIN_CTRL_EVENT_MAP(CDuiContainerCtrl)
		DUI_EVENT_HANDLER(DuiEvent_VisableChanged, OnVisableChangeEvent)
		DUI_EVENT_HANDLER(DuiEvent_Size, OnSizeEvent)
	DUI_END_CTRL_EVENT_MAP();

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiControlCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

public:
	CDuiContainerCtrl():
	m_hPreZOrderWnd(NULL),
	m_pLastMouseHoverCtrl(NULL),
	m_pCurrentCtrl(NULL),
	m_pLastCaptureCtrl(NULL)
	{
		
	}
	~CDuiContainerCtrl()
	{
		
	}


	LRESULT OnInitialize(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		return 0;
	}

	LRESULT OnZ_OrderChanged(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		/*修改子控件的位置*/
		AUTOLOCK_CS(m_ChildControlArray);
		//m_ChildControlArray.Sort(ControlSortByAxisZ,this);
		return 0;
	}

	LRESULT OnMouseHover(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE);
		if( Ctrl)
			return SendMessage(Ctrl, DuiMsg_MouseHover);

		return 0;
	}

	LRESULT OnMouseLeave(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE);

 		if (m_pCurrentCtrl)
 		{
			SendMessage(m_pCurrentCtrl, DuiMsg_MouseLeave, wParam, lParam);
			m_pCurrentCtrl = NULL;
		}

		return 0;
	}


	LRESULT OnMouseMove(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE);
		if ( Ctrl)
		{
			SendMessage(Ctrl, DuiMsg_MouseMove, wParam, lParam);

			if ( m_pCurrentCtrl != Ctrl)
			{
				if ( m_pCurrentCtrl )
					SendMessage(m_pCurrentCtrl, DuiMsg_MouseLeave, wParam, lParam);

				SendMessage(Ctrl, DuiMsg_MouseEnter, wParam, lParam);
				m_pCurrentCtrl = Ctrl;
			}

			
		}
		return 0;
	}

	LRESULT OnLButtonDown(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE);
		if( Ctrl)
		{
			m_pLastCaptureCtrl = Ctrl;
			return SendMessage(Ctrl, DuiMsg_LButtonDown, wParam, lParam);
		}

		
		return 0;
	}


	LRESULT OnLButtonUp(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{

		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),FALSE);
		if ( m_pLastCaptureCtrl && m_pLastCaptureCtrl != Ctrl)
		{
			 SendMessage(m_pLastCaptureCtrl, DuiMsg_LButtonUp, wParam, lParam);
			 m_pLastCaptureCtrl = NULL;
		}
		else
		{
			if ( Ctrl )
				return SendMessage(Ctrl, DuiMsg_LButtonUp, wParam, lParam);
		}
			

		return 0;
	}

	LRESULT OnLButtonDblClk(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE);
		if ( Ctrl)
			return SendMessage(Ctrl, DuiMsg_LButtonDblClk, wParam, lParam);
		

		return 0;
	}

	LRESULT OnMouseEnter(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = TRUE;
		IDuiControlCtrl* Ctrl = GetChild(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE);
		if ( m_pLastCaptureCtrl && Ctrl != m_pLastCaptureCtrl)
		{
			SendMessage(m_pLastCaptureCtrl, DuiMsg_LButtonUp, wParam, lParam);
			m_pLastCaptureCtrl = NULL;
		}
		if ( Ctrl )
			SendMessage(Ctrl, DuiMsg_MouseEnter, wParam, lParam);
		

		return 0;
	}

	BOOL OnVisableChangeEvent(const DuiEvent& envent)
	{
		if ( m_ChildControlArray.Find(envent.Ctrl) != -1)
		{
			SetPos(GetPos());
		}
	
		return TRUE;
	}

	BOOL OnSizeEvent(const DuiEvent& envent)
	{
		if ( m_ChildControlArray.Find(envent.Ctrl) != -1)
		{
			SetPos(GetPos());
		}

		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////
	

	//属性
	virtual CDuiRect GetInsetSize() 
	{
		return m_InsetRect;
	}

	virtual VOID SetInsetSize(const CDuiRect& InsetSize)
	{
		m_InsetRect = InsetSize;
	}
	
	virtual LPCWSTR GetGroup()
	{
		return m_strGroupName;
	}

	virtual VOID SetGroup(LPCWSTR lpszGroupName)
	{
		if ( lpszGroupName && wcslen(lpszGroupName))
		{
			m_strGroupName = lpszGroupName;
		}
	}

	virtual VOID BroadCastMessage(IDuiControlCtrl* pSender, INT nMsg, INT nCtrt = IIDuiControlCtrl, WPARAM wParam = 0, LPARAM lParam = 0, LPVOID lpData = 0)
	{
		AUTOLOCK_CS(m_ChildControlArray);
		INT nCount = m_ChildControlArray.Size();
		for ( INT nLoop = 0 ; nLoop < nCount ; nLoop++ )
		{
			IDuiControlCtrl* tCtrl = m_ChildControlArray[nLoop];
			if ( tCtrl )
			{
				if ( tCtrl->QueryInterface(nCtrt))
					pSender->SendMessage(tCtrl, nMsg, wParam, lParam, lpData);
				
				
				IDuiContainerCtrl* tContainerCtrl = (IDuiContainerCtrl*)tCtrl->QueryInterface(IIDuiContainerCtrl);
				if ( tContainerCtrl )
					tContainerCtrl->BroadCastMessage(pSender, nMsg, nCtrt,wParam, lParam, lpData);
				
			}
		}
	}

	virtual VOID DoFinalMessage()
	{
		__super::DoFinalMessage();
		AUTOLOCK_CS(m_ChildControlArray);
		INT nCount = m_ChildControlArray.Size();
		for ( INT nLoop = 0 ; nLoop < nCount ; nLoop++ )
		{
			IDuiControlCtrl* tCtrl = m_ChildControlArray[nLoop];
			if ( tCtrl )
			{
				tCtrl->DeleteThis();
			}
		}
	}


	virtual VOID AddChild(IDuiControlCtrl* pChild)
	{
		if( pChild )
		{
			AUTOLOCK_CS(m_ChildControlArray);
			if ( m_ChildControlArray.Find(pChild) == -1)
			{
				m_hPreZOrderWnd = NULL;
				m_ChildControlArray.Add(pChild);
				//m_ChildControlArray.Sort(ControlSortByAxisZ,this);
			}
		}
	}

	virtual VOID RemoveChild(IDuiControlCtrl* pChild, BOOL bDel = FALSE)
	{
		if ( pChild )
		{
			{
				AUTOLOCK_CS(m_ChildControlArray);
				INT nIndex = m_ChildControlArray.Find(pChild);
				if ( nIndex != -1)
				{
					m_ChildControlArray.Delete(nIndex);
				}
			}
			
			if ( bDel )
			{
				DestoryObject(pChild);
			}
		}
	}

	virtual DWORD GetChildCount()
	{
		AUTOLOCK_CS(m_ChildControlArray);
		return m_ChildControlArray.Size();
	}

	virtual IDuiControlCtrl* GetChild(DWORD dwIndex)
	{
		AUTOLOCK_CS( m_ChildControlArray);
		if ( m_ChildControlArray.Size() > (INT)dwIndex)
		{
			return m_ChildControlArray[dwIndex];
		}
		return NULL;
	}



	virtual IDuiControlCtrl* GetChild(LPCWSTR lpszName, BOOL hasSub = TRUE)
	{
		if( lpszName && wcslen(lpszName))
		{
			//先找自己的子控件
			AUTOLOCK_CS( m_ChildControlArray);
			for ( INT dwLoop = 0 ; dwLoop < m_ChildControlArray.Size() ; dwLoop++)
			{
				IDuiControlCtrl* pTemp = m_ChildControlArray[dwLoop];
				if ( pTemp && _wcsicmp(lpszName , pTemp->GetName()) == 0)
				{
					return pTemp;
				}
			}

			//没有找到，递归找
			if ( hasSub )
			{
				for ( INT dwLoop = 0 ; dwLoop < m_ChildControlArray.Size() ; dwLoop++)
				{
					IDuiContainerCtrl* tContainerCtrl = (IDuiContainerCtrl*)(m_ChildControlArray[dwLoop]->QueryInterface(IIDuiContainerCtrl));
					if ( tContainerCtrl )
					{
						IDuiControlCtrl* pRet = tContainerCtrl->GetChild(lpszName, hasSub);
						if ( pRet )
						{
							return pRet;
						}
					}
				}
			}
		}

		return NULL;
	}

	virtual IDuiControlCtrl* GetChild(DWORD x, DWORD y, BOOL bFinalCtrl = TRUE)
	{
		CDuiPoint point(x, y);
		
		DWORD dwChildCount = GetChildCount();
		for (DWORD nLoop = 0 ; nLoop < dwChildCount ; nLoop++)
		{
			CDuiControlCtrl* tControlCtrl = (CDuiControlCtrl*)GetChild(nLoop);
			if ( tControlCtrl && tControlCtrl->GetVisable())
			{
				if (::PtInRect(&tControlCtrl->GetPos(), point))
				{
					if ( bFinalCtrl )
					{
						IDuiContainerCtrl* tContainerCtrl = (IDuiContainerCtrl*)tControlCtrl->QueryInterface(IIDuiContainerCtrl);
						if ( tContainerCtrl)
						{
							return tContainerCtrl->GetChild(x, y);
						}
					}

					return tControlCtrl;
				}
			}
		}

		return NULL;
	}

	virtual VOID DoPaint(HDC hDC, const RECT& rcPaint )
	{
		
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_pos) ) 
			return;

 		CDuiRenderClip clip;
 		CDuiRenderClip::GenerateClip(hDC, rcTemp, clip);
 		CDuiControlCtrl::DoPaint(hDC, rcPaint);

		INT nCount = m_ChildControlArray.Size();

		if ( nCount )
		{
			RECT ThisPos = m_pos;
			ThisPos.left += m_InsetRect.left;
			ThisPos.top += m_InsetRect.top;
			ThisPos.right -= m_InsetRect.right;
			ThisPos.bottom -= m_InsetRect.bottom;

		
			//绝对坐标
			if( !::IntersectRect(&rcTemp, &rcPaint, &ThisPos) ) 
			{
				for( int it = 0; it < nCount; it++ ) {
					CDuiControlCtrl* tCtrl = (CDuiControlCtrl*)m_ChildControlArray[it];
					if( !tCtrl->GetVisable() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &tCtrl->GetPos()) ) continue;
					/*
					if( tCtrl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_pos, &tCtrl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
					*/
				}
			}
			else 
			{
				CDuiRenderClip childClip;
				CDuiRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for( int it = 0; it < nCount; it++ ) 
				{
					CDuiControlCtrl* tCtrl = (CDuiControlCtrl*)m_ChildControlArray[it];
					if ( tCtrl )
					{
						if( !tCtrl->GetVisable() ) continue;
						if( !::IntersectRect(&rcTemp, &rcPaint, &tCtrl->GetPos()) ) 
							continue;

						if( /*pControl ->IsFloat()*/ FALSE ) {
							if( !::IntersectRect(&rcTemp, &m_pos, &tCtrl->GetPos()) ) 
								continue;

							//CDuiRenderClip::UseOldClipBegin(hDC, childClip);
							tCtrl->DoPaint(hDC, rcPaint);
							//CDuiRenderClip::UseOldClipEnd(hDC, childClip);
						}
						else 
						{
							if( !::IntersectRect(&rcTemp, &ThisPos, &tCtrl->GetPos()) ) 
								continue;
							
							tCtrl->DoPaint(hDC, rcPaint);
						}
					}
				}
			}
		}
	}

	virtual VOID SetPos(const CDuiRect& pos)
	{
		__super::SetPos(pos);
		CDuiRect InsetRect = GetInsetSize();
		CDuiRect ThisPos = m_pos;

		ThisPos.left += m_InsetRect.left;
		ThisPos.top  += m_InsetRect.top;
		ThisPos.right -= m_InsetRect.right;
		ThisPos.bottom -= m_InsetRect.bottom;

	

		INT nCount = m_ChildControlArray.Size();
		for ( INT nLoop = 0 ; nLoop < nCount ; nLoop++)
		{
			CDuiControlCtrl* tCtrl = (CDuiControlCtrl*)m_ChildControlArray[nLoop];
			if ( tCtrl )
			{
				if (tCtrl->GetPos() == ThisPos)
				{
					continue;
				}
				tCtrl->SetPos(ThisPos);
			}
		}
	}

	virtual BOOL Invalidate()
	{
		SetPos(GetPos());
		return CDuiControlCtrl::Invalidate();
	}

	virtual BOOL InvalidateRect(const CDuiRect& rect)
	{
		return ::InvalidateRect(GetOwnerWindowCtrl()->GetHWND(), &rect, FALSE);;
	}

	public:
		static int ControlSortByAxisZ(void *const * _Ctrl_1, void *const * _Ctrl_2, void * Param)
		{
			 IDuiControlCtrl * Ctrl_1 = *(*(( IDuiControlCtrl * **)_Ctrl_1));
			 IDuiControlCtrl * Ctrl_2 = *(*(( IDuiControlCtrl * **)_Ctrl_2));
			
			DWORD dwGetAxisZ_1 = Ctrl_1->GetAxisZ();
			DWORD dwGetAxisZ_2 = Ctrl_2->GetAxisZ();
			return dwGetAxisZ_1 < dwGetAxisZ_2 ? -1 : (dwGetAxisZ_2 == dwGetAxisZ_1 ? 0 : 1); 
		}


private:
	HWND m_hPreZOrderWnd;
	IDuiControlCtrl* m_pLastMouseHoverCtrl;
	IDuiControlCtrl* m_pCurrentCtrl;
	IDuiControlCtrl* m_pLastCaptureCtrl;
};


};//namespace DuiKit{;