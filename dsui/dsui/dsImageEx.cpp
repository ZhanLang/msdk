// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsImageEx.h"
#include <process.h>
#include "dsUIBase.h"


dsImageEx::dsImageEx(LPCTSTR sResourceType, LPCTSTR sResource)
{
	Initialize();

	if (Load(sResourceType, sResource))
	{

	   nativeImage = NULL;
	   
	   lastResult = DllExports::GdipLoadImageFromStreamICM(m_pStream, &nativeImage);
	  
	   TestForAnimatedGIF();
    }
}


dsImageEx::dsImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement) : Image(filename, useEmbeddedColorManagement)
{
	Initialize();

	m_bIsInitialized = true;

	TestForAnimatedGIF();
}


dsImageEx::~dsImageEx()
{
	Destroy();
}


bool dsImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		void* pData = GlobalLock(hGlobal);
		if (pData)
			memcpy(pData, pBuff, nSize);
		
		GlobalUnlock(hGlobal);

		if (CreateStreamOnHGlobal(hGlobal, TRUE, &m_pStream) == S_OK)
			bResult = true;

	}


	return bResult;
}


bool dsImageEx::GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	// Find the resource

	hResInfo = FindResource(m_hInst , lpName, lpType);
	if (hResInfo == NULL) 
	{
		DWORD dwErr = GetLastError();
		return false;
	}

	// Load the resource
	hRes = LoadResource(m_hInst , hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(m_hInst , hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(m_hInst , hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}

bool dsImageEx::Load(CString sResourceType, CString sResource)
{
	bool bResult = false;


	byte*	pBuff = NULL;
	int		nSize = 0;
	if (GetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new byte[nSize];

			if (GetResource(sResource, sResourceType.GetBuffer(0), pBuff, nSize))
			{
				if (LoadFromBuffer(pBuff, nSize))
				{

					bResult = true;
				}
			}

			delete [] pBuff;
		}
	}


	m_bIsInitialized = bResult;

	return bResult;
}

CSize dsImageEx::GetSize()
{
	return CSize(GetWidth(), GetHeight());
}

bool dsImageEx::TestForAnimatedGIF()
{
   UINT count = 0;
   count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);

   GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   
   delete  pDimensionIDs;

   return m_nFrameCount > 1;
}

void dsImageEx::Initialize()
{
	m_nFrameCount = 0;
	m_pStream = NULL;
	lastResult = InvalidParameter;
	m_bIsInitialized = false;
	m_pPropertyItem = NULL;
	
#ifdef INDIGO_CTRL_PROJECT
	m_hInst = _Module.GetResourceInstance();
#else
	//m_hInst = _Module.m_hInst;//AfxGetResourceHandle();
#endif
}

void dsImageEx::Destroy()
{
	free(m_pPropertyItem);
	m_pPropertyItem = NULL;
	if (m_pStream)
		m_pStream->Release();
}




dsGifAnimate::~dsGifAnimate()
{
	Destroy();
}

bool dsGifAnimate::DrawFrameGIF()
{


	::WaitForSingleObject(m_hPause, INFINITE);

	GUID   pageGuid = FrameDimensionTime;

	

	long hmWidth = m_pimg->GetWidth();
	long hmHeight = m_pimg->GetHeight();

	
	m_pimg->SelectActiveFrame(&pageGuid, m_nFramePos++);

	if(GA_LINK_HWND == m_nLinkFlag)
	{
		HDC hDC = GetDC(m_hWnd);

		if (hDC)
		{
			Graphics graphics(hDC);
			graphics.DrawImage(this->m_pimg, m_pt.x, m_pt.y, hmWidth, hmHeight);
			ReleaseDC(m_hWnd, hDC);
		}
		
	}
	else if(GA_LINK_HDC == m_nLinkFlag)
	{
		Graphics graphics(m_hdc);
		graphics.DrawImage(this->m_pimg, m_pt.x, m_pt.y, hmWidth, hmHeight);
	}
	else if(GA_LINK_DSUI == m_nLinkFlag)
	{
		m_lock.Lock();

		m_pui->RedrawUI();
		//::UpdateWindow(m_pui->m_hwnd);

		m_lock.UnLock();

		//CRect rcVisible = m_pui->GetVisibleRect();

		//CDCHandle dc(::GetDC(m_pui->m_hwnd));

		//if (dc.m_hDC)
		//{
		//	Graphics graphics(dc);

		//	CRgn rgn;
		//	rgn.CreateRectRgnIndirect(rcVisible);
		//	::ExtSelectClipRgn(dc, rgn, RGN_AND);

		//	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
		//	//pgrah->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic); 
		//	//pgrah->SetSmoothingMode(SmoothingModeAntiAlias);
		//	graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

		//	graphics.DrawImage(this->m_pimg, 
		//		dsTools::GdiplusRectFromGdiRect(m_rc),
		//		m_rcSrc.left,
		//		m_rcSrc.top,
		//		m_rcSrc.Width(),
		//		m_rcSrc.Height(),
		//		UnitPixel);
		//	
		//	ReleaseDC(m_hWnd, dc);

		//	dc.SelectClipRgn(NULL);
		//}
	}	
	
	if (m_nFramePos == m_pimg->GetGifFrameCount())
		m_nFramePos = 0;

	long lPause = m_pimg->GetDelayProperty(m_nFramePos);

	if(lPause < 5)
		lPause = 100;
	else
		lPause *= 10;

	

	DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause);

	return dwErr == WAIT_OBJECT_0;
}



bool dsGifAnimate::StartAnimation(HWND hWnd, CPoint pt)
{

	if(IsAnimating())
		return FALSE;

	BeginAnimate();

	m_nLinkFlag = GA_LINK_HWND;
	m_hWnd = hWnd;
	m_pt = pt;

	

	if (!m_bIsInitialized)
	{
		ATLTRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (m_bGif)
	{
		if (m_hThread == NULL)
		{
		
			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
			
			if (!m_hThread)
			{
				ATLTRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
	} 

	return false;	

}

bool dsGifAnimate::StartAnimation(HDC hdc, CPoint pt )
{
	if(IsAnimating())
		return FALSE;

	BeginAnimate();

	m_nLinkFlag = GA_LINK_HDC;

	m_hdc = hdc;
	m_pt = pt;

	if (!m_bIsInitialized)
	{
		ATLTRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (m_bGif)
	{
		if (m_hThread == NULL)
		{

			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);

			if (!m_hThread)
			{
				ATLTRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
	} 

	return false;	
}

bool dsGifAnimate::StartAnimation(dsUIBase *pui, CRect rc, CRect rcSrc)
{

	if(IsAnimating())
		return FALSE;

	BeginAnimate();

	m_nLinkFlag = GA_LINK_DSUI;
	m_pui = pui;
	m_rc = rc;
	m_rcSrc = rcSrc;
	m_rcSrc.right = m_rcSrc.left + rc.Width();
	m_rcSrc.bottom = m_rcSrc.top + rc.Height();



	if (!m_bIsInitialized)
	{
		ATLTRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (m_bGif)
	{
		if (m_hThread == NULL)
		{

			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);

			if (!m_hThread)
			{
				ATLTRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
	} 

	return false;	

}
void dsGifAnimate::StopAnimate()
{
	EndAnimate();
}

UINT WINAPI dsGifAnimate::_ThreadAnimationProc(LPVOID pParam)
{
	ATLASSERT(pParam);
	dsGifAnimate *pGif = reinterpret_cast<dsGifAnimate *> (pParam);
	pGif->ThreadAnimation();

	return 0;
}

void dsGifAnimate::ThreadAnimation()
{
	m_nFramePos = 0;

	bool bExit = false;
	while (bExit == false)
	{
		bExit = DrawFrameGIF();
	}
}

void dsGifAnimate::UpdatePoint( POINT pt )
{
	m_pt = pt;
}

void dsGifAnimate::UpdateRect( CRect rc, CRect rcSrc )
{
	m_rc = rc;
	m_rcSrc = rcSrc;
	m_rcSrc.right = m_rcSrc.left + rc.Width();
	m_rcSrc.bottom = m_rcSrc.top + rc.Height();
}
void dsGifAnimate::SetPause( bool bPause )
{
	if (!m_bGif)
		return;

	if (bPause && !m_bPause)
	{
		::ResetEvent(m_hPause);
	}
	else
	{

		if (m_bPause && !bPause)
		{
			::SetEvent(m_hPause);
		}
	}

	m_bPause = bPause;
}

void dsGifAnimate::Destroy()
{
	StopAnimate();
	
	m_hThread = NULL;
	m_hExitEvent = NULL;
	m_hPause = NULL;

}

BOOL dsGifAnimate::Attatch( dsImageEx *pimgGif )
{
	if(NULL == pimgGif)
		return FALSE;

	if(!pimgGif->IsInitialized() || !pimgGif->IsAnimatedGIF())
		return FALSE;

	m_pimg = pimgGif;
	m_bGif = TRUE;

	return m_bIsInitialized = TRUE;
}

void dsGifAnimate::BeginAnimate()
{
	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);

	m_bAnimating = TRUE;
}

void dsGifAnimate::EndAnimate()
{
	m_nLinkFlag = GA_LINK_NULL;

	if (m_hThread)
	{
		// If pause un pause
		SetPause(false);
		SetEvent(m_hExitEvent);
		WaitForSingleObject(m_hThread, INFINITE);
	}

	CloseHandle(m_hThread);
	CloseHandle(m_hExitEvent);
	CloseHandle(m_hPause);

}
