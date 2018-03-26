#pragma once

namespace DuiKit{;

struct IDuiTextCtrl : public IDuiObject
{
	virtual VOID SetAutoLine(BOOL bAutoLine) = 0;
	virtual BOOL GetAutoLine() = 0;
};

};