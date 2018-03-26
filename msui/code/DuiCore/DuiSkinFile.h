#pragma once

#include <DuiImpl/DuiString.h>
#include <DuiImpl/DuiMap.h>
#include <DuiImpl/DuiCriticalsection.h>

#include <DuiCore/IDuiStream.h>
#include <DuiCore/IDuiSkin.h>
#include <DuiCore/IDuiFont.h>
using namespace DuiKit;
class CDuiSkinFile : public IDuiSkin
{
public:
	CDuiSkinFile();
	~CDuiSkinFile(void);
	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue)
	{

	}
	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder)
	{
		return TRUE;
	}

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiSkin, OBJECT_CORE_SKIN_FILE)
		DUI_DEFINE_INTERFACE(IDuiSkin, IIDuiSkin)
	DUI_END_DEFINE_INTERFACEMAP;

	//IDuiSkin
	virtual VOID SetName(LPCWSTR lpszName);
	virtual LPCWSTR GetName();
	virtual VOID SetCore(IDuiCore* Core);
	virtual IDuiCore* GetCore();


	virtual BOOL Open(LPVOID lpMem, DWORD dwSize, LPVOID lpParam = 0, DWORD dwParamSize = 0);
	virtual BOOL Close();
	virtual SkinType GetSkinType();
	virtual IDuiByteStream* GetStream(LPCWSTR lpszFileName, BOOL bCache = TRUE);


	virtual IDuiImage* GetImage(LPCWSTR lpszImage);
	virtual VOID AddImage(IDuiImage* Image);
	virtual	VOID RemoveImage(LPCWSTR lpszName);

	virtual VOID AddFont(LPCWSTR lpszFontName, IDuiFont* pFont);
	virtual VOID RemoveFont(LPCWSTR lpszFontName);
	virtual IDuiFont* GetFont(LPCWSTR lpszFontName);

	virtual IDuiLangSet* GetLangSet();

private:
	CDuiString m_strPath;
	CDuiString m_strName;
	IDuiCore*  m_pCore;
	BOOL m_bOpen;

	typedef CDuiMap<CDuiString, IDuiByteStream*> CSkinStreamMap;
	CSkinStreamMap m_skinStreamMap;
	DECLARE_AUTOLOCK_CS(m_skinStreamMap);

	typedef CDuiMap<CDuiString, IDuiImage*> CImageMap;
	CImageMap	m_imageMap;
	DECLARE_AUTOLOCK_CS(m_imageMap);

	typedef CDuiMap<CDuiString, IDuiFont*> CDuiFontMap;
	CDuiFontMap m_FontMap;
	DECLARE_AUTOLOCK_CS(m_FontMap);

	IDuiLangSet* m_pDuiLangSet;
};

