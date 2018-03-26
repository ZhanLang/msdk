#pragma once
#include <GdiPlus.h>
namespace DuiKit{;
class CDuiRenderClip{
public:
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;

	~CDuiRenderClip()
	{
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
		ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
		::SelectClipRgn(hDC, hOldRgn);
		::DeleteObject(hOldRgn);
		::DeleteObject(hRgn);
	}


	

	static void GenerateClip(HDC hDC, RECT rc, CDuiRenderClip& clip)
	{
		RECT rcClip = { 0 };
		::GetClipBox(hDC, &rcClip);
		clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		clip.hRgn = ::CreateRectRgnIndirect(&rc);
		::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
		clip.hDC = hDC;
		clip.rcItem = rc;
	}

	static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CDuiRenderClip& clip)
	{
		RECT rcClip = { 0 };
		::GetClipBox(hDC, &rcClip);
		clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		clip.hRgn = ::CreateRectRgnIndirect(&rc);
		HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
		::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
		::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
		clip.hDC = hDC;
		clip.rcItem = rc;
		::DeleteObject(hRgnItem);
	}


	static void UseOldClipBegin(HDC hDC, CDuiRenderClip& clip)
	{
		::SelectClipRgn(hDC, clip.hOldRgn);
	}

	static void UseOldClipEnd(HDC hDC, CDuiRenderClip& clip)
	{
		::SelectClipRgn(hDC, clip.hRgn);
	}
};

};