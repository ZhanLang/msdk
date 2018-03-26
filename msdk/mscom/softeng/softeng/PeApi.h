#pragma once

//PE中常用的API
class CPeApi
{
public:
	CPeApi(void);
	~CPeApi(void);

public:	// static 方法

	static BOOL IsPe();		//判定是否为PE系统
	static BOOL IsWindows64();	//目标系统是否为X86
	static BOOL IsPeVistaLatter();	//PE是否为vista之后的版本
	static BOOL IsWindowsVistaLatter();	//目标系统是否为Vista之后版本

	//目录操作
	static BOOL		PathIsSystemFolder(LPCTSTR lpszPath);	//判定是否为系统目录
	static CString	GetRealPath( LPCTSTR lpszPath);	//获取实际的路径


	//注册表操作
	static CString GetRealRegPath(LPCTSTR lpszRegPath);	//获取实际的注册表路径
	static HKEY GetRootKey( HKEY hRoot );
	static CString GetFullRegPath( HKEY hRoot, LPCTSTR lpszSub);
	

private:
	static CString GetPeRealRegPath(LPCTSTR lpszRegPath);
	static CString GetLocalRealRegPath(LPCTSTR lpszRegPath);
};

class CPERegLoader
{
public:
	CPERegLoader(LPCTSTR szFilePath, LPCTSTR szName)
	{
		m_bLoadResult = FALSE;
		LONG lRet = RegLoadKey(HKEY_LOCAL_MACHINE, szName, szFilePath);
		if( ERROR_SUCCESS == lRet ) 
		{
			m_strName = szName;
			m_bLoadResult = TRUE;
		}
	}

	~CPERegLoader()
	{
		if( m_bLoadResult ) 
		{
			RegUnLoadKey(HKEY_LOCAL_MACHINE, m_strName);
		}
	}


	BOOL GetLoadStatus() {return m_bLoadResult;}
private:
	CString m_strName;
	BOOL m_bLoadResult;
};
