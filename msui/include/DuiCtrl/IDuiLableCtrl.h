#pragma once

namespace DuiKit{;


enum TextValign
{
	TextValign_Top,
	TextValign_Center,
	TextValign_Bottom,
};

enum TextHalign
{
	TextHalign_Left,
	TextHalign_Center,
	TextHalign_Right,
};

struct IDuiLabelCtrl : public IDuiObject
{
	virtual VOID SetText(LPCWSTR lpszText) = 0;
	virtual LPCWSTR GetText() = 0;

	//attribute
	virtual VOID SetFont(LPCWSTR lpszFontName) = 0;
	virtual LPCWSTR GetFont() = 0;

	virtual VOID SetEnabledEffect(BOOL bEnabledEffect) = 0;
	virtual BOOL GetEnabledEffect() = 0;

	virtual VOID SetTextPadding(const CDuiRect& padding) = 0;
	virtual CDuiRect GetTextPadding() = 0;

	virtual VOID SetTextColor(DWORD dwTextColor) = 0;
	virtual DWORD GetTextColor() = 0;

	virtual VOID SetTextValign(TextValign valign) = 0;
	virtual TextValign GetTextValign() = 0;

	virtual VOID SetTextHalign(TextHalign halign) = 0;
	virtual TextHalign GetTextHalign() = 0;

};

};