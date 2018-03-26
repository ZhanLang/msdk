#pragma once
#include <DuiCtrl/IDuiPictureCtrl.h>
#include "DuiControlCtrl.h"
namespace DuiKit{;

class CDuiPictureCtrl : 
	public IDuiPictureCtrl ,
	public CDuiControlCtrl
{
public:
	CDuiPictureCtrl():
	  m_pImage(NULL)
	 {
		  m_showType = PICTURE_SHOWTYPE_CENTER | PICTURE_SHOWTYPE_STRETCH | PICTURE_SHOWTYPE_ORIGINAL;
	 }
	~CDuiPictureCtrl(){}
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiPictureCtrl, CTRL_PICTURE)
		DUI_DEFINE_INTERFACE(IDuiPictureCtrl, IIDuiPictureCtrl)
		DUI_DEFINE_INTERFACE(IDuiControlCtrl, IIDuiControlCtrl)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiGifCtrl)
		DUI_SETATTRIBUTE_STRING_FUNCTION(image,	SetImage)
		DUI_SETATTRIBUTE_SUB_CLASS(CDuiControlCtrl);
	DUI_END_SETATTRIBUTE;

	HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		if ( CDuiControlCtrl::DoCreate(pParent, pCore,pBuilder) == S_OK)
		{
			return S_OK;
		}

		return S_OK;
	}

	virtual VOID SetImage(LPCWSTR lpszFile) 
	{
		if ( lpszFile )
		{
			m_strImage = lpszFile;
		}

		if ( m_pImage )
			m_pImage->DeleteThis();

		m_pImage = ::CDuiControlCtrl::GetImage(m_strImage);
		if (m_pImage && !m_pImage->GetHandle())
		{
			m_pImage->Create();
		}
	}

	virtual LPCWSTR GetImage()
	{
		return m_strImage;
	}

	virtual DWORD GetImageWidth()
	{
		if ( m_pImage )
		{
			return m_pImage->GetWidth();
		}

		return 0;
	}

	virtual DWORD GetImageHeigh() 
	{
		if ( m_pImage )
		{
			return m_pImage->GetHeight();
		}

		return 0;
	}

	virtual VOID SetShowTyte(INT showType)
	{

	}

	virtual INT GetShowType()
	{
		return m_showType;
	}



	CDuiRect RectSizeWithConstantRatio( CDuiRect rcScreen, CDuiSize sizePicture)  
	{  
		CDuiRect rect(rcScreen);  
		double dWidth = rcScreen.GetWidth();  
		double dHeight = rcScreen.GetHeight();  
		double dAspectRatio = dWidth/dHeight;  
		double dPictureWidth = sizePicture.cx;  
		double dPictureHeight = sizePicture.cy;  
		double dPictureAspectRatio = dPictureWidth/dPictureHeight;  
		//If the aspect ratios are the same then the screen rectangle  
		// will do, otherwise we need to calculate the new rectangle  
		if (dPictureAspectRatio > dAspectRatio)  
		{  
			int nNewHeight = (int)(dWidth/dPictureWidth*dPictureHeight);  
			int nCenteringFactor = (rcScreen.GetHeight() - nNewHeight) / 2;  
			rect.SetRect( /*rcScreen.left*/0,  
				nCenteringFactor,  
				(int)dWidth + /*rcScreen.left*/0,  
				nNewHeight + nCenteringFactor);  
		}  
		else if (dPictureAspectRatio < dAspectRatio)  
		{  
			int nNewWidth = (int)(dHeight/dPictureHeight*dPictureWidth);  
			int nCenteringFactor = (rcScreen.GetWidth() - nNewWidth) / 2;  
			rect.SetRect( nCenteringFactor,  
				rcScreen.top,  
				nNewWidth + nCenteringFactor,  
				(int)(dHeight) + rcScreen.top);  
		}  
		return rect;  
	};  

	virtual VOID PaintStatusImage(HDC hDC, IDuiImage* pImage)
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

		if ( m_showType & PICTURE_SHOWTYPE_CENTER ) //¾ÓÖÐÏÔÊ¾
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

		CDuiRenderEngine::DrawImage( 
			hDC, 
			m_pImage->GetHandle(), 
			pos,
			m_rcPaint,
			m_pImage->GetAffectRect(), 
			m_pImage->GetScale9(), 
			m_pImage->IsAlphaChannel(), 
			m_pImage->GetFade(), 
			m_pImage->GetHole(), 
			m_pImage->GetXtiled(), 
			m_pImage->GetYtiled());
	}

	virtual VOID PaintStatusImage(HDC hDC)
	{
		RASSERTV(m_pImage);
		if ( !m_pImage->GetHandle() )
		{
			m_pImage->Create();
		}

		PaintStatusImage(hDC, m_pImage);
	}

public:
	CDuiString m_strImage;
	IDuiImage* m_pImage;

	INT m_showType;
};

};