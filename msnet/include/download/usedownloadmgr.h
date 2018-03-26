#pragma once
#include "downloadmgr_i.h"
#include <list>




//œ¬‘ÿƒ£øÈ∞Ô÷˙¿‡
class CDownloadModuleHelp
{
	typedef IDownLoadMgr*( *pCreateDownLoadMgr) (IDownLoadNotify* pNotify, LPCWSTR lpNameSpace /*= NULL*/,BOOL bCache /*= TRUE*/);
	typedef VOID (*pDistoryDownLoadMgr)(IDownLoadMgr** pMgr);
public:
	CDownloadModuleHelp(LPCTSTR lpDllPath = NULL):
	  m_hModule(NULL),
		m_pCreateDownLoadMgr(NULL),
		m_pDistoryDownLoadMgr(NULL)
	{
		Init(lpDllPath);
	}
	~CDownloadModuleHelp()
	{
		UnInit();
	}

	BOOL Init(LPCTSTR lpDllPath = NULL)
	{
		if (lpDllPath)
		{
			m_hModule = LoadLibrary(lpDllPath);
			m_pCreateDownLoadMgr=(pCreateDownLoadMgr)GetProcAddress(m_hModule,"CreateDownLoadMgr");
			m_pDistoryDownLoadMgr=(pDistoryDownLoadMgr)GetProcAddress(m_hModule,"DistoryDownLoadMgr");

			return m_hModule && m_pCreateDownLoadMgr && m_pDistoryDownLoadMgr;
		}
		return FALSE;
	}

	BOOL UnInit()
	{
		m_pCreateDownLoadMgr = NULL;
		m_pDistoryDownLoadMgr=NULL;
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}

		return TRUE;
	}

	IDownLoadMgr*  CreateDownLoadMgr(IDownLoadNotify* pNotify, LPCWSTR lpNameSpace = NULL,BOOL bCache =TRUE)
	{
		if (m_pCreateDownLoadMgr)
		{
			return m_pCreateDownLoadMgr(pNotify,lpNameSpace,bCache);
		}
		return NULL;
	}

	VOID DistoryDownLoadMgr(IDownLoadMgr** pMgr)
	{
		if (m_pDistoryDownLoadMgr)
		{
			m_pDistoryDownLoadMgr(pMgr);
		}
	}

private:
	HMODULE m_hModule;
	pCreateDownLoadMgr m_pCreateDownLoadMgr;
	pDistoryDownLoadMgr m_pDistoryDownLoadMgr;
};
