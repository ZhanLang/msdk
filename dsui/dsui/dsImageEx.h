/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	3,5th,2014

Module Name:

	dsImageEx.h

Abstract:


*/
/************************************************************************/

#ifndef __DSIMAGEEX_H__
#define __DSIMAGEEX_H__


#pragma comment(lib,"GdiPlus.lib")

#include <GdiPlus.h>
using namespace Gdiplus;

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>


class dsImageEx : public Image
{
public:
	dsImageEx(IN LPCTSTR  sResourceType, IN LPCTSTR  sResource);
	dsImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);

	
	~dsImageEx();
public:

	CSize	GetSize();
	bool	IsAnimatedGIF() { return m_nFrameCount > 1; }
	void	Destroy();
	
	inline UINT GetGifFrameCount(){return m_nFrameCount;}
	inline LONG GetDelayProperty(UINT uFramePos){
		return ((long*)m_pPropertyItem->value)[uFramePos];
	}
	inline BOOL IsInitialized(){return m_bIsInitialized;}


protected:

	bool				TestForAnimatedGIF();
	void				Initialize();

	IStream*			m_pStream;
	
	bool LoadFromBuffer(BYTE* pBuff, int nSize);
	bool GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize);
	bool Load(CString sResourceType, CString sResource);

	HINSTANCE		m_hInst;
	UINT			m_nFrameCount;
	bool			m_bIsInitialized;
	PropertyItem*	m_pPropertyItem;
};


#define GA_LINK_NULL	0
#define GA_LINK_HWND	1
#define GA_LINK_HDC		2
#define GA_LINK_DSUI	3

class dsUIBase;

//class dsLock
//{
//public:
//	CRITICAL_SECTION m_cs;
//public:
//	dsLock(void)
//	{
//		InitializeCriticalSection(&m_cs);
//	}
//
//	~dsLock(void)
//	{
//		::DeleteCriticalSection(&m_cs);
//	}
//
//	void Lock()		
//	{
//		::EnterCriticalSection(&m_cs);
//	}
//
//	void UnLock()	
//	{
//		::LeaveCriticalSection(&m_cs);
//	}
//};


class dsGifAnimate
{
public:

	dsImageEx *m_pimg;

	int m_nLinkFlag;
	HDC m_hdc;
	HWND m_hWnd;
	dsUIBase *m_pui;

	CPoint m_pt;
	CRect m_rc;
	CRect m_rcSrc;
	int m_nFramePos;
	BOOL m_bPause;

	bool			m_bIsInitialized;
	bool m_bAnimating;

	HANDLE m_hExitEvent;
	HANDLE m_hThread;
	HANDLE m_hPause;

	BOOL m_bGif;

	dsLock m_lock;

public:

	dsGifAnimate():m_pimg(NULL),m_hdc(NULL),m_hWnd(NULL),m_nFramePos(0),
		m_bPause(FALSE),m_hExitEvent(NULL),m_hThread(NULL),m_hPause(NULL),
		m_bGif(FALSE),m_bIsInitialized(FALSE),m_nLinkFlag(GA_LINK_NULL),m_bAnimating(FALSE),
		m_pui(NULL){}
	~dsGifAnimate();

public:

	BOOL Attatch(dsImageEx *pimgGif);
	void SetPause(bool bPause);
	bool	StartAnimation(HWND hWnd, CPoint pt);
	bool	StartAnimation(HDC hdc, CPoint pt);
	bool	StartAnimation(dsUIBase *pui, CRect rc, CRect rcSrc);
	void	StopAnimate();
	inline bool	IsAnimating(){return m_bAnimating;}
	void	UpdatePoint(POINT pt);
	void	UpdateRect(CRect rc, CRect rcSrc);
	void	Destroy();
	inline bool	IsPaused() { return m_bPause; }
	void ThreadAnimation();

protected:
	void BeginAnimate();
	void EndAnimate();
	bool DrawFrameGIF();
	static UINT WINAPI _ThreadAnimationProc(LPVOID pParam);

};


#endif /*__DSIMAGEEX_H__*/
