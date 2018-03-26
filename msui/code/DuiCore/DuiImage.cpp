#include "StdAfx.h"
#include "DuiImage.h"
#include <DuiCore/IDuiStream.h>
#include <DuiCore/IDuiSkin.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GdiPlus.h>
#include <atlimage.h>



CDuiImage::CDuiImage(void) :
	m_HBITMAP(NULL),
	m_AlphaChannel(FALSE),
	m_dwWidth(0),
	m_dwHeight(0),
	m_Fade(0xFF),
	m_bHole(FALSE),
	m_bXtiled(FALSE),
	m_bYtiled(FALSE),
	m_bLoadFile(FALSE)
{

}


CDuiImage::~CDuiImage(void)
{
}

HRESULT CDuiImage::DoCreate(IDuiObject* Parent, IDuiCore* Core,IDuiBuilder*pBuilder)
{
	IDuiSkin* tSkin = (IDuiSkin*)Parent->QueryInterface(IIDuiSkin);
	RASSERT(tSkin, E_FAIL);
	SetDuiSkin(tSkin);
	m_pSkin->AddImage(this);
	//return Create();

	return S_OK;
}

VOID CDuiImage::SetName(LPCWSTR lpszName)
{
	if ( lpszName && wcslen(lpszName))
	{
		m_strName = lpszName;
	}
}

LPCWSTR CDuiImage::GetName()
{
	return m_strName;
}

BOOL CDuiImage::SetFile(LPCWSTR lpszFile)
{
	if ( lpszFile && wcslen(lpszFile))
	{
		m_strFile = lpszFile;
	}

	return TRUE;
}

LPCWSTR CDuiImage::GetFile()
{
	return m_strFile;
}

VOID CDuiImage::SetDuiSkin(IDuiSkin* Skin)
{
	if ( Skin )
	{
		m_pSkin = Skin;
	}

}
IDuiSkin* CDuiImage::GetDuiSkin()
{
	return m_pSkin;
}

VOID CDuiImage::SetAffectRect( CDuiRect AffectRect)
{
	m_AffectRect = AffectRect;
}

CDuiRect CDuiImage::GetAffectRect()
{
	return m_AffectRect;
}

VOID CDuiImage::SetMask(DWORD dwMask)
{
	m_dwMask = dwMask;
}

DWORD CDuiImage::GetMask()
{
	return m_dwMask;
}


BOOL CDuiImage::Create()
{
	RASSERT( m_pSkin , FALSE);

	if ( m_HBITMAP )
	{
		return TRUE;
	}

	IDuiByteStream* tStream = m_pSkin->GetStream(GetFile());
	if ( !tStream )
	{
		return FALSE;
	}

	ATL::CImage img;
	IStream *stream = NULL;
	HGLOBAL global = ::GlobalAlloc( GMEM_FIXED, tStream->GetBufferSize());
	if( global == NULL )
	{
		return FALSE;
	}

	

	if( !mem_to_global( tStream->GetData(), tStream->GetBufferSize(), global ) )
	{
		::GlobalFree( global );
		return FALSE;
	}
	
	if( ::CreateStreamOnHGlobal( global, FALSE, &stream ) != S_OK )
	{
		::GlobalFree( global );
		return FALSE;
	}

	

	img.Load(stream);
	stream->Release();
	stream = NULL;

	::GlobalFree( global );
	int x,y;
	x = img.GetWidth(); y = img.GetHeight();
	bool bAlphaCh = false;

	int nBPP = img.GetBPP();
	if (nBPP != 1 && nBPP != 4 && nBPP != 8 && nBPP != 16 && nBPP != 24 && nBPP != 32){
		return NULL;
	}

	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = x;
	bmi.bmiHeader.biHeight = -y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = x * y * 4;

	bool bAlphaChannel = false;
	LPBYTE pDest = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
	if( !hBitmap ) {
		return NULL;
	}

	for (int j = 0; j < y; j++)
	{
		for (int i = 0; i < x; i++)
		{
			DWORD *pDw = (DWORD*)(pDest + (j*x + i)*4);
			LPBYTE lpTmp = (LPBYTE)img.GetPixelAddress(i, j);
			CopyColor(pDw, lpTmp, img.GetBPP());
		}
	}

	for( int i = 0; i < x * y; i++ ) 
	{
		if( pDest[i*4 + 3] < 255 )
		{
			pDest[i*4] = (BYTE)(DWORD(pDest[i*4])*pDest[i*4 + 3]/255);
			pDest[i*4 + 1] = (BYTE)(DWORD(pDest[i*4 + 1])*pDest[i*4 + 3]/255);
			pDest[i*4 + 2] = (BYTE)(DWORD(pDest[i*4 + 2])*pDest[i*4 + 3]/255); 
			bAlphaCh = true;
		}

		if( *(DWORD*)(&pDest[i*4]) == m_dwMask ) {
			pDest[i*4] = (BYTE)0;
			pDest[i*4 + 1] = (BYTE)0;
			pDest[i*4 + 2] = (BYTE)0; 
			pDest[i*4 + 3] = (BYTE)0;
			bAlphaCh = true;
		}
	}

	m_AlphaChannel = bAlphaCh;
	m_HBITMAP = hBitmap;
	m_dwWidth = x;
	m_dwHeight = y;

	if( m_AffectRect.left == 0 && m_AffectRect.right == 0 && m_AffectRect.top == 0 && m_AffectRect.bottom == 0 ) {
		m_AffectRect.right = m_dwWidth;
		m_AffectRect.bottom = m_dwHeight;
	}

	if (m_AffectRect.right > m_dwWidth) 
		m_AffectRect.right = m_dwWidth;


	if (m_AffectRect.bottom > m_dwHeight) m_AffectRect.bottom = m_dwHeight;

	return TRUE;
}

HBITMAP CDuiImage::GetHandle()
{
	return m_HBITMAP;
}

DWORD CDuiImage::GetHeight()
{
	return m_dwHeight;
}

DWORD CDuiImage::GetWidth()
{
	return m_dwWidth;
}

bool CDuiImage::mem_to_global( const void *buf, size_t size, HGLOBAL global )
{
	void *dest = ::GlobalLock( global );
	if( dest == NULL )
	{
		return false;
	}
	memcpy( dest, buf, size );
	::GlobalUnlock( global );
	return true;
}

void CDuiImage::CopyColor(DWORD* pDest, LPBYTE pSrc, int nBpp)
{
	LPBYTE pByte = (LPBYTE)pDest;

	if (nBpp == 32)
	{
		*pDest = *((DWORD*)pSrc);
	}
	else if (nBpp == 24)
	{
		pByte[0] = pSrc[0];
		pByte[1] = pSrc[1];
		pByte[2] = pSrc[2];
		pByte[3] = 255;
	}
}

BOOL CDuiImage::IsAlphaChannel()
{
	return m_AlphaChannel;
}


VOID CDuiImage::SetScale9(CDuiRect Scale9Rect)
{
	m_Scale9Rect = Scale9Rect;
}

CDuiRect CDuiImage::GetScale9()
{
	return m_Scale9Rect;
}

//设置图片的透明度
VOID CDuiImage::SetFade(BYTE fade)
{
	m_Fade = fade;
}


BYTE CDuiImage::GetFade()
{
	return m_Fade;
}

//属性是指定scale9绘制时要不要绘制中间部分
VOID CDuiImage::SetHole(BOOL bHole)
{
	m_bHole = bHole;
}

BOOL CDuiImage::GetHole()
{
	return m_bHole;
}

//属性设置成true就是指定图片在x轴不要拉伸而是平铺
VOID CDuiImage::SetXtiled(BOOL xtiled)
{
	m_bXtiled = xtiled;
}

BOOL CDuiImage::GetXtiled()
{
	return m_bXtiled;
}

//属性设置成true就是指定图片在y轴不要拉伸而是平铺
VOID CDuiImage::SetYtiled(BOOL ytiled)
{
	m_bYtiled = ytiled;
}

BOOL CDuiImage::GetYtiled()
{
	return m_bYtiled;
}