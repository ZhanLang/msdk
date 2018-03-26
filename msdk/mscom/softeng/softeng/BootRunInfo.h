#pragma once

#include <string>
#include <vector>
#include <list>
using namespace std;

#include <taskschd.h>
//#include "IExecuteJsonTask.h"

#include <Strsafe.h>
#include <atlstr.h>

#include "internal.h"

namespace BootRunInfo
{

	enum BOOT_RUN_TYPE
	{
		br_type_normal		= 0,
		br_type_registry	= 1,
		br_type_task		= 2,
		br_type_service		= 3,
	};

	typedef struct _tagUserInfo
	{
		std::wstring		strUserName;
		std::wstring		strSID;
		DWORD				dwIndex;
	}ST_UserInfo;

	typedef struct _tagBootRunItem 
	{
		std::wstring		strProName;		//进程名
		std::wstring		strPublisher;	
		std::wstring		strVer;
		BOOT_RUN_TYPE		runtype;
		DWORD				dwUser;

		_tagBootRunItem(){	}
		_tagBootRunItem(LPCWSTR lpszProName, LPCWSTR lpszPublisher, LPCWSTR lpszVer, BOOT_RUN_TYPE type, DWORD user)
		{
			strProName = L"";
			strPublisher = L"";
			strVer = L"";

			if(lpszProName)
				strProName = lpszProName;
			if(lpszPublisher)
				strPublisher = lpszPublisher;
			if(lpszVer)
				strVer = lpszVer;

			if( strProName.length()>0 && (strVer.length() == 0 || strPublisher.length() == 0) )
			{
				tstring ver;
				tstring publisher;
				if( GetFileVerAndCompany(lpszProName, ver, publisher) )
				{
					if(strVer.length() == 0)
					{
#if defined(UNICODE) || defined(_UNICODE)
						strVer = ver;
#else
						strVer = (LPCWSTR)_bstr_t(ver.c_str());
#endif
					}
					if(strPublisher.length() == 0)
					{
#if defined(UNICODE) || defined(_UNICODE)
						strPublisher = publisher;
#else
						strPublisher = (LPCWSTR)_bstr_t(publisher.c_str());
#endif
					}
				}
			}

			runtype = type;
			dwUser = user;
		}

	}ST_BootRunItem, * PST_BootRunItem;

	typedef struct _tagFileInfo
	{
		tstring		strDir;
		tstring		strFileName;
		tstring		strFullPath;
	}ST_FileInfo;
	typedef std::list<ST_FileInfo>	FileInfoList;

	class CBootRunInfo //: public IExecuteJsonTaskImpl
	{
	public:
		CBootRunInfo(void);
		~CBootRunInfo(void);

		//public:
		//STDMETHOD(Collect)(const Json::Value &jsRoot, Json::Value& result);

	protected:
		void Reset();
		void EnumOneDir(FileInfoList & filelist, LPCTSTR lpszDir, LPCTSTR lpszFilter, BOOL bTransLnk);

		void EnumTask();		//计划计划任务
		void EnumTaskOnNt6();
		void Nt6Enum(ITaskFolder* pRootFolder);
		void AddTaskItems(const CString& strTaskDir);

		//protected:
		//HRESULT GetAllInfo(const Json::Value &jsRoot, Json::Value& result);
		//HRESULT SerResult(const Json::Value &jsRoot, Json::Value& result);
	public:
		void GetAllUserSid();	//枚举用户
		std::vector<ST_UserInfo>		m_vtUserInfo;
	private:
		BOOL TestUserSid(WCHAR * strSid);

		BOOL GetImageName(LPCTSTR lpszValue, TCHAR * szImageName, DWORD dwLen);	//从启动cmd获取映像路径

		BOOL GetStartDir(CString& strDir, LPCTSTR lpszUserSID=NULL);		//
		bool ParseLink( LPCWSTR strLinkFile, LPWSTR szBuf, int cchMax );

		void AddBootInfoFromFileList(FileInfoList & filelist, BOOT_RUN_TYPE runtype, DWORD dwUser);
		void GetSidBySpecialWay();
	private:
		std::vector<PST_BootRunItem>	m_vtBootItems;
		//	std::map<std::wstring, DWORD>	m_mapSids;

	};

};	//namespace BootRunInfo