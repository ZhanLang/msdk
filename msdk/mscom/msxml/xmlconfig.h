////////////////////////////////////////////////////////////////////////////////
/**
* @file
* RAV 结构定义
* <p>项目名称：RsGUILib
* <br>文件名称：xmlconfig.h
* <br>实现功能：RAV 结构定义
* <br>作    者：Dooyan
* <br>编写日期：2007-1-8 14:20:34
* <br>
* <br>修改历史（自下而上填写 内容包括：日期  修改人  修改说明）
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#pragma once
#pragma warning (disable : 4786)

#include <list>
#define MD5_LEN 33
using std::list;
#pragma warning (disable : 4183)
#pragma warning (default : 4786)


/*//********** 总结构图标，结构详细列表搜索相应结构  **********

	  总结构				产品属性
	tagRISING_XML	---- tagPRODUCT_ATTRIBUTE
					 |
					 |		互斥软件列表				互斥产品节点
					 |-- CONFLICT_LIST	----------- tagCONFLICT_ITEM
					 |								 ......
					 |		安装系统列表				产品安装系统节点
					 |-- TARGETSYSTEM_LIST	------- tagTARGETSYSTEM_ITEM
					 |								 .......
					 |		默认安装参数
					 |-- DEFAULT_PARAMETER 
					 |
					 |		安装前运行列表				安装前运行程序节点
					 |-- RUNBEFOREINSTALL_LIST ---- tagRUNBEFOREINSTALL_ITEM
					 |
					 |		列表型组件列表			 组件					组件属性
					 |-- COMPONENT_LIST	  ---------	tagCOMPONET	------- tagCOMPONENT_ATTRIBUTE
					 |								 .......		|	
					 |												|		组件依赖关系			依赖节点
					 | 												|-- DEPEND_LIST	------------- tagDEPEND_ITEM
					 |												|								......	
					 |												|		组件互斥关系			互斥节点
					 | 												|-- COMCONFLICT_LIST  ------- tagCOMCONFLICT_ITEM
					 |												|								......	
					 |												|		组件文件列表			文件节点
					 |												|-- FILES_LIST  ------------- tagFILES_ITEM
					 |												|								......	
					 |												|		组件注册表列表			注册表节点
					 |												|-- REGISTS_LIST ------------ tagREGISTS_ITEM
					 |												|								......	
					 |												|		组件链接列表			链接节点
					 |												|-- LINKS_LIST -------------- tagLINKS_ITEM
					 |												|								......	
					 |												|		组件安装后运行列表		运行节点
					 |												|-- RUNAFTERINSTALL_LIST ---- tagRUNAFTERINSTALL_ITEM
					 |																				......	
					 |	
					 |
					 |		树型组件列表		树节点						组件
					 |-- COMPONENT_TREE ------ tagCOMPONENT_TREE_ITEM ---- tagCOMPONET
												......			

//*/

template<class T>
class pointer_list : public list<T>
{
public:
	void remove_pitems()
	{
		for( pointer_list<T>::iterator i=begin(); i!=end(); i++ )
		{
			delete *i;
			*i = 0;
		}
	}
	~pointer_list()
	{
		remove_pitems();
	}
	
	pointer_list()
	{
	}
	
	// 杜元添加 禁止列表复制操作
	pointer_list(const pointer_list& _X)
	{
		DebugBreak();// 指针列表不能复制，因为此列表会自动释放指针指向内存
	}

	pointer_list& operator=(const pointer_list& _X)
	{
		DebugBreak();// 指针列表不能复制，因为此列表会自动释放指针指向内存
		return (*this); 
	}
};
#pragma warning( default : 4183 )

//////////////////////////////////////////////////////////////////////////
//	product settings

// 产品属性结构 
typedef struct tagPRODUCT_ATTRIBUTE
{
	UINT uNameID;				// 默认名称
	UINT uPreSetup;				// 安装背景图片资源
	UINT uBackBmpID;			// 安装背景图片资源
	UINT ulisenceID;			// 安装版权资源
	TCHAR szDefName[MAX_PATH];	// 默认名称
} PRODUCT_ATTRIBUTE, *PPRODUCT_ATTRIBUTE;

// 目标系统 (本产品允许安装的系统环境)
typedef struct tagTARGETSYSTEM_ITEM
{
	UINT  uMajorVersion;		// 系统主版本号， 不读入
	UINT  uMinorVersion;		// 系统小版本号， 不读入
	TCHAR szWindowName[64];		// 系统名称字符串
} TARGETSYSTEM_ITEM, *PTARGETSYSTEM_ITEM;

typedef pointer_list<PTARGETSYSTEM_ITEM> TARGETSYSTEM_LIST;
typedef TARGETSYSTEM_LIST::iterator TARGETSYSTEM_LIST_ITER;

// 互斥产品检查特征。
typedef struct tagCONFLICT_ITEM
{
	UINT uNameID;					// 软件名称资源ID
	TCHAR szRegkey[MAX_PATH];		// 注册表位置
	TCHAR szRegkeyName[MAX_PATH];	// 注册表键名
	UINT uRegkeyDataType;			// 键值类型
	TCHAR szRegkeyValue[MAX_PATH];	// 键值
	UINT uTestType;					// 键值检查类型, 0 键值对应，1 文件或者目录是否存在
} CONFLICT_ITEM, *PCONFLICT_ITEM;

typedef pointer_list<PCONFLICT_ITEM> CONFLICT_LIST;
typedef CONFLICT_LIST::iterator CONFLICT_LIST_ITER;

// 安装内置设置 - 默认安装时的参数
typedef struct tagDEFAULT_PARAMETER
{
	UINT uLanguageCode;				// 默认使用语言 PageCode
	UINT uGroupNameID;				// 建立程序组名称资源ID 2233
	UINT uUser;						// 当前产品使用用户 ALLUSER
	TCHAR szSerialNum[128];			// 默认序列号 EIWOJI-EISLNI-SDIOEI-DIEKLI 
	TCHAR szUserID[64];				// 默认用户ID 888888888888
	TCHAR szRisingRootPath[MAX_PATH];//	默认瑞星安装目录 C:\Program Files\Rising
	TCHAR szRisingPath[MAX_PATH];	//	默认瑞星注册表目录 %RISINGREGROOT%\SOFTWARE\Rising
	UINT bSilence;					// 是否静默安装 0
	UINT bAutoInstall;				// 是否自动安装 0
	UINT bScanMemory;				// 是否扫描内存 0
} DEFAULT_PARAMETER, *PDEFAULT_PARAMETER;

// 安装前运行的模块信息
typedef struct tagRUNBEFOREINSTALL_ITEM
{
	UINT uNameID;				// 运行名称ID
	UINT bMustRun;				// 是否必须运行
	UINT bWaitEnd;				// 是否等待此程序结束
	UINT bSelected;				// 默认显示是否选中
	UINT bShowSel;				// 是否在列表中显示
	UINT bIsServer;				// 是否限制Server才能运行
	UINT bRunOS;				// 文件是否要求特定系统才安装
	TCHAR szProcPathName[MAX_PATH * 2]; //程序运行路径和参数
	TCHAR szProcArguments[MAX_PATH]; //程序运行路径和参数
} RUNBEFOREINSTALL_ITEM, *PRUNBEFOREINSTALL_ITEM;
typedef pointer_list<PRUNBEFOREINSTALL_ITEM> RUNBEFOREINSTALL_LIST;
typedef RUNBEFOREINSTALL_LIST::iterator RUNBEFOREINSTALL_LIST_ITER;


//////////////////////////////////////////////////////////////////////////
//	component settings
// 组件属性
typedef struct tagCOMPONENT_ATTRIBUTE
{
	UINT uNameID;				// 组件名称ID
	UINT bMustInstall;			// 该组件是否必须安装
	UINT bShowSel;				// 此组件是否在组件选择列表中显示
	UINT bSelected;				// 此组件是否在组件选择列表中默认被选中
	UINT bShare;				// 此组件是否共享，目前没有使用
	UINT uDescripeID;			// 组件描述资源ID
	UINT uInstallID;			// 组件安装条件资源ID
	UINT uNeedSpace;			// 组件需要磁盘空间(单位：字节)
	UINT bIsServer;				// 该组件是否要求Server才运行
	UINT uType;					// 组件类型，1表示是工具
	TCHAR szUpDate[64];			// 此组件的更新日期
	TCHAR szVersion[64];		// 此组件的版本号
//	TCHAR szCheck[MD5_LEN];		// 此组件文件的 MD5 校验值
	TCHAR szCoreVersion[32];	// 此组件的核心版本
	TCHAR szCodeName[32];		// 此组件的代码名称
	TCHAR szReferrFile[MAX_PATH];// 组件的配置文件名
	TCHAR szUseDLL[MAX_PATH];	// 组件的专用动态库
	TCHAR szVerFunc[MAX_PATH];	// 组件的根据版本不同，是否能够使用
} COMPONENT_ATTRIBUTE, *PCOMPONENT_ATTRIBUTE;

// 组件运行依赖关系
typedef struct tagDEPEND_ITEM
{
	TCHAR szCodeName[32];		// 需要依赖的组件名称
	TCHAR szCoreVersion[32];	// 要求的依赖组件版本
} DEPEND_ITEM, *PDEPEND_ITEM;
typedef pointer_list<PDEPEND_ITEM> DEPEND_LIST;
typedef DEPEND_LIST::iterator DEPEND_LIST_ITER;

// 互斥产品检查特征。
typedef struct tagCOMCONFLICT_ITEM
{
	TCHAR szCodeName[32];		// 需要互斥检查的组件名称
	TCHAR szCoreVersion[32];	// 要求互斥检查的版本
} COMCONFLICT_ITEM, *PCOMCONFLICT_ITEM;
typedef pointer_list<PCOMCONFLICT_ITEM> COMCONFLICT_LIST;
typedef COMCONFLICT_LIST::iterator COMCONFLICT_LIST_ITER;

typedef struct tagFILES_ITEM
{
	BOOL bExectue;				// 是否是运行程序
	UINT bRunOS;				// 文件是否要求特定系统才安装
	UINT uRpSize;				// 文件升级 RP 包尺寸
	UINT uComDll;				// COM组件 安装时注册 卸载时先反注册
	UINT uMovEx;				// 0：是否需要重起取决于是否复制成功；1：就算复制失败也不提示重启，比如RavExt.dll；2无论是否替换成功，更新都需要重起，比如某些驱动文件hookhelp.sys hooknots.sys
	UINT uNoCheck;				// 无需校验此文件
	TCHAR szPath[MAX_PATH];		// 安装路径
	TCHAR szSourcePath[MAX_PATH];		// 原文件路径
	TCHAR szCheck[MD5_LEN];		// 文件 MD5 校验值
	TCHAR szFileName[MAX_PATH];	// 文件名
} FILES_ITEM, *PFILES_ITEM;
typedef pointer_list<PFILES_ITEM> FILES_LIST;
typedef FILES_LIST::iterator FILES_LIST_ITER;

typedef struct tagREGISTS_ITEM
{
	TCHAR szRegkey[MAX_PATH];		// 注册表键
	TCHAR szRegkeyName[MAX_PATH];	// 注册表键名
	TCHAR szRegkeyValue[MAX_PATH];	// 注册表值
	UINT bRunOS;					// 注册表是否要求特定系统才安装
	UINT uRegkeyDataType;			//键值类型
} REGISTS_ITEM, *PREGISTS_ITEM;
typedef pointer_list<PREGISTS_ITEM> REGISTS_LIST;
typedef REGISTS_LIST::iterator REGISTS_LIST_ITER;

typedef struct tagLINKS_ITEM
{
	UINT uNameID;					// 生成的.lnk文件名资源ID
	UINT uLinkSubDirID;				// 创建LINK的子路径资源ID
	UINT uDescripeID;				// LINK 目标的描述资源ID
	UINT bRunOS;					// LINK是否要求特定系统才安装
	TCHAR szLinkDir[MAX_PATH];		// 创建LINK的路径
	TCHAR szPath[MAX_PATH];			// 用于定位LINK目标目录
	TCHAR szFileName[MAX_PATH];		//目标路径
	TCHAR szArguments[32];			//LINK 目标的参数 
} LINKS_ITEM, *PLINKS_ITEM;
typedef pointer_list<PLINKS_ITEM> LINKS_LIST;
typedef LINKS_LIST::iterator LINKS_LIST_ITER;

typedef struct tagRUNAFTERINSTALL_ITEM
{
	UINT uNameID;					// 运行名称ID
	UINT bMustRun;					// 是否必须运行
	UINT bWaitEnd;					// 是否等待此程序结束后
	UINT bSelected;					// 默认显示是否选中
	UINT bShowSel;					// 是否在列表中显示
	UINT bRunOS;					// 运行是否要求特定系统才安装
	TCHAR szProcPath[MAX_PATH];		// 程序运行路径和参数
	TCHAR szProcFileName[MAX_PATH]; // 程序运行路径和参数
	TCHAR szProcArguments[MAX_PATH]; // 程序运行路径和参数
	UINT bIsServer;					// 是否限制SVR运行
} RUNAFTERINSTALL_ITEM, *PRUNAFTERINSTALL_ITEM;
typedef pointer_list<PRUNAFTERINSTALL_ITEM> RUNAFTERINSTALL_LIST;
typedef RUNAFTERINSTALL_LIST::iterator RUNAFTERINSTALL_LIST_ITER;

typedef struct tagCOMPONET
{
	COMPONENT_ATTRIBUTE attrib;		// 组件属性
	DEPEND_LIST depend;				// 组件依赖关系
	COMCONFLICT_LIST conflict;		// 组件互斥关系
	FILES_LIST files;				// 组件文件列表
	REGISTS_LIST regists;			// 组件注册表列表
	LINKS_LIST links;				// 组件链接列表
	RUNAFTERINSTALL_LIST runafterinstall;// 组件安装后运行列表
} RSCOMPONENT, *PRSCOMPONENT;

//////////////////////////////////////////////////////////////////////////
// tree
typedef struct tagCOMPONENT_TREE_ITEM
{
	typedef tagCOMPONENT_TREE_ITEM* PCOMPONENT_TREE_ITEM;	// 树节点定义
	PCOMPONENT_TREE_ITEM pParent;	// 父节点指针
	PCOMPONENT_TREE_ITEM pNext;		// 下一个节点指针
	PCOMPONENT_TREE_ITEM pChild;	// 子节点指针
	PRSCOMPONENT pComponent;			// 当前节点指向的模块
} COMPONENT_TREE_ITEM, *PCOMPONENT_TREE_ITEM;


// 树基类内容
typedef struct tagCOMPONENT_TREE_DATA
{
	COMPONENT_TREE_ITEM  Root;
	PCOMPONENT_TREE_ITEM pHot;
} COMPONENT_TREE_DATA, *PCOMPONENT_TREE_DATA;

typedef pointer_list<PRSCOMPONENT> COMPONENT_LIST;
typedef COMPONENT_LIST::iterator COMPONENT_LIST_ITER;

// 树型数据保存类
class tagCOMPONENT_TREE : public COMPONENT_TREE_DATA 
{
public:
	tagCOMPONENT_TREE()
	{pHot = &Root; ZeroMemory(&Root, sizeof(Root)); };

	virtual ~tagCOMPONENT_TREE()
	{ DestroyChild(&Root); };
	
public:
	static PCOMPONENT_TREE_ITEM InsertTreeItem(PCOMPONENT_TREE_ITEM pItemNew, PCOMPONENT_TREE_ITEM pParentItem = 0)
	{
		if(!pItemNew) return NULL;
		if(!pParentItem) return NULL;
		
		if(!pParentItem->pChild)//if parent has no child
		{
			pParentItem->pChild = pItemNew;
			pItemNew->pParent = pParentItem;
		}
		else
		{	
			//else find the last child of the parent item
			PCOMPONENT_TREE_ITEM pBrother;
			for (pBrother = pParentItem->pChild; pBrother->pNext; pBrother = pBrother->pNext)
				;
			pBrother->pNext = pItemNew;
			pItemNew->pParent = pParentItem;
		}
		return pItemNew;
	}

	 void DestroyChild(PCOMPONENT_TREE_ITEM pItem)
	 {
		 PCOMPONENT_TREE_ITEM pTmp = pItem->pChild;
		 PCOMPONENT_TREE_ITEM pTmp1 = 0;
		 if(!pTmp)
			 return;
		 
		 do
		 {
			 DestroyChild(pTmp);
			 pTmp1 = pTmp->pNext;
			 delete pTmp;
		 }
		 while(pTmp=pTmp1);
		 pItem->pChild = 0;
	 }

	PCOMPONENT_TREE_ITEM MoveRoot()
	{ pHot = &Root; return pHot; }

	PCOMPONENT_TREE_ITEM MoveChild()
	{
		if(pHot->pChild)
		{
			pHot= pHot->pChild;
			return pHot;
		}
		else
			return NULL;
	}
	
	PCOMPONENT_TREE_ITEM MoveNext()
	{
		if(pHot->pNext)
		{
			pHot = pHot->pNext;
			return pHot;
		}
		else
			return NULL;
	}
	
	PCOMPONENT_TREE_ITEM MoveParent()
	{
		if(pHot->pParent)
		{
			pHot = pHot->pParent;
			return pHot;
		}
		else
			return NULL;
	}

	PCOMPONENT_TREE_ITEM GetNode()
	{	return pHot; }
	
//	static PCOMPONENT_TREE_ITEM InsertTreeItem(PCOMPONENT_TREE_ITEM pItemNew, PCOMPONENT_TREE_ITEM pParentItem);
};
typedef class tagCOMPONENT_TREE	  COMPONENT_TREE;
typedef class tagCOMPONENT_TREE*  PCOMPONENT_TREE;

// 所有数据
typedef struct tagRISING_XML
{
	CONFLICT_LIST conflict_list;			// 互斥软件列表
	TARGETSYSTEM_LIST targetsystem_list;	// 产品安装
	PRODUCT_ATTRIBUTE product_attribute;	// 产品属性
	DEFAULT_PARAMETER default_parameter;	// 默认安装参数
	RUNBEFOREINSTALL_LIST runbeforeinstall_list;	// 安装前运行
	COMPONENT_TREE component_tree;			// 树型组件列表
	COMPONENT_LIST component_list;			// 列表型组件列表
	DWORD	dwReserved;
}RISING_XML, *PRISING_XML;

#endif //XML_CONFIG_H	