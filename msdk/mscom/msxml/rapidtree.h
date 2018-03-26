
#pragma once

#define RAPIDXML_STATIC_POOL_SIZE	(8 * 1024)
#define RAPIDXML_DYNAMIC_POOL_SIZE	(8 * 1024)

#include <xml/ixmltree3.h>
#include <mscom/mscom_ex.h>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <fstream>
#include <vector>

using namespace rapidxml;
using namespace std;

#define TESTIFNULL_RETURN_NULL(X)	 if(!X) return NULL;
#define TESTIFNULL_AS_ROOT(XITEM)	 if(!XITEM) XITEM = GetRoot();

#define CASE_SENSITIVE	TRUE

//如果加上这里，貌似解析不能正常进行
const INT PARSE_FLAG = /*parse_declaration_node|parse_no_entity_translation*/parse_full;


// xmltree implementation with rapidXML
class CRapidTree : public IXMLTree3, public CUnknownImp
{
public:
	typedef xml_node<TCHAR>			rapid_node;
	typedef xml_document<TCHAR>		rapid_doc;
	typedef xml_attribute<TCHAR>	rapid_attr;

public:
	UNKNOWN_IMP2(IXMLTree2, IXMLTree3);

public:
	CRapidTree()
	{ 
		memset(m_szEncoding, 0, sizeof(m_szEncoding));
	};

	virtual ~CRapidTree(){};

	STDMETHOD(init_class)(IUnknown* prot, IUnknown* punkOuter)
	{
		return S_OK;
	}

	virtual HXMLTREEITEM GetRoot()
	{
		HXMLTREEITEM hRoot = (rapid_node*)&m_Doc;
		//INT nCount = GetChildCount(hRoot);
		//if(!nCount)
		//{
		//	// Create declaration
		//	rapid_node *declaration = m_Doc.allocate_node(node_declaration);

		//	rapid_attr *attr = m_Doc.allocate_attribute(_T("version"), _T("1.0"));
		//	declaration->append_attribute(attr);

		//	if(*m_szEncoding)
		//	{
		//		attr = m_Doc.allocate_attribute(_T("encoding"), m_szEncoding);
		//		declaration->append_attribute(attr);
		//	}
		//	
		//	m_Doc.append_node( declaration );
		//}
		return hRoot;
	}

	virtual INT	GetChildCount(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		rapid_node* pChild = pNode->first_node( lpChildName, 0, CASE_SENSITIVE );

		INT nCount = 0;
		for( ; pChild; pChild=pChild->next_sibling(lpChildName, 0, CASE_SENSITIVE) )
		{
			if(IsTreeItem(pChild))
				nCount ++;
		}

		return nCount;
	}

	virtual BOOL SetDocEncoding(LPCTSTR lpEncoding)
	{
		_tcscpy_s(m_szEncoding, MAX_PATH, lpEncoding);
		return TRUE;
	}

	virtual HXMLTREEITEM GetParent(HXMLTREEITEM hItem)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		return pNode->parent();
	}

	virtual HXMLTREEITEM GetChild(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		rapid_node*	pChild = (rapid_node*)pNode->first_node( lpChildName, 0, CASE_SENSITIVE );

		if(!pChild)
			return NULL;
		else if(IsTreeItem(pChild))
			return pChild;
		else 
			return GetNextItem(pChild, lpChildName);
	}

	virtual HXMLTREEITEM GetPrevItem(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;

		do 
		{
			pNode = pNode->previous_sibling( lpChildName, 0, CASE_SENSITIVE );
		} while(pNode && !IsTreeItem(pNode));

		return pNode;
	}

	virtual HXMLTREEITEM GetNextItem(HXMLTREEITEM hItem, LPCTSTR lpChildName = NULL)
	{
		TESTIFNULL_RETURN_NULL(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		TESTIFNULL_RETURN_NULL( pNode->parent() );

		do 
		{
			pNode = pNode->next_sibling( lpChildName, 0, CASE_SENSITIVE );
		} while(pNode && !IsTreeItem(pNode));

		return pNode;
	}

	virtual HXMLTREEITEM LocalItem(HXMLTREEITEM hItem, LPCTSTR lpPathName, BOOL bCreate = FALSE)
	{
		TESTIFNULL_AS_ROOT(hItem);
		if(!hItem)
			return NULL;

		tstring sPathName = lpPathName;
		if(sPathName.size() && *(sPathName.end() - 1) != _T('\\'))
			sPathName += _T("\\");

		INT nPath = 0;
		HXMLTREEITEM hItemSave = NULL;
		while ((nPath = (INT)sPathName.find(_T('\\'))) != -1)
		{
			sPathName[nPath] = 0;

			if(bCreate)
				hItemSave = hItem;

			hItem = GetChild(hItem, sPathName.c_str());

			if(!hItem)
			{
				if(bCreate)
					hItem = InsertChild(hItemSave, sPathName.c_str());
				else
					break;
			}

			sPathName = sPathName.substr(nPath + 1);
			//sPathName = &sPathName[nPath + 1];
// 			if (sPathName.length() - nPath)
// 			{
// 				sPathName = sPathName.c_str() + nPath + 1;
// 			}
// 			else
// 				sPathName = _T("");
		}
		return (sPathName.size() > 0 ? NULL : hItem);
	}

	virtual HXMLTREEITEM InsertAfterItem(HXMLTREEITEM hItem, LPCTSTR lpName)
	{
		TESTIFNULL_RETURN_NULL(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		rapid_node* pNodeParent = pNode->parent();

		TESTIFNULL_RETURN_NULL( pNodeParent );

		LPTSTR _lpName = m_Doc.allocate_string( lpName );
		rapid_node* pChild = m_Doc.allocate_node( node_element, _lpName );

		// rapidxml`s insert_node inserts before node
		pNode = pNode->next_sibling();
		pNodeParent->insert_node( pNode, pChild );

		return pChild;
	}

	virtual HXMLTREEITEM InsertBeforeItem(HXMLTREEITEM hItem, LPCTSTR lpName)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		LPTSTR _lpName = m_Doc.allocate_string( lpName );
		rapid_node* pChild = m_Doc.allocate_node( node_element, _lpName );

		rapid_node* pNodeParent = (rapid_node*)GetParent(hItem);
		if( !pNodeParent )
			pNodeParent = &m_Doc;

		// rapidxml`s insert_node inserts before node
		pNodeParent->insert_node( pNode, pChild );

		return pChild;
	}

	virtual HXMLTREEITEM InsertChild(HXMLTREEITEM hItem, LPCTSTR lpName)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		LPTSTR _lpName = m_Doc.allocate_string( lpName );
		rapid_node* pChild = m_Doc.allocate_node( node_element, _lpName );

		pNode->append_node( pChild );
		return pChild;
	}

	virtual BOOL DeleteItem(HXMLTREEITEM hItem)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		pNode->remove_all_attributes();
		pNode->remove_all_nodes();

		rapid_node* pParent = pNode->parent();
		if(!pParent)
		{
			m_Doc.remove_all_attributes();
			m_Doc.remove_all_nodes();
		}
		else
			pParent->remove_node( pNode );

		return TRUE;
	}

	LPCTSTR GetText(HXMLTREEITEM hItem)
	{
		TESTIFNULL_RETURN_NULL(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		TESTIFNULL_RETURN_NULL( pNode->parent() );

		switch(pNode->type())
		{
		case node_element:
			for(pNode = pNode->first_node();
				pNode;
				pNode = pNode->next_sibling())
			{
				if(pNode->type() == node_data)
					return pNode->value();
			}
			break;
		}
		return NULL;
	}

	virtual LPCTSTR GetName(HXMLTREEITEM hItem)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		return pNode->name();
	}

	virtual LPCTSTR GetAttribute(HXMLTREEITEM hItem, LPCTSTR lpAttName = NULL)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		if( pNode->type() != node_element )
			return NULL;

		rapid_attr* pAttr = pNode->first_attribute( lpAttName, 0, CASE_SENSITIVE );
		if( pAttr )
			return pAttr->value();
		else
			return NULL;
	}

	INT GetAttributeInt(HXMLTREEITEM hItem, INT nDefValue = 0, LPCTSTR lpAttName = NULL)
	{
		LPCTSTR lpRet = GetAttribute(hItem, lpAttName);
		if(lpRet)
			return _ttoi(lpRet);
		return nDefValue;		
	}

	virtual INT GetAttributeCount(HXMLTREEITEM hItem)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		if( pNode->type() != node_element )
			return 0;

		INT nCount = 0;
		rapid_attr* pAttr = pNode->first_attribute();

		while( pAttr )
		{
			nCount++;
			pAttr = pAttr->next_attribute();
		}

		return nCount;
	}

	virtual LPCTSTR GetAttribute(HXMLTREEITEM hItem, INT nIndex, LPTSTR pName, INT nLen)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		if( pNode->type() != node_element )
			return NULL;

		rapid_attr* pAttr = pNode->first_attribute();
		while( pAttr )
		{
			if( nIndex == 0 )
			{
				if( pName )
					_tcsncpy_s( pName, nLen, pAttr->name(), nLen );

				return pAttr->value();
			}

			nIndex--;
			pAttr = pAttr->next_attribute();
		}

		return NULL;
	}

	virtual LPCTSTR GetComment(HXMLTREEITEM hItem)
	{
		TESTIFNULL_RETURN_NULL(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		pNode = pNode->previous_sibling();

		if( !pNode )
			return NULL;
		
		if( pNode->type() == node_comment )
			return pNode->value();
		else
			return NULL;
	}

	virtual BOOL SetName(HXMLTREEITEM hItem, LPCTSTR lpName)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		if( pNode->type() != node_element )
			return FALSE;

		LPTSTR _lpName = m_Doc.allocate_string( lpName );
		pNode->name( _lpName );

		return TRUE;
	}

	virtual BOOL SetText(HXMLTREEITEM hItem, LPCTSTR lpText)
	{
		TESTIFNULL_RETURN_NULL(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		if( pNode->type() != node_element )
			return FALSE;

		if( !pNode->parent() )
			return FALSE;

		LPTSTR _lpText = m_Doc.allocate_string( lpText );
		rapid_node* pChild = pNode->first_node();

		for( ; pChild; pChild = pChild->next_sibling() )
		{
			if(pChild->type() == node_data)
			{
				pChild->value(_lpText);
				return TRUE;
			}
		}

		if(!pChild)
		{
			rapid_node* pTextNode = m_Doc.allocate_node( node_data );
			pTextNode->value( _lpText );
			pNode->append_node( pTextNode );
		}
		return TRUE;
	}

	virtual BOOL SetAttribute(HXMLTREEITEM hItem, LPCTSTR lpAttName, LPCTSTR lpAttValue = NULL)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		if( pNode->type() != node_element )
			return FALSE;

		rapid_attr* pAttr = pNode->first_attribute( lpAttName, 0, CASE_SENSITIVE );

		if( pAttr )
		{
			if( lpAttValue )
			{
				LPTSTR _lpAttValue = m_Doc.allocate_string( lpAttValue );
				pAttr->value( _lpAttValue );
			}
			else
				pNode->remove_attribute( pAttr );
		}
		else
		{
			if( lpAttValue )
			{
				LPTSTR _lpAttName = m_Doc.allocate_string( lpAttName );
				LPTSTR _lpAttValue = m_Doc.allocate_string( lpAttValue );
				pAttr = m_Doc.allocate_attribute( _lpAttName, _lpAttValue );

				pNode->append_attribute( pAttr );
			}
		}

		return TRUE;
	}

	virtual BOOL SetAttributeInt(HXMLTREEITEM hItem, LPCTSTR lpAttName, INT nValue = 0)
	{
		TCHAR szTemp[100] = {0};
		if( _itot_s(nValue, szTemp, _countof(szTemp), 10) )
			return FALSE;

		return SetAttribute(hItem, lpAttName, szTemp);
	}

	virtual HXMLTREEITEM SetComment(HXMLTREEITEM hItem, LPCTSTR lpszComment = NULL)
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;
		rapid_node* pNodePre = pNode->previous_sibling();
		rapid_node* pComment = NULL;

		if( pNodePre && pNodePre->type()==node_comment )
			pComment = pNodePre;

		if(pComment)
		{// 修改或者删除
			if(lpszComment)
			{
				LPTSTR _lpszComment = m_Doc.allocate_string( lpszComment );
				pComment->value(_lpszComment);
				return pComment; 
			}
			else
			{
				DeleteItem(pNodePre);
			}
			return NULL;
		}

		if(lpszComment)
		{
			LPTSTR _lpszComment = m_Doc.allocate_string( lpszComment );
			pComment = m_Doc.allocate_node( node_comment );
			pComment->value( _lpszComment );
			
			rapid_node* pParent = pNode->parent();
			if(!pParent)
				pParent = &m_Doc;

			pParent->insert_node( pNode, pComment );
			return pComment;
		}

		return NULL;
	}

	virtual BOOL Load(LPCTSTR lpFileName)
	{
		//m_Doc.remove_all_attributes();
		//m_Doc.remove_all_nodes();
		m_Doc.clear();

		FILE* file;
		_tfopen_s( &file, lpFileName, _T("rb") );

		if( file )
		{
			BOOL bResult = LoadFile( file );
			fclose( file );
			return bResult;
		}
		else
			return FALSE;
	}
/*
	virtual BOOL Load(LPCTSTR lpszFileName)
	{
		char * filename = NULL;
// 		ifstream inputFile;
// 		inputFile.open(lpszFileName);
#ifdef UNICODE
		int len = WideCharToMultiByte( CP_ACP, 0, LPWSTR(lpszFileName), -1, NULL, 0, NULL, NULL );
		LPSTR szA = new char[len + 1];
		ZeroMemory(szA, len + 1);
		len = WideCharToMultiByte( CP_ACP, 0, LPWSTR(lpszFileName), -1, szA, len, NULL, NULL );
		filename = szA;
#else
		filename = lpszFileName;
#endif
		ifstream stream(filename, ios::binary); // if file is valid
		if(!stream) return FALSE;
		
		file<> fAnsii(filename);

		delete [] szA;
		return LoadBuff((LPBYTE)fAnsii.data(), (INT)fAnsii.size(),TRUE);

	}
	*/
	virtual BOOL LoadBuff(LPCTSTR lpFileBuff)
	{
		//m_Doc.remove_all_attributes();
		//m_Doc.remove_all_nodes();
		m_Doc.clear();

		return LoadBuff( (LPBYTE)lpFileBuff, (INT)(_tcslen((LPTSTR)lpFileBuff) + 1)*sizeof(TCHAR), TRUE );
	}

	virtual BOOL Save(LPCTSTR lpFileName)
	{
		basic_string<TCHAR> sEncoding;
		xml_node<TCHAR> * pfnode = m_Doc.first_node(); // first node should be <xml....
		if(pfnode)
		{
			// get encoding attribute
			xml_attribute<TCHAR> * pattEncoding = pfnode->first_attribute(_T("encoding"), 0, false);
			if(pattEncoding)
			{
				sEncoding = pattEncoding->value();
				_tcsupr_s((TCHAR*)sEncoding.c_str(), sEncoding.size() + 1);				
			}
		}		

		basic_string<TCHAR> s;
		print(std::back_inserter(s), m_Doc, 0);

		// 删除多余的回车符
		basic_string<TCHAR>::iterator itStr = s.begin() + s.rfind(_T('>'));
		if(itStr != s.end())
		{
			s.erase(itStr + 1, s.end());
		}

		
		vector<char> vDataBuffer;
#ifdef _UNICODE
		//vector<char> vDataBuffer;
		
		if(_tcsicmp(sEncoding.c_str(), _T("UNICODE")) == 0)
		{
			vDataBuffer.push_back((const char)0xff);
			vDataBuffer.push_back((const char)0xfe);
			char * pBegin = (char*)s.c_str();
			char * pEnd = pBegin + s.length() * sizeof(TCHAR);
			for(char * pCur = pBegin;pCur<pEnd;pCur++)
			{
				vDataBuffer.push_back(*pCur);
			}

// 			BYTE mark[2] = { 0xFF, 0xFE };
// 			if( !WriteFile( hFile, mark, 2, &written, NULL ) || written!=2 )
// 			return FALSE;

		}
		else 
		{
			if(_tcsicmp(sEncoding.c_str(), _T("UTF-8")) == 0)
			{
				int len = WideCharToMultiByte( CP_UTF8, 0, LPWSTR(s.c_str()), -1, NULL, 0, NULL, NULL );
				vDataBuffer.resize(len);			
				len = WideCharToMultiByte( CP_UTF8, 0, LPWSTR(s.c_str()), -1, &vDataBuffer.front(), len, NULL, NULL );

			}
			else
			{
				int len = WideCharToMultiByte( CP_ACP, 0, LPWSTR(s.c_str()), -1, NULL, 0, NULL, NULL );
				vDataBuffer.resize(len);
				len = WideCharToMultiByte( CP_ACP, 0, LPWSTR(s.c_str()), -1, &vDataBuffer.front(), len, NULL, NULL );
			}
			while(vDataBuffer.back() == '\0') vDataBuffer.pop_back();
		}


#endif
				
		HANDLE hFile = CreateFile( lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0 , NULL );
		if( hFile==INVALID_HANDLE_VALUE )
			return FALSE;

		DWORD written;
		
		if( !WriteFile( hFile, &vDataBuffer.front(), (DWORD)vDataBuffer.size(), &written, NULL ) 
			|| written!=vDataBuffer.size() )
			return FALSE;

		CloseHandle( hFile );

		return TRUE;
	}

	virtual int GetXML( HXMLTREEITEM hItem, LPTSTR pBuff, int bufLen, INT* nWritten )
	{
		TESTIFNULL_AS_ROOT(hItem);

		rapid_node* pNode = (rapid_node*)hItem;

		tstring s;
		print( back_inserter(s), *pNode, 0 );

		// convert to utf-8
		LPCTSTR strXML = s.c_str();
		int length = (int)s.length()+1;

		if( nWritten )
			*nWritten = 0;

		if( !pBuff || bufLen == 0 )
		{
			return length;
		}

		if( bufLen < length )
		{
			_tcsncpy_s( pBuff, bufLen, strXML, bufLen );

			if( nWritten )
				*nWritten = bufLen;

			return length;
		}
		
		_tcsncpy_s( pBuff, bufLen, strXML, length );
		if( nWritten )
			*nWritten = length;

		return 0;
	}

	HXMLTREEITEM CopyBranch( HXMLTREEITEM hSourceNode )
	{
		return NULL;
	}

	HXMLTREEITEM CopyBranchAsChild( HXMLTREEITEM hDestNode, HXMLTREEITEM hSourceNode )
	{
		TESTIFNULL_RETURN_NULL(hSourceNode)
		TESTIFNULL_AS_ROOT(hDestNode);

		rapid_node* pDest = (rapid_node*)hDestNode;
		rapid_node* pSource = (rapid_node*)hSourceNode;

		rapid_node* pChild = m_Doc.clone_node( pSource );
		pDest->append_node( pChild );

		return pChild;
	}

private:
	BOOL IsTreeItem(HXMLTREEITEM hItem)
	{
		rapid_node* pNode = (rapid_node*)hItem;
		switch(pNode->type())
		{
		case node_document:
		case node_element:
			return TRUE;
		}
		return FALSE;
	}

	BOOL LoadFile( FILE* file )
	{
		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		long length = 0;
		fseek( file, 0, SEEK_END );
		length = ftell( file );
		fseek( file, 0, SEEK_SET );

		if( length==0 )
			return FALSE;

		LPBYTE pBuffer = (LPBYTE)m_Doc.allocate_string( NULL, (length+sizeof(TCHAR))/sizeof(TCHAR)+1 );
		if( !pBuffer )
			return FALSE;

		if( fread( pBuffer, length, 1, file ) != 1 ) 
			return false;

		*(TCHAR*)&pBuffer[length] = 0;

		// 真实长度 + 1
		length = length + sizeof(TCHAR);

		return LoadBuff( pBuffer, length, TRUE );
	}

	BOOL LoadBuff( LPBYTE pBuff, INT nSize/*真实的buffer大小而不是文字的个数*/, BOOL bCopy )
	{
		LPSTR szBuff = NULL;
		if( bCopy )
		{
			szBuff = (LPSTR)m_Doc.allocate_string( NULL, nSize/sizeof(TCHAR) + 1 );
			// szBuff = new char[nSize + 2];
			ZeroMemory(szBuff, nSize + 2);
			memcpy( szBuff, pBuff, nSize);

		}
		else 
			szBuff = (LPSTR)pBuff;

		//m_Doc.remove_all_attributes();
		//m_Doc.remove_all_nodes();

		// Locate the first '<'
		CHAR* data = szBuff;
		while( *data )
		{
			if( *data == '<' )
				break;

			data++;
		}

		if( !(*data) )
			return FALSE;

		CHAR* zero = data+1;
		BOOL bRet = TRUE;

		if( !(*zero) )
		{
			try
			{
#ifdef _UNICODE
				m_Doc.parse<PARSE_FLAG>( LPWSTR(data) );
#else
				int len = WideCharToMultiByte( CP_ACP, 0, LPWSTR(data), -1, NULL, 0, NULL, NULL );
				LPSTR szA = m_Doc.allocate_string( NULL, len );
				len = WideCharToMultiByte( CP_ACP, 0, LPWSTR(data), -1, szA, len, NULL, NULL );

				m_Doc.parse<PARSE_FLAG>( szA );
#endif
			}
			catch( parse_error& e )
			{
				printf(e.what());
				bRet = FALSE;
			}
		}
		else
		{
			
			
			try
			{
#ifdef _UNICODE
				// 不一定是utf8的
				DWORD dwCodePage = GetEncoding(pBuff, nSize);		
				
				// 转成unicode
				int len = MultiByteToWideChar( dwCodePage, 0, data, -1, NULL, 0 );
				LPWSTR szW = (LPWSTR)m_Doc.allocate_string( NULL, len*sizeof(WCHAR) );
				MultiByteToWideChar( dwCodePage, 0, data, -1, szW, len );
				m_Doc.parse<PARSE_FLAG>( szW );
#else
				m_Doc.parse<PARSE_FLAG>( data );
#endif
			}
			catch( parse_error& e )
			{
				printf(e.what());
				bRet = FALSE;
			}
			
		}
		return bRet;
	}

	DWORD GetEncoding(LPBYTE pBuff, int nBufferSize)
	{
		char * pdataBuffer = NULL;
		DWORD dwCodePage = CP_ACP;
		try
		{
			do
			{
				// 尝试解析encoding属性
			xml_document<> xdAnsii;
			
			pdataBuffer = new char[nBufferSize];
			
			ZeroMemory(pdataBuffer, nBufferSize);			
			memcpy(pdataBuffer, pBuff, nBufferSize);

			xdAnsii.parse<PARSE_FLAG>(pdataBuffer); 

			xml_node<> * pxnEncode = xdAnsii.first_node();					
			if(!pxnEncode) break;					

			xml_attribute<> * pxaEncoding = pxnEncode->first_attribute("encoding", 0, FALSE);
			if(!pxaEncoding) break;

			string sEncoding = pxaEncoding->value();
			_strupr_s((char*)sEncoding.c_str(), sEncoding.size() + 1);

			if(_stricmp("UTF-8", sEncoding.c_str()) == 0) dwCodePage = CP_UTF8; // 其他都是ansii
			}while(FALSE);
		}
		catch(parse_error & e)
		{
			e;
		}
		delete [] pdataBuffer;

		return dwCodePage;

	}
	virtual VOID FreeXMLString(LPCTSTR lpXML)
	{
	}

private:
	rapid_doc	m_Doc;
	TCHAR		m_szEncoding[MAX_PATH];
};