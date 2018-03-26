/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#if !defined(AFX_FSSPEAKING_H__F6C6F409_5AA9_4555_8ECF_52ECC50EF0BE__INCLUDED_)
#define AFX_FSSPEAKING_H__F6C6F409_5AA9_4555_8ECF_52ECC50EF0BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

enum fsInetFileDialogDirection	
{
	IFDD_TOSERVER,				
	IFDD_FROMSERVER				
};

typedef void (*fntInetFileDialogFunc) (fsInetFileDialogDirection enDir, LPCSTR pszMsg, LPVOID lp1, LPVOID lp2);

class fsSpeaking  
{
public:
	void Mute (BOOL bMute);
	
	
	
	virtual void SetDialogFunc (fntInetFileDialogFunc pfn, LPVOID lpParam1, LPVOID lpParam2);
	fsSpeaking();
	virtual ~fsSpeaking();

protected:
	BOOL m_bMute;
	
	void DialogHttpResponse (HINTERNET hInet);
	
	void DialogFtpResponse ();
	
	
	
	virtual void Dialog (fsInetFileDialogDirection enDir, LPCSTR pszMsg);

	fntInetFileDialogFunc m_pfnDlgFunc;		
	LPVOID m_lpDlgParam1, m_lpDlgParam2;	
};

#endif 
