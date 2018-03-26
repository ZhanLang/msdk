#pragma once

#include <string>
#include <vector>

#include <xmlvalue/XmlValueA.h>
namespace msdk{;
namespace UTIL{;


class CMsRegData
{
public:
	enum RegType
	{
		RegNone		=0,
		RegSz		=1,
		RegBinary	=3,
		RegDword	=4,
	} ;

	RegType type()
	{
		return regType;
	}

	std::vector<BYTE> toBinary()
	{
		return *(Value.bValue);
	}

	std::basic_string<TCHAR> toString()
	{
		return *(Value.sValue);
	}

	DWORD toDword()
	{
		return Value.dwValue;
	}


	CMsRegData():regType( RegNone),m_hKey(NULL)
	{
		Value.bValue = 0;
	}

	CMsRegData(const TCHAR* v) : regType(RegSz),m_hKey(NULL)
	{
		Value.sValue = new std::basic_string<TCHAR>(v);
	}

	CMsRegData(std::basic_string<TCHAR> const& v): regType(RegSz),m_hKey(NULL)
	{
		Value.sValue = new std::basic_string<TCHAR>(v);
	}

	CMsRegData(DWORD v) : regType(RegDword),m_hKey(NULL)
	{
		Value.dwValue = v;
	}

	CMsRegData(CMsRegData const& v):regType(RegNone)
	{
		*this = v;
	}

	CMsRegData(BYTE* value, int nBytes) : regType(RegBinary)
	{
		Value.bValue = new std::vector<BYTE>();
		Value.bValue->resize(nBytes);
		memcpy(&(Value.bValue->begin()), value, nBytes);
	}

	CMsRegData& operator=(CMsRegData const& v)
	{
		switch(v.regType)
		{
		case RegBinary: Value.bValue = new std::vector<BYTE>(*v.Value.bValue); break;
		case RegDword:  Value.dwValue = v.Value.dwValue; break;
		case RegSz:		Value.sValue = new std::basic_string<TCHAR>(*v.Value.sValue); break;
		}

		regType = v.regType;
		return *this;
	}

	CMsRegData& operator=(const TCHAR* v)
	{
		return operator=(CMsRegData(v));
	}

	CMsRegData& operator=(DWORD v)
	{
		return operator=(CMsRegData(v));
	}

	void setKey(HKEY hKey)
	{
		m_hKey = hKey;
	}

	void setSubKey(const TCHAR* subKey)
	{
		sKey = subKey;
	}

	void Invalidate()
	{
		switch( regType)
		{
		case RegBinary:		delete Value.bValue; break;
		case RegSz:			delete Value.sValue; break;
		}
	}

	void Initialize()
	{
		RegGetValue(m_hKey, NULL, m_sKey.c_str(), )
	}

private:
	HKEY m_hKey;
	RegType regType;
	std::basic_string<TCHAR> m_sKey;
	union
	{
		DWORD					  dwValue;
		std::vector<BYTE>*		  bValue;
		std::basic_string<TCHAR>* sValue;
	} Value;
};

#include "mscom/msbase.h"
#include "util/utility.h"

class CMsRegValue
{
public:
	CMsRegValue() : m_hKey(NULL)
	{

	}

	CMsRegData operator[](const TCHAR* value)
	{
		CMsRegData Data;
		Data.setKey(m_hKey);
		Data.setSubKey(value);
		return Data;
	}

public: 
	void setKey(HKEY hKey)
	{
		m_hKey = hKey;
	}
private:
	HKEY m_hKey;
};

class CMsRegKey
{
public:
	CMsRegKey(HKEY hParentKey, LPCTSTR szKey, BOOL bAutoCreate = TRUE)
	{
		m_bAutoCreate = bAutoCreate;
		m_hParent = hParentKey;
		m_hKey = NULL;
		m_sKey = szKey;
		Initialize();
	}

	~CMsRegKey()
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}


	CMsRegKey operator[](TCHAR* value)
	{
		return CMsRegKey(m_hKey, value,m_bAutoCreate);
	}

	operator HKEY ()
	{
		return m_hKey;
	}

	

	BOOL IsValid()
	{
		return m_hKey != NULL;
	}

	CMsRegValue Vlaue;
private:
	BOOL CreateKey()
	{
		return RegCreateKey(m_hParent, m_sKey.c_str(), &m_hKey) !=  ERROR_SUCCESS;
	}

	BOOL Initialize()
	{
		RegOpenKey(m_hParent, m_sKey.c_str(), &m_hKey);
		if( !m_hKey && m_bAutoCreate )
		{
			RegCreateKey(m_hParent, m_sKey.c_str(), &m_hKey);
		}

		Vlaue.setKey(m_hKey);
		return m_hKey != 0;
	}

private:
	BOOL m_bAutoCreate;
	HKEY m_hKey;
	HKEY m_hParent;
	std::basic_string<TCHAR> m_sKey;
};


};};