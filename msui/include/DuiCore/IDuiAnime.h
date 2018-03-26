#pragma once

namespace DuiKit{;

struct IDuiControlCtrl;
struct IDuiAnime : public IDuiObject
{
	virtual VOID SetName(LPCWSTR lpszName) = 0;
	virtual LPCWSTR GetName() = 0;

	virtual VOID SetControl(IDuiControlCtrl* pControl) = 0;
	virtual BOOL DoAnimation() = 0;
};
};