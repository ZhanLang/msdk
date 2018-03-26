#pragma once

#include <softeng/softengdef.h>

class CUseSoftScan : ISoftCallback
{
public:
	CUseSoftScan(void);
	virtual ~CUseSoftScan(void);

	HRESULT Init( WCHAR cRoot );
	HRESULT Uninit();

	SOFTID FindSoft(BOOL bOnlyExist, DWORD softLib = SOFTLIB_DEFAULE|SOFTLIB_USER|SOFTLIB_ENUM|SOFTLIB_MERGE);
	SOFTID FindSoftEx(BOOL bOnlyExist, DWORD softLib = SOFTLIB_DEFAULE|SOFTLIB_USER|SOFTLIB_ENUM|SOFTLIB_MERGE, LPCTSTR lpCondition = NULL);
	SOFTID FindNextSoft();
	BOOL  IsExist(SOFTID softid);

	LPCWSTR GetSoftClass(DWORD softClass);
	HRESULT GetSoftFile(SOFTID softid, SOFTFILE_LIST& fileList);
	HRESULT GetSoftPath(SOFTID softid, SOFTPATH_LIST& pathList);

	LPCWSTR GetSoftInfo(SOFTID, DWORD dwKey);
	LPCWSTR EnumSoftInfo(SOFTID, DWORD& dwKey, DWORD& enumPos);

	HRESULT Clean(SOFTID , DWORD dwClean, LPCWSTR lpszBk);

public:
	ISoftScan* m_pSoftScan;
	ISoftScan* SoftPtr(){return m_pSoftScan;}

protected:
	virtual VOID OnSoftFind(DWORD dwSoftID);
	virtual VOID OnSoftClean(DWORD dwSoftID);

	HMODULE m_hEng;

	pCreate		m_pCreate;
	pDestory	m_pDestory;

	BOOL		m_bInit;
};
