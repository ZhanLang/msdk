
#pragma once

#include <Richedit.h>

namespace DuiKit{;

struct IDuiRichEditCtrl : public IDuiObject
{
	BOOL IsMultiLine();
	void SetMultiLine(BOOL bMultiLine);
	BOOL IsWantTab();
	void SetWantTab(BOOL bWantTab = TRUE);
	BOOL IsWantReturn();
	void SetWantReturn(BOOL bWantReturn = TRUE);
	BOOL IsWantCtrlReturn();
	void SetWantCtrlReturn(BOOL bWantCtrlReturn = TRUE);
	BOOL IsRich();
	void SetRich(BOOL bRich = TRUE);
	BOOL IsReadOnly();
	void SetReadOnly(BOOL bReadOnly = TRUE);
	BOOL GetWordWrap();
	void SetWordWrap(BOOL bWordWrap = TRUE);
	int GetFont();
	void SetFont(int index);
	void SetFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic);
	void SetEnabled(BOOL bEnabled);
	LONG GetWinStyle();
	void SetWinStyle(LONG lStyle);
	DWORD GetTextColor();
	void SetTextColor(DWORD dwTextColor);
	int GetLimitText();
	void SetLimitText(int iChars);
	long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
	CDuiString GetText() const;
	void SetText(LPCTSTR pstrText);
	BOOL GetModify() const;
	void SetModify(BOOL bModified = TRUE) const;
	void GetSel(CHARRANGE &cr) const;
	void GetSel(long& nStartChar, long& nEndChar) const;
	int SetSel(CHARRANGE &cr);
	int SetSel(long nStartChar, long nEndChar);
	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo);
	void ReplaceSelW(LPCWSTR lpszNewText, BOOL bCanUndo = false);
	CDuiString GetSelText() const;
	int SetSelAll();
	int SetSelNone();
	WORD GetSelectionType() const;
	BOOL GetZoom(int& nNum, int& nDen) const;
	BOOL SetZoom(int nNum, int nDen);
	BOOL SetZoomOff();
	BOOL GetAutoURLDetect() const;
	BOOL SetAutoURLDetect(BOOL bAutoDetect = TRUE);
	DWORD GetEventMask() const;
	DWORD SetEventMask(DWORD dwEventMask);
	CDuiString GetTextRange(long nStartChar, long nEndChar) const;
	void HideSelection(BOOL bHide = TRUE, BOOL bChangeStyle = false);
	void ScrollCaret();
	int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = false);
	int AppendText(LPCTSTR lpstrText, BOOL bCanUndo = false);
	DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
	BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);
	DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
	BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
	BOOL SetWordCharFormat(CHARFORMAT2 &cf);
	DWORD GetParaFormat(PARAFORMAT2 &pf) const;
	BOOL SetParaFormat(PARAFORMAT2 &pf);
	BOOL Redo();
	BOOL Undo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();
	int GetLineCount() const;
	CDuiString GetLine(int nIndex, int nMaxLength) const;
	int LineIndex(int nLine = -1) const;
	int LineLength(int nLine = -1) const;
	BOOL LineScroll(int nLines, int nChars = 0);
	CDuiPoint GetCharPos(long lChar) const;
	long LineFromChar(long nIndex) const;
	CDuiPoint PosFromChar(UINT nChar) const;
	int CharFromPos(CDuiPoint pt) const;
	void EmptyUndoBuffer();
	UINT SetUndoLimit(UINT nLimit);
	long StreamIn(int nFormat, EDITSTREAM &es);
	long StreamOut(int nFormat, EDITSTREAM &es);
	void SetAccumulateDBCMode(BOOL bDBCMode);
	BOOL IsAccumulateDBCMode();

	void DoInit();
	// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
	// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
	BOOL SetDropAcceptFile(BOOL bAccept);
	virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
	IDropTarget* GetTxDropTarget();
	virtual BOOL OnTxViewChanged(BOOL bUpdate);
	virtual void OnTxNotify(DWORD iNotify, void *pv);

	void SetScrollPos(SIZE szPos, BOOL bMsg = TRUE);
	void LineUp();
	void LineDown();
	void PageUp();
	void PageDown();
	void HomeUp();
	void EndDown();
	void LineLeft();
	void LineRight();
	void PageLeft();
	void PageRight();
	void HomeLeft();
	void EndRight();

	

	LPCTSTR GetNormalImage();
	void SetNormalImage(LPCTSTR pStrImage);
	LPCTSTR GetHotImage();
	void SetHotImage(LPCTSTR pStrImage);
	LPCTSTR GetFocusedImage();
	void SetFocusedImage(LPCTSTR pStrImage);
	LPCTSTR GetDisabledImage();
	void SetDisabledImage(LPCTSTR pStrImage);
	void PaintStatusImage(HDC hDC);
	RECT GetTextPadding() const;
	void SetTextPadding(RECT rc);

	void SetTipValue(LPCTSTR pStrTipValue);
	LPCTSTR GetTipValue();
	void SetTipValueColor(LPCTSTR pStrColor);
	DWORD GetTipValueColor();
	void SetTipValueAlign(UINT uAlign);
	UINT GetTipValueAlign();
};
};