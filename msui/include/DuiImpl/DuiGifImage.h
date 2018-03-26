#pragma once
#include <comdef.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
class CDuiGifImage
{
public:
	CDuiGifImage(void)
	{
		Gdiplus::GdiplusStartup( &m_gdiplusToken,&m_gdiplusStartupInput, NULL);
		m_pImage = NULL;
		m_nFrameCnt = 0;
		m_nFramePos = 0;
		m_pFrameDelay = NULL;
	}

	~CDuiGifImage(void)
	{
		Destroy();
		Gdiplus::GdiplusShutdown( m_gdiplusToken );
	}

public:
	BOOL LoadFromFile(LPCTSTR pszFileName)
	{
		Destroy();

		if (NULL == pszFileName || NULL == *pszFileName)
			return FALSE;

		m_pImage = new Gdiplus::Image(pszFileName);
		if (NULL == m_pImage)
			return FALSE;

		if (m_pImage->GetLastStatus() != Gdiplus::Ok)
		{
			delete m_pImage;
			m_pImage = NULL;
			return FALSE;
		}

		return ReadGifInfo();
	}

	BOOL LoadFromIStream(IStream* pStream)
	{
		Destroy();

		if (NULL == pStream)
			return FALSE;

		m_pImage = new Gdiplus::Image(pStream);
		if (NULL == m_pImage)
			return FALSE;

		if (m_pImage->GetLastStatus() != Gdiplus::Ok)
		{
			delete m_pImage;
			m_pImage = NULL;
			return FALSE;
		}

		return ReadGifInfo();
	}

	BOOL LoadFromBuffer(const BYTE* lpBuf, DWORD dwSize)
	{
		if (NULL == lpBuf || dwSize <= 0)
			return FALSE;

		HGLOBAL hGlobal = ::GlobalAlloc(GHND, dwSize);
		if (NULL == hGlobal)
			return FALSE;

		LPVOID lpBuffer = ::GlobalLock(hGlobal);
		if (NULL == lpBuffer)
		{
			::GlobalFree(hGlobal);
			return FALSE;
		}

		memcpy(lpBuffer, lpBuf, dwSize);
		::GlobalUnlock(hGlobal);

		LPSTREAM lpStream = NULL;
		HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream);
		if (hr != S_OK)
		{
			::GlobalFree(hGlobal);
			return FALSE;
		}

		BOOL bRet = LoadFromIStream(lpStream);
		lpStream->Release();

		return bRet;
	}

	BOOL LoadFromResource(HINSTANCE hInstance, LPCTSTR pszResourceName, LPCTSTR pszResType)
	{
		HRSRC hResource = ::FindResource(hInstance, pszResourceName, pszResType);
		if( hResource == NULL )
			return FALSE;

		DWORD dwSize = 0;
		HGLOBAL hGlobal = ::LoadResource(hInstance, hResource);
		if( hGlobal == NULL ) 
		{
			::FreeResource(hResource);
			return FALSE;
		}

		dwSize = ::SizeofResource(hInstance, hResource);
		if( dwSize == 0 )
			return FALSE;

		BOOL bRet = LoadFromBuffer((LPBYTE)::LockResource(hGlobal),dwSize);
		::FreeResource(hResource);

		return bRet;
	}

	BOOL LoadFromResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR pszResType)
	{
		return TRUE;
	}

	BOOL SaveAsFile(LPCTSTR pszFileName)
	{
		if (NULL == pszFileName || NULL == m_pImage)
			return FALSE;

		LPCTSTR lpExtension = _tcsrchr(pszFileName, _T('.'));
		if (NULL == lpExtension)
			return FALSE;

		CLSID clsid = GetEncoderClsidByExtension(lpExtension);
		if (CLSID_NULL == clsid)
			return FALSE;

		Gdiplus::Status status = m_pImage->Save(pszFileName, &clsid, NULL);
		return (status != Gdiplus::Ok) ? FALSE : TRUE;
	}

	void Destroy()
	{
		m_nFrameCnt = 0;
		m_nFramePos = 0;

		if (m_pFrameDelay != NULL)
		{
			delete []m_pFrameDelay;
			m_pFrameDelay = NULL;
		}

		if (m_pImage != NULL)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
	}

	BOOL IsAnimatedGif()
	{
		return m_nFrameCnt > 1;
	}

	UINT GetFrameCount()
	{
		return m_nFrameCnt;
	}

	long GetFrameDelay(int nFramePos = -1)
	{
		if (!IsAnimatedGif() || NULL == m_pFrameDelay)
			return 0;

		int nFramePos2;
		if (nFramePos != -1)
			nFramePos2 = nFramePos;
		else
			nFramePos2 = m_nFramePos;

		if (nFramePos2 >= 0 && nFramePos2 < (int)m_nFrameCnt)
			return m_pFrameDelay[nFramePos2];
		else
			return 0;
	}

	void ActiveNextFrame()
	{
		if (m_pImage != NULL && IsAnimatedGif())
		{
			m_nFramePos++;
			if (m_nFramePos == m_nFrameCnt)
				m_nFramePos = 0;

			if (m_nFramePos >= 0 && m_nFramePos < m_nFrameCnt)
			{
				static GUID Guid = Gdiplus::FrameDimensionTime;
				Gdiplus::Status status = m_pImage->SelectActiveFrame(&Guid, m_nFramePos);
			}
		}
	}

	void SelectActiveFrame(int nFramePos)
	{
		if (m_pImage != NULL && IsAnimatedGif() 
			&& nFramePos >= 0 && nFramePos < (int)m_nFrameCnt)
		{
			static GUID Guid = Gdiplus::FrameDimensionTime;
			Gdiplus::Status status = m_pImage->SelectActiveFrame(&Guid, nFramePos);
			m_nFramePos = nFramePos;
		}
	}

	BOOL Draw(HDC hDestDC, int xDest, int yDest, int nFramePos = -1)
	{
		Gdiplus::Graphics graphics(hDestDC);
		Gdiplus::Status status = graphics.DrawImage(m_pImage, xDest, yDest);
		if(status != Gdiplus::Ok)
			return FALSE;
		else
			return TRUE;
	}

	BOOL Draw(HDC hDestDC, const RECT& rectDest, int nFramePos = -1)
	{
		if (NULL == m_pImage)
			return FALSE;

		if (nFramePos != -1)
			SelectActiveFrame(nFramePos);

		int nWidth = rectDest.right-rectDest.left;
		int nHeight = rectDest.bottom-rectDest.top;

		Gdiplus::Graphics graphics(hDestDC);
		Gdiplus::Status status = graphics.DrawImage(m_pImage, 
			Gdiplus::Rect(rectDest.left, rectDest.top, nWidth, nHeight));
		if(status != Gdiplus::Ok)
			return FALSE;
		else
			return TRUE;
	}

	int GetWidth() const
	{

		if (m_pImage != NULL)
			return m_pImage->GetWidth();
		else
			return 0;
	}

	int GetHeight() const
	{
		if (m_pImage != NULL)
			return m_pImage->GetHeight();
		else
			return 0;
	}

	UINT GetCurFramePos()
	{
		return m_nFramePos;
	}

	BOOL GetRawFormat(GUID * lpGuid)
	{
		Gdiplus::Status status = m_pImage->GetRawFormat(lpGuid);
		return (Gdiplus::Ok == status) ? TRUE : FALSE;
	}


private:
	CLSID GetEncoderClsidByExtension(const WCHAR * lpExtension)
	{
		CLSID clsid = CLSID_NULL;

		if (NULL == lpExtension)
			return clsid;

		UINT numEncoders = 0, size = 0;
		Gdiplus::Status status = Gdiplus::GetImageEncodersSize(&numEncoders, &size);  
		if (status != Gdiplus::Ok)
			return clsid;

		Gdiplus::ImageCodecInfo* lpEncoders = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (NULL == lpEncoders)
			return clsid;

		status = Gdiplus::GetImageEncoders(numEncoders, size, lpEncoders);
		if (Gdiplus::Ok == status)
		{
			for (UINT i = 0; i < numEncoders; i++)
			{
				BOOL bFind = FALSE;
				const WCHAR * pStart = lpEncoders[i].FilenameExtension;
				const WCHAR * pEnd = wcschr(pStart, L';');
				do 
				{
					if (NULL == pEnd)
					{
						LPCWSTR lpExt = ::wcsrchr(pStart, L'.');
						if ((lpExt != NULL) && (_wcsicmp(lpExt, lpExtension) == 0))
						{
							clsid = lpEncoders[i].Clsid;
							bFind = TRUE;
						}
						break;
					}

					int nLen = pEnd-pStart;
					if (nLen < MAX_PATH)
					{
						WCHAR cBuf[MAX_PATH] = {0};
						wcsncpy(cBuf, pStart, nLen);
						LPCWSTR lpExt = ::wcsrchr(cBuf, L'.');
						if ((lpExt != NULL) && (_wcsicmp(lpExt, lpExtension) == 0))
						{
							clsid = lpEncoders[i].Clsid;
							bFind = TRUE;
							break;
						}
					}
					pStart = pEnd+1;
					if (L'\0' == *pStart)
						break;
					pEnd = wcschr(pStart, L';');
				} while (1);
				if (bFind)
					break;
			}
		}

		free(lpEncoders);

		return clsid;
	}

	BOOL ReadGifInfo()
	{
		if (NULL == m_pImage)
			return FALSE;

		UINT nCount = 0;
		nCount = m_pImage->GetFrameDimensionsCount();
		if (nCount <= 0)
			return FALSE;

		GUID* pDimensionIDs = new GUID[nCount];
		if (NULL == pDimensionIDs)
			return FALSE;
		m_pImage->GetFrameDimensionsList(pDimensionIDs, nCount);
		m_nFrameCnt = m_pImage->GetFrameCount(&pDimensionIDs[0]);
		delete pDimensionIDs;

		if (m_nFrameCnt <= 1)
			return TRUE;

		UINT nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		if (nSize <= 0)
			return FALSE;

		Gdiplus::PropertyItem * pPropertyItem = (Gdiplus::PropertyItem *)malloc(nSize);
		if (NULL == pPropertyItem)
			return FALSE;

		m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pPropertyItem);

		m_pFrameDelay = new long[m_nFrameCnt];
		if (NULL == m_pFrameDelay)
		{
			free(pPropertyItem);
			return FALSE;
		}

		for (int i = 0; i < (int)m_nFrameCnt; i++)
		{
			m_pFrameDelay[i] = ((long*)pPropertyItem->value)[i] * 10;	// 帧切换延迟时间，以1/100秒为单位
			if (m_pFrameDelay[i] < 100)
				m_pFrameDelay[i] = 100;
		}

		free(pPropertyItem);

		return TRUE;
	}

private:
	Gdiplus::Image* m_pImage;		// 图片对象指针
	UINT m_nFrameCnt;				// GIF动画总帧数
	UINT m_nFramePos;				// 当前帧
	long *m_pFrameDelay;			// 每帧切换延时时间
	ULONG_PTR				m_gdiplusToken;
	Gdiplus::GdiplusStartupInput		m_gdiplusStartupInput;
};
