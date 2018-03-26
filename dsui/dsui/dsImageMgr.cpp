#include "stdafx.h"
#include "dsImageMgr.h"
#include "shellapi.h"

dsImageMgr* dsImageMgr::m_ImageMgrInstance = NULL;

dsImageMgr::dsImageMgr()
{
	GdiplusStartupInput gsi;
	GdiplusStartupOutput gso;
	GdiplusStartup(&m_token, &gsi, &gso);
}

dsImageMgr::~dsImageMgr()
{
	//ReleaseAllImage();
	GdiplusShutdown(m_token);
}

dsImageMgr * dsImageMgr::getInstance()
{
	if( !m_ImageMgrInstance ) 
	{
		m_ImageMgrInstance = new dsImageMgr;
	}
	return m_ImageMgrInstance;
}

int dsImageMgr::NewImage( __in LPCWSTR lpszFileName, __in Image *pimgs /*= NULL*/, BOOL bForceReplace /*= FALSE*/ )
{

	dsLocalRefLock lock(m_lock);

	Image *pimg = NULL;

	if(NULL == lpszFileName)
		return INVALID_IMAGEID;
	int nNewImageID = GetImageID(lpszFileName);

	if(INVALID_IMAGEID == nNewImageID)
		nNewImageID = CreateNewImageID();
	else if(bForceReplace)
		ReleaseImage(nNewImageID);
	else
		return IMAGERES_EXIST;

	if(INVALID_IMAGEID != nNewImageID){

		if(NULL == pimg){
			if(INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(lpszFileName))
				return INVALID_IMAGEID;
			pimg = new Image(lpszFileName, TRUE);

			if(0 == pimg->GetWidth()){
				delete pimg;
				return INVALID_IMAGEID;
			}
		}

		if(NULL != pimg){

			MAP_IMAGE::_Pairib pib = m_mapImage.insert(make_pair(nNewImageID, DSIMAGEINFO(lpszFileName, pimg)));

			if(pib.second)
			{
				MAP_IMAGEMARK::_Pairib pibMark = m_mapImageMark.insert(make_pair(lpszFileName, nNewImageID));

				if (!pibMark.second)
				{
					m_mapImage.erase(pib.first);
					delete pimg;

					_ASSERT(FALSE);
				}
				else
					return nNewImageID;
			}
			else{
				delete pimg;
				_ASSERT(FALSE);

			}
		}
	}
	return INVALID_IMAGEID;
}

int dsImageMgr::NewIconImage( HINSTANCE hInst, __in LPCWSTR lpszFileName, BOOL bForceReplace /*= FALSE*/ )
{
	dsLocalRefLock lock(m_lock);
	
	HICON hIcon = NULL;

	if(NULL == lpszFileName)
		return INVALID_IMAGEID;
	int nNewImageID = GetImageID(lpszFileName);

	if(INVALID_IMAGEID == nNewImageID)
		nNewImageID = CreateNewImageID();
	else if(bForceReplace)
		ReleaseImage(nNewImageID);
	else
		return IMAGERES_EXIST;

	if(INVALID_IMAGEID != nNewImageID){


		//hIcon = ExtractIcon(hInst, lpszFileName, 0);

		HICON hIconTemp = NULL;
		int nCount = (int)ExtractIcon(hInst, lpszFileName, -1);

		if(0 == nCount)
			return INVALID_IMAGEID;

		int nMaxIndex = 0;
		int nMaxWidth = 0;

		ICONINFO ii;
		for (int i=0; i<nCount; i++)
		{
			hIconTemp = ExtractIcon(hInst, lpszFileName, i);

			::GetIconInfo(hIconTemp, &ii);

			CBitmapHandle bm(ii.hbmColor);

			CSize size;

			bm.GetSize(size);

			if(size.cx > nMaxWidth)
			{
				nMaxWidth = size.cx;
				nMaxIndex = i;
			}

			::DestroyIcon(hIconTemp);
		}

		hIcon = ExtractIcon(hInst, lpszFileName, nMaxIndex);

		if(NULL != hIcon){

			MAP_IMAGE::_Pairib pib = m_mapImage.insert(make_pair(nNewImageID, DSIMAGEINFO(lpszFileName, NULL, hIcon)));

			if(pib.second)
			{
				MAP_IMAGEMARK::_Pairib pibMark = m_mapImageMark.insert(make_pair(lpszFileName, nNewImageID));

				if (!pibMark.second)
				{
					m_mapImage.erase(pib.first);
					DeleteObject((HGDIOBJ)hIcon);

					_ASSERT(FALSE);
				}
				else
					return nNewImageID;
			}
			else{
				DeleteObject((HGDIOBJ)hIcon);
				_ASSERT(FALSE);

			}
		}
	}
	return INVALID_IMAGEID;
}

int dsImageMgr::GetImageID( __in LPCWSTR lpszFileName )
{

	dsLocalRefLock lock(m_lock);

	if(NULL == lpszFileName)
		return INVALID_IMAGEID;

	MAP_IMAGEMARK::iterator it = m_mapImageMark.find(lpszFileName);

	if(it != m_mapImageMark.end())
	{
		return it->second;
	}
	return INVALID_IMAGEID;
}

Image * dsImageMgr::GetImagePtr( __in LPCWSTR lpszFileName )
{
	return GetImagePtr(GetImageID(lpszFileName));
}

Image * dsImageMgr::GetImagePtr( int nImageID )
{
	dsLocalRefLock lock(m_lock);

	MAP_IMAGE::iterator it = m_mapImage.find(nImageID);

	if (it != m_mapImage.end())
	{
		return it->second.pimg;
	}

	return NULL;
}



HICON dsImageMgr::GetIcon( int nImageID )
{
	dsLocalRefLock lock(m_lock);

	MAP_IMAGE::iterator it = m_mapImage.find(nImageID);

	if (it != m_mapImage.end())
	{
		return it->second.hIcon;
	}

	return NULL;
}
BOOL dsImageMgr::ReleaseImage( LPCWSTR lpszFileName )
{
	return ReleaseImage(GetImageID(lpszFileName));
}

BOOL dsImageMgr::ReleaseImage( int nImageID )
{
	dsLocalRefLock lock(m_lock);

	if(nImageID < 0)
		return FALSE;

	MAP_IMAGE::iterator it = m_mapImage.find(nImageID);

	if(it != m_mapImage.end())
	{
		MAP_IMAGEMARK::iterator itMark = m_mapImageMark.begin();

		

		while(itMark != m_mapImageMark.end()){
			if(itMark->second == nImageID){
				m_mapImageMark.erase(itMark);
				break;
			}

			itMark ++;
		}

		it->second.Release();

		m_mapImage.erase(it);

		return TRUE;
	}

	return FALSE;
}

void dsImageMgr::ReleaseAllImage()
{
	dsLocalRefLock lock(m_lock);

	m_mapImageMark.clear();

	MAP_IMAGE::iterator it = m_mapImage.begin();

	while (it != m_mapImage.end())
	{
		try{
			//it->second.Release();
			it ++;
		}
		catch(...){}
		
	}

	m_mapImage.clear();
}

int dsImageMgr::CreateNewImageID()
{
	dsLocalRefLock lock(m_lock);
	return m_mapImageMark.size();
}

DSUI_API IdsImageMgr * IdsGet_ImageMgr()
{
	return dsImageMgr::getInstance();
}