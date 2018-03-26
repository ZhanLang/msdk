////////////////////////////////////////////////////////////////////////////////
/**
* @file
* XML 配置处理, IRsXMLProfile 接口功能实现
* <p>项目名称：RsGUILib
* <br>文件名称：xmlprofile.h
* <br>实现功能：将 XML 的读取 处理成 与 INI文件类似
* <br>作    者：Dooyan
* <br>编写日期：2007-1-8 13:36:22
* <br>
* <br>修改历史（自下而上填写 内容包括：日期  修改人  修改说明）
*/
////////////////////////////////////////////////////////////////////////////////

// CXMLProfile.h: interface for the CXMLProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CXMLProfile_H__988109CE_B80A_4C55_87E5_CEC50BC39566__INCLUDED_)
#define AFX_CXMLProfile_H__988109CE_B80A_4C55_87E5_CEC50BC39566__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XMLite.h"
#include "xmlconfig.h"
#include "IRsXMLProfile.h"

struct _tagXMLNode;
typedef _tagXMLNode XNode, *LPXNode;

// 将 XML 的读取 处理成 与 INI文件类似
class CXMLProfile : public IRsXMLProfile
{
public:
	CXMLProfile();
	virtual ~CXMLProfile();

public:

	virtual BOOL Open(IN LPCTSTR lpFileName, BOOL bCreate = FALSE);
	virtual BOOL Goto(IN LPCTSTR lpKey, BOOL bCreate = FALSE);

	virtual INT GetKeyCount(IN LPCTSTR lpKeyName);
	virtual LPCTSTR GetKeyName();
	virtual LPCTSTR GetSubKeyName(INT nIndex);
	virtual LPCTSTR GetValue();
	virtual LPCTSTR GetAttrib(IN LPCTSTR lpAtt);
	virtual LPCTSTR GetAttribName(IN INT nIndex);
	virtual INT GetAttribCount(IN LPCTSTR lpAttribName);
	virtual BOOL Close();

	virtual BOOL SetKeyName(IN LPCTSTR lpName);
	virtual BOOL SetValue(IN LPCTSTR lpValue);
	virtual BOOL SetAttrib(IN LPCTSTR lpAtt, IN LPCTSTR lpValue);
	virtual BOOL Save(IN LPCTSTR lpFileName);

	virtual BOOL Release() 
	{	delete this; return TRUE; };

private:
	BOOL PreFormatXML(IN OUT LPTSTR szBuff, IN INT nBuffSize);

	LPXNode m_pRoot;
	LPXNode m_pCurrent;
};

LPCTSTR GetPrivateXMLValue(IN LPCTSTR lpKey, IN LPCTSTR lpDef, OUT LPTSTR lpBuff, IN INT nSize, IN LPCTSTR lpFileName);
INT		GetPrivateXMLValueInt(IN LPCTSTR lpKey, IN INT nDef, IN LPCTSTR lpFileName);

LPCTSTR GetPrivateXMLAttrib(IN LPCTSTR lpKey, IN LPCTSTR lpAttr, IN LPCTSTR lpDef, OUT LPTSTR lpBuff, IN INT nSize, IN LPCTSTR lpFileName);
INT		GetPrivateXMLAttribInt(IN LPCTSTR lpKey, IN LPCTSTR lpAttr, IN INT nDef, IN LPCTSTR lpFileName);

BOOL SetPrivateXMLValue(IN LPCTSTR lpKey, IN LPTSTR lpBuff, IN LPCTSTR lpFileName);
BOOL SetPrivateXMLValueInt(IN LPCTSTR lpKey, IN INT nBuff, IN LPCTSTR lpFileName);

BOOL SetPrivateXMLAttrib(IN LPCTSTR lpKey, IN LPCTSTR lpAttr, IN LPTSTR lpBuff, IN LPCTSTR lpFileName);
BOOL SetPrivateXMLAttribInt(IN LPCTSTR lpKey, IN LPCTSTR lpAttr, IN INT nBuff, IN LPCTSTR lpFileName);

LPCTSTR GetPrivateXMLName(IN LPCTSTR lpKey, IN LPCTSTR lpDef, OUT LPTSTR lpBuff, IN INT nSize, IN LPCTSTR lpFileName);
BOOL SetPrivateXMLName(IN LPCTSTR lpKey, IN LPCTSTR lpBuff, IN LPCTSTR lpFileName);

INT GetPrivateXMLKeyCount(IN LPCTSTR lpKey, IN LPCTSTR lpBuff, IN LPCTSTR lpFileName);
INT GetPrivateXMLAttribCount(IN LPCTSTR lpKey, IN LPCTSTR lpBuff, IN LPCTSTR lpFileName);
#endif // !defined(AFX_CXMLProfile_H__988109CE_B80A_4C55_87E5_CEC50BC39566__INCLUDED_)
