/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	3,17th,2014

Module Name:

	dsImageInfo.h 

Abstract:


*/
/************************************************************************/


#ifndef __DSIMAGEINFO_H__
#define __DSIMAGEINFO_H__


#pragma comment(lib,"GdiPlus.lib")

#include <GdiPlus.h>
using namespace Gdiplus;

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

class dsImageInfo{
public:
	Image *m_pimg;
	UINT			m_nFrameCount;
	UINT m_uPropertySize;
	PropertyItem*	m_pPropertyItem;

public:

	dsImageInfo():m_pimg(NULL), m_nFrameCount(0), m_pPropertyItem(NULL),m_uPropertySize(0)
	{

	}

	dsImageInfo(const dsImageInfo &dsii){
		m_pimg = dsii.m_pimg;
		
		if(NULL != m_pimg){
			m_nFrameCount = dsii.m_nFrameCount;
			m_uPropertySize = dsii.m_uPropertySize;

			if(m_uPropertySize > 1)
			{
				m_pPropertyItem = new PropertyItem[m_uPropertySize];
				memcpy_s(m_pPropertyItem, m_uPropertySize * sizeof PropertyItem, dsii.m_pPropertyItem, m_uPropertySize * sizeof PropertyItem);
			}
		}
	}

	dsImageInfo(Image *pimg){
		Attach(pimg);
	}

	~dsImageInfo(){
		m_pimg = NULL;
		if(NULL != m_pPropertyItem)
			delete m_pPropertyItem;
		m_pPropertyItem = NULL;
	}


	BOOL Attach(Image *pimg){

		if(NULL == pimg)
			return FALSE;

		UINT count = 0;
		count = pimg->GetFrameDimensionsCount();
		GUID* pDimensionIDs = new GUID[count];

		// Get the list of frame dimensions from the Image object.
		pimg->GetFrameDimensionsList(pDimensionIDs, count);

		// Get the number of frames in the first dimension.
		m_nFrameCount = pimg->GetFrameCount(&pDimensionIDs[0]);

		// Assume that the image has a property item of type PropertyItemEquipMake.
		// Get the size of that property item.

		m_uPropertySize = pimg->GetPropertyItemSize(PropertyTagFrameDelay);

		if(m_uPropertySize > 1)
		{
			// Allocate a buffer to receive the property item.
			//m_pPropertyItem = (PropertyItem*) malloc(m_uPropertySize);
			m_pPropertyItem = new PropertyItem[m_uPropertySize];
			pimg->GetPropertyItem(PropertyTagFrameDelay, m_uPropertySize, m_pPropertyItem);
		}

		return TRUE;

	}

	BOOL IsGif(){
		return m_nFrameCount > 1;}
};


#endif /*__DSIMAGEINFO_H__*/