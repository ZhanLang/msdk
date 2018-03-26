
#pragma once
namespace DuiKit{;

struct IDuiImage : public IDuiObject
{
	virtual VOID SetName(LPCWSTR lpszName) = 0;
	virtual LPCWSTR GetName() = 0;

	virtual BOOL SetFile(LPCWSTR lpszFile) = 0;
	virtual LPCWSTR GetFile() = 0;

	virtual VOID SetDuiSkin(IDuiSkin* Skin) = 0;
	virtual IDuiSkin* GetDuiSkin() = 0;

	/*设置作用的范围*/
	virtual VOID SetAffectRect( CDuiRect AffectRect) = 0;
	virtual CDuiRect GetAffectRect() = 0;

	//属性是指图片安装scale9方式绘制
	virtual VOID SetScale9(CDuiRect Scale9Rect) = 0;
	virtual CDuiRect GetScale9() = 0;

	//属性是给不支持alpha通道的图片格式（如bmp）指定透明色
	virtual VOID SetMask(DWORD dwMask) = 0;
	virtual DWORD GetMask() = 0;

	//设置图片的透明度
	virtual VOID SetFade(BYTE fade) = 0; 
	virtual BYTE GetFade() = 0;

	//属性是指定scale9绘制时要不要绘制中间部分
	virtual VOID SetHole(BOOL bHole) = 0;
	virtual BOOL GetHole() = 0;

	//属性设置成true就是指定图片在x轴不要拉伸而是平铺
	virtual VOID SetXtiled(BOOL xtiled) = 0;
	virtual BOOL GetXtiled() = 0;

	//属性设置成true就是指定图片在y轴不要拉伸而是平铺
	virtual VOID SetYtiled(BOOL ytiled) = 0;
	virtual BOOL GetYtiled() = 0;


	//


	virtual BOOL Create() = 0;
	virtual HBITMAP GetHandle() = 0;

	virtual DWORD GetHeight() = 0;		//图片的原始高度
	virtual DWORD GetWidth() = 0;		//图片的原始宽度

	//是否支持透明
	virtual BOOL  IsAlphaChannel() = 0;
};

};//namespace DuiKit{;