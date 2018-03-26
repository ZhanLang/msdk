/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,3th,2014

Module Name:

	dsPaintMission.h 

Abstract: dsui 绘制任务


*/
/************************************************************************/

#ifndef __DSPAINTMISSON_H__
#define __DSPAINTMISSON_H__

#include <list>
#include <vector>
using namespace std;


typedef struct tagSuperText
{
	CString strText;
	COLORREF crText;
	HFONT hFontText;
	CSize sizeDraw;
}SUPERTEXT,*PSUPERTEXT;


typedef vector<SUPERTEXT> VEC_SUPERTEXT;



class dsPaintMission:public IdsPaintMission
{
public:

	int m_nState;
	CRect m_rcDst;//相对位置，相对于ui元素左上角
	CRect m_rcDstCoordMark;
	int m_nPaintMode;

	int m_nImageID;
	CRect m_rcSrc;
	CRect m_rcSquares9;
	COLORREF m_crText;
	LPARAM m_lParam;

	UINT m_uFormat;
	CString m_strText;
	HFONT m_hFont;
	BOOL m_bEnabled;
	BOOL m_bMarkAsIcon;
	BOOL m_bMarkAsGif;

	VEC_SUPERTEXT m_vecSuperText;

	CSize m_sizeSuperTextScale;

	DWORD m_dwGifKey;
	DWORD m_dwOldGifKey;

	BOOL m_bUseUIText;

	//dsGifAnimate m_gif;

public:
	dsPaintMission();
	~dsPaintMission();

public:

	int GetState();

	Image *GetImagePtr();
	HICON GetIcon();

	LPCWSTR GetTextPtr();

	CString &GetTextStrQuote();

	BOOL GetRealDstRect(__in LPRECT lprcUI, __out LPRECT lprcDst);

	BOOL GetRealSrcRect(__out LPRECT lprcSrc);


	BOOL Get9SquaresStretchTable(__in LPRECT lprcUI, __out LPNSST lpnsst);

public:
	
	static BOOL IsRectForAllDraw(__in LPRECT lprc);

	static BOOL SetRectForAllDraw(__inout LPRECT lprc);


public:
	//////////////////////////////////////////////////////////////////////////
	//override

	virtual void Reset();
	virtual void SetState(int nState);
	virtual BOOL SetDstRect(__in LPRECT lprc);
	virtual BOOL SetDstCoordMark(__in LPRECT lprc);
	virtual void SetPaintMode(int nPaintMode);
	virtual void SetImageID(int nImageID);
	virtual BOOL SetSrcRect(__in LPRECT lprc);
	virtual BOOL SetSquares9(__in LPRECT lprc);
	virtual BOOL SetText(__in LPCWSTR lpszText);
	virtual void SetTextDrawFlag(UINT uFormat);
	virtual void SetColor(COLORREF cr);
	virtual void SetParam(LPARAM lParam);
	virtual void SetFont(int nFontID);
	virtual BOOL Enable(BOOL bEnable);
	virtual void MarkAsIcon(BOOL bEnable);

	virtual BOOL AddSuperTextElement(LPCWSTR lpszText, COLORREF crText, int nFontID);
	virtual void ResetSuperText();
	virtual BOOL SetSuperTextElement(int nPos, LPCWSTR lpszText, COLORREF crText, int nFontID);

	//virtual BOOL MarkAsGif(BOOL bMark);
	virtual BOOL MarkAsGif(DWORD dwKey);

	virtual BOOL UseUIText(BOOL bUse);

	
	
};

#endif /*__DSPAINTMISSON_H__*/