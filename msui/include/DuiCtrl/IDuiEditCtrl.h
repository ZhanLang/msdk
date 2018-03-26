#pragma once



namespace DuiKit{;
struct IDuiEditCtrl: public IDuiObject
{
	virtual VOID SetEnabled(BOOL bEnable = TRUE) = 0;
	virtual BOOL IsEnabled() = 0;
	virtual VOID SetText(LPCTSTR pstrText) = 0;
	virtual LPCWSTR GetText() = 0;

	virtual VOID SetDefaultText(LPCWSTR lpszText) = 0;
	virtual LPCWSTR GetDefaultText() = 0;

	virtual VOID SetMaxChar(UINT uMax) = 0;
	virtual UINT GetMaxChar() = 0;
	virtual VOID SetReadOnly(BOOL bReadOnly) = 0;
	virtual BOOL IsReadOnly() const = 0;
	virtual VOID SetPasswordMode(BOOL bPasswordMode) = 0;
	virtual BOOL IsPasswordMode() const = 0;
	virtual VOID SetPasswordChar(WCHAR cPasswordChar) = 0;
	virtual WCHAR GetPasswordChar() const = 0;
	virtual VOID SetNumberOnly(BOOL bNumberOnly) = 0;
	virtual BOOL IsNumberOnly() const = 0;

	
	/*暂时不支持*/
	virtual LPCWSTR GetNormalImage() = 0;
	virtual VOID SetNormalImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHotImage() = 0;
	virtual VOID SetHotImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetFocusedImage() = 0;
	virtual VOID SetFocusedImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetDisabledImage() = 0;
	virtual VOID SetDisabledImage(LPCWSTR pStrImage) = 0;
	virtual VOID SetNativeEditBkColor(DWORD dwBkColor) = 0;
	virtual DWORD GetNativeEditBkColor() const = 0;

	/*边框颜色*/
	virtual DWORD GetNormalBorderColor() = 0;
	virtual VOID SetNormalBorderColor(DWORD dwColor) = 0;
	virtual DWORD GetHotBorderColor() = 0;
	virtual VOID SetHotBorderColor(DWORD dwColor) = 0;
	virtual DWORD GetFocusedBorderColor() = 0;
	virtual VOID SetFocusedBorderColor(DWORD dwColor) = 0;
	virtual DWORD GetDisabledBorderColor() = 0;
	virtual VOID SetDisabledBorderColor(DWORD dwColor) = 0;


	virtual VOID SetSel(long nStartChar, long nEndChar) = 0;
	virtual VOID SetSelAll() = 0;
	virtual VOID SetReplaceSel(LPCWSTR lpszReplace) = 0;

	
	virtual DWORD GetTextColor() = 0;
	virtual VOID SetTextColor(DWORD dwColor) = 0;

	virtual DWORD GetDefaultTextColor() = 0;
	virtual VOID SetDefaultTextColor(DWORD dwColor) = 0;

	virtual LPCWSTR GetFont() = 0;
	virtual VOID SetFont(LPCWSTR lpszFont) = 0;

	virtual CDuiRect GetTextPadding() = 0;
	virtual VOID SetTextPadding(const CDuiRect& TextPadding ) = 0;
};
};