
#ifndef RapidXMLWrapper_H_
#define RapidXMLWrapper_H_

#include <locale>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <tchar.h>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

using namespace std ;

class RapidXMLWrapper 
{
public:
    static wstring widen( const string& str )
    {
        std::wstring temp(str.length(),L' ');
        mbstowcs(&temp[0], str.c_str(), str.length());
        return temp; 
    }

    static string narrow( const wstring& str )
    {
        std::string temp(str.length() * 2, ' ');
        size_t len = wcstombs(&temp[0], str.c_str(), temp.length()); 
        temp.erase(temp.begin() + len, temp.end());
        return temp; 
    }
};


typedef basic_string<TCHAR> tstring;

class CXmlDocumentWrapper
{
private:
    rapidxml::xml_document<> m_doc;
    rapidxml::file<>* m_file;
public:

    CXmlDocumentWrapper()
        :m_file(NULL)
    {

    }
    ~CXmlDocumentWrapper()
    {
        delete m_file;
    }

    rapidxml::xml_node<>* AsNode()
    {
        // find the first element child
        rapidxml::xml_node<> *node =  m_doc.first_node();
        while (node)
        {
            if (node->type() == rapidxml::node_element)
            {
                return node;
            }

            node = node->next_sibling();
        }

        return NULL;
    }

	rapidxml::xml_document<>* AsDocument()
	{
		rapidxml::xml_node<> *node =  m_doc.first_node();

		while (node)
		{
			if (node->type() == rapidxml::node_declaration)
			{
				return node->document();
			}

			node = node->next_sibling();
		}

		return NULL;
	}

    rapidxml::xml_document<>* Interface()
    {
        return &m_doc;
    }

    bool Load(const wchar_t * pwszFile)
    {
        return Load(RapidXMLWrapper::narrow(std::wstring(pwszFile)).c_str());
    }

    bool Load(const char * pszFile)
    {
        delete m_file;
        m_file = new rapidxml::file<>(pszFile);
        try 
        {
            m_doc.parse<rapidxml::parse_full>(m_file->data());
            return true;
        }
        catch( rapidxml::parse_error& e)
        {
            e.what();
        }
        return false;
    }

    bool LoadXML(const char * pszXML)
    {
        try 
        {
            m_doc.parse</*0*/rapidxml::parse_full>(m_doc.allocate_string(pszXML));
            return true;
        }
        catch( rapidxml::parse_error& e)
        {
            e.what();
        }
        return false;
    }

    bool Save(const char* pszFile, bool bReserved = true)
    {
        std::ofstream ofs(pszFile);
		if(!ofs.is_open())
		{
				return false;
		}

        ofs << m_doc;
        ofs.close();
        return true;
    }

	bool SaveUtf8(const char* pszFile, bool bReserved = true)
	{

// 		basic_string<CHAR> sEncoding;
// 		rapidxml::xml_node<CHAR> * pfnode = m_doc.first_node(); // first node should be <xml....
// 		if(pfnode)
// 		{
// 			// get encoding attribute
// 			rapidxml::xml_attribute<CHAR> * pattEncoding = pfnode->first_attribute(_T("encoding"), 0, false);
// 			if(pattEncoding)
// 			{
// 				sEncoding = pattEncoding->value();
// 				_tcsupr_s((CHAR*)sEncoding.c_str(), sEncoding.size() + 1);
// 			}
// 		}

		std::ofstream ofs(pszFile);
		if(!ofs.is_open())
		{
			return false;
		}
		ofs << (char)0xEF << (char)0xBB << (char)0xBF;
		ofs << m_doc;
		ofs.close();
		return true;
	}

    bool Save(const wchar_t* pwszFile, bool bReserved = true)
    {
        return Save(RapidXMLWrapper::narrow(pwszFile).c_str(), bReserved);
    }

    std::string GetXML()
    {
        basic_ostringstream<char> oss;
        oss << m_doc;
        return oss.str();
    }
};

struct DOMNodeList 
{
    std::vector<rapidxml::xml_node<>*> m_nodes;
};

class CXmlNodeWrapper
{
private:
    rapidxml::xml_node<>* m_pNode;

    void UUIDFromString( tstring &val, UUID& uuid )
    {
        basic_istringstream<TCHAR> is;
        unsigned long ulData = 0;

        is.str(val.substr(0, 8));
        is >> hex >> uuid.Data1;

        is.clear();
        is.str(val.substr(9, 4));

        is >> hex >> ulData;
        uuid.Data2 = static_cast<unsigned long>(ulData & 0xFFFF);

        is.clear();
        is.str(val.substr(14, 4));
        is >> hex >> ulData;
        uuid.Data3 = static_cast<unsigned short>(ulData & 0xFFFF);

        is.clear();
        is.str(val.substr(19, 4));
        is >> hex >> ulData;
        uuid.Data4[0] = (unsigned char)(ulData >> 8) & 0xFF;
        uuid.Data4[1] = (unsigned char)ulData & 0xFF;

        is.clear();
        is.str(val.substr(24, 4));
        is >> hex >> ulData;
        uuid.Data4[2] = (unsigned char)(ulData >> 8) & 0xFF;
        uuid.Data4[3] = (unsigned char)ulData & 0xFF;

        is.clear();
        is.str(val.substr(28, 4));
        is >> hex >> ulData;
        uuid.Data4[4] = (unsigned char)(ulData >> 24) & 0xFF;
        uuid.Data4[5] = (unsigned char)(ulData >> 16) & 0xFF;
        uuid.Data4[6] = (unsigned char)(ulData >> 8) & 0xFF;
        uuid.Data4[7] = (unsigned char)ulData & 0xFF;
    }
public:
    CXmlNodeWrapper()
        :m_pNode(NULL)
    {

    }
    CXmlNodeWrapper(rapidxml::xml_node<>* node)
        :m_pNode(node)
    {

    }

    ~CXmlNodeWrapper()
    {

    }

    void operator=(rapidxml::xml_node<>* pNode)
    {
        m_pNode = pNode;
    }
    bool operator==(rapidxml::xml_node<>* pNode)
    {
        return m_pNode == pNode;
    }
    bool operator!=(rapidxml::xml_node<>* pNode)
    {
        return m_pNode != pNode;
    }

    rapidxml::node_type GetNodeType()
    {
        if (m_pNode)
        {
            return m_pNode->type();
        }

        return  rapidxml::node_element;
    }

    bool ExistAttribute(const TCHAR* AttribName)
    {
        if (m_pNode)
        {
#ifdef _UNICODE
            return !!m_pNode->first_attribute(RapidXMLWrapper::narrow(AttribName).c_str());
#else
            return !!m_pNode->first_attribute(AttribName);
#endif
        }

        return false;
    }

    int NumAttributes()
    {
        if (m_pNode)
        {
            int nCount = 0;
            rapidxml::xml_attribute<>* attr = m_pNode->first_attribute();
            while(attr)
            {
                nCount++;
                attr = attr->next_attribute();
            }

            return nCount;
        }

        return 0;
    }

    tstring Name()
    {
        if (m_pNode)
        {
#ifdef _UNICODE
            return RapidXMLWrapper::widen(m_pNode->name());
#else
            return m_pNode->name();
#endif
        }
        return tstring();
    }

    rapidxml::xml_node<>* Parent()
    {
        if (m_pNode)
        {
            return m_pNode->parent();
        }

        return NULL;
    }

    rapidxml::xml_node<>* Interface()
    {
        return m_pNode;
    }

    void ReplaceNode(rapidxml::xml_node<>* pOldNode,rapidxml::xml_node<>* pNewNode)
    {
        if (m_pNode)
        {
            m_pNode->insert_node(pOldNode, pNewNode);
            m_pNode->remove_node(pOldNode);
        }
    }

    rapidxml::xml_node<>* InsertBefore(rapidxml::xml_node<>* refNode, rapidxml::xml_node<>* pNode)
    {
        if (m_pNode)
        {
            m_pNode->insert_node(refNode, pNode);
            return pNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* InsertAfter(rapidxml::xml_node<>* refNode, const TCHAR* nodeName, rapidxml::node_type type = rapidxml::node_element)
    {
        if (m_pNode)
        {
            rapidxml::xml_node<>* ref = refNode->next_sibling();
            rapidxml::xml_document<>* doc = m_pNode->document();
#ifdef _UNICODE
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(RapidXMLWrapper::narrow(nodeName).c_str()));
#else
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(nodeName));
#endif
            if (ref)
            {
                m_pNode->insert_node(ref, newNode);
            }
            else
            {
                m_pNode->append_node(newNode);
            }

            return newNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* InsertBefore(rapidxml::xml_node<>* refNode, const TCHAR* nodeName, rapidxml::node_type type = rapidxml::node_element)
    {
        if (m_pNode)
        {
            rapidxml::xml_document<>* doc = m_pNode->document();
#ifdef _UNICODE
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(RapidXMLWrapper::narrow(nodeName).c_str()));
#else
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(nodeName));
#endif
            m_pNode->insert_node(refNode, newNode);

            return newNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* InsertNode(int index,const TCHAR* nodeName, rapidxml::node_type type = rapidxml::node_element)
    {
        if (m_pNode)
        {
            rapidxml::xml_document<>* doc = m_pNode->document();
#ifdef _UNICODE
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(RapidXMLWrapper::narrow(nodeName).c_str()));
#else
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(nodeName));
#endif
            int count = 0;
            for (rapidxml::xml_node<>* node = m_pNode->first_node();
                node;
                node = node->next_sibling(), count++)
            {
                if (count == index)
                {
                    m_pNode->insert_node(node, newNode);
                    return newNode;
                }
            }

            m_pNode->append_node(newNode);
            return newNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* InsertNode(int index,rapidxml::xml_node<>* pNode)
    {
        if (m_pNode)
        {
            int count = 0;
            for (rapidxml::xml_node<>* node = m_pNode->first_node();
                node;
                node = node->next_sibling(), count++)
            {
                if (count == index)
                {
                    m_pNode->insert_node(node, pNode);
                    return pNode;
                }
            }

            m_pNode->append_node(pNode);
            return pNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* AppendNode(const TCHAR* nodeName, rapidxml::node_type type = rapidxml::node_element)
    {
        if (m_pNode)
        {
            rapidxml::xml_document<>* doc = m_pNode->document();
#ifdef _UNICODE
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(RapidXMLWrapper::narrow(nodeName).c_str()));
#else
            rapidxml::xml_node<>* newNode = doc->allocate_node(type,
                doc->allocate_string(nodeName));
#endif

            m_pNode->append_node(newNode);
            return newNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* AppendNode(rapidxml::xml_node<>* pNode)
    {
        if (m_pNode)
        {
            m_pNode->append_node(pNode);
            return pNode;
        }

        return NULL;
    }

    rapidxml::xml_node<>* RemoveNode(rapidxml::xml_node<>* pNode)
    {
        if (m_pNode)
        {
            m_pNode->remove_node(pNode);
            return pNode;
        }

        return NULL;
    }

    void RemoveNodes(const TCHAR * searchString)
    {
        if (m_pNode)
        {
            rapidxml::xml_node<> * node =
#ifdef _UNICODE
            m_pNode->first_node(RapidXMLWrapper::narrow(searchString).c_str());
#else
            m_pNode->first_node(searchString);
#endif
            while (node)
            {
                m_pNode->remove_node(node);
                node = 
#ifdef _UNICODE
                    m_pNode->next_sibling(RapidXMLWrapper::narrow(searchString).c_str());
#else
                    m_pNode->next_sibling(searchString);
#endif
            }
        }
    }

    long NumNodes()
    {
        long nCount = 0;
        if (m_pNode)
        {
            for (rapidxml::xml_node<>* node = m_pNode->first_node();
                node;
                node = node->next_sibling())
            {
                nCount++;
            }
        }

        return nCount;
    }

    rapidxml::xml_node<>* FindNode(const TCHAR* searchString)
    {
        if (m_pNode)
        {
#ifdef _UNICODE
            return m_pNode->first_node(RapidXMLWrapper::narrow(searchString).c_str());
#else
            return m_pNode->first_node(searchString);
#endif
        }

        return NULL;
    }

    rapidxml::xml_node<>* GetPrevSibling()
    {
        if (m_pNode)
        {
            return m_pNode->previous_sibling();
        }

        return NULL;
    }

    rapidxml::xml_node<>* GetNextSibling()
    {
        if (m_pNode)
        {
            return m_pNode->next_sibling();
        }

        return NULL;
    }

    rapidxml::xml_document<>* Document()
    {
        if (m_pNode)
        {
            return m_pNode->document();
        }

        return NULL;
    }


    void SetValue(const TCHAR* valueName, const TCHAR* value)
    {
        if (m_pNode)
        {
            rapidxml::xml_document<>* doc = m_pNode->document();
            rapidxml::xml_attribute<>* attr = 
#ifdef _UNICODE                
            m_pNode->first_attribute(RapidXMLWrapper::narrow(valueName).c_str());
#else
            m_pNode->first_attribute(valueName);
#endif
            if (!attr)
            {
                // append the new attribute
#ifdef _UNICODE                
                attr = doc->allocate_attribute(
                    doc->allocate_string(
                    RapidXMLWrapper::narrow(valueName).c_str()));
#else
                attr = doc->allocate_attribute(
                    doc->allocate_string(valueName));
#endif
                m_pNode->append_attribute(attr);
            }

                // set the value
#ifdef _UNICODE
                attr->value(doc->allocate_string(RapidXMLWrapper::narrow(value).c_str()));
#else
                attr->value(doc->allocate_string(value));
#endif
            
        }
    }
    void SetValue(const TCHAR* valueName, int value)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            _countof(val),
            _T("%d"),
            value);
#else
        _stprintf(
            val,
            _T("%d"),
            value);
#endif
        SetValue(valueName, val);
    }
    void SetValue(const TCHAR* valueName, short value)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            _countof(val),
            _T("%d"),
            value);
#else
        _stprintf(
            val,
            _T("%d"),
            value);
#endif
        SetValue(valueName, val);
    }
    void SetValue(const TCHAR* valueName, double value)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            _countof(val),
            _T("%f"),
            value);
#else
        _stprintf(
            val,
            _T("%f"),
            value);
#endif
        SetValue(valueName, val);
    }

    void SetValue(const TCHAR* valueName, float value)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            _countof(val),
            _T("%f"),
            value);
#else
        _stprintf(
            val,
            _T("%f"),
            value);
#endif
        SetValue(valueName, val);
    }
    void SetValue(const TCHAR* valueName, bool value)
    {
        SetValue(valueName, value ? _T("true") : _T("false"));
    }

    void SetValue(const TCHAR* valueName, UUID& value)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(val,
            sizeof(val)/sizeof(val[0]),
            _T("%8.8X-%4.4X-%4.4X-%2.2X%2.2X%-2.2X%2.2X%2.2X%2.2X%2.2X%2.2X"),
            value.Data1, value.Data2, value.Data3, 
            value.Data4[0], value.Data4[1], value.Data4[2], 
            value.Data4[3], value.Data4[4], value.Data4[5], 
            value.Data4[6], value.Data4[7] );
#else
        _stprintf(val,
            _T("%8.8X-%4.4X-%4.4X-%2.2X%2.2X%-2.2X%2.2X%2.2X%2.2X%2.2X%2.2X"),
            (unsigned int)value.Data1, value.Data2, value.Data3, 
            value.Data4[0], value.Data4[1], value.Data4[2], 
            value.Data4[3], value.Data4[4], value.Data4[5], 
            value.Data4[6], value.Data4[7] );
#endif
        SetValue(valueName,val);
    }

    void SetText(const TCHAR* text)
    {
		/*
        if (m_pNode)
        {
            rapidxml::xml_document<>* doc = m_pNode->document();
#ifdef _UNICODE
            m_pNode->value(doc->allocate_string(RapidXMLWrapper::narrow(text).c_str()));
#else
            m_pNode->value(doc->allocate_string(text));
#endif
        }
		*/
		rapidxml::xml_document<>* doc = m_pNode->document();
		rapidxml::xml_node<>* pNode = (rapidxml::xml_node<>*)m_pNode;
		if( pNode->type() != rapidxml::node_element )
			return ;

		if( !pNode->parent() )
			return ;

		LPTSTR _lpText = doc->allocate_string( text );
		rapidxml::xml_node<>* pChild = pNode->first_node();

		for( ; pChild; pChild = pChild->next_sibling() )
		{
			if(pChild->type() == rapidxml::node_data)
			{
				pChild->value(_lpText);
				return ;
			}
		}

		if(!pChild)
		{
			rapidxml::xml_node<>* pTextNode = doc->allocate_node( rapidxml::node_data );
			pTextNode->value( _lpText );
			pNode->append_node( pTextNode );
		}
		return ;

    }
    void SetText(int text)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            sizeof(val)/sizeof(val[0]),
#else
        _stprintf(
            val,
#endif
            _T("%ds"),
            text);
        SetText(val);
    }
    void SetText(short text)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            sizeof(val)/sizeof(val[0]),
#else
        _stprintf(
            val,
#endif
            _T("%d"),
            text);
        SetText(val);
    }
    void SetText(double text)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            sizeof(val)/sizeof(val[0]),
#else
        _stprintf(
            val,
#endif
            _T("%f"),
            text);
        SetText(val);
    }
    void SetText(float text)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            sizeof(val)/sizeof(val[0]),
#else
        _stprintf(
            val,
#endif
            _T("%f"),
            text);
        SetText(val);
    }
    void SetText(bool text)
    {
        SetText(text ? _T("true") : _T("false"));
    }

    void SetText(UUID& text)
    {
        TCHAR val[100] = {0};
#if _MSC_VER >=1400
        _stprintf_s(
            val,
            sizeof(val)/sizeof(val[0]),
#else
        _stprintf(
            val,
#endif
            _T("%8.8X-%4.4X-%4.4X-%2.2X%2.2X%-2.2X%2.2X%2.2X%2.2X%2.2X%2.2X"),
            (unsigned int)text.Data1, text.Data2, text.Data3, 
            text.Data4[0], text.Data4[1], text.Data4[2], 
            text.Data4[3], text.Data4[4], text.Data4[5], 
            text.Data4[6], text.Data4[7] );
        SetText(val);
    }

    tstring GetValue(const TCHAR* valueName)
    {
        if (m_pNode)
        {
            rapidxml::xml_document<>* doc = m_pNode->document();
            rapidxml::xml_attribute<>* attr =
#ifdef _UNICODE
                m_pNode->first_attribute( doc->allocate_string(RapidXMLWrapper::narrow(valueName).c_str()));
#else
                m_pNode->first_attribute( doc->allocate_string(valueName));
#endif
            if (attr)
            {
#ifdef _UNICODE
                return RapidXMLWrapper::widen(attr->value());
#else
                return string(attr->value());
#endif
            }
        }

        return tstring();
    }

    int GetIntValue(const TCHAR* valueName)
    {
        tstring val = GetValue(valueName);
        if (!val.empty())
        {
            return _ttoi(val.c_str());
        }

        return 0xFFFFFFFF;
    }

    DWORD GetHexValue(const TCHAR* valueName)
    {
        unsigned int dwRet = 0xFFFFFFFF;
        tstring val = GetValue(valueName);
        if (!val.empty())
        {
            transform(val.begin(), val.end(), val.begin(), ::tolower);
#if _MSC_VER >=1400
            _stscanf_s
#else
            _stscanf
#endif
                (val.c_str(),_T("0x%x"), &dwRet);
        }

        return dwRet;
    }

    COLORREF GetColorValue(const TCHAR* valueName)
    {
        COLORREF crRet = RGB(0,0,0);
        DWORD dwColor = GetHexValue(valueName);
        crRet = RGB((dwColor & 0xFF0000)>> 16, (dwColor & 0x00FF00) >> 8, dwColor & 0xff);
        return crRet;
    }

    UUID GetUUIDValue(const TCHAR* valueName)
    {
        UUID uuid = {0};
        tstring val = GetValue(valueName);
        if (!val.empty())
        {
            UUIDFromString(val, uuid);
        }

        return uuid;
    }

    tstring GetText()
    {
        if (m_pNode)
        {
            tstring text = 
#ifdef _UNICODE
                RapidXMLWrapper::widen(m_pNode->value());
#else
                m_pNode->value();
#endif

            return text;
        }

        return tstring();
    }


    int GetIntText()
    {
        int nRet = 0xFFFFFFFF;
        if (m_pNode)
        {
            tstring text = 
#ifdef _UNICODE
                RapidXMLWrapper::widen(m_pNode->value());
#else
                m_pNode->value();
#endif
            if (!text.empty())
            {
                nRet = _ttoi(text.c_str());
            }
        }

        return nRet;
    }

    DWORD GetHexText()
    {
        unsigned int dwRet = 0xFFFFFFFF;
        if (m_pNode)
        {
            tstring text = 
#ifdef _UNICODE
                RapidXMLWrapper::widen(m_pNode->value());
#else
                m_pNode->value();
#endif
            if (!text.empty())
            {
                transform(text.begin(), text.end(), text.begin(), ::tolower);

#if _MSC_VER >=1400
                _stscanf_s
#else
                _stscanf
#endif
                    (text.c_str(),_T("0x%x"), &dwRet);
            }
        }

        return dwRet;
    }

    COLORREF GetColorText()
    {
        COLORREF crRet = RGB(0,0,0);
        if (m_pNode)
        {
            tstring text = 
#ifdef _UNICODE
                RapidXMLWrapper::widen(m_pNode->value());
#else
                m_pNode->value();
#endif
            if (!text.empty())
            {
                transform(text.begin(), text.end(), text.begin(), ::tolower);
                unsigned int dwRet = 0;

#if _MSC_VER >=1400
                _stscanf_s
#else
                _stscanf
#endif
                (text.c_str(),_T("0x%x"), &dwRet);

                crRet = RGB((dwRet & 0xFF0000)>> 16, (dwRet & 0x00FF00) >> 8, dwRet & 0xff);
            }
        }

        return crRet;
    }

    UUID GetUUIDText()
    {
        UUID uuid = {0};
        if (m_pNode)
        {
            tstring text = 
#ifdef _UNICODE
                RapidXMLWrapper::widen(m_pNode->value());
#else
                m_pNode->value();
#endif
            UUIDFromString(text, uuid);
        }

        return uuid;
    }

    bool IsValid()
    {
        return (m_pNode != NULL);
    }

    DOMNodeList FindNodes(const TCHAR* searchString)
    {
        if (m_pNode)
        {
            DOMNodeList nl;
#ifdef _UNICODE
            string searchStr = RapidXMLWrapper::narrow(searchString);
            for (rapidxml::xml_node<>* node = m_pNode->first_node(
                searchStr.c_str());
                node;
                node = node->next_sibling(searchStr.c_str()))
#else
            for (rapidxml::xml_node<>* node = m_pNode->first_node(searchString);
                node;
                node = node->next_sibling(searchString))
#endif
            {
                nl.m_nodes.push_back(node);
            }

            return nl;
        }
        return DOMNodeList();
    }

    DOMNodeList ChildNodes()
    {
        if (m_pNode)
        {
            DOMNodeList nl;
            for (rapidxml::xml_node<>* node = m_pNode->first_node();
                node;
                node = node->next_sibling())
            {
                nl.m_nodes.push_back(node);
            }
            return nl;
        }

        return DOMNodeList();
    }
};

class CXmlNodelistWrapper;

class CXmlNodeIterator
    : public std::iterator<std::bidirectional_iterator_tag, CXmlNodeWrapper>
{
private:
    std::vector<rapidxml::xml_node<>*> m_nodelist;
    size_t m_nIndex;
    CXmlNodeWrapper m_refNode;
public:
    explicit CXmlNodeIterator(size_t index,std::vector<rapidxml::xml_node<>*>& nodelist)
        :m_nodelist(nodelist)
        ,m_nIndex(index)
    {
        if (  m_nIndex >= nodelist.size() || m_nIndex == 0xFFFFFFFF)
        {
            m_nIndex = 0xFFFFFFFF;
        }
    }

    ~CXmlNodeIterator()
    {
    }

    size_t Index() const { return m_nIndex; }
    void Index(size_t val) { m_nIndex = val; }
    
    CXmlNodeIterator& operator++() // prefix
    {
        size_t nIndex = ++m_nIndex;
        if (nIndex == 0 || nIndex >= m_nodelist.size())
        {
            m_nIndex = 0xFFFFFFFF;
        }
        return *this;
    }

    CXmlNodeIterator operator++(int) // postfix
    {
        CXmlNodeIterator old = *this;
        size_t nIndex = ++m_nIndex;
        if (nIndex == 0 || nIndex >= m_nodelist.size())
        {
            m_nIndex = 0xFFFFFFFF;
        }
        return old;
    }

    CXmlNodeIterator& operator--() // prefix
    {
        size_t nIndex = m_nIndex--;
        if (nIndex == 0xFFFFFFFF || m_nIndex == 0xFFFFFFFF )
        {
            m_nIndex = 0xFFFFFFFF;
        }
        return *this;
    }

    CXmlNodeIterator operator--(int) // postfix
    {
        CXmlNodeIterator old = *this;
        size_t nIndex = m_nIndex--;
        if (nIndex == 0xFFFFFFFF ||m_nIndex == 0xFFFFFFFF )
        {
            m_nIndex = 0xFFFFFFFF;
        }
        return old;
    }

    CXmlNodeIterator& operator+=(int n)
    {
        if(m_nIndex == 0xFFFFFFFF)
        {
            return *this;
        }

        m_nIndex += n;
        if (++m_nIndex >= m_nodelist.size())
        {
            m_nIndex = 0xFFFFFFFF;
        }

        return *this;
    }

    CXmlNodeIterator& operator-=(int n)
    {
        if (m_nIndex < (size_t)n)
        {
            m_nIndex = 0xFFFFFFFF;
            return *this;
        }

        m_nIndex -= n;

        if (--m_nIndex < 0)
        {
            m_nIndex = 0xFFFFFFFF;
        }

        return *this;
    }

    CXmlNodeIterator operator+(CXmlNodeIterator& iter)
    {
        return CXmlNodeIterator(iter.Index() + m_nIndex, m_nodelist);
    } 

    CXmlNodeIterator operator-(CXmlNodeIterator& iter)
    {
        return CXmlNodeIterator(iter.Index() - m_nIndex, m_nodelist);
    } 

    bool operator>(CXmlNodeIterator& iter)
    {
        return m_nIndex > iter.Index();
    }

    bool operator>=(CXmlNodeIterator& iter)
    {
        return m_nIndex >= iter.Index();
    }

    bool operator<(CXmlNodeIterator& iter)
    {
        return m_nIndex < iter.Index();
    }

    bool operator<=(CXmlNodeIterator& iter)
    {
        return m_nIndex <= iter.Index();
    }

    bool operator==(CXmlNodeIterator& iter)
    {
        return m_nIndex == iter.Index();
    }

    bool operator!=(CXmlNodeIterator& iter)
    {
        return m_nIndex != iter.Index();
    }

    reference operator*()
    {
        m_refNode = m_nodelist.at(m_nIndex);
        return m_refNode;
    }

    pointer operator->()
    {
        m_refNode = m_nodelist.at(m_nIndex);
        return &m_refNode;
    }
};

class CXmlNodelistWrapper
{
private:
    rapidxml::xml_node<>* m_refNode;
    DOMNodeList m_pNodelist;
public:   
    typedef rapidxml::xml_node<>* value_type;
    typedef CXmlNodeIterator iterator;
    typedef const CXmlNodeIterator const_iterator;
    typedef CXmlNodeWrapper& reference;
    typedef const CXmlNodeWrapper const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    CXmlNodelistWrapper(const DOMNodeList& Nodelist)
        :m_refNode(NULL)
        ,m_pNodelist(Nodelist)
    {

    }

    CXmlNodelistWrapper(DOMNodeList* pNodelist)
        :m_refNode(NULL)
        ,m_pNodelist(*pNodelist)
    {

    }

    CXmlNodelistWrapper()
        :m_refNode(NULL)
    {

    }

    virtual ~CXmlNodelistWrapper()
    {

    }

    rapidxml::xml_document<>* AsDocument()
    {
        if (!m_pNodelist.m_nodes.empty())
        {
            rapidxml::xml_node<>* node = m_pNodelist.m_nodes.at(0);
            return node->document();
        }
        return NULL;
    }

    rapidxml::xml_node<>* Node(size_t nIndex)
    {
        if (m_pNodelist.m_nodes.size() > nIndex)
        {
            return m_pNodelist.m_nodes.at(nIndex);
        }

        return NULL;
    }

    bool IsValid()
    {
        return (m_pNodelist.m_nodes.empty() == false);
    }

    int Count()
    {
        return m_pNodelist.m_nodes.size();
    }

    iterator begin()
    {
        iterator iter(0, m_pNodelist.m_nodes);
        return iter;
    }

    const_iterator begin() const
    {
        const_iterator iter(0, (std::vector<rapidxml::xml_node<>*>&)m_pNodelist.m_nodes);
        return iter;
    }

    iterator end()
    {
        iterator iter(0xFFFFFFFF, m_pNodelist.m_nodes);
        return iter;
    }

    reference operator[](size_type index)
    {
        m_refNode = IsValid() ? m_pNodelist.m_nodes.at(index) : NULL;
        return (reference)m_refNode;
    }

    const_reference operator[](size_type index) const
    {
        CXmlNodeIterator refNode(index, (std::vector<rapidxml::xml_node<>*>&)m_pNodelist.m_nodes);
        return *refNode;
    }

    reference at(size_type index)
    {
        if (index >= m_pNodelist.m_nodes.size())
        {
            std::string err_msg = "out of XMLrapidxml::xml_node<>list range";

            throw std::out_of_range(err_msg);
        }
        m_refNode = IsValid() ? m_pNodelist.m_nodes.at(index) : NULL;
        return reference(m_refNode);
    }

    reference front()
    {
        m_refNode = IsValid() ? m_pNodelist.m_nodes.at(0) : NULL;
        return (reference)m_refNode;
    }

    const_reference front() const
    {
        CXmlNodeIterator refNode(0, (std::vector<rapidxml::xml_node<>*>&)m_pNodelist.m_nodes);
        return *refNode;
    }

    reference back()
    {
        m_refNode = IsValid() ? m_pNodelist.m_nodes.at(m_pNodelist.m_nodes.size() -1) : NULL;
        return (reference)m_refNode;        
    }

    const_reference back() const
    {
        CXmlNodeIterator refNode(m_pNodelist.m_nodes.size() -1, (std::vector<rapidxml::xml_node<>*>&)m_pNodelist.m_nodes);
        return *refNode;
    }

    size_type size()
    {
        return (size_type)(IsValid() ? m_pNodelist.m_nodes.size() : 0);
    }

    bool empty()
    {
        return IsValid() ? (m_pNodelist.m_nodes.size() == 0) : true;
    }

    void operator=(DOMNodeList& pNode)
    {
        m_pNodelist = pNode;
    }

    void operator=(DOMNodeList* pNode)
    {
        m_pNodelist = *pNode;
    }
};

#endif