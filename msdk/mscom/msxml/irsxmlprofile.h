#ifndef IRSXMLPROFILE_H
#define IRSXMLPROFILE_H

#include <objbase.h>

interface IRsXMLProfile
{
	/**
	* 打开 创建 XML 文件
	* <p>打开 创建 XML 文件
	* @param lpFileName			XML 文件绝对路径
	* @param bCreate			如果文件不存在，是否创建
	* @return					成功返回  TRUE, 否则返回 FALSE
	*/
	virtual BOOL Open(IN LPCTSTR lpFileName, BOOL bCreate = FALSE) = 0;
	
	/**
	* 定位 XML 节点
	* <p>定位 XML 节点	
	* @param lpKey				XML 节点路径 "procect2007\rav\rising"
	* @param bCreate			如果文件不节点，是否创建
	* @return					成功返回  TRUE, 否则返回 FALSE
	*/
	virtual BOOL Goto(IN LPCTSTR lpKey, BOOL bCreate = FALSE) = 0;

	/**
	* 获得当前子节点数量
	* <p>获得当前子节点数量
	* @param lpKeyName			指定子节点名称， NULL为所有子节点
	* @return					返回子节点数量
	*/
	virtual INT GetKeyCount(IN LPCTSTR lpKeyName) = 0;
	/**
	* 获得当前节点名字
	* <p>获得当前节点名字
	* @return					当前节点名字
	*/
	virtual LPCTSTR GetKeyName() = 0;
	/**
	* 获得子节点名字
	* <p>获得子节点名字
	* @param nIndex				该子节点的序号
	* @return					指定子节点名字
	*/
	virtual LPCTSTR GetSubKeyName(INT nIndex) = 0;
	/**
	* 获得节点内容
	* <p>获得节点内容
	* @return					返回节点内容
	*/
	virtual LPCTSTR GetValue() = 0;
	/**
	* 获得节电属性
	* <p>获得节电属性
	* @param lpAtt				属性的名字
	* @return					返回属性的内容
	*/
	virtual LPCTSTR GetAttrib(IN LPCTSTR lpAtt) = 0;
	/**
	* 获得节点属性名字
	* <p>获得节点属性名字
	* @param nIndex				节点属性序号
	* @return					返回属性的名字
	*/
	virtual LPCTSTR GetAttribName(IN INT nIndex) = 0;
	/**
	* 获得节点属性个数
	* <p>获得节点属性个数
	* @param lpAttribName		指定节点属性名称， NULL为所有节点属性
	* @return					节点属性的个数
	*/
	virtual INT GetAttribCount(IN LPCTSTR lpAttribName) = 0;
	/**
	* 关闭 XML 文件
	* <p>关闭 XML 文件
	* @return					永远 为 TRUE 
	*/
	virtual BOOL Close() = 0;

	/**
	* 设定当前节点名称
	* <p>设定当前节点名称
	* @param lpName				新的节点名称
	* @return					返回 TRUE 
	*/
	virtual BOOL SetKeyName(IN LPCTSTR lpName) = 0;
	/**
	* 设定当前节点内容
	* <p>设定当前节点内容
	* @param lpValue			新的节点内容
	* @return					返回 TRUE 
	*/
	virtual BOOL SetValue(IN LPCTSTR lpValue) = 0;
	/**
	* 设定当前节点属性
	* <p>设定当前节点属性
	* @param lpAtt				节点属性名字
	* @param lpValue			新的节点属性内容
	* @return 
	*/
	virtual BOOL SetAttrib(IN LPCTSTR lpAtt, IN LPCTSTR lpValue) = 0;
	/**
	* 保存到文件
	* <p>保存到文件
	* @param lpFileName			保存文件的全路径文件名
	* @return					
	*/
	virtual BOOL Save(IN LPCTSTR lpFileName) = 0;

	/**
	* 释放当前接口
	* <p>释放当前接口， 释放接口使用内存
	* @return					返回 TRUE
	*/
	virtual BOOL Release() = 0;
};

#endif // IRSXMLLORDER_H	