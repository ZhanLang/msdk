#pragma once
namespace DuiKit{;

enum 
{
	PICTURE_SHOWTYPE_NULL = 0x0001,		//按原始比，从控件的右上角开始计算，图片保持原始大小
	PICTURE_SHOWTYPE_FULL = 0x0002,		//将控件填充满
	PICTURE_SHOWTYPE_CENTER = 0x0004,	//图片居中显示
	PICTURE_SHOWTYPE_ORIGINAL = 0x0008,	//原始尺寸
	PICTURE_SHOWTYPE_STRETCH = 0x0010,	//按比例缩放
};

struct IDuiPictureCtrl : public IDuiObject
{
	virtual VOID SetImage(LPCWSTR lpszFile) = 0;
	virtual LPCWSTR GetImage() = 0;

	virtual DWORD GetImageWidth() = 0;
	virtual DWORD GetImageHeigh() = 0;

	virtual VOID SetShowTyte(INT showType) = 0;
	virtual INT GetShowType() = 0;
};

};