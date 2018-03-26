#pragma once

#include <DuiCtrl\IDuiScrollCtrl.h>


namespace DuiKit{;


class CDuiScrollCtrl : 
	public IDuiScrollCtrl,
	public CDuiContainerCtrl
{
public:
	CDuiScrollCtrl():
	  m_bHorizontalScroll(TRUE),
	  m_bVerticalScroll(TRUE),
	  m_bShowBottomButton(TRUE),
	  m_bShowTopButton(TRUE),
	  m_bShowRightButton(TRUE),
	  m_bShowLeftButton(TRUE),
	  m_dwTopButtonState(0),
	  m_dwBottomButtonState(0),
	  m_dwVerticalRailState(0),
	  m_dwVerticalThumbState(0),
	  m_dwLeftButtonState(0),
	  m_dwRightButtonState(0),
	  m_dwHorizontalThumbState(0),
	  m_dwHorizontalRailState(0),
	  m_dwHorizontalLine(8),
	  m_dwVerticalLine(8),
	  m_dwHorizontalHeight(15),
	  m_dwVerticalWidth(15),
	  m_dwHorizontalButtonWidth(16),
	  m_dwVerticalButtonHeight(16)
	{

	}

	~CDuiScrollCtrl()
	{

	}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiScrollCtrl, CTRL_SCROLL)
		DUI_DEFINE_INTERFACE(IDuiScrollCtrl, IIDuiScrollCtrl)
		DUI_DEFINE_INTERFACE(IDuiContainerCtrl, IIDuiContainerCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiScrollCtrl)
		DUI_SETATTRIBUTE_STRING(TopButtonNormalImage,			   m_strTopButtonNormalImage) ;
		DUI_SETATTRIBUTE_STRING(TopButtonHotImage,                 m_strTopButtonHotImage)             
		DUI_SETATTRIBUTE_STRING(TopButtonPushedImage,              m_strTopButtonPushedImage)         
		DUI_SETATTRIBUTE_STRING(TopButtonDisabledImage,            m_strTopButtonDisabledImage)       
		DUI_SETATTRIBUTE_STRING(BottomButtonNormalImage,           m_strBottomButtonNormalImage)      
		DUI_SETATTRIBUTE_STRING(BottomButtonHotImage,              m_strBottomButtonHotImage)         
		DUI_SETATTRIBUTE_STRING(BottomButtonPushedImage,           m_strBottomButtonPushedImage)      
		DUI_SETATTRIBUTE_STRING(BottomButtonDisabledImage,         m_strBottomButtonDisabledImage)    
		DUI_SETATTRIBUTE_STRING(VerticalThumbNormalImage,          m_strVerticalThumbNormalImage)     
		DUI_SETATTRIBUTE_STRING(VerticalThumbHotImage,             m_strVerticalThumbHotImage)        
		DUI_SETATTRIBUTE_STRING(VerticalThumbPushedImage,          m_strVerticalThumbPushedImage)     
		DUI_SETATTRIBUTE_STRING(VerticalThumbDisabledImage,        m_strVerticalThumbDisabledImage)   
		DUI_SETATTRIBUTE_STRING(VerticalRailNormalImage,           m_strVerticalRailNormalImage)      
		DUI_SETATTRIBUTE_STRING(VerticalRailHotImage,              m_strVerticalRailHotImage)         
		DUI_SETATTRIBUTE_STRING(VerticalRailPushedImage,           m_strVerticalRailPushedImage)      
		DUI_SETATTRIBUTE_STRING(VerticalRailDisabledImage,         m_strVerticalRailDisabledImage)    
		DUI_SETATTRIBUTE_STRING(LeftButtonNormalImage,             m_strLeftButtonNormalImage)        
		DUI_SETATTRIBUTE_STRING(LeftButtonHotImage,                m_strLeftButtonHotImage)           
		DUI_SETATTRIBUTE_STRING(LeftButtonPushedImage,             m_strLeftButtonPushedImage)        
		DUI_SETATTRIBUTE_STRING(LeftButtonDisabledImage,           m_strLeftButtonDisabledImage)      
		DUI_SETATTRIBUTE_STRING(RightButtonNormalImage,            m_strRightButtonNormalImage)       
		DUI_SETATTRIBUTE_STRING(RightButtonHotImage,               m_strRightButtonHotImage)          
		DUI_SETATTRIBUTE_STRING(RightButtonPushedImage,            m_strRightButtonPushedImage)       
		DUI_SETATTRIBUTE_STRING(RightButtonDisabledImage,          m_strRightButtonDisabledImage)     
		DUI_SETATTRIBUTE_STRING(HorizontalThumbNormalImage,        m_strHorizontalThumbNormalImage)   
		DUI_SETATTRIBUTE_STRING(HorizontalThumbHotImage,           m_strHorizontalThumbHotImage)      
		DUI_SETATTRIBUTE_STRING(HorizontalThumbPushedImage,        m_strHorizontalThumbPushedImage)   
		DUI_SETATTRIBUTE_STRING(HorizontalThumbDisabledImage,      m_strHorizontalThumbDisabledImage) 
		DUI_SETATTRIBUTE_STRING(HorizontalRailNormalImage,         m_strHorizontalRailNormalImage)    
		DUI_SETATTRIBUTE_STRING(HorizontalRailHotImage,            m_strHorizontalRailHotImage)       
		DUI_SETATTRIBUTE_STRING(HorizontalRailPushedImage,         m_strHorizontalRailPushedImage)    
		DUI_SETATTRIBUTE_STRING(HorizontalRailDisabledImage,       m_strHorizontalRailDisabledImage)

		DUI_SETATTRIBUTE_BOOL(ShowBottomButton,m_bShowBottomButton) ;
		DUI_SETATTRIBUTE_BOOL(ShowTopButton,m_bShowTopButton);
		DUI_SETATTRIBUTE_BOOL(ShowRightButton,m_bShowRightButton);
		DUI_SETATTRIBUTE_BOOL(ShowLeftButton,m_bShowRightButton);

		DUI_SETATTRIBUTE_BOOL(HorizontalScroll, m_bHorizontalScroll)
		DUI_SETATTRIBUTE_BOOL(VerticalScroll, m_bVerticalScroll)

		DUI_SETATTRIBUTE_INT(HorizontalLine, m_dwHorizontalLine)
		DUI_SETATTRIBUTE_INT(VerticalLine, m_dwVerticalLine)
		DUI_SETATTRIBUTE_INT(HorizontalHeight, m_dwHorizontalHeight)
		DUI_SETATTRIBUTE_INT(VerticalWidth, m_dwVerticalWidth)
		DUI_SETATTRIBUTE_INT(HorizontalButtonWidth, m_dwHorizontalButtonWidth)
		DUI_SETATTRIBUTE_INT(VerticalButtonHeight, m_dwVerticalButtonHeight)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiContainerCtrl)
	DUI_END_SETATTRIBUTE;
	
	DUI_BEGIN_MSG_MAP(CDuiContainerCtrl)
		DUI_MESSAGE_HANDLER(DuiMsg_Initialize, OnInitialize)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseMove, OnMouseMove)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDown, OnLButtonDown)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonUp, OnLButtonUp)
		DUI_MESSAGE_HANDLER(DuiMsg_LButtonDblClk, OnLButtonDblClk)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseEnter, OnMouseEnter)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseLeave, OnMouseLeave)
		DUI_MESSAGE_HANDLER(DuiMsg_MouseHover, OnMouseHover)
	DUI_END_MSG_MAP();
	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiContainerCtrl::DoCreate(pParent, pCore, pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	virtual LPCWSTR GetLeftButtonNormalImage()
	{
		return m_strLeftButtonNormalImage;
	}

	virtual VOID SetLeftButtonNormalImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strLeftButtonNormalImage = lpszImage;
		}
	}

	virtual LPCWSTR GetLeftButtonHotImage()
	{
		return m_strLeftButtonHotImage;
	}

	virtual VOID SetLeftButtonHotImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strLeftButtonHotImage = lpszImage;
		}
	}


	virtual LPCWSTR GetLeftButtonPushedImage()
	{
		return m_strLeftButtonPushedImage;
	}

	virtual VOID SetLeftButtonPushedImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strLeftButtonPushedImage = lpszImage;
		}
	}


	virtual LPCWSTR GetLeftButtonDisabledImage()
	{
		return m_strLeftButtonDisabledImage;
	}

	virtual VOID SetLeftButtonDisabledImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strLeftButtonDisabledImage = lpszImage;
		}
	}


	virtual VOID SetShowLeftButton(BOOL bShow)
	{
		m_bShowLeftButton = bShow;
	}

	virtual BOOL GetShowLeftButton()
	{
		return m_bShowLeftButton;
	}

	//操作下边按钮

	virtual LPCWSTR GetRightButtonNormalImage()
	{
		return m_strRightButtonNormalImage;
	}

	virtual VOID SetRightButtonNormalImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strRightButtonNormalImage = lpszImage;
		}
	}


	virtual LPCWSTR GetRightButtonHotImage() 
	{
		return m_strRightButtonHotImage;
	}

	virtual VOID SetRightButtonHotImage(LPCWSTR lpszImage)
	{
		if ( lpszImage)
		{
			m_strRightButtonHotImage = lpszImage;
		}
	}


	virtual LPCWSTR GetRightButtonPushedImage()
	{
		return m_strRightButtonPushedImage;
	}

	virtual VOID SetRightButtonPushedImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strRightButtonPushedImage = lpszImage;
		}
	}


	virtual LPCWSTR GetRightButtonDisabledImage()
	{
		return m_strRightButtonDisabledImage;
	}

	virtual VOID SetRightButtonDisabledImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strRightButtonDisabledImage = lpszImage;
		}
	}


	virtual VOID SetShowRightButton(BOOL bShow)
	{
		m_bShowRightButton = bShow;
	}

	virtual BOOL GetShowRightButton()
	{
		return m_bShowRightButton;
	}


	virtual LPCWSTR GetHorizontalThumbNormalImage()
	{
		return m_strHorizontalThumbNormalImage;
	}

	virtual VOID SetHorizontalThumbNormalImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalThumbNormalImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalThumbHotImage()
	{
		return m_strHorizontalThumbHotImage;
	}

	virtual VOID SetHorizontalThumbHotImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalThumbHotImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalThumbPushedImage()
	{
		return m_strHorizontalThumbPushedImage;
	}

	virtual VOID SetHorizontalThumbPushedImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalThumbPushedImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalThumbDisabledImage()
	{
		return m_strHorizontalThumbDisabledImage;
	}

	virtual VOID SetHorizontalThumbDisabledImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalThumbDisabledImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalRailNormalImage()
	{
		return m_strHorizontalRailNormalImage;
	}

	virtual VOID SetHorizontalRailNormalImage(LPCWSTR pStrImage) 
	{
		if ( pStrImage )
		{
			m_strHorizontalRailNormalImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalRailHotImage()
	{
		return m_strHorizontalRailHotImage;
	}

	virtual VOID SetHorizontalRailHotImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalRailHotImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalRailPushedImage()
	{
		return m_strHorizontalRailPushedImage;
	}

	virtual VOID SetHorizontalRailPushedImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalRailPushedImage = pStrImage;
		}
	}


	virtual LPCWSTR GetHorizontalRailDisabledImage()
	{
		return m_strHorizontalRailDisabledImage;
	}

	virtual VOID SetHorizontalRailDisabledImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strHorizontalRailDisabledImage = pStrImage;
		}
	}

	virtual LPCWSTR GetTopButtonNormalImage()
	{
		return m_strTopButtonNormalImage;
	}

	virtual VOID SetTopButtonNormalImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strTopButtonNormalImage = lpszImage;
		}
	}

	virtual LPCWSTR GetTopButtonHotImage()
	{
		return m_strTopButtonHotImage;
	}

	virtual VOID SetTopButtonHotImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strTopButtonHotImage = lpszImage;
		}
	}


	virtual LPCWSTR GetTopButtonPushedImage()
	{
		return m_strTopButtonPushedImage;
	}

	virtual VOID SetTopButtonPushedImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strTopButtonPushedImage = lpszImage;
		}
	}


	virtual LPCWSTR GetTopButtonDisabledImage()
	{
		return m_strTopButtonDisabledImage;
	}

	virtual VOID SetTopButtonDisabledImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strTopButtonDisabledImage = lpszImage;
		}
	}


	virtual VOID SetShowTopButton(BOOL bShow)
	{
		m_bShowTopButton = bShow;
	}

	virtual BOOL GetShowTopButton()
	{
		return m_bShowTopButton;
	}

	//操作下边按钮

	virtual LPCWSTR GetBottomButtonNormalImage()
	{
		return m_strBottomButtonNormalImage;
	}

	virtual VOID SetBottomButtonNormalImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strBottomButtonNormalImage = lpszImage;
		}
	}


	virtual LPCWSTR GetBottomButtonHotImage() 
	{
		return m_strBottomButtonHotImage;
	}

	virtual VOID SetBottomButtonHotImage(LPCWSTR lpszImage)
	{
		if ( lpszImage)
		{
			m_strBottomButtonHotImage = lpszImage;
		}
	}


	virtual LPCWSTR GetBottomButtonPushedImage()
	{
		return m_strBottomButtonPushedImage;
	}

	virtual VOID SetBottomButtonPushedImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strBottomButtonPushedImage = lpszImage;
		}
	}


	virtual LPCWSTR GetBottomButtonDisabledImage()
	{
		return m_strBottomButtonDisabledImage;
	}

	virtual VOID SetBottomButtonDisabledImage(LPCWSTR lpszImage)
	{
		if ( lpszImage )
		{
			m_strBottomButtonDisabledImage = lpszImage;
		}
	}


	virtual VOID SetShowBottomButton(BOOL bShow)
	{
		m_bShowBottomButton = bShow;
	}

	virtual BOOL GetShowBottomButton()
	{
		return m_bShowBottomButton;
	}


	virtual LPCWSTR GetVerticalThumbNormalImage()
	{
		return m_strVerticalThumbNormalImage;
	}

	virtual VOID SetVerticalThumbNormalImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalThumbNormalImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalThumbHotImage()
	{
		return m_strVerticalThumbHotImage;
	}

	virtual VOID SetVerticalThumbHotImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalThumbHotImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalThumbPushedImage()
	{
		return m_strVerticalThumbPushedImage;
	}

	virtual VOID SetVerticalThumbPushedImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalThumbPushedImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalThumbDisabledImage()
	{
		return m_strVerticalThumbDisabledImage;
	}

	virtual VOID SetVerticalThumbDisabledImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalThumbDisabledImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalRailNormalImage()
	{
		return m_strVerticalRailNormalImage;
	}

	virtual VOID SetVerticalRailNormalImage(LPCWSTR pStrImage) 
	{
		if ( pStrImage )
		{
			m_strVerticalRailNormalImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalRailHotImage()
	{
		return m_strVerticalRailHotImage;
	}

	virtual VOID SetVerticalRailHotImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalRailHotImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalRailPushedImage()
	{
		return m_strVerticalRailPushedImage;
	}

	virtual VOID SetVerticalRailPushedImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalRailPushedImage = pStrImage;
		}
	}


	virtual LPCWSTR GetVerticalRailDisabledImage()
	{
		return m_strVerticalRailDisabledImage;
	}

	virtual VOID SetVerticalRailDisabledImage(LPCWSTR pStrImage)
	{
		if ( pStrImage )
		{
			m_strVerticalRailDisabledImage = pStrImage;
		}
	}


	virtual VOID SetHorizontalScroll(BOOL bShow) 
	{
		m_bHorizontalScroll = bShow;
	}

	virtual BOOL GetHorizontalScroll() 
	{
		return m_bHorizontalScroll;
	}

	virtual VOID SetVerticalScroll(BOOL bShow) 
	{
		m_bVerticalScroll = bShow;
	}

	virtual BOOL GetVerticalScroll()
	{
		return m_bVerticalScroll;
	}

	virtual DWORD GetHorizontalLine()
	{
		return m_dwHorizontalLine;
	}

	virtual VOID SetHorizontalLine(DWORD dwLine)
	{
		m_dwHorizontalLine = dwLine;
	}

	virtual DWORD GetVerticalLine()
	{
		return m_dwVerticalLine;
	}

	virtual VOID SetVerticalLine(DWORD dwLine)
	{
		m_dwVerticalLine = dwLine;
	}

	virtual DWORD GetHorizontalHeight()
	{
		return m_dwHorizontalHeight;
	}

	virtual VOID SetHorizontalHeight(DWORD dwHeight)
	{
		m_dwHorizontalHeight = dwHeight;
	}

	virtual DWORD GetVerticalWidth()
	{
		return m_dwVerticalWidth;
	}
	virtual VOID SetVerticalWidth(DWORD dwHeight)
	{
		m_dwVerticalWidth = dwHeight;
	}

	virtual DWORD GetHorizontalButtonWidth()
	{
		return m_dwHorizontalButtonWidth;
	}

	virtual VOID SetHorizontalButtonWidth(DWORD dwWidth) 
	{
		m_dwHorizontalButtonWidth = dwWidth;
	}

	virtual DWORD GetVerticalButtonHeight()
	{
		return m_dwVerticalButtonHeight;
	}
	virtual VOID SetVerticalButtonHeight(DWORD dwWidth)
	{
		m_dwVerticalButtonHeight = dwWidth;
	}

	virtual CDuiRect GetClientPos()
	{
		return m_rcClient;
	}
	virtual VOID SetPos(const CDuiRect& pos)
	{
		CDuiControlCtrl::SetPos(pos);
		CDuiRect rcHorizontalScrollPos;
		CDuiRect rcVerticalScrollPos;
		CDuiRect rcClient = m_pos;
		if ( GetHorizontalScroll() )
		{
			rcHorizontalScrollPos = m_pos;
			rcHorizontalScrollPos.top = m_pos.bottom - GetHorizontalHeight();

			if ( GetVerticalScroll())
			{
				rcHorizontalScrollPos.right = m_pos.right -  GetVerticalWidth();
			}
			SetHorizontalScrollPos(rcHorizontalScrollPos);
		}
		if ( GetVerticalScroll() )
		{
			rcVerticalScrollPos = m_pos;
			rcVerticalScrollPos.left = m_pos.right - GetVerticalWidth();

			if ( GetHorizontalScroll() )
			{
				rcVerticalScrollPos.bottom = m_pos.bottom - GetHorizontalHeight();
			}
			SetVerticalScrollPos(rcVerticalScrollPos);
		}
		

		rcClient.bottom = rcClient.bottom - rcHorizontalScrollPos.GetHeight();
		rcClient.right = rcClient.right - rcVerticalScrollPos.GetWidth();
		SetClientPos(rcClient);
	}

	virtual VOID SetVerticalScrollPos(const CDuiRect& pos)
	{
		m_rcVerticalScrollPos = pos;

		m_rcTopButton.Empty();
		m_rcBottomButton.Empty();
		m_rcVerticalThumb.Empty();
		m_rcVerticalRail.Empty();


		//设置top
		if ( GetShowTopButton() )
		{
			m_rcTopButton = m_rcVerticalScrollPos;
			m_rcTopButton.bottom = m_rcTopButton.top + GetVerticalButtonHeight();
		}

		//设置buttom
		if (GetShowBottomButton())
		{
			m_rcBottomButton = m_rcVerticalScrollPos;
			m_rcBottomButton.top = m_rcBottomButton.bottom - GetVerticalButtonHeight();
		}

		//设置滚动槽的位置
		m_rcVerticalThumb = m_rcVerticalScrollPos;
		//m_rcVerticalThumb.top = m_rcVerticalThumb.top + m_rcTopButton.GetHeight();
		//m_rcVerticalThumb.bottom = m_rcVerticalThumb.bottom - m_rcBottomButton.GetHeight();

		//设置滚动把手的位置
	}

	virtual VOID SetHorizontalScrollPos(const CDuiRect& pos)
	{
		m_rcHorizontalScrollPos = pos;
		m_rcLeftButton.Empty();
		m_rcRightButton.Empty();
		m_rcHorizontalThumb.Empty();
		m_rcHorizontalRail.Empty();
		//设置left

		if ( GetShowLeftButton())
		{
			m_rcLeftButton = pos;
			m_rcLeftButton.right = m_rcLeftButton.left + GetHorizontalButtonWidth();
		}

		//设置right

		if ( GetShowRightButton() )
		{
			m_rcRightButton = pos;
			m_rcRightButton.left = m_rcRightButton.right - GetHorizontalButtonWidth();
		}

		//设置滚动槽的位置
		m_rcHorizontalThumb = m_rcHorizontalScrollPos;
		//m_rcHorizontalThumb.left = m_rcHorizontalThumb.left + m_rcLeftButton.GetWidth();
		//m_rcHorizontalThumb.right = m_rcHorizontalThumb.right - m_rcRightButton.GetWidth();


		//设置滚动把手的位置
	}

	virtual VOID SetClientPos(const CDuiRect& pos)
	{
		m_rcClient = pos;
	}

	virtual VOID DoPaint(HDC hDC, const RECT& rcPaint )
	{
		CDuiControlCtrl::PaintBkImage(hDC);
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_pos) ) 
			return;

		if ( ::IntersectRect(&rcTemp, &rcPaint, &m_rcClient) )
		{
			DoPaintClient(hDC);
		}
		
		if ( ::IntersectRect(&rcTemp, &rcPaint, &m_rcHorizontalScrollPos) )
		{
			DoPaintHorizontalScroll(hDC);
		}

		if ( ::IntersectRect(&rcTemp, &rcPaint, &m_rcVerticalScrollPos) )
		{
			DoPaintVerticalScroll(hDC);
		}
	}

	virtual VOID DoPaintClient(HDC hDC)
	{
		CDuiRenderEngine::DrawColor(hDC, m_rcClient, 0xFFFF00FF);
	}

	virtual VOID DoPaintHorizontalScroll(HDC hDC)
	{
		do 
		{
			if( (m_dwHorizontalThumbState & UISTATE_DISABLED) != 0 ) {
				if( !m_strHorizontalThumbNormalImage.IsEmpty() ) {
					if( !PrintImage(hDC, (LPCTSTR)m_strHorizontalThumbNormalImage, m_rcHorizontalScrollPos, m_rcHorizontalScrollPos) ) 
						m_strHorizontalThumbNormalImage.Empty();
					else break;
				}
			}
			else if( (m_dwHorizontalThumbState & UISTATE_PUSHED) != 0 ) {
				if( !m_strHorizontalThumbPushedImage.IsEmpty() ) {
					if( !PrintImage(hDC, (LPCTSTR)m_strHorizontalThumbPushedImage, m_rcHorizontalScrollPos, m_rcHorizontalScrollPos) ) 
						m_strHorizontalThumbPushedImage.Empty();
					else break;
				}
			}
			else if( (m_dwHorizontalThumbState & UISTATE_HOT) != 0 ) {
				if( !m_strHorizontalThumbHotImage.IsEmpty() ) {
					if( !PrintImage(hDC, (LPCTSTR)m_strHorizontalThumbHotImage, m_rcHorizontalScrollPos, m_rcHorizontalScrollPos) ) 
						m_strHorizontalThumbHotImage.Empty();
					else break;
				}
			}

		} while (FALSE);
		


		////////////////////////////////////////////////////////////

		do 
		{
			if( (m_dwLeftButtonState & UISTATE_DISABLED) != 0 ) {
				if( !m_strLeftButtonDisabledImage.IsEmpty() ) {
					if( !PrintImage(hDC, (LPCTSTR)m_strLeftButtonDisabledImage, m_rcLeftButton, m_rcLeftButton) ) 
						m_strLeftButtonDisabledImage.Empty();
					else break;
				}
			}
			else if( (m_dwLeftButtonState & UISTATE_PUSHED) != 0 ) {
				if( !m_strLeftButtonPushedImage.IsEmpty() ) {
					if( !PrintImage(hDC, (LPCTSTR)m_strLeftButtonPushedImage, m_rcLeftButton, m_rcLeftButton) ) 
						m_strLeftButtonPushedImage.Empty();
					else break;
				}
			}
			else if( (m_dwLeftButtonState & UISTATE_HOT) != 0 ) {
				if( !m_strLeftButtonHotImage.IsEmpty() ) {
					if( !PrintImage(hDC, (LPCTSTR)m_strLeftButtonHotImage, m_rcLeftButton, m_rcLeftButton) ) 
						m_strLeftButtonHotImage.Empty();
					else break;
				}
			}

			if( !m_strLeftButtonNormalImage.IsEmpty() ) {
				if( !PrintImage(hDC, (LPCTSTR)m_strLeftButtonNormalImage, m_rcLeftButton, m_rcLeftButton) ) 
					m_strLeftButtonNormalImage.Empty();
				else break;
			}

			DWORD dwBorderColor = 0xFF85E4FF;
			int nBorderSize = 2;
			CDuiRenderEngine::DrawRect(hDC, m_rcLeftButton, nBorderSize, dwBorderColor);

		} while (FALSE);
		
	}

	virtual VOID DoPaintVerticalScroll(HDC hDC)
	{
		CDuiRenderEngine::DrawImage(hDC, GetImage(m_strVerticalThumbNormalImage), m_rcVerticalThumb, m_rcVerticalScrollPos);
		CDuiRenderEngine::DrawImage(hDC, GetImage(m_strTopButtonHotImage), m_rcTopButton, m_rcTopButton);
		CDuiRenderEngine::DrawImage(hDC, GetImage(m_strBottomButtonHotImage), m_rcBottomButton, m_rcBottomButton);
		//CDuiRenderEngine::DrawImage(hDC, GetImage(m_strVerticalThumbNormalImage), m_rcVerticalThumb, m_rcVerticalScrollPos);
		//CDuiRenderEngine::DrawColor(hDC, m_rcLeftButton,		0xFF0FD0AF);
		//CDuiRenderEngine::DrawColor(hDC, m_rcRightButton,		0xFF1AF0DA);
		
	}

protected:
	LRESULT OnInitialize(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		return 0;
	}

	LRESULT OnMouseHover(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		return 0;
	}

	LRESULT OnMouseLeave(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		return 0;
	}


	LRESULT OnMouseMove(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiPoint point(lParam);
		if ( PtInRect(&m_rcClient, point))
		{
			if ( m_dwLeftButtonState & UISTATE_HOT)
			{
				m_dwLeftButtonState &= ~UISTATE_HOT;
				m_dwRightButtonState &= ~UISTATE_HOT;

				m_dwTopButtonState &= ~UISTATE_HOT;
				m_dwBottomButtonState &= ~UISTATE_HOT;

				m_dwHorizontalThumbState &= ~UISTATE_HOT;
				m_dwVerticalThumbState &= ~UISTATE_HOT;

				Invalidate();
			}
		}
		else if ( PtInRect(&m_rcHorizontalScrollPos, point))
		{
			if ( !(m_dwLeftButtonState & UISTATE_HOT) )
			{
				m_dwLeftButtonState  |= UISTATE_HOT;
				m_dwRightButtonState |= UISTATE_HOT;

				if( ::PtInRect(&m_rcHorizontalThumb, point) ) 
					m_dwHorizontalThumbState |= UISTATE_HOT;

				Invalidate();
			}
		}
		else if ( PtInRect(&m_rcVerticalScrollPos, point))
		{
			if ( !(m_dwTopButtonState & UISTATE_HOT) )
			{
				m_dwTopButtonState		|= UISTATE_HOT;
				m_dwBottomButtonState	|= UISTATE_HOT;

				if( ::PtInRect(&m_rcVerticalThumb, point) ) 
					m_dwVerticalThumbState |= UISTATE_HOT;

				Invalidate();
			}
		}


		return 0;
	}

	LRESULT OnLButtonDown(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiPoint point(lParam);

		if ( PtInRect(&m_rcTopButton, point) )
		{
			m_dwTopButtonState |= UISTATE_PUSHED;
			InvalidateRect(m_rcTopButton);
		}
		else if ( PtInRect(&m_rcBottomButton, point) )
		{
			m_dwBottomButtonState |= UISTATE_PUSHED;
			InvalidateRect(m_rcBottomButton);
		}
		else if ( PtInRect(&m_rcLeftButton, point) )
		{
			m_dwLeftButtonState  |= UISTATE_PUSHED;
			InvalidateRect(m_rcLeftButton);
		}
		else if ( PtInRect(&m_rcRightButton, point) )
		{
			m_dwRightButtonState |= UISTATE_PUSHED;
			InvalidateRect(m_rcRightButton);
		}
		return 0;
	}


	LRESULT OnLButtonUp(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiPoint point(lParam);
		return 0;
	}

	LRESULT OnLButtonDblClk(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiPoint point(lParam);
		return 0;
	}

	LRESULT OnMouseEnter(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		CDuiPoint point(lParam);
		return 0;
	}

public:
	BOOL m_bHorizontalScroll;
	BOOL m_bVerticalScroll;

	CDuiString m_strTopButtonNormalImage;
	CDuiString m_strTopButtonHotImage;
	CDuiString m_strTopButtonPushedImage;
	CDuiString m_strTopButtonDisabledImage;
	
	
	CDuiString m_strBottomButtonNormalImage;
	CDuiString m_strBottomButtonHotImage;
	CDuiString m_strBottomButtonPushedImage;
	CDuiString m_strBottomButtonDisabledImage;

	

	CDuiString m_strVerticalThumbNormalImage;
	CDuiString m_strVerticalThumbHotImage;
	CDuiString m_strVerticalThumbPushedImage;
	CDuiString m_strVerticalThumbDisabledImage;
	

	CDuiString m_strVerticalRailNormalImage;
	CDuiString m_strVerticalRailHotImage;
	CDuiString m_strVerticalRailPushedImage;
	CDuiString m_strVerticalRailDisabledImage;

	DWORD m_dwTopButtonState;
	DWORD m_dwBottomButtonState;
	DWORD m_dwVerticalRailState;
	DWORD m_dwVerticalThumbState;

	CDuiRect m_rcTopButton;
	CDuiRect m_rcBottomButton;
	CDuiRect m_rcVerticalThumb;
	CDuiRect m_rcVerticalRail;
	BOOL m_bShowBottomButton;
	BOOL m_bShowTopButton;


public:
	CDuiString m_strLeftButtonNormalImage;
	CDuiString m_strLeftButtonHotImage;
	CDuiString m_strLeftButtonPushedImage;
	CDuiString m_strLeftButtonDisabledImage;

	CDuiString m_strRightButtonNormalImage;
	CDuiString m_strRightButtonHotImage;
	CDuiString m_strRightButtonPushedImage;
	CDuiString m_strRightButtonDisabledImage;

	CDuiString m_strHorizontalThumbNormalImage;
	CDuiString m_strHorizontalThumbHotImage;
	CDuiString m_strHorizontalThumbPushedImage;
	CDuiString m_strHorizontalThumbDisabledImage;
	CDuiString m_strHorizontalRailNormalImage;
	CDuiString m_strHorizontalRailHotImage;
	CDuiString m_strHorizontalRailPushedImage;
	CDuiString m_strHorizontalRailDisabledImage;

	DWORD m_dwLeftButtonState;
	DWORD m_dwRightButtonState;
	DWORD m_dwHorizontalThumbState;
	DWORD m_dwHorizontalRailState;

	CDuiRect m_rcLeftButton;
	CDuiRect m_rcRightButton;
	CDuiRect m_rcHorizontalThumb;
	CDuiRect m_rcHorizontalRail;
	DWORD m_dwState;
	BOOL m_bShowRightButton;
	BOOL m_bShowLeftButton;

	DWORD m_dwHorizontalLine;
	DWORD m_dwVerticalLine;

	DWORD m_dwHorizontalHeight;
	DWORD m_dwVerticalWidth;

	DWORD m_dwHorizontalButtonWidth;
	DWORD m_dwVerticalButtonHeight;

	CDuiRect m_rcClient;
	CDuiRect m_rcHorizontalScrollPos;
	CDuiRect m_rcVerticalScrollPos;
};

};