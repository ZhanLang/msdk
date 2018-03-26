#pragma once
namespace DuiKit{;
struct IDuiGroupCtrl : public IDuiObject
{
	virtual VOID SetNormalImage(LPCWSTR lpszNormalImage) = 0;
	virtual LPCWSTR GetNormalImage() = 0;

	virtual VOID SetPushedImage(LPCWSTR lpszPushedImage) = 0;
	virtual LPCWSTR GetPushedImage() = 0;

	virtual VOID SetDisabledImage(LPCWSTR lpszDisabledImage) = 0;
	virtual LPCWSTR GetDisableImage() = 0;

	virtual VOID SetHotImage(LPCWSTR lpszHotImage) = 0;
	virtual LPCWSTR GetHotImage() = 0;

	virtual VOID SetEnabled(BOOL bEnabled) = 0;
	virtual BOOL GetEnabled() = 0;


	virtual DWORD GetState() = 0;
};

};
