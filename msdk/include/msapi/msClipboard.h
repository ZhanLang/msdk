#pragma once
namespace msdk{;
namespace msapi{;

static BOOL WriteAsciiStringToClipboard(LPCWSTR sClipdata, HWND hOwningWnd)
{

	BOOL bRet = FALSE;

	do 
	{
		if ( !OpenClipboard(hOwningWnd))
			break;

		if ( !EmptyClipboard() )
			break;

		HGLOBAL hClipboardData = NULL;
		size_t sLen = wcslen(sClipdata);

		hClipboardData = GlobalAlloc(GMEM_DDESHARE, (sLen+1)*sizeof(wchar_t));
		if( !hClipboardData )
			break;

		wchar_t * pchData = (wchar_t*)GlobalLock(hClipboardData);
		if( !pchData )
			break;

		wcscpy_s(pchData, sLen+1, sClipdata);

		if ( !GlobalUnlock(hClipboardData) )
			break;

		if ( SetClipboardData(CF_UNICODETEXT, hClipboardData) )
		{
			bRet = TRUE;
			break;
		}
		
	} while (0);

	CloseClipboard();

	return bRet;
}

static int CopyFileToClipboard(const char szFileName[], DWORD dwDropEffect = DROPEFFECT_COPY)
{
	UINT uDropEffect;
	HGLOBAL hGblEffect;
	LPDWORD lpdDropEffect;
	DROPFILES stDrop;

	HGLOBAL hGblFiles;
	LPSTR lpData;

	//与剪贴板交互的数据格式并非标准剪贴板格式，因此需要注册
	uDropEffect = RegisterClipboardFormatA("Preferred DropEffect");
	//为剪贴板数据分配空间
	hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,sizeof(DWORD));
	lpdDropEffect = (LPDWORD)GlobalLock(hGblEffect);
	//设置操作的模式
	*lpdDropEffect = dwDropEffect;//复制; 剪贴则用DROPEFFECT_MOVE
	GlobalUnlock(hGblEffect);

	//剪贴板数据需要一个这个结构
	stDrop.pFiles = sizeof(DROPFILES);//文件列表相对于结构的偏移量
	stDrop.pt.x = 0;
	stDrop.pt.y = 0;
	stDrop.fNC = FALSE;
	stDrop.fWide = FALSE;//宽字符设置

	//分配数据空间，并且预留文件名的空间
	//文件列表为两个'\0'结尾，因此需要多分配2个字节
	hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,\
		sizeof(DROPFILES)+strlen(szFileName)+2);

	lpData = (LPSTR)GlobalLock(hGblFiles);
	memcpy(lpData,&stDrop,sizeof(DROPFILES));
	strcpy(lpData+sizeof(DROPFILES),szFileName);
	GlobalUnlock(hGblFiles);

	//普通的剪贴板操作
	OpenClipboard(NULL);
	EmptyClipboard();
	//设置剪贴板文件信息
	SetClipboardData(CF_HDROP,hGblFiles);
	//设置剪贴板操作信息
	SetClipboardData(uDropEffect,hGblEffect);
	CloseClipboard();

	return 1;
}
};};