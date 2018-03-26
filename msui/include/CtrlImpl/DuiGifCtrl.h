#pragma once
#include <DuiCtrl/IDuiGifCtrl.h>
#include "DuiPictureCtrl.h"
#include <DuiImpl/DuiGifImage.h>
#include <DuiCore/IDuiStream.h>

namespace DuiKit{;


class CDuiGifCtrl : 
	public IDuiGifCtrl, 
	public CDuiPictureCtrl
{
public:
	CDuiGifCtrl(){}
	~CDuiGifCtrl(){}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiGifCtrl, CTRL_GIF)
		DUI_DEFINE_INTERFACE(IDuiGifCtrl, IIDuiGifCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiGifCtrl)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiPictureCtrl);
	DUI_END_SETATTRIBUTE;

	DUI_BEGIN_MSG_MAP(CDuiGifCtrl)
		DUI_MESSAGE_HANDLER_NO_PARAM(DuiMsg_Initialize, OnInitialize)
		DUI_MESSAGE_HANDLER(DuiMsg_Timer, OnDuiTimer)
	DUI_END_MSG_MAP_SUPER()

	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiPictureCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	//IDuiGifCtrl
	virtual VOID SetImage(LPCWSTR lpszFile)
	{
		if ( lpszFile )
		{
			m_strImage = lpszFile;
		}

		IDuiByteStream* pStream = GetDuiSkin()->GetStream(lpszFile,FALSE);
		if ( pStream )
		{
			m_gifImage.LoadFromBuffer(pStream->GetData(), pStream->GetBufferSize());
			pStream->DeleteThis();
		}
		else
		{
			m_gifImage.LoadFromFile(lpszFile);
		}
	}

	virtual LPCWSTR GetImage()
	{
		return m_strImage;
	}

	virtual DWORD GetImageWidth()
	{
		return m_gifImage.GetWidth();
	}

	virtual DWORD GetImageHeigh() 
	{
		return m_gifImage.GetHeight();
	}

	virtual VOID PaintStatusImage(HDC hDC)
	{
		if ( GetVisable())
		{
			

			CDuiRect pos = m_pos;
			if ( m_showType & PICTURE_SHOWTYPE_NULL)
			{
				pos.right = pos.left + GetImageWidth();
				pos.bottom = pos.top + GetImageHeigh();
			}
			if ( m_showType & PICTURE_SHOWTYPE_FULL )
			{
				CDuiRect pos = m_pos;
			}

			if ( m_showType & PICTURE_SHOWTYPE_ORIGINAL )
			{
				pos.right = pos.left + GetImageWidth();
				pos.bottom = pos.top + GetImageHeigh();
			}


			if ( m_showType & PICTURE_SHOWTYPE_STRETCH )
			{
				if( !(m_pos.GetWidth() > GetImageWidth() && m_pos.GetHeight() > GetImageHeigh()))
					pos =	RectSizeWithConstantRatio(m_pos, CDuiSize( GetImageWidth(),GetImageHeigh()));
			}

			if ( m_showType & PICTURE_SHOWTYPE_CENTER ) //æ”÷–œ‘ æ
			{
				DWORD dwTempWidth = pos.GetWidth();
				DWORD dwTempHeight = pos.GetHeight();
				DWORD dwHCenter = (m_pos.GetHeight() - pos.GetHeight()) / 2;
				DWORD dwVCenter = (m_pos.GetWidth() - pos.GetWidth() ) /  2;
				pos.top = m_pos.top + dwHCenter;
				pos.bottom = pos.top + dwTempHeight;
				pos.left = m_pos.left + dwVCenter;
				pos.right = pos.left + dwTempWidth ;
			}
			m_gifImage.Draw(hDC, pos,m_gifImage.GetCurFramePos());
		}
	}

	LRESULT OnDuiTimer(INT nMsg, IDuiControlCtrl* tToCtrl, IDuiControlCtrl* tCtrl, WPARAM wParam, LPARAM lParam, LPVOID , BOOL& bHandle)
	{
		if ( wParam == 2008)
		{
			bHandle = FALSE;
			KillTimer(2008);
			m_gifImage.ActiveNextFrame();
			Invalidate();
			SetTimer( 2008, m_gifImage.GetFrameDelay(m_gifImage.GetCurFramePos()));
		}
		return 0;
	}

	LRESULT OnInitialize()
	{
		if ( m_gifImage.IsAnimatedGif())
		{
			SetTimer( 2008, m_gifImage.GetFrameDelay(0));
		}

		return 0;
	}

private:
	CDuiGifImage m_gifImage;
	
};

};