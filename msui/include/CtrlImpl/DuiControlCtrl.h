#pragma once

#include <DuiImpl/DuiVector.h>
#include <DuiImpl/DuiString.h>
#include <DuiImpl/DuiMap.h>
#include <DuiImpl/DuiCriticalsection.h>
#include <DuiImpl/DuiRenderEngine.h>

#include <DuiCore/IDuiImage.h>
#include "DuiImpl/DuiRenderClip.h"
namespace DuiKit{;

class CDuiControlCtrl : public IDuiControlCtrl
{
	CDuiString m_strControlName;
	CDuiString m_strNameSpace;
	BOOL	   m_bVisable;
	IDuiSkin*  m_pDuiSkin;

	//位
	DWORD	   m_dwMaxHeigh;
	DWORD	   m_dwMaxWidth;
	DWORD	   m_dwMinHeigh;
	DWORD	   m_dwMinWidth;

	DWORD	   m_dwAxisX;
	DWORD	   m_dwAxisY;
	DWORD	   m_dwAxisZ;
	Valign	   m_Valign;
	Halign	   m_Halign;

	CDuiRect	m_BorderSize;
	CDuiRect	m_Padding;
	//绘制
	CDuiString m_bkImage;
	DWORD	   m_dwBkColor;
	//

	typedef CDuiMap<CDuiString, CDuiString>	CUserDataMap;
	DECLARE_AUTOLOCK_CS(m_userDataMap);
	CUserDataMap m_userDataMap;

public:
	CDuiControlCtrl() :
		m_dwMaxHeigh(0),
		m_dwMaxWidth(0),
		m_dwMinHeigh(0),
		m_dwMinWidth(),
		m_dwAxisX(0),
		m_dwAxisY(0),
		m_dwAxisZ(0),
		m_Valign(ValignNull),
		m_Halign(HalignNull),
		m_dwBkColor(0),
		m_bInit(FALSE),
		m_bUpdateNeeded(TRUE),
		m_pRootWnd(NULL),
		m_pOwnerWnd(NULL),
		m_dwFixedWidth(0),
		m_dwFixedHeight(0)
	{
		m_pParam		 = NULL;
		m_pDuiCore		 = NULL;
		m_pParentControl = NULL;
		m_bVisable		 = TRUE;
		m_pDuiSkin		 = NULL;
	}

	~CDuiControlCtrl()
	{
		DoFinalMessage();
		{
			AUTOLOCK_CS(m_DuiPreMessageFilterSet);
			m_DuiPreMessageFilterSet.Clear();
		}

		{
			AUTOLOCK_CS(m_DuiProMessageFilterSet);
			m_DuiProMessageFilterSet.Clear();
		}
	}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiControlCtrl, CTRL_CONTROL)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiControlCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(name, SetName);
		DUI_SETATTRIBUTE_STRING_FUNCTION(space, SetNameSpace);
		DUI_SETATTRIBUTE_BOOL(visable, m_bVisable);
		DUI_SETATTRIBUTE_INT_FUNCTION(height, SetFixedHeight);
		DUI_SETATTRIBUTE_INT_FUNCTION(width, SetFixedWidth);
		DUI_SETATTRIBUTE_INT_FUNCTION(maxheight, SetMaxHeight);
		DUI_SETATTRIBUTE_INT_FUNCTION(maxwidth, SetMaxWidth);
		DUI_SETATTRIBUTE_INT_FUNCTION(minheigh, SetMinHeight);
		DUI_SETATTRIBUTE_INT_FUNCTION(minwidth, SetMinWidth);
		DUI_SETATTRIBUTE_RECT_FUNCTION(bordersize, SetBorderSize);
		DUI_SETATTRIBUTE_SIZE_FUNCTION(borderround, SetBorderRound)
		DUI_SETATTRIBUTE_RECT_FUNCTION(padding, SetPadding);
		//
		DUI_SETATTRIBUTE_INT_FUNCTION(AxisX, SetAxisX);
		DUI_SETATTRIBUTE_INT_FUNCTION(AxisY, SetAxisY);
		DUI_SETATTRIBUTE_INT_FUNCTION(AxisZ, SetAxisZ);
		DUI_BENGIN_SUB_SETATTRIBUTE(valign)
			DUI_SUB_SETATTRIBUTE_FUNCTION(top, SetValign, ValignTop)
			DUI_SUB_SETATTRIBUTE_FUNCTION(center, SetValign, ValignCenter)
			DUI_SUB_SETATTRIBUTE_FUNCTION(bottom, SetValign, ValignBottom)
		DUI_END_SUB_SETATTRIBUTE
		
		DUI_BENGIN_SUB_SETATTRIBUTE(halign)
			DUI_SUB_SETATTRIBUTE_FUNCTION(left, SetHalign, HalignLeft)
			DUI_SUB_SETATTRIBUTE_FUNCTION(center, SetHalign, HalignCenter)
			DUI_SUB_SETATTRIBUTE_FUNCTION(right, SetHalign, HalignRight)
		DUI_END_SUB_SETATTRIBUTE

		//
		DUI_SETATTRIBUTE_STRING_FUNCTION(bkimage, SetBkImage);
		DUI_SETATTRIBUTE_INT_FUNCTION(bkcolor,SetBkColor)

	DUI_END_SETATTRIBUTE;

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( !pCore ) 
			return E_FAIL;

		m_pDuiCore = pCore;

		if ( pParent )
		{
			IDuiControlCtrl* tCtrl = (IDuiControlCtrl*)pParent->QueryInterface(IIDuiControlCtrl);
			if( tCtrl )
			{
 				SetParentCtrl(tCtrl);
 				m_pRootWnd = tCtrl->GetRootCtrl();
			}
		}
		
		if ( !m_pRootWnd )
		{
			m_pRootWnd = (IDuiWindowCtrl*)this->QueryInterface(IIDuiWindowCtrl);
		}

		ASSERT(m_pRootWnd);

		for ( IDuiControlCtrl* tCtrl = this ; tCtrl ; tCtrl = tCtrl->GetParentCtrl())
		{
			m_pOwnerWnd = (IDuiWindowCtrl*)tCtrl->QueryInterface(IIDuiWindowCtrl);
			if ( m_pOwnerWnd )
			{
				break;
			}
		}
		if ( !m_pOwnerWnd )
		{
			m_pOwnerWnd = (IDuiWindowCtrl*)this->QueryInterface(IIDuiWindowCtrl);
		}

		ASSERT(m_pOwnerWnd);
		SendMessage(this,DuiMsg_Create);
		return S_OK;
	}

public://用于继承的共享方法
	
	DWORD GetAdjustColor(DWORD dwColor)
	{
		return 0;
	}

	//获取图像
	IDuiImage* GetImage(LPCWSTR lpszName)
	{
		RASSERT( m_pDuiSkin && m_pDuiCore && lpszName && wcslen(lpszName), NULL);
		
		if ( lpszName[0] == L'{') //具备从别的SKIN中获取图片
		{
			WCHAR szSkinName[MAX_PATH] = { 0 };
			WCHAR szImageName[MAX_PATH]= { 0 };
			int n = swscanf_s(lpszName,L"{ %[^}] }.{ %[^}] }",szSkinName,MAX_PATH, szImageName, MAX_PATH);
			if ( n == 2) //
			{
				IDuiSkin* tSkin = m_pDuiCore->GetSkin(szSkinName);
				if ( tSkin )
				{
					return tSkin->GetImage(szImageName);
				}
			}
		}

		return m_pDuiSkin->GetImage(lpszName);
	}

	//获取语言相关的
	LPCWSTR GetLangText(LPCWSTR lpszName)
	{
		RASSERT(m_pDuiCore, NULL);
		RASSERT( m_pDuiSkin && m_pDuiCore && lpszName && wcslen(lpszName), lpszName);

		if ( lpszName[0] == L'{')
		{
			WCHAR szSplit_1[MAX_PATH] = { 0 };
			WCHAR szSplit_2[MAX_PATH]= { 0 };
			WCHAR szSplit_3[MAX_PATH]= { 0 };
			
			IDuiSkin* tSkin = NULL;
			IDuiLang* tLang = NULL;
			int nCount = swscanf_s(lpszName,L"{ %[^}] }.{ %[^}] }.{ %[^}] }",szSplit_1,MAX_PATH, szSplit_2, MAX_PATH,szSplit_3,MAX_PATH);
			if ( nCount == 3)
			{
				tSkin = m_pDuiCore->GetSkin(szSplit_1);
				if ( tSkin )
				{
					if (!(wcscmp(szSplit_2, L"*") != 0 && wcslen(szSplit_2)))
						tLang = tSkin->GetLangSet()->GetLang(m_pDuiCore->GetCurrentLang());
					else
						tLang = tSkin->GetLangSet()->GetLang(szSplit_2);
				}

				if ( tLang )
				{
					LPCWSTR lpszText = tLang->GetText(szSplit_3);
					if ( lpszText )
					{
						return lpszText;
					}
				}
			}
			else if (nCount == 2)
			{
				tSkin = m_pDuiSkin;
				BOOL bCurLang = FALSE;
				if ( !(wcscmp(szSplit_1, L"*") != 0 && wcslen(szSplit_1)))
					tLang = tSkin->GetLangSet()->GetLang(m_pDuiCore->GetCurrentLang());
				else
					tLang = tSkin->GetLangSet()->GetLang(szSplit_1);

				if ( tLang )
				{
					LPCWSTR lpszText = tLang->GetText(szSplit_2);
					if ( lpszText )
					{
						return lpszText;
					}
				}
			}
			else
			{
				tSkin = m_pDuiSkin;
				tLang = tSkin->GetLangSet()->GetLang(m_pDuiCore->GetCurrentLang());
				if ( tLang )
				{
					LPCWSTR lpszText = tLang->GetText(szSplit_1);
					if ( lpszText )
					{
						return lpszText;
					}
				}
			}
		}

		return lpszName;
	}

	//用于继承的方法
	IDuiFont* GetFont(LPCWSTR lpszName)
	{
		RASSERT(lpszName, NULL);
		if ( wcslen(lpszName) > 2 && lpszName[0] == L'{')
		{
			WCHAR szSplit_1[MAX_PATH] = { 0 };
			WCHAR szSplit_2[MAX_PATH]= { 0 };

			int nCount = swscanf_s(lpszName,L"{ %[^}] }.{ %[^}] }.{ %[^}] }",szSplit_1,MAX_PATH, szSplit_2, MAX_PATH);
			if ( nCount == 2)
			{
				IDuiSkin* tSkin = GetDuiCore()->GetSkin(szSplit_1);
				if ( tSkin )
				{
					return tSkin->GetFont(szSplit_2);
				}
			}
			if ( nCount == 1)
			{
				return GetDuiSkin()->GetFont(szSplit_1);
			}
		}

		return GetDuiSkin()->GetFont(lpszName);
	}

	BOOL PrintImage(HDC hDC, LPCWSTR lpszImage, const CDuiRect& pos, const CDuiRect& paintRect)
	{
		BOOL bTempImage = FALSE;
		IDuiImage* tImage = GetImage(lpszImage);
		if ( !tImage )
		{
			tImage = (IDuiImage*)CreateObject(OBJECT_CORE_IMAGE);
			if ( !tImage )
			{
				return FALSE;
			}

			tImage->SetDuiSkin(m_pDuiSkin);
			tImage->SetFile(lpszImage);
			bTempImage = TRUE;
		}

		if (tImage)
		{
			if ( !tImage->GetHandle())
			{
				tImage->Create();
			}

			CDuiRenderEngine::DrawImage(hDC, tImage, pos, paintRect);
			if ( bTempImage )
			{
				tImage->DeleteThis();
			}
			return TRUE;
		}

		return FALSE;
	}

	//用于继承的方法
public:

	DUI_BEGIN_MSG_MAP(CDuiControlCtrl)
		DUI_MESSAGE_HANDLER_NO_PARAM(DuiMsg_Initialize, OnInitialize)
		DUI_MESSAGE_HANDLER(DuiMsg_Timer, OnDuiTimer)
	DUI_END_MSG_MAP_SUPER()

	LRESULT OnDuiTimer(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		bHandle = FALSE;
		DuiEvent event = { this, DuiEvent_Timer, wParam};
		DoEvent(event);
		return 0;
	}

	LRESULT OnInitialize()
	{
		DuiEvent event;
		event.Ctrl = this;
		event.nEvent = DuiEvent_Initialize;
		DoEvent(event);
		return 0;
	}

	//绘制前
	virtual VOID DoPrePaint(HDC hDC, RECT& rcPaint)
	{

	}

	//绘制
	virtual VOID DoPaint(HDC hDC, const RECT& rcPaint )
	{
		CFuncTime funcTime(GetObjectClassName(), GetName());
		m_bUpdateNeeded = FALSE;
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_pos) ) 
			return;

		if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
			CDuiRenderClip roundClip;
			CDuiRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_pos, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}
		else {
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}
	}

	virtual VOID PaintBkColor(HDC hDC)
	{
		CDuiRenderEngine::DrawColor(hDC, m_rcPaint, m_dwBkColor);
	}

	

	virtual VOID PaintBkImage(HDC hDC)
	{
		CDuiString strBkImage = GetBkImage();
		IDuiImage* tImage = GetImage(strBkImage);
		if ( tImage )
		{

		}

		if (tImage)
		{
			if ( !tImage->GetHandle())
			{
				tImage->Create();
			}

			CDuiRenderEngine::DrawImage(hDC, tImage, GetPos(), m_rcPaint);
		}
	}

	virtual VOID PaintStatusImage(HDC hDC)
	{

	}

	virtual VOID PaintText(HDC hDC)
	{

	}

	virtual VOID PaintBorder(HDC hDC)
	{
		/*
		if(m_dwBkColor != 0 || m_dwFocusBorderColor != 0)
		{
			if(m_nBorderSize > 0 && ( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ))//画圆角边框
			{
				if (IsFocused() && m_dwFocusBorderColor != 0)
					CDuiRenderEngine::DrawRoundRect(hDC, m_pos, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor));
				else
					CDuiRenderEngine::DrawRoundRect(hDC, m_pos, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor));
			}
			else
			{
				if (IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize > 0)
					CDuiRenderEngine::DrawRect(hDC, m_pos, m_nBorderSize, GetAdjustColor(m_dwFocusBorderColor));
				else if(m_BorderSize.left > 0 || m_BorderSize.top > 0 || m_BorderSize.right > 0 || m_BorderSize.bottom > 0)
				{
					RECT rcBorder;

					if(m_BorderSize.left > 0){
						rcBorder		= m_pos;
						rcBorder.right	= m_pos.left;
						CDuiRenderEngine::DrawLine(hDC,rcBorder,m_BorderSize.left,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
					if(m_BorderSize.top > 0){
						rcBorder		= m_pos;
						rcBorder.bottom	= m_pos.top;
						CDuiRenderEngine::DrawLine(hDC,rcBorder,m_BorderSize.top,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
					if(m_BorderSize.right > 0){
						rcBorder		= m_pos;
						rcBorder.left	= m_pos.right;
						CDuiRenderEngine::DrawLine(hDC,rcBorder,m_BorderSize.right,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
					if(m_BorderSize.bottom > 0){
						rcBorder		= m_pos;
						rcBorder.top	= m_pos.bottom;
						CDuiRenderEngine::DrawLine(hDC,rcBorder,m_BorderSize.bottom,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
				}
				else if(m_nBorderSize > 0)
					CDuiRenderEngine::DrawRect(hDC, m_pos, m_nBorderSize, GetAdjustColor(m_dwBorderColor));
			}
		}
		*/
	}

	//绘制后
	virtual VOID DoProPaint(HDC hDC)
	{

	}

	virtual DWORD GetControlFlags()
	{
		return 0;
	}

public:
	

public:
	//IUIObject
	
	virtual VOID SetDuiCore(IDuiCore* pCore)
	{
		m_pDuiCore = pCore;
	}

	virtual IDuiCore* GetDuiCore() 
	{
		return m_pDuiCore;
	}

	virtual VOID SetDuiSkin(IDuiSkin* pSkin)
	{
		m_pDuiSkin = pSkin;
	}

	virtual IDuiSkin* GetDuiSkin()
	{
		return m_pDuiSkin;
	}

	virtual VOID AddPreMessageFilter(IDuiPreMessageFilter* pFilter)
	{
		if ( pFilter )
		{
			AUTOLOCK_CS(m_DuiPreMessageFilterSet);
			DWORD dwIndex = m_DuiPreMessageFilterSet.Find(pFilter);
			if ( dwIndex == -1)
			{
				m_DuiPreMessageFilterSet.Add(pFilter);
			}
		}
	}

	virtual VOID RemovePreMessageFilter(IDuiPreMessageFilter* pFilter)
	{
		if ( pFilter )
		{
			AUTOLOCK_CS(m_DuiPreMessageFilterSet);
			DWORD dwIndex = m_DuiPreMessageFilterSet.Find(pFilter);
			if ( dwIndex != -1)
			{
				m_DuiPreMessageFilterSet.Delete(dwIndex);
			}
		}
	}

	virtual VOID AddProMessageFilter(IDuiProMessageFilter* pFilter)
	{
		if ( pFilter )
		{
			AUTOLOCK_CS(m_DuiProMessageFilterSet);
			m_DuiProMessageFilterSet.Add(pFilter);
		}
	}

	virtual VOID RemoveProMessageFilter(IDuiProMessageFilter* pFilter)
	{
		if ( pFilter )
		{
			AUTOLOCK_CS(m_DuiProMessageFilterSet);
			DWORD dwIndex = m_DuiProMessageFilterSet.Find(pFilter);
			if ( dwIndex != -1)
			{
				m_DuiProMessageFilterSet.Delete(dwIndex);
			}
		}
	}

	virtual VOID AddRoutedEventHandler(IDuiRoutedEventHandler* RoutedEventHandler)
	{
		AUTOLOCK_CS(m_DuiRoutedEventHandlerSet);
		INT nIndex = m_DuiRoutedEventHandlerSet.Find(RoutedEventHandler);
		if ( nIndex == -1)
		{
			m_DuiRoutedEventHandlerSet.Add(RoutedEventHandler);
		}
	}

	virtual VOID RemoveRoutedEventHandler(IDuiRoutedEventHandler* RoutedEventHandler)
	{
		INT nIndex = m_DuiRoutedEventHandlerSet.Find(RoutedEventHandler);
		if ( nIndex == -1)
		{
			m_DuiRoutedEventHandlerSet.Delete(nIndex);
		}
	}

	virtual LRESULT SendMessage(IDuiControlCtrl* CtrlTo,INT nMsg,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL)
	{
		CDuiControlCtrl* tCtrl = static_cast<CDuiControlCtrl*>(CtrlTo);

		if ( tCtrl )
		{
			DuiMsg duiMsg = { this, CtrlTo, nMsg, wParam, lParam, lpData};
			BOOL bHandle = TRUE;
			LRESULT lResult = tCtrl->DoPreMessage(duiMsg,bHandle);
			if ( !bHandle )
				return lResult;
			
			
			return tCtrl->DoMessage(duiMsg,bHandle);
		}

		return 0;
	}

	virtual BOOL SetTimer(UINT nTimerID, UINT uElapse)
	{
		IDuiWindowCtrl* tWnd = GetRootCtrl();
		if ( tWnd )
		{
			return tWnd->SetTimer(this, nTimerID, uElapse);
		}

		return FALSE;
	}

	virtual BOOL KillTimer(UINT nTimerID)
	{
		IDuiWindowCtrl* tWnd = GetRootCtrl();
		if ( tWnd )
		{
			return tWnd->KillTimer(this, nTimerID);
		}

		return FALSE;
	}

	virtual BOOL PostMessage(IDuiControlCtrl* CtrlTo,INT nMsg,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL)
	{
		for (IDuiControlCtrl* pCtrl = CtrlTo ; pCtrl ; pCtrl = pCtrl->GetParentCtrl())
		{
			if ( pCtrl->QueryInterface(IIDuiWindowCtrl) )
			{
				return pCtrl->PostMessage(CtrlTo, nMsg, wParam , lParam ,lpData);
			}
		}

		return FALSE;
	}



	LRESULT DoPreMessage(const DuiMsg& duiMsg, BOOL& bHandle)
	{
		AUTOLOCK_CS(m_DuiPreMessageFilterSet);
		for (INT dwLoop = 0 ; dwLoop < m_DuiPreMessageFilterSet.Size() ; dwLoop++)
		{
			IDuiPreMessageFilter* pPreMessageFilter = m_DuiPreMessageFilterSet[dwLoop];
			LRESULT lResult = pPreMessageFilter->OnPreMessageFilter(duiMsg,bHandle);
			if ( !bHandle)
				return lResult;
			
		}

		return 0;
	}

	virtual BOOL DoEvent(const DuiEvent& event)
	{
		AUTOLOCK_CS(m_DuiRoutedEventHandlerSet);
		INT nCount = m_DuiRoutedEventHandlerSet.Size();
		for( INT nLoop = 0 ; nLoop < nCount ; nLoop++)
		{
			IDuiRoutedEventHandler* tDuiRoutedEventHandler = m_DuiRoutedEventHandlerSet[nLoop];
			if ( tDuiRoutedEventHandler )
			{
				if ( !tDuiRoutedEventHandler->OnRoutedEvent(event) )
				{
					break;
				}
			}
		}

		if ( m_pParentControl )
		{
			CDuiControlCtrl* tCtrl = static_cast<CDuiControlCtrl*>(m_pParentControl);
			return tCtrl->DoEvent(event);
		}

		return TRUE;
	}

	

	virtual CDuiRect& GetPos()
	{
		return m_pos;
	}

	virtual VOID SetPos(const CDuiRect& pos)
	{
		m_pos = pos;

		m_pos.left += m_Padding.left;
		m_pos.top  += m_Padding.top;
		m_pos.right -= m_Padding.right;
		m_pos.bottom -= m_Padding.bottom;
	}
	
	//预估该控件所占的大小
	virtual SIZE EstimateSize(SIZE szAvailable)
	{

		SIZE size = { m_pos.GetWidth() , m_pos.GetHeight()};
		return size;
	}

	virtual VOID DoFinalMessage()
	{
		SendMessage(this,DuiMsg_Destory);
		
		//销毁所有定时器
		IDuiControlCtrl* tParent = m_pParentControl;
		for ( ; tParent ; tParent = tParent->GetParentCtrl());
		if ( tParent )
		{
			IDuiWindowCtrl* tWnd = (IDuiWindowCtrl*)tParent->QueryInterface(IIDuiWindowCtrl);
			if ( tWnd )
			{
				return tWnd->KillTimer(this);
			}
		}
	}


	virtual IDuiControlCtrl* GetParentCtrl()
	{
		return m_pParentControl;
	}

	virtual VOID	 SetParentCtrl(IDuiControlCtrl* pCtrl)
	{
		IDuiContainerCtrl* tContainerCtl = (IDuiContainerCtrl*)pCtrl->QueryInterface(IIDuiContainerCtrl);
		if( tContainerCtl )
		{
			tContainerCtl->AddChild(this);
		}

		m_pParentControl = pCtrl;
	}

	virtual IDuiWindowCtrl* GetOwnerWindowCtrl()
	{
		return m_pOwnerWnd;
	}

	virtual VOID SetUserData(LPCWSTR lpszKey,  LPCWSTR lpszValue)
	{
		if ( lpszKey && wcslen(lpszKey) && lpszValue)
		{
			AUTOLOCK_CS(m_userDataMap);
			m_userDataMap.Insert(lpszKey, lpszValue);
		}
	}

	virtual LPCWSTR GetUserData(LPCWSTR lpszKey)
	{
		if ( lpszKey && wcslen(lpszKey))
		{
			AUTOLOCK_CS(m_userDataMap);
			CUserDataMap::Iterator it = m_userDataMap.Find(lpszKey);
			if ( it )
			{
				return it->Value;
			}
		}

		return NULL;
	}

	virtual BOOL AddAnimation(IDuiAnime* pAnime)
	{
		return TRUE;
	}

	virtual BOOL RemoveAnimation(IDuiAnime* pAnime)
	{
		return TRUE;
	}

	//属性
	virtual DWORD GetWidth()
	{
		return m_pos.GetWidth();
	}

	virtual VOID  SetWidth(DWORD dwWidth)
	{
		DWORD dwMaxWidth = GetMaxWidth() ? GetMaxWidth() : 9999;
		if (dwWidth < dwMaxWidth && dwWidth > GetMinHeight())
		{
			m_pos.right = m_pos.left + dwWidth;

			if ( m_pParentControl )
				SendMessage(m_pParentControl, DuiMsg_SizeChanged);
			else
				SendMessage(this, DuiMsg_SizeChanged);

			DuiEvent event;
			event.Ctrl = this;
			event.nEvent = DuiEvent_Size;
			DoEvent(event);
		}

	}

	virtual DWORD GetHeight()
	{
		return m_pos.GetHeight();
	}

	virtual VOID SetHeight(DWORD dwHeight)
	{
		DWORD dwMaxHeight = GetMaxHeight() ? GetMaxHeight() : 9999;
		if (dwHeight < dwMaxHeight && dwHeight > GetMinHeight())
		{
			m_pos.bottom = m_pos.top + dwHeight;
			if ( m_pParentControl )
				SendMessage(m_pParentControl, DuiMsg_SizeChanged);
			else
				SendMessage(this, DuiMsg_SizeChanged);

		
			DuiEvent event;
			event.Ctrl = this;
			event.nEvent = DuiEvent_Size;
			DoEvent(event);
		}
	}

	virtual DWORD GetFixedWidth()
	{
		return m_dwFixedWidth;
	}

	virtual VOID SetFixedWidth(DWORD dwWidth)
	{
		m_dwFixedWidth = dwWidth;
	}

	virtual DWORD GetFixedHeight()
	{
		return m_dwFixedHeight;
	}

	virtual VOID SetFixedHeight(DWORD dwHeight)
	{
		m_dwFixedHeight = dwHeight;
	}

	virtual DWORD GetMaxWidth()
	{
		return m_dwMaxWidth;
	}

	virtual VOID  SetMaxWidth(DWORD dwWidth)
	{
		m_dwMaxWidth = dwWidth;
	}

	virtual DWORD GetMaxHeight()
	{
		return m_dwMaxHeigh;
	}

	virtual VOID SetMaxHeight(DWORD dwHeight)
	{
		m_dwMaxHeigh = dwHeight;
	}

	virtual DWORD GetMinWidth()
	{
		return m_dwMinWidth;
	}

	virtual VOID  SetMinWidth(DWORD dwWidth)
	{
		m_dwMinWidth = dwWidth;
	}

	virtual DWORD GetMinHeight()
	{
		return m_dwMinHeigh;
	}

	virtual VOID SetMinHeight(DWORD dwHeight)
	{
		m_dwMinHeigh = dwHeight;
	}

	virtual CDuiRect GetBorderSize()
	{
		return m_BorderSize;
	}

	virtual VOID SetBorderSize(const CDuiRect& BorderSize)
	{
		m_BorderSize = BorderSize;
	}

	virtual VOID SetBorderRound(const CDuiSize& cxyRound)
	{
		m_cxyBorderRound = cxyRound;
	}

	virtual CDuiSize GetBorderRound()
	{
		return m_cxyBorderRound;
	}

	virtual CDuiRect GetPadding()
	{
		return m_Padding;
	}

	virtual VOID SetPadding(const CDuiRect& Padding)
	{
		m_Padding = Padding;
	}

	virtual DWORD GetAxisX()
	{
		return m_dwAxisX;
	}

	virtual VOID  SetAxisX(DWORD dwAxis)
	{
		m_dwAxisX = dwAxis;
	}

	virtual DWORD GetAxisY()
	{
		return m_dwAxisY;
	}

	virtual VOID  SetAxisY(DWORD dwAxis)
	{
		m_dwAxisY = dwAxis;
	}

	virtual DWORD GetAxisZ() const
	{
		return m_dwAxisZ;
	}

	virtual VOID  SetAxisZ(DWORD dwAxis)
	{
		m_dwAxisZ = dwAxis;
		if ( m_pParentControl )
		{
			SendMessage(m_pParentControl, DuiMsg_Z_OrderChanged);
		}
	}

	virtual Valign  GetValign()
	{
		return m_Valign;
	}

	virtual VOID  SetValign(Valign pos)
	{
		m_Valign = pos;
	}

	virtual Halign  GetHalign()
	{
		return m_Halign;
	}

	virtual VOID  SetHalign(Halign pos)
	{
		m_Halign = pos;
	}

	virtual VOID  SetBkImage(LPCWSTR lpszBkImage)
	{
		if ( lpszBkImage && wcslen(lpszBkImage))
		{
			m_bkImage = lpszBkImage;
		}
	}

	virtual LPCWSTR GetBkImage()
	{
		return m_bkImage;
	}

	virtual VOID SetBkColor(DWORD dwBkColor)
	{
		m_dwBkColor = dwBkColor;
	}

	virtual DWORD GetBkColor()
	{
		return m_dwBkColor;
	}


	virtual LPCWSTR GetName()
	{
		return m_strControlName;
	}

	virtual VOID SetName(LPCWSTR lpszName)
	{
		if ( lpszName && wcslen(lpszName))
		{
			m_strControlName = lpszName;
		}
	}

	virtual VOID SetVisable(BOOL bVisable)
	{
		m_bVisable = bVisable;

		DuiEvent event = { 0 };
		event.Ctrl = this;
		event.nEvent = DuiEvent_VisableChanged;
		event.wParam = m_bVisable;
		DoEvent(event);
	}

	virtual BOOL GetVisable()
	{
		return m_bVisable;
	}


	virtual LPCWSTR GetNameSpace()
	{
		return m_strNameSpace;
	}

	virtual VOID SetNameSpace(LPCWSTR lpszName)
	{
		if ( lpszName && wcslen(lpszName))
		{
			m_strNameSpace = lpszName;
		}
	}

	virtual BOOL Invalidate()
	{
		return ::InvalidateRect(GetOwnerWindowCtrl()->GetHWND(), &m_pos, FALSE);;
	}

	virtual BOOL IsInvalidate()
	{
		return m_bUpdateNeeded;
	}

	virtual IDuiWindowCtrl* GetRootCtrl()
	{
		return m_pRootWnd;
	}

protected:
	LPVOID m_pParam;
	IDuiCore* m_pDuiCore;
	IDuiControlCtrl* m_pParentControl;

	typedef CDuiVector<IDuiPreMessageFilter*> CDuiPreMessageFilterSet;
	CDuiPreMessageFilterSet m_DuiPreMessageFilterSet;
	DECLARE_AUTOLOCK_CS(m_DuiPreMessageFilterSet);

	typedef CDuiVector<IDuiProMessageFilter*> CDuiProMessageFilterSet;
	CDuiProMessageFilterSet m_DuiProMessageFilterSet;
	DECLARE_AUTOLOCK_CS(m_DuiProMessageFilterSet);

	typedef CDuiVector<IDuiRoutedEventHandler*> CDuiRoutedEventHandlerSet;
	CDuiRoutedEventHandlerSet m_DuiRoutedEventHandlerSet;
	DECLARE_AUTOLOCK_CS(m_DuiRoutedEventHandlerSet);

	CDuiRect	m_pos;
	BOOL		m_bInit; //第一次初始化标记
	BOOL		m_bUpdateNeeded;
	CDuiRect	m_rcPaint;
	CDuiSize	m_cxyBorderRound;
	IDuiWindowCtrl* m_pRootWnd;
	IDuiWindowCtrl* m_pOwnerWnd;
	DWORD m_dwFixedWidth;
	DWORD m_dwFixedHeight;
};

};//namespace DuiKit{;