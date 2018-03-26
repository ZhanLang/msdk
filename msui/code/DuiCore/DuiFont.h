#pragma once

#include <DuiImpl/DuiString.h>
#include <DuiCore/IDuiFont.h>

using namespace DuiKit;
class CDuiFont
	:public IDuiFont
{
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiFont, OBJECT_CORE_FONT)
		DUI_DEFINE_INTERFACE(IDuiFont, IIDuiFont)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiLangText)
		DUI_SETATTRIBUTE_STRING_FUNCTION(name, SetName);
		DUI_SETATTRIBUTE_STRING_FUNCTION(typeface, SetTypeFace);
		DUI_SETATTRIBUTE_INT_FUNCTION(size, SetSize);
		DUI_SETATTRIBUTE_BOOL(bold, m_bBold);
		DUI_SETATTRIBUTE_BOOL(underline, m_bUnderLine);
		DUI_SETATTRIBUTE_BOOL(italic, m_bItalic);
		DUI_SETATTRIBUTE_BOOL(default, m_bDefault);
	DUI_END_SETATTRIBUTE;

	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder);

public:
	CDuiFont();
	~CDuiFont(void);

	virtual VOID SetDuiSkin(IDuiSkin* Skin);
	virtual IDuiSkin* GetDuiSkin();


	virtual HFONT GetHandle();
	virtual VOID Attach(HFONT hFont);
	virtual VOID Detach();

	virtual HFONT CreateFont();

	virtual VOID SetTypeFace(LPCWSTR lpszTypeFace);
	virtual LPCWSTR GetTypeFace();

	virtual VOID SetSize(INT nSize);
	virtual INT GetSize();

	virtual VOID SetBold(BOOL bBold);
	virtual BOOL GetBold();

	virtual VOID SetUnderLine(BOOL bUnderline);
	virtual BOOL GetUnderLine();

	virtual VOID SetItalic(BOOL bItalic);
	virtual BOOL GetItalic();

	virtual VOID SetName(LPCWSTR lpszName);
	virtual LPCWSTR GetName();

private:
	CDuiString  m_strName;
	CDuiString  m_strTypeFace;
	INT		    m_nSize;
	BOOL		m_bBold;
	BOOL		m_bUnderLine;
	BOOL		m_bItalic;
	BOOL		m_bDefault;
	HFONT		m_hFont;
	IDuiSkin* m_pSkin;
};

