#pragma once

namespace DuiKit{;

struct IDuiScrollCtrl : public IDuiObject
{
	
	//垂直
	virtual LPCWSTR GetTopButtonNormalImage() = 0;
	virtual VOID SetTopButtonNormalImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetTopButtonHotImage() = 0;
	virtual VOID SetTopButtonHotImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetTopButtonPushedImage() = 0;
	virtual VOID SetTopButtonPushedImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetTopButtonDisabledImage() = 0;
	virtual VOID SetTopButtonDisabledImage(LPCWSTR lpszImage) = 0;

	virtual VOID SetShowTopButton(BOOL bShow) = 0;
	virtual BOOL GetShowTopButton() = 0;

	//操作下边按钮
	virtual LPCWSTR GetBottomButtonNormalImage() = 0;
	virtual VOID SetBottomButtonNormalImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetBottomButtonHotImage() = 0;
	virtual VOID SetBottomButtonHotImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetBottomButtonPushedImage() = 0;
	virtual VOID SetBottomButtonPushedImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetBottomButtonDisabledImage() = 0;
	virtual VOID SetBottomButtonDisabledImage(LPCWSTR lpszImage) = 0;

	virtual VOID SetShowBottomButton(BOOL bShow) = 0;
	virtual BOOL GetShowBottomButton() = 0;

	virtual LPCWSTR GetVerticalThumbNormalImage() = 0;
	virtual VOID SetVerticalThumbNormalImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetVerticalThumbHotImage() = 0;
	virtual VOID SetVerticalThumbHotImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetVerticalThumbPushedImage() = 0;
	virtual VOID SetVerticalThumbPushedImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetVerticalThumbDisabledImage() = 0;
	virtual VOID SetVerticalThumbDisabledImage(LPCWSTR pStrImage) = 0;

	virtual LPCWSTR GetVerticalRailNormalImage() = 0;
	virtual VOID SetVerticalRailNormalImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetVerticalRailHotImage() = 0;
	virtual VOID SetVerticalRailHotImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetVerticalRailPushedImage() = 0;
	virtual VOID SetVerticalRailPushedImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetVerticalRailDisabledImage() = 0;
	virtual VOID SetVerticalRailDisabledImage(LPCWSTR pStrImage) = 0;

	virtual VOID SetVerticalScroll(BOOL bShow) = 0;
	virtual BOOL GetVerticalScroll() = 0;

// 	virtual INT GetVerticalScrollRange()  = 0;
// 	virtual VOID SetVerticalScrollRange(INT nRange) = 0;
// 	virtual INT GetVerticalScrollPos() = 0;
// 	virtual VOID SetVerticalScrollPos(INT nPos) = 0;

	//水平
	//操作左边的按钮
	virtual LPCWSTR GetLeftButtonNormalImage() = 0;
	virtual VOID SetLeftButtonNormalImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetLeftButtonHotImage() = 0;
	virtual VOID SetLeftButtonHotImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetLeftButtonPushedImage() = 0;
	virtual VOID SetLeftButtonPushedImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetLeftButtonDisabledImage() = 0;
	virtual VOID SetLeftButtonDisabledImage(LPCWSTR lpszImage) = 0;

	virtual VOID SetShowLeftButton(BOOL bShow) = 0;
	virtual BOOL GetShowLeftButton() = 0;

	//操作右边按钮
	virtual LPCWSTR GetRightButtonNormalImage() = 0;
	virtual VOID SetRightButtonNormalImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetRightButtonHotImage() = 0;
	virtual VOID SetRightButtonHotImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetRightButtonPushedImage() = 0;
	virtual VOID SetRightButtonPushedImage(LPCWSTR lpszImage) = 0;

	virtual LPCWSTR GetRightButtonDisabledImage() = 0;
	virtual VOID SetRightButtonDisabledImage(LPCWSTR lpszImage) = 0;

	virtual VOID SetShowRightButton(BOOL bShow) = 0;
	virtual BOOL GetShowRightButton() = 0;


	virtual LPCWSTR GetHorizontalThumbNormalImage() = 0;
	virtual VOID SetHorizontalThumbNormalImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHorizontalThumbHotImage() = 0;
	virtual VOID SetHorizontalThumbHotImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHorizontalThumbPushedImage() = 0;
	virtual VOID SetHorizontalThumbPushedImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHorizontalThumbDisabledImage() = 0;
	virtual VOID SetHorizontalThumbDisabledImage(LPCWSTR pStrImage) = 0;

	virtual LPCWSTR GetHorizontalRailNormalImage() = 0;
	virtual VOID SetHorizontalRailNormalImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHorizontalRailHotImage() = 0;
	virtual VOID SetHorizontalRailHotImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHorizontalRailPushedImage() = 0;
	virtual VOID SetHorizontalRailPushedImage(LPCWSTR pStrImage) = 0;
	virtual LPCWSTR GetHorizontalRailDisabledImage() = 0;
	virtual VOID SetHorizontalRailDisabledImage(LPCWSTR pStrImage) = 0;

	virtual VOID SetHorizontalScroll(BOOL bShow) = 0;
	virtual BOOL GetHorizontalScroll() = 0;

	virtual DWORD GetHorizontalLine() = 0;
	virtual VOID SetHorizontalLine(DWORD dwLine) = 0;

	virtual DWORD GetVerticalLine() = 0;
	virtual VOID SetVerticalLine(DWORD dwLine) = 0;

	virtual DWORD GetHorizontalHeight() = 0;
	virtual VOID SetHorizontalHeight(DWORD dwHeight) = 0;

	virtual DWORD GetVerticalWidth() = 0;
	virtual VOID SetVerticalWidth(DWORD dwHeight) = 0;

	virtual DWORD GetHorizontalButtonWidth() = 0;
	virtual VOID SetHorizontalButtonWidth(DWORD dwWidth) = 0;

	virtual DWORD GetVerticalButtonHeight() = 0;
	virtual VOID SetVerticalButtonHeight(DWORD dwWidth) = 0;

	virtual CDuiRect GetClientPos() = 0;
// 	virtual INT GetHorizontalScrollRange()  = 0;
// 	virtual VOID SetHorizontalScrollRange(INT nRange) = 0;
// 	virtual INT GetHorizontalScrollPos() = 0;
// 	virtual VOID SetHorizontalScrollPos(INT nPos) = 0;
	
};

};