/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,3th,2014

Module Name:

	dsImageMgr.h 

Abstract: dsui Õº∆¨π‹¿Ì


*/
/************************************************************************/

#ifndef __DSIMAGEMGR_H__
#define __DSIMAGEMGR_H__

#include <map>
using namespace std;

#include <GdiPlus.h>
using namespace Gdiplus;

#pragma comment(lib,"Gdiplus.lib")

#include <dsLock.h>
//#include "dsImageEx.h"

#define INVALID_IMAGEID	-1
#define IMAGERES_EXIST -2

typedef struct tagDSImageInfo
{
	CString strImageName;
	Image *pimg;
	HICON hIcon;

	tagDSImageInfo(__in LPCWSTR lpszName, __in Image* _pimg, HICON _hIcon = NULL){
		strImageName = lpszName;
		pimg = _pimg;
		hIcon = _hIcon;
	}

	tagDSImageInfo(CString &strName, __in Image* _pimg, HICON _hIcon = NULL){
		strImageName = strName;
		pimg = _pimg;
		hIcon = _hIcon;
	}

	void Release(){

		if(NULL != pimg){
			try{delete pimg;}
			catch(...){}
			
			
		}

		if(NULL != hIcon)
			DeleteObject((HGDIOBJ)hIcon);
	}

}DSIMAGEINFO,*PDSIMAGEINFO,*LPDSIMAGEINFO;

class dsImageMgr:public IdsImageMgr
{
public:

	
	typedef map<int,DSIMAGEINFO> MAP_IMAGE;
	typedef map<CString,int> MAP_IMAGEMARK;//here

	
public:
	static dsImageMgr g_dsimInstance;
	MAP_IMAGE m_mapImage;
	MAP_IMAGEMARK m_mapImageMark;
	ULONG_PTR m_token;

	dsLock m_lock;

public:
	dsImageMgr();


	~dsImageMgr();
	
	static dsImageMgr *getInstance();

	virtual int NewImage(__in LPCWSTR lpszFileName, __in Image *pimg = NULL, BOOL bForceReplace = FALSE);
	virtual int NewIconImage(HINSTANCE hInst, __in LPCWSTR lpszFileName, BOOL bForceReplace = FALSE);


	virtual int GetImageID(__in LPCWSTR lpszFileName);

	Image *GetImagePtr(__in LPCWSTR lpszFileName);

	virtual Image *GetImagePtr(int nImageID);

	HICON GetIcon(int nImageID);

	virtual BOOL ReleaseImage(LPCWSTR lpszFileName);

	virtual BOOL ReleaseImage(int nImageID);

	virtual void ReleaseAllImage();


private:

	int CreateNewImageID();
	static dsImageMgr* m_ImageMgrInstance;


};




#endif /*__DSIMAGEMGR_H__*/