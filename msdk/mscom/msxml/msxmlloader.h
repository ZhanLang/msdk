////////////////////////////////////////////////////////////////////////////////
/**
* @file
* IRsXMLLoader 接口实现, XML 的解析
* <p>项目名称：RsGUILib
* <br>文件名称：rsxmlloader.h
* <br>实现功能：IRsXMLLoader 接口实现
* <br>作    者：Dooyan
* <br>编写日期：2007-1-8 13:59:37
* <br>
* <br>修改历史（自下而上填写 内容包括：日期  修改人  修改说明）
*/
////////////////////////////////////////////////////////////////////////////////

// RsXMLLoader.h: interface for the CRsXMLLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSXMLLOADER_H__7F85A30B_E929_4989_A2E3_52E4244DDA96__INCLUDED_)
#define AFX_RSXMLLOADER_H__7F85A30B_E929_4989_A2E3_52E4244DDA96__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "XMLite.h"
#include "xmlconfig.h"

// XNode, *LPXNode			XML 节点	
// XNodes, *LPXNodes		XML 节点组
// XAttr, *LPXAttr			XML 节点属性	
// XAttrs					XML 节点属性组	
#include "IRsXMLLoader.h"
class CRsXMLLoader  : public IRsXMLLoader
{
public:
	CRsXMLLoader();
	virtual ~CRsXMLLoader();
	
public:
	virtual BOOL LoadXMLFiles(LPCTSTR lpPath, LPCTSTR lpFileName, PRISING_XML pBuff, UINT uSize = NULL, BOOL bLoadModuleXML = TRUE);
	virtual BOOL SaveXMLFiles(LPCTSTR lpPath, LPCTSTR lpFileName, PRISING_XML pBuff, UINT uSize = NULL, BOOL bSaveModuleXML = TRUE);

	virtual INT SetVerFunc(IN INT nIndex, IN BOOL bEnable, IN OUT LPTSTR lpVerFuncString, IN INT nMaxLen);
	virtual BOOL GetVerFunc(IN INT nIndex, IN LPCTSTR lpVerFuncString);

	INT VFA32TOB2(IN LPCTSTR lpA32, OUT LPTSTR lpB2, IN INT nLen);
	INT VFB2TOA32(IN LPCTSTR lpB2, IN INT nLen, OUT LPTSTR lpA32, IN INT n32Len);

	virtual BOOL Release() { delete this; return 1;};
private:
	/**
	* 组合文件全路径
	* <p>组合文件全路径
	* @param lpBuff				输出 Buffer
	* @param uBuffLen			输出 Buffer 的大小
	* @param lpPath				文件路径, 为 NULL 则使用内置路径
	* @param lpFileName			文件名
	* @return					成功返回 TRUE 
	*/
	BOOL CombinePathName(LPTSTR lpBuff, UINT uBuffLen, LPCTSTR lpPath, LPCTSTR lpFileName);
	/**
	* XML 文件内容预处理
	* <p> XML 文件内容预处理, 将其中会引起XML解析器错误的东西删除掉
	* @param rssFile			包含 XML 文件内容string
	* @return					成功返回 TRUE
	*/
	BOOL PreFormatXML(string &rssFile);

	// 读取文件， 并将文件内容填充到结构
	/**
	* 寻找模块
	* <p>寻找模块
	* @param lpCodeName			模块名称
	* @param ppComponentList	模块列表
	* @return					返回模块指针, 失败返回 NULL
	*/
	PRSCOMPONENT FindComponent(LPCTSTR lpCodeName, COMPONENT_LIST* ppComponentList);

	/**
	* 填充 PRISING_XML 结构
	* <p>根据XML内容 填充 PRISING_XML 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillAll(LPXNode lpNode, PRISING_XML pBuff);
	/**
	* 填充 PRODUCT_ATTRIBUTE 结构
	* <p>根据XML内容 填充 PRODUCT_ATTRIBUTE 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillProduct(LPXNode lpNode, PPRODUCT_ATTRIBUTE pBuff);
	/**
	* 填充 TARGETSYSTEM_LIST 结构
	* <p>根据XML内容 填充 TARGETSYSTEM_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillTargetSystem(LPXNode lpNode, TARGETSYSTEM_LIST* pBuff);
	/**
	* 填充 CONFLICT_LIST 结构
	* <p>根据XML内容 填充 CONFLICT_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillConflict(LPXNode lpNode, CONFLICT_LIST* pBuff);
	/**
	* 填充 DEFAULT_PARAMETER 结构
	* <p>根据XML内容 填充 DEFAULT_PARAMETER 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillDefault(LPXNode lpNode, PDEFAULT_PARAMETER pBuff);
	/**
	* 填充 RUNBEFOREINSTALL_LIST 结构
	* <p>根据XML内容 填充 RUNBEFOREINSTALL_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillRunBeforeInstall(LPXNode lpNode, RUNBEFOREINSTALL_LIST* pBuff);
	/**
	* 填充 RSCOMPONENT 结构
	* <p>根据XML内容 填充 RSCOMPONENT 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponent(LPXNode lpNode, PRSCOMPONENT pBuff);
	/**
	* 填充 COMPONENT_ATTRIBUTE 结构
	* <p>根据XML内容 填充 COMPONENT_ATTRIBUTE 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentAttrib(LPXNode lpNode, PCOMPONENT_ATTRIBUTE pBuff);
	/**
	* 填充 COMPONENT_TREE 结构
	* <p>根据XML内容 填充 COMPONENT_TREE 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @param ppComponentList	组件列表
	* @param pParent			目前树节点			
	* @return 
	*/
	BOOL xFillComponentTree(LPXNode lpNode, PCOMPONENT_TREE pBuff, COMPONENT_LIST *ppComponentList, PCOMPONENT_TREE_ITEM pParent = NULL);
	/**
	* 填充 COMPONENT_LIST 结构
	* <p>根据XML内容 填充 COMPONENT_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentList(LPXNode lpNode, COMPONENT_LIST* pBuff);
	/**
	* 填充 DEPEND_LIST 结构
	* <p>根据XML内容 填充 DEPEND_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
//	BOOL xFillComponentVersion(LPXNode lpNode, VERSION_LIST* pBuff);
	BOOL xFillComponentDepend(LPXNode lpNode, DEPEND_LIST* pBuff);
	/**
	* 填充 COMCONFLICT_LIST 结构
	* <p>根据XML内容 填充 COMCONFLICT_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentConflict(LPXNode lpNode, COMCONFLICT_LIST* pBuff);
	/**
	* 填充 FILES_LIST 结构
	* <p>根据XML内容 填充 FILES_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentFiles(LPXNode lpNode, FILES_LIST* pBuff);
	/**
	* 填充 REGISTS_LIST 结构
	* <p>根据XML内容 填充 REGISTS_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentRegisters(LPXNode lpNode, REGISTS_LIST* pBuff);
	/**
	* 填充 LINKS_LIST 结构
	* <p>根据XML内容 填充 LINKS_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentLinks(LPXNode lpNode, LINKS_LIST* pBuff);
	/**
	* 填充 RUNAFTERINSTALL_LIST 结构
	* <p>根据XML内容 填充 RUNAFTERINSTALL_LIST 结构
	* @param lpNode				XML 对应节点
	* @param pBuff				数据结构指针
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentRunLast(LPXNode lpNode, RUNAFTERINSTALL_LIST* pBuff);
	
	/**
	* 获得 XML 节点属性内容
	* <p>获得 XML 节点属性内容
	* @param lpNode				XML 节点
	* @param attrname			XML 节点属性名
	* @param lpBuff				输出 Buffer
	* @param uSize				输出 Buffer 大小
	* @return					成功返回 TRUE
	*/
	BOOL xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname, LPTSTR lpBuff, UINT uSize);
	/**
	* 获得 XML 节点属性值
	* <p>获得 XML 节点属性值
	* @param lpNode				XML 节点
	* @param attrname			XML 节点属性名
	* @param uValue				输出 数值
	* @return					成功返回 TRUE
	*/
	BOOL xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname, UINT& uValue);

	// 保存文件， 将结构内容填充到文件
	/**
	* 设定 XML 节点属性内容
	* <p>设定 XML 节点属性内容
	* @param lpNode				XML 节点
	* @param attrname			XML 节点属性名
	* @param lpBuff				新的节点属性内容
	* @return 
	*/
	BOOL xSetNodeAttr(LPXNode lpNode, LPCTSTR attrname, LPCTSTR lpBuff);
	/**
	* 设定 XML 节点属性值
	* <p>设定 XML 节点属性值
	* @param lpNode				XML 节点
	* @param attrname			XML 节点属性名
	* @param uValue				输出 数值
	* @return 
	*/
	BOOL xSetNodeAttr(LPXNode lpNode, LPCTSTR attrname, UINT uValue);
	/**
	* 创建新的子节点
	* <p>创建新的子节点
	* @param lpNode				新的节点的父节点
	* @param szNodename			新的节点名称
	* @param lpBuff				新的节点内容
	* @return					新节点的指针
	*/
	LPXNode xCreateChildNode(LPXNode lpNode, LPCTSTR szNodename, LPCTSTR lpBuff);

	/**
	* 根据 RSCOMPONENT 结构填充 XML 内容
	* <p>根据 RSCOMPONENT 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponent(PRSCOMPONENT pBuff, LPXNode lpParentNode);
	/**
	* 根据 COMPONENT_ATTRIBUTE 结构填充 XML 内容
	* <p>根据 COMPONENT_ATTRIBUTE 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentAttrib(PCOMPONENT_ATTRIBUTE pBuff, LPXNode lpParentNode);
	/**
	* 根据 VERSION_LIST 结构填充 XML 内容
	* <p>根据 VERSION_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
//	BOOL xFillComponentVersion(VERSION_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 DEPEND_LIST 结构填充 XML 内容
	* <p>根据 DEPEND_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentDepend(DEPEND_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 COMCONFLICT_LIST 结构填充 XML 内容
	* <p>根据 COMCONFLICT_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentConflict(COMCONFLICT_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 FILES_LIST 结构填充 XML 内容
	* <p>根据 FILES_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentFiles(FILES_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 REGISTS_LIST 结构填充 XML 内容
	* <p>根据 REGISTS_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentRegisters(REGISTS_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 LINKS_LIST 结构填充 XML 内容
	* <p>根据 LINKS_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentLinks(LINKS_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 RUNAFTERINSTALL_LIST 结构填充 XML 内容
	* <p>根据 RUNAFTERINSTALL_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentRunLast(RUNAFTERINSTALL_LIST* pBuff, LPXNode lpParentNode);

	/**
	* 根据 PRISING_XML 结构填充 XML 内容
	* <p>根据 PRISING_XML 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillAll(PRISING_XML pBuff, LPXNode lpParentNode);
	/**
	* 根据 PPRODUCT_ATTRIBUTE 结构填充 XML 内容
	* <p>根据 PPRODUCT_ATTRIBUTE 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillProduct(PPRODUCT_ATTRIBUTE pBuff, LPXNode lpParentNode);
	/**
	* 根据 TARGETSYSTEM_LIST 结构填充 XML 内容
	* <p>根据 TARGETSYSTEM_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillTargetSystem(TARGETSYSTEM_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 CONFLICT_LIST 结构填充 XML 内容
	* <p>根据 CONFLICT_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillConflict(CONFLICT_LIST* pBuff, LPXNode lpParentNode);
	/**
	* 根据 PDEFAULT_PARAMETER 结构填充 XML 内容
	* <p>根据 PDEFAULT_PARAMETER 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillDefault(PDEFAULT_PARAMETER pBuff, LPXNode lpParentNode);
	/**
	* 根据 RUNBEFOREINSTALL_LIST 结构填充 XML 内容
	* <p>根据 RUNBEFOREINSTALL_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillRunBeforeInstall(RUNBEFOREINSTALL_LIST* pBuff, LPXNode lpParentNode);

	/**
	* 根据 PCOMPONENT_TREE 结构填充 XML 内容
	* <p>根据 PCOMPONENT_TREE 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param pParent			数据结构中该树节点的父节点指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentTree(PCOMPONENT_TREE pBuff, PCOMPONENT_TREE_ITEM pParent, LPXNode lpParentNode);
	/**
	* 根据 COMPONENT_LIST 结构填充 XML 内容
	* <p>根据 COMPONENT_LIST 结构填充 XML 内容
	* @param pBuff				数据结构指针
	* @param lpParentNode		XML 对应的父节点
	* @return					成功返回 TRUE
	*/
	BOOL xFillComponentList(COMPONENT_LIST* pBuff, LPXNode lpParentNode);

private:

	TCHAR m_szPathName[MAX_PATH];
	BOOL m_bSaveModuleFiles;
	BOOL m_bLoadModuleFiles;
};

#endif // !defined(AFX_RSXMLLOADER_H__7F85A30B_E929_4989_A2E3_52E4244DDA96__INCLUDED_)
