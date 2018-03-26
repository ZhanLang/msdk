#pragma once
namespace DuiKit{;


/*
皮肤接口文件，每一个CTRL都能拥有自己的皮肤对象
*/

enum SkinType
{
	SkinFile, //文件类型
	SkinZip,  
	SkinMemZip,
};

struct IDuiByteStream;
struct IDuiImage;
struct IDuiFont;

struct IDuiSkin : public IDuiObject
{
	virtual VOID SetName(LPCWSTR lpszName) = 0;
	virtual LPCWSTR GetName() = 0;

	virtual VOID SetCore(IDuiCore* Core) = 0;
	virtual IDuiCore* GetCore() = 0;

	virtual BOOL Open(LPVOID lpMem, DWORD dwSize, LPVOID lpParam = 0, DWORD dwParamSize = 0) = 0;
	virtual BOOL Close() = 0;

	virtual SkinType GetSkinType() = 0;
	virtual IDuiByteStream* GetStream(LPCWSTR lpszFileName, BOOL bCache = TRUE) = 0;


	virtual IDuiImage* GetImage(LPCWSTR lpszImage) = 0;
	virtual VOID AddImage(IDuiImage* Image) = 0;
	virtual	VOID RemoveImage(LPCWSTR lpszName) = 0;

	virtual VOID AddFont(LPCWSTR lpszFontName, IDuiFont* pFont) = 0;
	virtual VOID RemoveFont(LPCWSTR lpszFontName) = 0 ;
	virtual IDuiFont* GetFont(LPCWSTR lpszFontName) = 0;


	virtual IDuiLangSet* GetLangSet() = 0;
};




};//namespace DuiKit{;