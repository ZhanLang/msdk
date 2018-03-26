#pragma once

namespace DuiKit{;
struct IDuiFont: public IDuiObject
{

	virtual VOID SetDuiSkin(IDuiSkin* Skin) = 0;
	virtual IDuiSkin* GetDuiSkin() = 0;

	virtual HFONT GetHandle() = 0;
	virtual VOID Attach(HFONT hFont) = 0;
	virtual VOID Detach() = 0;
	
	virtual HFONT CreateFont() = 0;

	virtual VOID SetTypeFace(LPCWSTR lpszTypeFace) = 0;
	virtual LPCWSTR GetTypeFace() = 0;

	virtual VOID SetSize(INT nSize) = 0;
	virtual INT GetSize() = 0;

	virtual VOID SetBold(BOOL bBold) = 0;
	virtual BOOL GetBold() = 0;

	virtual VOID SetUnderLine(BOOL bUnderline) = 0;
	virtual BOOL GetUnderLine() = 0;

	virtual VOID SetItalic(BOOL bItalic) = 0;
	virtual BOOL GetItalic() = 0;

	virtual VOID SetName(LPCWSTR lpszName) = 0;
	virtual LPCWSTR GetName() = 0;

};



};//namespace DuiKit{;