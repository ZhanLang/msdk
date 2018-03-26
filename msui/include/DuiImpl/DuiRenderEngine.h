#pragma once
#include <GdiPlus.h>
#include <DuiCore/IDuiImage.h>
#include <DuiCore/IDuiFont.h>

namespace DuiKit{;
static const float OneThird = 1.0f / 3;
class CDuiRenderEngine
{
public:
	
	static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L) {
		const float
			R = (float)GetRValue(ARGB),
			G = (float)GetGValue(ARGB),
			B = (float)GetBValue(ARGB),
			nR = (R<0?0:(R>255?255:R))/255,
			nG = (G<0?0:(G>255?255:G))/255,
			nB = (B<0?0:(B>255?255:B))/255,
			m = min(min(nR,nG),nB),
			M = max(max(nR,nG),nB);
		*L = (m + M)/2;
		if (M==m) *H = *S = 0;
		else {
			const float
				f = (nR==m)?(nG-nB):((nG==m)?(nB-nR):(nR-nG)),
				i = (nR==m)?3.0f:((nG==m)?5.0f:1.0f);
			*H = (i-f/(M-m));
			if (*H>=6) *H-=6;
			*H*=60;
			*S = (2*(*L)<=1)?((M-m)/(M+m)):((M-m)/(2-M-m));
		}
	}

	static void HSLtoRGB(DWORD* ARGB, float H, float S, float L) {
		const float
			q = 2*L<1?L*(1+S):(L+S-L*S),
			p = 2*L-q,
			h = H/360,
			tr = h + OneThird,
			tg = h,
			tb = h - OneThird,
			ntr = tr<0?tr+1:(tr>1?tr-1:tr),
			ntg = tg<0?tg+1:(tg>1?tg-1:tg),
			ntb = tb<0?tb+1:(tb>1?tb-1:tb),
			R = 255*(6*ntr<1?p+(q-p)*6*ntr:(2*ntr<1?q:(3*ntr<2?p+(q-p)*6*(2.0f*OneThird-ntr):p))),
			G = 255*(6*ntg<1?p+(q-p)*6*ntg:(2*ntg<1?q:(3*ntg<2?p+(q-p)*6*(2.0f*OneThird-ntg):p))),
			B = 255*(6*ntb<1?p+(q-p)*6*ntb:(2*ntb<1?q:(3*ntb<2?p+(q-p)*6*(2.0f*OneThird-ntb):p)));
		*ARGB &= 0xFF000000;
		*ARGB |= RGB( (BYTE)(R<0?0:(R>255?255:R)), (BYTE)(G<0?0:(G>255?255:G)), (BYTE)(B<0?0:(B>255?255:B)) );
	}

	 static DWORD AdjustColor(DWORD dwColor, short H, short S, short L)
	 {
		 if( H == 180 && S == 100 && L == 100 ) return dwColor;
		 float fH, fS, fL;
		 float S1 = S / 100.0f;
		 float L1 = L / 100.0f;
		 RGBtoHSL(dwColor, &fH, &fS, &fL);
		 fH += (H - 180);
		 fH = fH > 0 ? fH : fH + 360; 
		 fS *= S1;
		 fL *= L1;
		 HSLtoRGB(&dwColor, fH, fS, fL);
		 return dwColor;
	 }

	 static void DrawColor(HDC hDC, const RECT& rc, DWORD color)
	 {
		 if( color <= 0x00FFFFFF ) return;
		 if( color >= 0xFF000000 )
		 {
			 ::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
			 ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		 }
		 else
		 {
			 // Create a new 32bpp bitmap with room for an alpha channel
			 BITMAPINFO bmi = { 0 };
			 bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			 bmi.bmiHeader.biWidth = 1;
			 bmi.bmiHeader.biHeight = 1;
			 bmi.bmiHeader.biPlanes = 1;
			 bmi.bmiHeader.biBitCount = 32;
			 bmi.bmiHeader.biCompression = BI_RGB;
			 bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
			 LPDWORD pDest = NULL;
			 HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
			 if( !hBitmap ) return;

			 *pDest = color;

			 RECT rcBmpPart = {0, 0, 1, 1};
			 RECT rcCorners = {0};
			 DrawImage(hDC, hBitmap, rc, rc, rcBmpPart, rcCorners, true, 255);
			 ::DeleteObject(hBitmap);
		 }
	 }

	 static VOID DrawImage(HDC hDC, IDuiImage* tImage, const CDuiRect& pos, const CDuiRect& rcPaint)
	 {
		 RASSERTV(tImage);
		 CDuiRenderEngine::DrawImage(
			 hDC, 
			 tImage->GetHandle(), 
			 pos, 
			 rcPaint, 
			 tImage->GetAffectRect(), 
			 tImage->GetScale9(), 
			 tImage->IsAlphaChannel(), 
			 tImage->GetFade(), 
			 tImage->GetHole(), 
			 tImage->GetXtiled(), 
			 tImage->GetYtiled()
			 );
	 }

	static VOID DrawText(HDC hDC, RECT& rc, LPCTSTR pstrText,DWORD dwTextColor, IDuiFont *tFont, UINT uStyle)
	{
		 ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		 RASSERTV(pstrText && wcslen(pstrText));

		 ::SetBkMode(hDC, TRANSPARENT);
		 ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

		 HFONT hOldFont = (HFONT)::SelectObject(hDC, tFont->GetHandle());
		 ::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
		 ::SelectObject(hDC, hOldFont);
	}

	static void DrawImage(HDC hDC ,
		HBITMAP hBitmap, 
		const RECT& rc, 
		const RECT& rcPaint,
		const RECT& rcBmpPart, 
		const RECT& rcCorners, 
		BOOL alphaChannel, 
		BYTE uFade = 255, 
		 BOOL hole = FALSE, BOOL xtiled = FALSE, BOOL ytiled = FALSE)
	 {
		 ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

		 typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
		 static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

		 if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
		 if( hBitmap == NULL ) return;

		 HDC hCloneDC = ::CreateCompatibleDC(hDC);
		 HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
		 ::SetStretchBltMode(hDC, HALFTONE);

		 RECT rcTemp = {0};
		 RECT rcDest = {0};
		 if( lpAlphaBlend && (alphaChannel || uFade < 255) ) {
			 BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
			 // middle
			 if( !hole ) {
				 rcDest.left = rc.left + rcCorners.left;
				 rcDest.top = rc.top + rcCorners.top;
				 rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				 rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 if( !xtiled && !ytiled ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							 rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
							 rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
					 }
					 else if( xtiled && ytiled ) {
						 LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						 LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						 int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						 int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						 for( int j = 0; j < iTimesY; ++j ) {
							 LONG lDestTop = rcDest.top + lHeight * j;
							 LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							 LONG lDrawHeight = lHeight;
							 if( lDestBottom > rcDest.bottom ) {
								 lDrawHeight -= lDestBottom - rcDest.bottom;
								 lDestBottom = rcDest.bottom;
							 }
							 for( int i = 0; i < iTimesX; ++i ) {
								 LONG lDestLeft = rcDest.left + lWidth * i;
								 LONG lDestRight = rcDest.left + lWidth * (i + 1);
								 LONG lDrawWidth = lWidth;
								 if( lDestRight > rcDest.right ) {
									 lDrawWidth -= lDestRight - rcDest.right;
									 lDestRight = rcDest.right;
								 }
								 lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
									 lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
									 rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
							 }
						 }
					 }
					 else if( xtiled ) {
						 LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						 int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						 for( int i = 0; i < iTimes; ++i ) {
							 LONG lDestLeft = rcDest.left + lWidth * i;
							 LONG lDestRight = rcDest.left + lWidth * (i + 1);
							 LONG lDrawWidth = lWidth;
							 if( lDestRight > rcDest.right ) {
								 lDrawWidth -= lDestRight - rcDest.right;
								 lDestRight = rcDest.right;
							 }
							 lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
								 hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								 lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
						 }
					 }
					 else { // ytiled
						 LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						 int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						 for( int i = 0; i < iTimes; ++i ) {
							 LONG lDestTop = rcDest.top + lHeight * i;
							 LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							 LONG lDrawHeight = lHeight;
							 if( lDestBottom > rcDest.bottom ) {
								 lDrawHeight -= lDestBottom - rcDest.bottom;
								 lDestBottom = rcDest.bottom;
							 }
							 lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
								 hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								 rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
						 }
					 }
				 }
			 }

			 // left-top
			 if( rcCorners.left > 0 && rcCorners.top > 0 ) {
				 rcDest.left = rc.left;
				 rcDest.top = rc.top;
				 rcDest.right = rcCorners.left;
				 rcDest.bottom = rcCorners.top;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
				 }
			 }
			 // top
			 if( rcCorners.top > 0 ) {
				 rcDest.left = rc.left + rcCorners.left;
				 rcDest.top = rc.top;
				 rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				 rcDest.bottom = rcCorners.top;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
						 rcCorners.left - rcCorners.right, rcCorners.top, bf);
				 }
			 }
			 // right-top
			 if( rcCorners.right > 0 && rcCorners.top > 0 ) {
				 rcDest.left = rc.right - rcCorners.right;
				 rcDest.top = rc.top;
				 rcDest.right = rcCorners.right;
				 rcDest.bottom = rcCorners.top;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
				 }
			 }
			 // left
			 if( rcCorners.left > 0 ) {
				 rcDest.left = rc.left;
				 rcDest.top = rc.top + rcCorners.top;
				 rcDest.right = rcCorners.left;
				 rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
						 rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				 }
			 }
			 // right
			 if( rcCorners.right > 0 ) {
				 rcDest.left = rc.right - rcCorners.right;
				 rcDest.top = rc.top + rcCorners.top;
				 rcDest.right = rcCorners.right;
				 rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
						 rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				 }
			 }
			 // left-bottom
			 if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
				 rcDest.left = rc.left;
				 rcDest.top = rc.bottom - rcCorners.bottom;
				 rcDest.right = rcCorners.left;
				 rcDest.bottom = rcCorners.bottom;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
				 }
			 }
			 // bottom
			 if( rcCorners.bottom > 0 ) {
				 rcDest.left = rc.left + rcCorners.left;
				 rcDest.top = rc.bottom - rcCorners.bottom;
				 rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				 rcDest.bottom = rcCorners.bottom;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
						 rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
				 }
			 }
			 // right-bottom
			 if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
				 rcDest.left = rc.right - rcCorners.right;
				 rcDest.top = rc.bottom - rcCorners.bottom;
				 rcDest.right = rcCorners.right;
				 rcDest.bottom = rcCorners.bottom;
				 rcDest.right += rcDest.left;
				 rcDest.bottom += rcDest.top;
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					 rcDest.right -= rcDest.left;
					 rcDest.bottom -= rcDest.top;
					 lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						 rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
						 rcCorners.bottom, bf);
				 }
			 }
		 }
		 else 
		 {
			 if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
				 && rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
				 && rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
			 {
				 if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
					 ::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
						 hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
				 }
			 }
			 else
			 {
				 // middle
				 if( !hole ) {
					 rcDest.left = rc.left + rcCorners.left;
					 rcDest.top = rc.top + rcCorners.top;
					 rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					 rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 if( !xtiled && !ytiled ) {
							 rcDest.right -= rcDest.left;
							 rcDest.bottom -= rcDest.top;
							 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
								 rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								 rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
								 rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
						 }
						 else if( xtiled && ytiled ) {
							 LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							 LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							 int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							 int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							 for( int j = 0; j < iTimesY; ++j ) {
								 LONG lDestTop = rcDest.top + lHeight * j;
								 LONG lDestBottom = rcDest.top + lHeight * (j + 1);
								 LONG lDrawHeight = lHeight;
								 if( lDestBottom > rcDest.bottom ) {
									 lDrawHeight -= lDestBottom - rcDest.bottom;
									 lDestBottom = rcDest.bottom;
								 }
								 for( int i = 0; i < iTimesX; ++i ) {
									 LONG lDestLeft = rcDest.left + lWidth * i;
									 LONG lDestRight = rcDest.left + lWidth * (i + 1);
									 LONG lDrawWidth = lWidth;
									 if( lDestRight > rcDest.right ) {
										 lDrawWidth -= lDestRight - rcDest.right;
										 lDestRight = rcDest.right;
									 }
									 ::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
										 lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
										 rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
								 }
							 }
						 }
						 else if( xtiled ) {
							 LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							 int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							 for( int i = 0; i < iTimes; ++i ) {
								 LONG lDestLeft = rcDest.left + lWidth * i;
								 LONG lDestRight = rcDest.left + lWidth * (i + 1);
								 LONG lDrawWidth = lWidth;
								 if( lDestRight > rcDest.right ) {
									 lDrawWidth -= lDestRight - rcDest.right;
									 lDestRight = rcDest.right;
								 }
								 ::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
									 hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									 lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
							 }
						 }
						 else { // ytiled
							 LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							 int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							 for( int i = 0; i < iTimes; ++i ) {
								 LONG lDestTop = rcDest.top + lHeight * i;
								 LONG lDestBottom = rcDest.top + lHeight * (i + 1);
								 LONG lDrawHeight = lHeight;
								 if( lDestBottom > rcDest.bottom ) {
									 lDrawHeight -= lDestBottom - rcDest.bottom;
									 lDestBottom = rcDest.bottom;
								 }
								 ::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
									 hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									 rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
							 }
						 }
					 }
				 }

				 // left-top
				 if( rcCorners.left > 0 && rcCorners.top > 0 ) {
					 rcDest.left = rc.left;
					 rcDest.top = rc.top;
					 rcDest.right = rcCorners.left;
					 rcDest.bottom = rcCorners.top;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
					 }
				 }
				 // top
				 if( rcCorners.top > 0 ) {
					 rcDest.left = rc.left + rcCorners.left;
					 rcDest.top = rc.top;
					 rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					 rcDest.bottom = rcCorners.top;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
							 rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
					 }
				 }
				 // right-top
				 if( rcCorners.right > 0 && rcCorners.top > 0 ) {
					 rcDest.left = rc.right - rcCorners.right;
					 rcDest.top = rc.top;
					 rcDest.right = rcCorners.right;
					 rcDest.bottom = rcCorners.top;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
					 }
				 }
				 // left
				 if( rcCorners.left > 0 ) {
					 rcDest.left = rc.left;
					 rcDest.top = rc.top + rcCorners.top;
					 rcDest.right = rcCorners.left;
					 rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
							 rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					 }
				 }
				 // right
				 if( rcCorners.right > 0 ) {
					 rcDest.left = rc.right - rcCorners.right;
					 rcDest.top = rc.top + rcCorners.top;
					 rcDest.right = rcCorners.right;
					 rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
							 rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					 }
				 }
				 // left-bottom
				 if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
					 rcDest.left = rc.left;
					 rcDest.top = rc.bottom - rcCorners.bottom;
					 rcDest.right = rcCorners.left;
					 rcDest.bottom = rcCorners.bottom;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
					 }
				 }
				 // bottom
				 if( rcCorners.bottom > 0 ) {
					 rcDest.left = rc.left + rcCorners.left;
					 rcDest.top = rc.bottom - rcCorners.bottom;
					 rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					 rcDest.bottom = rcCorners.bottom;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
							 rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
					 }
				 }
				 // right-bottom
				 if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
					 rcDest.left = rc.right - rcCorners.right;
					 rcDest.top = rc.bottom - rcCorners.bottom;
					 rcDest.right = rcCorners.right;
					 rcDest.bottom = rcCorners.bottom;
					 rcDest.right += rcDest.left;
					 rcDest.bottom += rcDest.top;
					 if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						 rcDest.right -= rcDest.left;
						 rcDest.bottom -= rcDest.top;
						 ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							 rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
							 rcCorners.bottom, SRCCOPY);
					 }
				 }
			 }
		 }

		 ::SelectObject(hCloneDC, hOldBitmap);
		 ::DeleteDC(hCloneDC);
	 }

	 static VOID DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor)
	 {
		 ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		 HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
		 HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
		 ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
		 ::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
		 ::SelectObject(hDC, hOldPen);
		 ::DeleteObject(hPen);
	 }

	 static VOID DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
	 {
		 ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		 HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
		 HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
		 ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
		 ::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
		 ::SelectObject(hDC, hOldPen);
		 ::DeleteObject(hPen);
	 }

	 static VOID DrawLine( HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
	 {
		 ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

		 LOGPEN lg;
		 lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
		 lg.lopnStyle = nStyle;
		 lg.lopnWidth.x = nSize;
		 HPEN hPen = CreatePenIndirect(&lg);
		 HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
		 POINT ptTemp = { 0 };
		 ::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
		 ::LineTo(hDC, rc.right, rc.bottom);
		 ::SelectObject(hDC, hOldPen);
		 ::DeleteObject(hPen);
	 }

	 static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
		 int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
	 {
		 HDC hTempDC = ::CreateCompatibleDC(hDC);
		 if (NULL == hTempDC)
			 return FALSE;

		 //Creates Source DIB
		 LPBITMAPINFO lpbiSrc = NULL;
		 // Fill in the BITMAPINFOHEADER
		 lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		 if (lpbiSrc == NULL)
		 {
			 ::DeleteDC(hTempDC);
			 return FALSE;
		 }
		 lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		 lpbiSrc->bmiHeader.biWidth = dwWidth;
		 lpbiSrc->bmiHeader.biHeight = dwHeight;
		 lpbiSrc->bmiHeader.biPlanes = 1;
		 lpbiSrc->bmiHeader.biBitCount = 32;
		 lpbiSrc->bmiHeader.biCompression = BI_RGB;
		 lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
		 lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
		 lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
		 lpbiSrc->bmiHeader.biClrUsed = 0;
		 lpbiSrc->bmiHeader.biClrImportant = 0;

		 COLORREF* pSrcBits = NULL;
		 HBITMAP hSrcDib = CreateDIBSection (
			 hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
			 NULL, NULL);

		 if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
		 {
			 delete [] lpbiSrc;
			 ::DeleteDC(hTempDC);
			 return FALSE;
		 }

		 HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
		 ::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
		 ::SelectObject (hTempDC, hOldTempBmp);

		 //Creates Destination DIB
		 LPBITMAPINFO lpbiDest = NULL;
		 // Fill in the BITMAPINFOHEADER
		 lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		 if (lpbiDest == NULL)
		 {
			 delete [] lpbiSrc;
			 ::DeleteObject(hSrcDib);
			 ::DeleteDC(hTempDC);
			 return FALSE;
		 }

		 lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		 lpbiDest->bmiHeader.biWidth = dwWidth;
		 lpbiDest->bmiHeader.biHeight = dwHeight;
		 lpbiDest->bmiHeader.biPlanes = 1;
		 lpbiDest->bmiHeader.biBitCount = 32;
		 lpbiDest->bmiHeader.biCompression = BI_RGB;
		 lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
		 lpbiDest->bmiHeader.biXPelsPerMeter = 0;
		 lpbiDest->bmiHeader.biYPelsPerMeter = 0;
		 lpbiDest->bmiHeader.biClrUsed = 0;
		 lpbiDest->bmiHeader.biClrImportant = 0;

		 COLORREF* pDestBits = NULL;
		 HBITMAP hDestDib = CreateDIBSection (
			 hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
			 NULL, NULL);

		 if ((NULL == hDestDib) || (NULL == pDestBits))
		 {
			 delete [] lpbiSrc;
			 ::DeleteObject(hSrcDib);
			 ::DeleteDC(hTempDC);
			 return FALSE;
		 }

		 ::SelectObject (hTempDC, hDestDib);
		 ::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
		 ::SelectObject (hTempDC, hOldTempBmp);

		 double src_darken;
		 BYTE nAlpha;

		 for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
		 {
			 nAlpha = LOBYTE(*pSrcBits >> 24);
			 src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
			 if( src_darken < 0.0 ) src_darken = 0.0;
			 *pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
		 } //for

		 ::SelectObject (hTempDC, hDestDib);
		 ::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
		 ::SelectObject (hTempDC, hOldTempBmp);

		 delete [] lpbiDest;
		 ::DeleteObject(hDestDib);

		 delete [] lpbiSrc;
		 ::DeleteObject(hSrcDib);

		 ::DeleteDC(hTempDC);
		 return TRUE;
	 }

	 static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
	 {
		 return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
			 GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
			 GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

	 } 


	
};

};