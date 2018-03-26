#pragma once

#include <Shellapi.h>

namespace msdk{;
namespace msapi{;

static BOOL ShellOpenFolder( LPCTSTR lpszFolder)
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	_stprintf_s(szBuffer, MAX_PATH,_T("/Open,\"%s\""), lpszFolder);
	BOOL bRet = FALSE;
	// Prepare shell execution params
	SHELLEXECUTEINFO shExecInfo   = { 0 };
	shExecInfo.cbSize             = sizeof(shExecInfo);
	shExecInfo.lpFile             = _T("explorer.exe");
	shExecInfo.lpParameters       = szBuffer;
	shExecInfo.nShow              = SW_SHOWNORMAL;
	shExecInfo.lpVerb             = _T("open"); // Context menu item
	shExecInfo.fMask              = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;

	try
	{
		bRet = ShellExecuteEx(&shExecInfo);
	}
	catch(...){}

	return bRet;
}

};};