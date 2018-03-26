#pragma once
namespace DuiKit{;

struct IDuiTableLayoutCtrl : public IDuiObject
{
	virtual DWORD GetCurrentSelect() = 0;
	virtual VOID SelectItem(DWORD dwIndex) = 0;
	virtual VOID SelectItem(IDuiControlCtrl* Control) = 0;
};

};