#include "StdAfx.h"
#include "UseSoftScan.h"

CUseSoftScan::CUseSoftScan(void) : m_hEng( NULL )
{
	m_bInit = FALSE;
}

CUseSoftScan::~CUseSoftScan(void)
{
}

HRESULT CUseSoftScan::Init( WCHAR cRoot )
{
	if ( m_bInit )
		return S_OK;

	m_hEng = LoadLibrary(_T("softeng.dll"));
	if ( m_hEng )
	{
		m_pCreate =  (pCreate)GetProcAddress(m_hEng, "CreateSoftEng");
		m_pDestory = (pDestory)GetProcAddress(m_hEng, "DestorySoftEng");
		if ( !( m_pCreate && m_pDestory ))
		{
			return E_FAIL;
		}
	}

	m_pSoftScan = m_pCreate(this);
	if ( !m_pSoftScan )
	{
		return E_FAIL;
	}

	m_bInit = TRUE;
	//return m_pSoftScan->Init( cRoot,NULL ) ? S_OK : E_FAIL;
	/*
	RASSERT(m_softEng.LoadMSCom(_T("softeng.dll"), TRUE), E_FAIL);
	RFAILED(m_softEng.CreateInstance(CLSID_SoftScan, NULL, NULL, __uuidof(ISoftScan), (void**)&m_pSoftScan));
	RASSERT(m_pSoftScan, E_FAIL);
	*/
	return S_OK;
}

HRESULT CUseSoftScan::Uninit()
{
	if(m_pSoftScan)
	{
		m_pSoftScan = NULL;
	}

	if ( m_pDestory && m_pSoftScan )
	{
		m_pDestory( m_pSoftScan );

	}
	FreeLibrary(m_hEng);

	return S_OK;
}

SOFTID CUseSoftScan::FindSoft(BOOL bOnlyExist, DWORD softLib/* = SOFTLIB_DEFAULE|SOFTLIB_USER|SOFTLIB_ENUM|SOFTLIB_MERGE*/)
{
	RASSERT(m_pSoftScan, INVALID_SOFTID);
	return m_pSoftScan->FindSoft(bOnlyExist, softLib);
}

SOFTID CUseSoftScan::FindSoftEx(BOOL bOnlyExist, DWORD softLib, LPCTSTR lpCondition)
{
	ISoftEdit* pSoftEdit = (ISoftEdit*)m_pSoftScan;
	RASSERT(pSoftEdit, INVALID_SOFTID);

	return pSoftEdit->FindSoftEx(bOnlyExist, softLib, lpCondition);
}

SOFTID CUseSoftScan::FindNextSoft()
{
	RASSERT(m_pSoftScan, INVALID_SOFTID);
	return m_pSoftScan->FindNextSoft();
}

BOOL CUseSoftScan::IsExist(SOFTID softid)
{
	RASSERT(m_pSoftScan, FALSE);
	return m_pSoftScan->IsExist(softid);
}

LPCWSTR CUseSoftScan::GetSoftClass(DWORD softClass)
{
	RASSERT(m_pSoftScan, NULL);
	return m_pSoftScan->GetSoftClass(softClass);
}

HRESULT CUseSoftScan::GetSoftFile(SOFTID softid, SOFTFILE_LIST& fileList)
{
	RASSERT(m_pSoftScan, E_FAIL);
	return m_pSoftScan->GetSoftFile(softid, fileList);
}

HRESULT CUseSoftScan::GetSoftPath(SOFTID softid, SOFTPATH_LIST& pathList)
{
	RASSERT(m_pSoftScan, E_FAIL);
	return m_pSoftScan->GetSoftPath(softid, pathList);
}

LPCWSTR CUseSoftScan::GetSoftInfo(SOFTID softid, DWORD dwKey)
{
	RASSERT(m_pSoftScan, NULL);
	return m_pSoftScan->GetSoftInfo(softid, dwKey);
}

LPCWSTR CUseSoftScan::EnumSoftInfo(SOFTID softid, DWORD& dwKey, DWORD& enumPos)
{
	RASSERT(m_pSoftScan, NULL);
	return m_pSoftScan->EnumSoftInfo(softid, dwKey, enumPos);
}



HRESULT CUseSoftScan::Clean(SOFTID dwSoftID, DWORD dwClean, LPCWSTR lpszBk)
{
// 	if ( m_pSoftScan )
// 		return m_pSoftScan->Clean(dwSoftID, dwClean);

	return E_FAIL;
}

VOID CUseSoftScan::OnSoftFind(DWORD dwSoftID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

VOID CUseSoftScan::OnSoftClean(DWORD dwSoftID)
{
	throw std::logic_error("The method or operation is not implemented.");
}
