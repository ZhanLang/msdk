#pragma once
#include <util/CRuleBuffer.h>

typedef DWORD SOFTID;
#define INVALID_SOFTID	0

#define SOFTLIB_DEFAULE		0x0001	//默认库
#define SOFTLIB_USER		0x0002	//用户库
#define SOFTLIB_ENUM		0x0004	//扫描

#define SOFTLIB_MERGE		0x10000	//使用SOFTLIB_ENUM有效，枚举结果和之前结果按名称合并

#define SOFTID_USER_BASE	1000000
#define SOFTID_ENUM_MASK	0x80000000

#pragma pack(push, 4)

typedef struct tag_softfile 
{
	WCHAR   strFile[MAX_PATH*2];
}ST_SOFTFILE;
typedef CRuleBuffer<ST_SOFTFILE> SOFTFILE_LIST;

enum
{
	PATHTYPE_FILE = 0,		//文件
	PATHTYPE_COM,			//COM

	PATHTYPE_NOFILE = 100,
	PATHTYPE_REG_KEY,		//注册表项
	PATHTYPE_REG_VALUE,		//注册表值
};
typedef struct tag_softpath 
{
	int		nType;
	WCHAR   strPath[MAX_PATH*2];
}ST_SOFTPATH;
typedef CRuleBuffer<ST_SOFTPATH> SOFTPATH_LIST;

enum
{
	//内置的具备的属性值
	SOFT_APPID = 0,	//标识是系统库（0）还是用户库（其他用户子自定义值）
	SOFT_PATH,		//路径
	SOFT_CLASS,		//类型
	SOFT_NAME,		//名称
	SOFT_COMPANY,	//公司
	SOFT_TYPE,		//
	SOFT_VERSION,	//版本
	
	//一下是设置用的
	SOFT_VERREG,	//描述版本的注册表
	SOFT_INSREG,	//描述是否安装的注册表


	

	// 下载地址
	SOFT_DLURL,




	SOFT_MORE = 100,
	SOFT_UNINSTALLSTR,	//卸载路径完整路径，包括命令行
	SOFT_REG_UNINSTALL,	//注册表卸载路径
	SOFT_VER,
	SOFT_SIZE,

	
	

	SOFT_WHITE,	//白名单，扫描的时候不做处理


	SOFT_CLEAN_LEAVE,	//清理级别
	
};


//处理白名单
enum 
{
	SOFT_WHITE_SOFT_NAME_WHOLEWORD,		//通过软件名称完全匹配
	SOFT_WHITE_SOFT_NAME,				//通过软件名不完全匹配

	SOFT_WHITE_SOFT_COMPANY_WHOLEWORD,	//通过公司名完全匹配
	SOFT_WHITE_SOFT_COMPANY,			//通过公司名不完全匹配
};


#pragma pack()


//扫描回调
interface ISoftCallback
{
	virtual VOID OnSoftFind( DWORD dwSoftID) = 0;	//一个软件被扫描到
	virtual VOID OnSoftClean( DWORD dwSoftID) = 0;		//一个软件被清理
};

interface ISoftScan
{
	STDMETHOD_(BOOL, Init)( /*WCHAR cRoot, LPCWSTR lpszBk*//*备份的目录*/) = 0;

	STDMETHOD_(BOOL, IsPe)() = 0;

	//枚举软件库, 同时支持全库或只枚举存在的
	STDMETHOD_(SOFTID, FindSoft)(BOOL bOnlyExist = TRUE, DWORD softLib = SOFTLIB_DEFAULE|SOFTLIB_USER|SOFTLIB_ENUM|SOFTLIB_MERGE) = 0;
	STDMETHOD_(SOFTID, FindNextSoft)() = 0;
	STDMETHOD_(BOOL,   IsExist)(SOFTID) = 0;

	STDMETHOD(ScanSoft)(LPCWSTR lpExpendPath, LPCWSTR lpRegPath, OUT LPWSTR lpPath, DWORD dwPathSize) = 0;
	STDMETHOD(ScanSoftSubPath)(SOFTID softid, LPCWSTR lpSubPath, SOFTFILE_LIST& fileList) = 0;

	//获取软件信息, 软件名称、厂商等等
	STDMETHOD_(LPCWSTR, GetSoftInfo)(SOFTID softid, DWORD dwKey) = 0;
	STDMETHOD_(LPCWSTR, EnumSoftInfo)(SOFTID softid, DWORD& dwKey, DWORD& enumPos) = 0; //第一个开始enumPos=-1
	STDMETHOD_(LPCWSTR, GetSoftClass)(DWORD softClass) = 0;

	//获取某软件相关的进程列表信息
	STDMETHOD(GetSoftFile)(SOFTID softid, SOFTFILE_LIST& fileList) = 0;

	//软件安装信息（方便强行卸载删除文件或注册表）
	STDMETHOD(GetSoftPath)(SOFTID softid, SOFTPATH_LIST& pathList) = 0;

	/*
	STDMETHOD(Clean)( DWORD dwSoftID, DWORD dwClean) = 0;

	STDMETHOD(DelTemp)() = 0;
	STDMETHOD(DelRun)()	= 0;
	*/

};
MS_DEFINE_IID(ISoftScan, "{407A7A84-A3E2-4c50-B145-9E0245C96C3E}");

typedef struct tag_softrule 
{
	WCHAR   strPath[MAX_PATH*2];
	WCHAR   strRegPath[MAX_PATH*2];
	DWORD	dwAppid;
}ST_SOFTRULE;
typedef CRuleBuffer<ST_SOFTRULE> SOFTRULE_LIST;

typedef struct tag_softAttribute
{
	DWORD dwKey;
	WCHAR strValue[1024];
}ST_SOFTATTRIBUTE;
typedef CRuleBuffer<ST_SOFTATTRIBUTE> SOFTATTRIBUTE_LIST;


typedef struct tag_softWhite
{
	DWORD whiteAttr;
	WCHAR strValue[MAX_PATH];
}ST_SOFT_WHITE;

typedef CRuleBuffer<ST_SOFT_WHITE> SOFT_WHITE_LIST;

interface ISoftEdit : public ISoftScan
{
	STDMETHOD_(SOFTID, FindSoftEx)(BOOL bOnlyExist, DWORD softLib, LPCWSTR lpCondition = NULL) = 0;
	STDMETHOD(GetCurMaxSoftid)(SOFTID* pdefSoftid, SOFTID* puserSoftid) = 0;
	STDMETHOD_(BOOL, IsHave)(SOFTID softid) = 0;
	STDMETHOD(DelSoft)(SOFTID softid) = 0;

	STDMETHOD(GetSoftRule)(SOFTID softid, SOFTRULE_LIST& ruleList) = 0;
	STDMETHOD(SetSoftRule)(SOFTID softid, SOFTRULE_LIST& ruleList) = 0;

	STDMETHOD(GetSoftAttribute)(SOFTID softid, SOFTATTRIBUTE_LIST& atttibuteList) = 0;
	STDMETHOD(SetSoftAttribute)(SOFTID softid, SOFTATTRIBUTE_LIST& atttibuteList) = 0;

	STDMETHOD(GetSoftFileRule)(SOFTID softid, SOFTFILE_LIST& fileList) = 0;
	STDMETHOD(SetSoftFileRule)(SOFTID softid, SOFTFILE_LIST& fileList) = 0;

	STDMETHOD(GetSoftPathRule)(SOFTID softid, SOFTPATH_LIST& pathList) = 0;
	STDMETHOD(SetSoftPathRule)(SOFTID softid, SOFTPATH_LIST& pathList) = 0;


	STDMETHOD(SetSoftWhite)( SOFT_WHITE_LIST& softWhite) = 0;
	STDMETHOD(GetSoftWhite)( SOFT_WHITE_LIST& softWhite) = 0;
};
MS_DEFINE_IID(ISoftEdit, "{9C0E71D4-C9DB-4db1-AED8-2C134A35515E}");


enum
{
	SOFT_CLEAN_ALL,

	SOFT_CLEAN_BK,	//清理并备份
};


extern "C"
{
	typedef ISoftScan*  (WINAPI *pCreate)(ISoftCallback* pCallback);
	ISoftScan* WINAPI CreateSoftEng(ISoftCallback* pCallback);

	typedef VOID( *pDestory)(ISoftScan* pSoftScan);
	VOID WINAPI DestorySoftEng( ISoftScan* pSoftScan);
};

