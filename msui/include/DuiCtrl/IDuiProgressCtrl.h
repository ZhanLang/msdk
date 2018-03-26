#pragma once
namespace DuiKit{;
struct IDuiProgressCtrl : public IDuiObject
{
	virtual VOID SetForegroundImage(LPCWSTR lpszImage) = 0;
	virtual LPCWSTR GetForegroundImage() = 0;

	virtual VOID SetHorizontal(BOOL bHorizontal) = 0;
	virtual BOOL GetHorizontal() = 0;

	virtual INT GetMinValue() const = 0;
	virtual VOID SetMinValue(INT nMin) = 0;
	virtual INT GetMaxValue() const = 0;
	virtual VOID SetMaxValue(INT nMax) = 0;
	virtual INT GetValue() const = 0;
	virtual VOID SetValue(INT nValue) = 0;
};

};