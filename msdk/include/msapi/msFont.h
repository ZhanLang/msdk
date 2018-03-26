#pragma once

namespace msdk{;
namespace msapi{;


static HANDLE AddFont(LPCTSTR szPath)
{
	HANDLE hFile = CreateFile( szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return NULL;
	}

	DWORD dwFileSize    = ::GetFileSize(hFile, NULL);
	BYTE* lpBuffer      = new BYTE[dwFileSize+1];

	DWORD dwReadSize    = 0;
	if (!ReadFile( hFile, lpBuffer, dwFileSize, &dwReadSize, NULL))
	{  
		delete[] lpBuffer;
		CloseHandle(hFile);
		return NULL;
	}

	lpBuffer[dwReadSize] = '\0';

	DWORD dwFontNumber  = 0;
	HANDLE hFont = AddFontMemResourceEx(lpBuffer, dwReadSize, 0, &dwFontNumber);
	delete[] lpBuffer;
	CloseHandle(hFile);

	return hFont;
}

static void FreeFont(HANDLE hFont)
{
	RemoveFontMemResourceEx(hFont);
}



};
};
