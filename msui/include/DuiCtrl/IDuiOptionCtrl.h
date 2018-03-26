#pragma once
namespace DuiKit{;

struct IDuiOptionCtrl : public IDuiObject
{
	virtual VOID SetSelectedImage(LPCWSTR lpszImage) = 0;
	virtual LPCWSTR GetSelectedImage() = 0;

	virtual VOID SetSelectedHotImage(LPCWSTR lpszImage) = 0;
	virtual LPCWSTR GetSelectedHotImage() = 0;
	
	virtual BOOL GetSelected() = 0;
	virtual VOID SetSelected(BOOL bSelected) = 0;

	virtual LPCWSTR GetGroup() = 0;
	virtual VOID SetGroup(LPCWSTR lpszGroupName = NULL) = 0;


};
};