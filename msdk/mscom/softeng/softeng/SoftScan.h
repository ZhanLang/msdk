#pragma once
#include <softeng/softengdef.h>
#include "SoftWareEnum.h"

#include ".\sqlite\CppSQLite3u.h"
#include <map>
#include <list>
#include <json/json.h>
struct ST_SOFT_RULE
{
	SOFTID	softid;
	DWORD	appid;				//标识是系统库（0）还是用户库（其他用户子自定义值）
	HKEY    hRootKey;			//主键
	CString	strSubKey;			//子键
	CString	strValueName;		//键值名称
	CString strExpendPath;		//修正路径，从注册表里取到路径后再用这个修正
};
typedef std::list<ST_SOFT_RULE> SOFTRULES;

typedef std::map<DWORD, CString>	KV_MAP;
struct ST_SOFT_INFO
{
	BOOL    bIsExist;
	KV_MAP  kvValue;
};
typedef std::map<SOFTID, ST_SOFT_INFO> SOFTINFO_MAP;

class CSoftScan : public ISoftEdit
{
public:
	CSoftScan(ISoftCallback* pCallback);
	virtual ~CSoftScan(void);

	//ISoftScan

	STDMETHOD_(BOOL, Init)( /*WCHAR cRoot, LPCWSTR lpszBk*//*备份的目录*/);

	STDMETHOD_(BOOL, IsPe)();

	//枚举软件库, 同时支持全库或只枚举存在的
	STDMETHOD_(SOFTID, FindSoft)(BOOL bOnlyExist = TRUE, DWORD softLib = SOFTLIB_DEFAULE|SOFTLIB_USER|SOFTLIB_ENUM|SOFTLIB_MERGE);
	STDMETHOD_(SOFTID, FindNextSoft)();
	STDMETHOD_(BOOL,   IsExist)(SOFTID);

	STDMETHOD(ScanSoft)(LPCWSTR lpExpendPath, LPCWSTR lpRegPath, LPWSTR lpPath, DWORD dwPathSize);
	STDMETHOD(ScanSoftSubPath)(SOFTID softid, LPCWSTR lpSubPath, SOFTFILE_LIST& fileList);

	//获取软件信息, 软件名称、厂商等等
	STDMETHOD_(LPCWSTR, GetSoftInfo)(SOFTID, DWORD dwKey);
	STDMETHOD_(LPCWSTR, EnumSoftInfo)(SOFTID, DWORD& dwKey, DWORD& enumPos);
	
	STDMETHOD_(LPCWSTR, GetSoftClass)(DWORD softClass);

	STDMETHOD(GetSoftFile)(SOFTID softid, SOFTFILE_LIST& fileList);
	STDMETHOD(GetSoftPath)(SOFTID softid, SOFTPATH_LIST& pathList);

	///ISoftEdit/////////////////////////////////////////////////////////
	STDMETHOD_(SOFTID, FindSoftEx)(BOOL bOnlyExist, DWORD softLib, LPCWSTR lpCondition = NULL);
	STDMETHOD(GetCurMaxSoftid)(SOFTID* pdefSoftid, SOFTID* puserSoftid);
	STDMETHOD(DelSoft)(SOFTID softid);
	STDMETHOD_(BOOL, IsHave)(SOFTID softid);

	STDMETHOD(GetSoftRule)(SOFTID softid, SOFTRULE_LIST& ruleList);
	STDMETHOD(SetSoftRule)(SOFTID softid, SOFTRULE_LIST& ruleList);

	STDMETHOD(GetSoftAttribute)(SOFTID softid, SOFTATTRIBUTE_LIST& atttibuteList);
	STDMETHOD(SetSoftAttribute)(SOFTID softid, SOFTATTRIBUTE_LIST& atttibuteList);

	STDMETHOD(GetSoftFileRule)(SOFTID softid, SOFTFILE_LIST& fileList);
	STDMETHOD(SetSoftFileRule)(SOFTID softid, SOFTFILE_LIST& fileList);

	STDMETHOD(GetSoftPathRule)(SOFTID softid, SOFTPATH_LIST& pathList);
	STDMETHOD(SetSoftPathRule)(SOFTID softid, SOFTPATH_LIST& pathList);

	STDMETHOD(SetSoftWhite)( SOFT_WHITE_LIST& softWhite);
	STDMETHOD(GetSoftWhite)( SOFT_WHITE_LIST& softWhite);

	//ISfotClean
	//STDMETHOD(Clean)( DWORD dwSoftID, DWORD dwClean);

	//STDMETHOD(DelTemp)();
//	STDMETHOD(DelRun)();

private:
	CString		m_strModulePath;
	BOOL		m_bIsSvc;
	//软件信息
	SOFTINFO_MAP	m_softInfo;
	SOFTINFO_MAP::const_iterator m_find;
	KV_MAP			m_softClass;

	SOFT_WHITE_LIST	m_softWiteList;

	HRESULT RefushSoftInfo(BOOL bOnlyExist, LPCWSTR lpCondition);
	HRESULT	RefushSoftClass();

	HRESULT GetSoftRules(SOFTRULES& rules, LPCWSTR lpCondition);
	HRESULT ScanSoft(CString &strPath, LPCTSTR lpExpendPath, HKEY hRootKey, LPCTSTR lpSubKey, LPCTSTR lpValueName);
	HRESULT ScanPath(CString &strPath, LPCTSTR lpExpendPath);
	HRESULT ScanService(CString &strPath, LPCTSTR lpSrvName);
	HRESULT GetSoftAttribute(SOFTID softid, KV_MAP &kvValue, DWORD softLib);

	HRESULT ParseSoftFile(LPCTSTR lpPath, LPCTSTR lpFile, SOFTFILE_LIST& fileList);
	HRESULT InsertFile(LPCTSTR lpFile, SOFTFILE_LIST& fileList);

	HRESULT ParseSoftPath(LPCTSTR lpPath, LPCTSTR lpBuf, int nType, SOFTPATH_LIST& pathList);
	HRESULT InsertPath(LPCTSTR lpPath, int nType, SOFTPATH_LIST& pathList);

	HRESULT EnumSoftInfo(BOOL bMerge);
	HRESULT CheckEnumSoft(const REG_SOFTWARE_INFO &softinfo, BOOL bMerge);

	HRESULT GetMaxSoftid(CppSQLite3DB& db, SOFTID &softid);

	STDMETHOD(SetSoftWhite)( DWORD softLib, SOFT_WHITE_LIST& softWhite);
	STDMETHOD(GetSoftWhite)( DWORD softLib, SOFT_WHITE_LIST& softWhite);


	//////////////////////////////////////////////////////////////////////////
	//清理动作
	//BOOL DelSoftFiles(Json::Value& root,DWORD dwSoftID, SOFTFILE_LIST& files);
	//BOOL DelSoftPaths(Json::Value& root,DWORD dwSoftID, SOFTPATH_LIST& dirs);

	/////////////////////////////////////////////////////////////
	//数据库通用操作
	BOOL  m_dbOK;
	BOOL  m_udbOK;
	CppSQLite3DB m_DB;		//系统库
	CppSQLite3DB m_uDB;		//用户库
	/////////////////////////////////////////////////////////////
	BOOL    Open(DWORD softLib);
	BOOL    OpenDB(DWORD softLib);
	VOID    CloseDB(DWORD softLib);
	HRESULT BeginTrans(DWORD softLib);
	HRESULT EndTrans(DWORD softLib, BOOL bFailedRollBack = TRUE) ;
	HRESULT RollBackTrans(DWORD softLib) ;

	//void    TableExists(); //判断表是否存在，没有就创建表
	HRESULT execDML(const TCHAR* sql, DWORD softLib);

	//setting
	BOOL m_bDebug;

	ISoftCallback* m_pCallback;
};


