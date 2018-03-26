/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,3th,2014

Module Name:

	dsTools.h 

Abstract: 

*/
/************************************************************************/


#ifndef __DSTOOLS_H__
#define __DSTOOLS_H__

#include <GdiPlus.h>
using namespace Gdiplus;

#include "dsLayoutInfo.h"

namespace dsTools{

	inline RectF GdiplusRectFromGdiRect(__in LPRECT lprc){
		return RectF(lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top);
	}

	template<class T>
	inline T Exchange2Objects(T &_value1, T &_value2){//返回第一个 值
		T temp = _value1;
		_value1 = _value2;
		_value2 = temp;
		return temp;
	}

	inline BOOL UpdateLayoutRect(__in LPRECT lprcBase, __in LPRECT lprcElement, __in LPRECT lprcCoordMark, __out LPRECT lprcAfter){
		if(NULL == lprcBase || NULL == lprcElement || NULL == lprcCoordMark || NULL == lprcAfter)
			return FALSE;
		
		//::CopyRect(lprcAfter, lprcElement);

		BOOL bNeedEnumChild = FALSE;

		if(COORDMARK_BOTTOMRIGHT == lprcCoordMark->left){
			lprcAfter->left = lprcBase->right - lprcElement->left;
			bNeedEnumChild = TRUE;}
		else
			lprcAfter->left = lprcBase->left + lprcElement->left;

		if(COORDMARK_BOTTOMRIGHT == lprcCoordMark->right){
			lprcAfter->right = lprcBase->right - lprcElement->right;
			bNeedEnumChild = TRUE;}
		else
			lprcAfter->right = lprcBase->left + lprcElement->right;

		if(COORDMARK_BOTTOMRIGHT == lprcCoordMark->top){
			lprcAfter->top = lprcBase->bottom - lprcElement->top;
			bNeedEnumChild = TRUE;}
		else
			lprcAfter->top = lprcBase->top + lprcElement->top;

		if(COORDMARK_BOTTOMRIGHT == lprcCoordMark->bottom){
			lprcAfter->bottom = lprcBase->bottom - lprcElement->bottom;
			bNeedEnumChild = TRUE;}
		else
			lprcAfter->bottom = lprcBase->top + lprcElement->bottom;

		if(!::EqualRect(lprcAfter, lprcElement))
			return TRUE;

		return bNeedEnumChild;
		
	}

	inline long GetApproximation(float f){
		if(f -(long)f > 0.5)
			return (long)f + 1;
		else
			return (long)f;
	}

};




#endif /*__DSTOOLS_H__*/