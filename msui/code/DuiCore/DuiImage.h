#pragma once

#include <DuiImpl/DuiString.h>
#include <DuiCore/IDuiImage.h>

using namespace DuiKit;
class CDuiImage : public IDuiImage
{
	/*属性*/
	CDuiString m_strName;
	CDuiString m_strFile;
	CDuiRect   m_AffectRect;
	CDuiRect   m_Scale9Rect;

	DWORD		m_dwMask;
	BYTE		m_Fade;
	BOOL		m_bHole;
	BOOL		m_bXtiled;
	BOOL		m_bYtiled;

	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiImage, OBJECT_CORE_IMAGE)
		DUI_DEFINE_INTERFACE(IDuiImage, IIDuiImage)
	DUI_END_DEFINE_INTERFACEMAP;

	DUI_BEGIN_SETATTRIBUTE(CDuiLang)
		DUI_SETATTRIBUTE_STRING_FUNCTION(name, SetName);
		DUI_SETATTRIBUTE_STRING_FUNCTION(file, SetFile);
		DUI_SETATTRIBUTE_RECT_FUNCTION(affect, SetAffectRect);
		DUI_SETATTRIBUTE_INT_FUNCTION(mask, SetMask);
		DUI_SETATTRIBUTE_RECT_FUNCTION(scale9, SetScale9);
		DUI_SETATTRIBUTE_BOOL_FUNCTION(hole, SetHole);
		DUI_SETATTRIBUTE_BOOL_FUNCTION(xtiled, SetXtiled);
		DUI_SETATTRIBUTE_BOOL_FUNCTION(ytiled, SetYtiled);
		DUI_SETATTRIBUTE_INT_FUNCTION(fade, SetFade);
	DUI_END_SETATTRIBUTE;

	virtual HRESULT DoCreate(IDuiObject* Parent, IDuiCore* Core,IDuiBuilder*pBuilder);


public:
	CDuiImage(void);
	~CDuiImage(void);

	virtual VOID SetName(LPCWSTR lpszName);
	virtual LPCWSTR GetName();

	virtual BOOL SetFile(LPCWSTR lpszFile);
	virtual LPCWSTR GetFile();

	virtual VOID SetDuiSkin(IDuiSkin* Skin);
	virtual IDuiSkin* GetDuiSkin();

	virtual VOID SetAffectRect( CDuiRect AffectRect);
	virtual CDuiRect GetAffectRect();

	virtual VOID SetScale9(CDuiRect Scale9Rect);
	virtual CDuiRect GetScale9();

	//设置图片的透明度
	virtual VOID SetFade(BYTE fade); 
	virtual BYTE GetFade();

	//属性是指定scale9绘制时要不要绘制中间部分
	virtual VOID SetHole(BOOL bHole);
	virtual BOOL GetHole();

	//属性设置成true就是指定图片在x轴不要拉伸而是平铺
	virtual VOID SetXtiled(BOOL xtiled);
	virtual BOOL GetXtiled();

	//属性设置成true就是指定图片在y轴不要拉伸而是平铺
	virtual VOID SetYtiled(BOOL ytiled);
	virtual BOOL GetYtiled();

	virtual VOID SetMask(DWORD dwMask);
	virtual DWORD GetMask();

	virtual BOOL Create();
	virtual HBITMAP GetHandle();

	virtual DWORD GetHeight();	//图片的原始高度
	virtual DWORD GetWidth();		//图片的原始宽度
	virtual BOOL  IsAlphaChannel();
private:
	static bool mem_to_global( const void *buf, size_t size, HGLOBAL global );
	static void CopyColor(DWORD* pDest, LPBYTE pSrc, int nBpp);

private:
	IDuiSkin* m_pSkin;
	HBITMAP	m_HBITMAP;
	BOOL	m_AlphaChannel;
	BOOL	m_dwWidth;
	BOOL	m_dwHeight;
	BOOL	m_bLoadFile;
};

