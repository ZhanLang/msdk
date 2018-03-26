#pragma once
namespace DuiKit{;

struct IDuiLang : public IDuiObject
{
	virtual VOID SetDuiSkin(IDuiSkin* Skin) = 0;
	virtual IDuiSkin* GetDuiSkin() = 0;

	virtual LPCWSTR GetLangName() = 0;
	virtual LPCWSTR GetText(LPCWSTR lpszTextName) = 0;
	virtual VOID	AddText(LPCWSTR lpszTextName, LPCWSTR lpszText) = 0;
};

struct IDuiLangSet: public IDuiObject
{
	virtual VOID SetDuiSkin(IDuiSkin* Skin) = 0;
	virtual IDuiSkin* GetDuiSkin() = 0;

	virtual IDuiLang* GetLang(LPCWSTR lpszLangName) = 0;
	virtual VOID AddLang(LPCWSTR lpszLangName, IDuiLang* pLang) = 0;
	virtual VOID RemoveLang(LPCWSTR lpszLangName) = 0;

	virtual LPCWSTR GetText(LPCWSTR lpszLangName, LPCWSTR lpszTextName) = 0;

	//表达式获取
	//{lang}.{id}
	//{cur}.{id}
	virtual LPCWSTR GetText(LPCWSTR lpszLangExp) = 0;
};


};//namespace DuiKit{;