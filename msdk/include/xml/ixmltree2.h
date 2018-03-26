
#ifndef _IXMLTREEITEM2_H
#define _IXMLTREEITEM2_H

#include <mscom/msbase.h>

namespace msdk {

#define HXMLTREEITEM	LPVOID

#ifndef PURE
	#define PURE = 0
#endif

interface IXMLTree2 : public IUnknown
{
public:

	/**
	* Get Root
	* <p>Get Root TreeItem Handle
	* @return				Root TreeItem Handle, never be NULL
	*/
	virtual HXMLTREEITEM GetRoot() PURE;

	/**
	* Get Parent
	* <p>Get Parent TreeItem Handle
	* @param hItem			Special TreeItem
	* @return				Special TreeItem' Parent Handle, no parent will be NULL
	*/
	virtual HXMLTREEITEM GetParent(HXMLTREEITEM hItem) PURE;

	/**
	* Local treeItem
	* <p>Local treeItem as Targen PathName
	* @param hItem			Special TreeItem
	* @param lpPathName		Relative Path  eg:"Document\\itemPath\\itemName"
	* @param bCreate		Create if TreeItem not exist
	* @return				handle of Localed Item or NULL
	*/
	virtual HXMLTREEITEM LocalItem(HXMLTREEITEM hItem, LPCTSTR lpPathName, BOOL bCreate = FALSE) PURE;

	/**
	* Get Child
	* <p>Get Child TreeItem Handle
	* @param hItem			Special TreeItem	
	* @param lpChildName	Name of child item
	* @return				Special TreeItem' child Handle, no found it will be NULL
	*/
	virtual HXMLTREEITEM GetChild(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL) PURE;

	/**
	* Get Next Item
	* <p>Get Next TreeItem Handle
	* @param hItem			Special TreeItem	
	* @param lpChildName	Name of child item
	* @return				Special TreeItem' Next Handle, no found it will be NULL
	*/
	virtual HXMLTREEITEM GetNextItem(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL) PURE;
	/**
	* Insert TreeItem after Special TreeItem	
	* <p>Insert TreeItem after Special TreeItem
	* @param hItem			Special TreeItem
	* @param lpName			Name of new TreeItem
	* @return				Handle of new TreeItem
	*/
	virtual HXMLTREEITEM InsertAfterItem(HXMLTREEITEM hItem, LPCTSTR lpName) PURE;
	/**
	* Insert TreeItem before Special TreeItem	
	* <p>Insert TreeItem before Special TreeItem
	* @param hItem			Special TreeItem
	* @param lpName			Name of new TreeItem
	* @return				Handle of new TreeItem
	*/
	virtual HXMLTREEITEM InsertBeforeItem(HXMLTREEITEM hItem, LPCTSTR lpName) PURE;
	/**
	* Insert TreeItem as Special TreeItem' last child
	* <p>Insert TreeItem as Special TreeItem' last child
	* @param hItem			Special TreeItem
	* @param lpName			Name of new TreeItem
	* @return				Handle of new TreeItem
	*/
	virtual HXMLTREEITEM InsertChild(HXMLTREEITEM hItem, LPCTSTR lpName) PURE;
	/**
	* delete treeitem
	* <p>delete treeitem 
	* @param hItem			Special TreeItem
	* @return				deleted return TRUE
	*/
	virtual BOOL DeleteItem(HXMLTREEITEM hItem) PURE;

	// TreeItem Attributes
	/**
	* <p><b>struct of XML file </b><br>
	* @code 
	<--Comment-->
	<Name AttName="AttValue">Text</Name>
	*endcode 
	*/
	virtual LPCTSTR GetText(HXMLTREEITEM hItem) PURE;
	virtual LPCTSTR GetName(HXMLTREEITEM hItem) PURE;
	virtual LPCTSTR GetAttribute(HXMLTREEITEM hItem, LPCTSTR lpAttName = NULL) PURE;
	virtual INT GetAttributeInt(HXMLTREEITEM hItem, INT nDefValue =0,LPCTSTR lpAttName = NULL) PURE;
	virtual INT GetAttributeCount(HXMLTREEITEM hItem) PURE;
	virtual LPCTSTR GetAttribute(HXMLTREEITEM hItem, INT nIndex, LPTSTR pName, INT nLen) PURE;
	virtual LPCTSTR GetComment(HXMLTREEITEM hItem) PURE;
	virtual BOOL SetName(HXMLTREEITEM hItem, LPCTSTR lpName) PURE;
	virtual BOOL SetText(HXMLTREEITEM hItem, LPCTSTR lpText) PURE;
	virtual BOOL SetAttribute(HXMLTREEITEM hItem, LPCTSTR lpAttName, LPCTSTR lpAttValue = NULL) PURE;
	virtual BOOL SetAttributeInt(HXMLTREEITEM hItem, LPCTSTR lpAttName, INT nValue = 0) PURE;
	virtual HXMLTREEITEM SetComment(HXMLTREEITEM hItem, LPCTSTR lpszComment = NULL) PURE;

	// XML file 
	/**
	* Load XML from File
	* <p>Load XML from File
	* @param lpFileName			file name to load as xml file
	* @return					success by return TRUE
	*/
	virtual BOOL Load(LPCTSTR lpFileName) PURE;

	/**
	* Load XML from buffer
	* <p>Load XML from buffer
	* @param lpFileName			file buffer to load as xml content
	* @return					success by return TRUE
	*/
	virtual BOOL LoadBuff(LPCTSTR lpFileBuff) PURE;
	/**
	* Save XML to File
	* <p>Save XML to File
	* @param lpFileName			file name to Save as xml file
	* @return					success by return TRUE
	*/
	virtual BOOL Save(LPCTSTR lpFileName) PURE;

	/**
	* Release Interface
	* <p>Release Interface and destory this
	* @return		success return TRUE
	*/
	//virtual BOOL Release() PURE;


	virtual BOOL SetDocEncoding(LPCTSTR lpEncoding) PURE;

	// Added by zhangming1 2009/11/20


	/**
	* Get GetChildCount
	* <p>Get Child node count with specified name
	* @param hItem			Special TreeItem	
	* @param lpChildName	Name of child item, NULL to get all
	* @return				child count
	*/
	virtual INT	GetChildCount(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL) PURE;

	/**
	* Get FreeXMLString
	* For any XML String from return value, call this funcion to release it
	*/
	virtual VOID FreeXMLString(LPCTSTR lpXML) PURE;
};

#ifndef _UNICODE 
struct __declspec(uuid("{B043ECF0-FECE-4764-A004-17B9F682F3EF}")) IXMLTree2 ;
#else
struct __declspec(uuid("{A125909B-9C40-4762-8609-F166B58E4332}")) IXMLTree2 ;
#endif 

} //namespace msdk

#endif //_IXMLTREEITEM2_H