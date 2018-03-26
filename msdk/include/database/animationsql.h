#pragma once
#include <map>
#include <string>
enum
{
	SQL_FILED_TYPE_INTEGER		= 0x0001,
	SQL_FILED_TYPE_REAL			= 0x0002,
	SQL_FILED_TYPE_TEXT			= 0x0004,

	SQL_FILED_TYPE_PRIMARY_KEY	= 0x0100,
	SQL_FILED_TYPE_NOT_NULL		= 0x0200,
};

//////////////////////////////////////////////////////////////////////////
class CSqlValueA
{
public:
	CSqlValueA();
	CSqlValueA(int value);
	CSqlValueA(double value);
	CSqlValueA(std::string const& value);
	CSqlValueA(const char* value);
public:
	std::string m_strValue;
};

class CSqlValueW
{
public:
	CSqlValueW();
	CSqlValueW(int value);
	CSqlValueW(double value);
	CSqlValueW(std::wstring const& value);
	CSqlValueW(LPCWSTR value);
public:
	std::wstring m_strValue;
};


typedef std::map<std::string , int>			CTableFieldA;
typedef std::map<std::string, CSqlValueA>	CFiledValueA;
typedef std::map<std::wstring , int>		CTableFieldW;
typedef std::map<std::wstring, CSqlValueW>	CFiledValueW;

typedef std::map<int, std::string>			CSelectFiledA;
typedef std::map<int, std::wstring>			CSelectFiledW;


//SELECT
class CSqlSelectA
{
public:
	CSqlSelectA(const char* szTable);
	CSqlSelectA& Select(LPCSTR szSql = NULL);
	CSqlSelectA& Select(const CSelectFiledA& filed);
	CSqlSelectA& Where(const CFiledValueA& filed);
	std::string GetSql();
private:
	std::string m_strTable;
	std::string	m_strSql;
	CSelectFiledA  m_fileSelect;
	CFiledValueA m_filedValue;
};


class CSqlSelectW
{
public:
	CSqlSelectW(LPCWSTR szTable);
	CSqlSelectW& Select(LPCWSTR szSql = NULL);
	CSqlSelectW& Select(const CSelectFiledW& filed);
	CSqlSelectW& Where(const CFiledValueW& filed);
	LPCWSTR GetSql();
private:
	std::wstring m_strTable;
	std::wstring	m_strSql;
	CSelectFiledW  m_fileSelect;
	CFiledValueW m_filedValue;
};


//UPDATE
class CSqlUpdateA
{
public:
	CSqlUpdateA(const char* szTable);
	CSqlUpdateA& Update(const CFiledValueA& filed);
	CSqlUpdateA& Where(const CFiledValueA& filed);
	LPCSTR GetSql();
private:
	std::string m_strTable;
	CFiledValueA m_Update;
	CFiledValueA m_where;
	std::string m_strSql;
};


class CSqlUpdateW
{
public:
	CSqlUpdateW(LPCWSTR szTable);
	CSqlUpdateW& Update(const CFiledValueW& filed);
	CSqlUpdateW& Where(const CFiledValueW& filed);
	LPCWSTR GetSql();
private:
	std::wstring m_strTable;
	CFiledValueW m_Update;
	CFiledValueW m_where;
	std::wstring m_strSql;
};

//DELETE
class CSqlDeleteA
{
public:
	CSqlDeleteA(const char* szTable);
	CSqlDeleteA& Where(const CFiledValueA& filed);
	std::string GetSql();
public:
	std::string m_strTable;
	CFiledValueA m_where;
	std::string m_strSql;
};



class CSqlDeleteW
{
public:
	CSqlDeleteW(LPCWSTR szTable);
	CSqlDeleteW& Where(const CFiledValueW& filed);
	LPCWSTR GetSql();
public:
	std::wstring m_strTable;
	CFiledValueW m_where;
	std::wstring m_strSql;
};


//INSERT
class CSqlInsertA
{
public:
	CSqlInsertA(const char* szTable);
	CSqlInsertA& Insert(const CFiledValueA& filed);
	std::string GetSql();
public:
	std::string m_strTable;
	CFiledValueA m_insert;
	std::string m_strSql;
};


class CSqlInsertW
{
public:
	CSqlInsertW(LPCWSTR szTable);
	CSqlInsertW& Insert(const CFiledValueW& filed);
	LPCWSTR GetSql();
public:
	std::wstring m_strTable;
	CFiledValueW m_insert;
	std::wstring m_strSql;
};


//TABLE
class CSqlTableA
{
public:
	CSqlTableA(const char* szTable);
	CSqlTableA& Create(const CTableFieldA& field);
	CSqlTableA& Drop();
	std::string GetSql();
public:
	std::string m_strTable;
	std::string m_strSql;
	CTableFieldA m_create;
};

class CSqlTableW
{
public:
	CSqlTableW(LPCWSTR szTable);
	CSqlTableW& Create(const CTableFieldW& field);
	CSqlTableW& Drop();
	LPCWSTR GetSql();
public:
	std::wstring m_strTable;
	std::wstring m_strSql;
	CTableFieldW m_create;
};