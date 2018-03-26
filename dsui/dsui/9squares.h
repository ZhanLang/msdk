/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	12,26th,2012

Module Name:

	9squares.h

Abstract: 各格序号如下

[0] [1] [2]
[3] [4] [5]
[6] [7] [8]


*/
/************************************************************************/

#ifndef __9SQUARES_H__
#define __9SQUARES_H__

#include "dsdef.h"


struct tagStrecthObject
{
	RECT rcDst;
	RECT rcSrc;
	int  nPaintMode;
};

typedef struct tag9SquaresStretchTable
{
	tagStrecthObject tables[9];
}NSST,*LPNSST;

class ds9SquaresStretch
{
public:
	static BOOL Analysis(__in LPRECT lprcDst, __in LPRECT lprcSrc, __in LPRECT lprc9SquaresInfo, __out LPNSST lpnsst)
	{
		if (NULL == lprcDst || NULL == lprcSrc || NULL == lprc9SquaresInfo || NULL == lpnsst)
		{
			return FALSE;
		}

		if(lprc9SquaresInfo->left == 0 && lprc9SquaresInfo->top == 0 && lprc9SquaresInfo->right == 0 && lprc9SquaresInfo->bottom == 0)
			return FALSE;

		int nDstWidth = lprcDst->right - lprcDst->left;
		if (nDstWidth < (lprc9SquaresInfo->left + lprc9SquaresInfo->right) )
		{
			return FALSE;
		}

		int nDstHeight = lprcDst->bottom - lprcDst->top;
		if (nDstHeight < (lprc9SquaresInfo->top + lprc9SquaresInfo->bottom))
		{
			return FALSE;
		}

		ZeroMemory(lpnsst, sizeof NSST);


		::SetRect(&(lpnsst->tables[0].rcDst),
			lprcDst->left, lprcDst->top, lprcDst->left + lprc9SquaresInfo->left, lprcDst->top + lprc9SquaresInfo->top);
		::SetRect(&(lpnsst->tables[0].rcSrc),
			lprcSrc->left, lprcSrc->top, lprcSrc->left + lprc9SquaresInfo->left, lprcSrc->top + lprc9SquaresInfo->top);
		lpnsst->tables[0].nPaintMode = DSUI_PAINTMODE_NORMAL;

		::SetRect(&(lpnsst->tables[1].rcDst),
			lpnsst->tables[0].rcDst.right, lprcDst->top, lprcDst->right - lprc9SquaresInfo->right, lprcDst->top + lprc9SquaresInfo->top);
		::SetRect(&(lpnsst->tables[1].rcSrc),
			lpnsst->tables[0].rcSrc.right, lprcSrc->top, lprcSrc->right - lprc9SquaresInfo->right, lprcSrc->top + lprc9SquaresInfo->top);
		lpnsst->tables[1].nPaintMode = DSUI_PAINTMODE_STRETCH;

		::SetRect(&(lpnsst->tables[2].rcDst),
			lprcDst->right - lprc9SquaresInfo->right, lprcDst->top, lprcDst->right, lprcDst->top + lprc9SquaresInfo->top);
		::SetRect(&(lpnsst->tables[2].rcSrc),
			lprcSrc->right - lprc9SquaresInfo->right, lprcSrc->top, lprcSrc->right, lprcSrc->top + lprc9SquaresInfo->top);
		lpnsst->tables[2].nPaintMode = DSUI_PAINTMODE_NORMAL;

		::SetRect(&(lpnsst->tables[3].rcDst),
			lprcDst->left, lpnsst->tables[0].rcDst.bottom, lpnsst->tables[0].rcDst.right, lprcDst->bottom - lprc9SquaresInfo->bottom);
		::SetRect(&(lpnsst->tables[3].rcSrc),
			lprcSrc->left, lpnsst->tables[0].rcSrc.bottom, lpnsst->tables[0].rcSrc.right, lprcSrc->bottom - lprc9SquaresInfo->bottom);
		lpnsst->tables[3].nPaintMode = DSUI_PAINTMODE_STRETCH;

		::SetRect(&(lpnsst->tables[4].rcDst),
			lpnsst->tables[1].rcDst.left, lpnsst->tables[3].rcDst.top, lpnsst->tables[1].rcDst.right, lpnsst->tables[3].rcDst.bottom);
		::SetRect(&(lpnsst->tables[4].rcSrc),
			lpnsst->tables[1].rcSrc.left, lpnsst->tables[3].rcSrc.top, lpnsst->tables[1].rcSrc.right, lpnsst->tables[3].rcSrc.bottom);
		lpnsst->tables[4].nPaintMode = DSUI_PAINTMODE_STRETCH;

		::SetRect(&(lpnsst->tables[5].rcDst),
			lpnsst->tables[2].rcDst.left, lpnsst->tables[4].rcDst.top, lpnsst->tables[2].rcDst.right, lpnsst->tables[4].rcDst.bottom);
		::SetRect(&(lpnsst->tables[5].rcSrc),
			lpnsst->tables[2].rcSrc.left, lpnsst->tables[4].rcSrc.top, lpnsst->tables[2].rcSrc.right, lpnsst->tables[4].rcSrc.bottom);
		lpnsst->tables[5].nPaintMode = DSUI_PAINTMODE_STRETCH;

		::SetRect(&(lpnsst->tables[6].rcDst),
			lpnsst->tables[3].rcDst.left, lprcDst->bottom - lprc9SquaresInfo->bottom,  lpnsst->tables[3].rcDst.right, lprcDst->bottom);
		::SetRect(&(lpnsst->tables[6].rcSrc),
			lpnsst->tables[3].rcSrc.left, lprcSrc->bottom - lprc9SquaresInfo->bottom,  lpnsst->tables[3].rcSrc.right, lprcSrc->bottom);
		lpnsst->tables[6].nPaintMode = DSUI_PAINTMODE_NORMAL;

		::SetRect(&(lpnsst->tables[7].rcDst),
			lpnsst->tables[4].rcDst.left, lpnsst->tables[6].rcDst.top, lpnsst->tables[4].rcDst.right, lprcDst->bottom);
		::SetRect(&(lpnsst->tables[7].rcSrc),
			lpnsst->tables[4].rcSrc.left, lpnsst->tables[6].rcSrc.top, lpnsst->tables[4].rcSrc.right, lprcSrc->bottom);
		lpnsst->tables[7].nPaintMode = DSUI_PAINTMODE_STRETCH;

		::SetRect(&(lpnsst->tables[8].rcDst),
			lpnsst->tables[5].rcDst.left, lpnsst->tables[7].rcDst.top, lpnsst->tables[5].rcDst.right, lprcDst->bottom);
		::SetRect(&(lpnsst->tables[8].rcSrc),
			lpnsst->tables[5].rcSrc.left, lpnsst->tables[7].rcSrc.top, lpnsst->tables[5].rcSrc.right, lprcSrc->bottom);
		lpnsst->tables[8].nPaintMode = DSUI_PAINTMODE_NORMAL;

		return TRUE;
	}
};

#endif /*__9SQUARES_H__*/