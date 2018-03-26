
#include "stdafx.h"
#include "animationsql.h"

//////////////////////////////////////////////////////////////////////////
CSqlInsertA::CSqlInsertA(const char* szTable)
{
	m_strTable+="\"";
	m_strTable += szTable;
	m_strTable+="\"";
}

CSqlInsertA& CSqlInsertA::Insert(const CFiledValueA& filed)
{
	m_insert.clear();
	m_insert = filed;

	return *this;
}

std::string CSqlInsertA::GetSql()
{
	m_strSql.clear();

	m_strSql += "INSERT INTO ";
	m_strSql += m_strTable;

	m_strSql += " ( ";
	for (CFiledValueA::iterator it = m_insert.begin() ; it != m_insert.end() ; it++)
	{
		if (it != m_insert.begin()){
			m_strSql += ",";
		}

		m_strSql += "\"";
		m_strSql += it->first;
		m_strSql += "\"";
	}
	m_strSql += ") VALUES (";

	for (CFiledValueA::iterator it = m_insert.begin() ; it != m_insert.end() ; it++)
	{
		if (it != m_insert.begin()){
			m_strSql += ",";
		}

		m_strSql += it->second.m_strValue;
	}
	
	m_strSql+=");";
	
	return m_strSql.c_str();
}







CSqlSelectA::CSqlSelectA(const char* szTable)
{
	m_strTable+="\"";
	m_strTable += szTable;
	m_strTable+="\"";
}

CSqlSelectA& CSqlSelectA::Select(LPCSTR szSql /*= NULL*/)
{
	m_strSql.clear();
	m_fileSelect.clear();
	m_filedValue.clear();

	return *this;
}

CSqlSelectA& CSqlSelectA::Select(const CSelectFiledA& filed)
{
	m_strSql.clear();
	m_fileSelect.clear();
	m_filedValue.clear();

	m_fileSelect = filed;
	return *this;

}

CSqlSelectA& CSqlSelectA::Where(const CFiledValueA& filed)
{
	m_filedValue = filed;
	return *this;
}

std::string CSqlSelectA::GetSql()
{
	m_strSql = "SELECT ";
	if(m_fileSelect.size())
	{
		for (CSelectFiledA::iterator it = m_fileSelect.begin() ; it != m_fileSelect.end() ; it++) {
			if (it != m_fileSelect.begin()){
				m_strSql += ",";
			}

			m_strSql+="\""; 
			m_strSql+=it->first;
			m_strSql+="\" ";
		}
	}
	else{
		m_strSql += " * ";
	}

	m_strSql+= "FROM ";
	m_strSql+=m_strTable;

	for (CFiledValueA::iterator it = m_filedValue.begin() ; it != m_filedValue.end() ; it++)
	{
		if (it == m_filedValue.begin()){
			m_strSql += " WHERE ";
		}
		else{
			m_strSql += " AND ";
		}

		m_strSql += "\"";
		m_strSql += it->first;
		m_strSql += "\" = ";

		m_strSql+= it->second.m_strValue;
	}

	m_strSql += ";";
	return m_strSql.c_str();
}


CSqlDeleteA::CSqlDeleteA(const char* szTable)
{
	m_strTable+="\"";
	m_strTable += szTable;
	m_strTable+="\"";
}

CSqlDeleteA& CSqlDeleteA::Where(const CFiledValueA& filed)
{
	m_where.clear();
	m_where = filed;
	return *this;
}

std::string CSqlDeleteA::GetSql()
{
	m_strSql.clear();

	m_strSql += "DELETE FROM ";

	m_strSql += m_strTable;

	for (CFiledValueA::iterator it = m_where.begin() ; it != m_where.end() ; it++)
	{
		if (it == m_where.begin()){
			m_strSql += " WHERE ";
		}
		else{
			m_strSql += " AND ";
		}

		m_strSql += "\"";
		m_strSql += it->first;
		m_strSql += "\" = ";

		m_strSql+= it->second.m_strValue;
	}

	m_strSql += ";";
	return m_strSql.c_str();
}

CSqlUpdateA::CSqlUpdateA(const char* szTable)
{
	m_strTable+="\"";
	m_strTable += szTable;
	m_strTable+="\"";
}

CSqlUpdateA& CSqlUpdateA::Update(const CFiledValueA& filed)
{
	m_Update.clear();
	m_Update = filed;


	return *this;
}

CSqlUpdateA& CSqlUpdateA::Where(const CFiledValueA& filed)
{
	m_where.clear();
	m_where = filed;


	return *this;
}

LPCSTR CSqlUpdateA::GetSql()
{
	m_strSql.clear();

	m_strSql += "UPDATE ";
	m_strSql += m_strTable;
	m_strSql += " SET ";

	for (CFiledValueA::iterator it = m_Update.begin() ; it != m_Update.end() ; it++)
	{
		if (it != m_Update.begin())
		{
			m_strSql += " , ";
		}

		m_strSql += "\"";
		m_strSql += it->first;
		m_strSql += "\"=";
		m_strSql += it->second.m_strValue;
	}

	for (CFiledValueA::iterator it = m_where.begin() ; it != m_where.end() ; it++)
	{
		if (it == m_where.begin()){
			m_strSql += " WHERE ";
		}
		else{
			m_strSql += " AND ";
		}

		m_strSql += "\"";
		m_strSql += it->first;
		m_strSql += "\" = ";

		m_strSql+= it->second.m_strValue;
	}

	m_strSql += ";";
	return m_strSql.c_str();
}


CSqlTableA::CSqlTableA(const char* szTable)
{
	m_strTable += "\"";
	m_strTable += szTable;
	m_strTable += "\"";
}



CSqlTableA& CSqlTableA::Create(const CTableFieldA& field)
{
	m_create.clear();
	m_create = field;

	return *this;
	
}

std::string CSqlTableA::GetSql()
{
	m_strSql.clear();
	m_strSql = "CREATE TABLE ";
	m_strSql += m_strTable;

	m_strSql += " (";

	for ( CTableFieldA::const_iterator it = m_create.begin() ; it != m_create.end() ; it++)
	{
		std::string strFieldName = it->first;
		int		nFieldType	 = it->second;

		if (it != m_create.begin())
		{
			m_strSql += ",";
		}

		m_strSql+="\""; 
		m_strSql+=strFieldName;
		m_strSql+="\" ";

		if ( nFieldType & SQL_FILED_TYPE_INTEGER)
		{
			m_strSql += " INTEGER ";
		}
		else if (nFieldType & SQL_FILED_TYPE_REAL)
		{
			m_strSql += " REAL ";
		}
		else if (nFieldType & SQL_FILED_TYPE_TEXT)
		{
			m_strSql += " TEXT ";
		}

		if (nFieldType & SQL_FILED_TYPE_PRIMARY_KEY)
		{
			m_strSql += " PRIMARY KEY ";
		}
		else if (nFieldType & SQL_FILED_TYPE_NOT_NULL)
		{
			m_strSql += " NOT NULL ";
		}
	}

	m_strSql += " );";

	return m_strSql.c_str();
}

CSqlTableA& CSqlTableA::Drop()
{
	m_strSql += "DELETE FROM ";
	m_strSql += m_strTable;
	return *this;
}



CSqlValueA::CSqlValueA() 
{
	m_strValue = "";
}

CSqlValueA::CSqlValueA(int value)
{
	char buf[MAX_PATH] = {0};
	sprintf_s(buf, MAX_PATH, "%d", value);
	m_strValue = buf;
}

CSqlValueA::CSqlValueA(double value)
{
	char buf[MAX_PATH] = {0};
	sprintf_s(buf, MAX_PATH, "%f", value);
	m_strValue = buf;
}

CSqlValueA::CSqlValueA(std::string const& value)
{
	char buf[1024] = {0};
	sprintf_s(buf, MAX_PATH, "\"%s\"", value.c_str());
	m_strValue = buf;
}

CSqlValueA::CSqlValueA(const char* value)
{
	char buf[1024] = {0};
	sprintf_s(buf, MAX_PATH, "\"%s\"", value);
	m_strValue = buf;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CSqlInsertW::CSqlInsertW(LPCWSTR szTable)
{
	m_strTable += L"\"";
	m_strTable += szTable;
	m_strTable += L"\"";
}

CSqlInsertW& CSqlInsertW::Insert(const CFiledValueW& filed)
{
	m_insert.clear();
	m_insert = filed;

	return *this;
}

LPCWSTR CSqlInsertW::GetSql()
{
	m_strSql.clear();

	m_strSql += L"INSERT INTO ";
	m_strSql += m_strTable;

	m_strSql += L" ( ";
	for (CFiledValueW::iterator it = m_insert.begin() ; it != m_insert.end() ; it++)
	{
		if (it != m_insert.begin()){
			m_strSql += L",";
		}

		m_strSql += L"\"";
		m_strSql += it->first;
		m_strSql += L"\"";
	}
	m_strSql += L") VALUES (";

	for (CFiledValueW::iterator it = m_insert.begin() ; it != m_insert.end() ; it++)
	{
		if (it != m_insert.begin()){
			m_strSql += L",";
		}

		m_strSql += it->second.m_strValue;
	}

	m_strSql+=L");";

	return m_strSql.c_str();
}










//////////////////////////////////////////////////////////////////////////

CSqlSelectW::CSqlSelectW(LPCWSTR szTable)
{
	m_strTable += L"\"";
	m_strTable += szTable;
	m_strTable += L"\"";
}

CSqlSelectW& CSqlSelectW::Select(LPCWSTR szSql /*= NULL*/)
{
	m_strSql.clear();
	m_fileSelect.clear();
	m_filedValue.clear();

	return *this;
}

CSqlSelectW& CSqlSelectW::Select(const CSelectFiledW& filed)
{
	m_strSql.clear();
	m_fileSelect.clear();
	m_filedValue.clear();

	m_fileSelect = filed;
	return *this;

}

CSqlSelectW& CSqlSelectW::Where(const CFiledValueW& filed)
{
	m_filedValue = filed;
	return *this;
}

LPCWSTR CSqlSelectW::GetSql()
{
	m_strSql = L"SELECT ";
	if(m_fileSelect.size())
	{
		for (CSelectFiledW::iterator it = m_fileSelect.begin() ; it != m_fileSelect.end() ; it++) {
			if (it != m_fileSelect.begin()){
				m_strSql += L",";
			}

			m_strSql+=L"\""; 
			m_strSql+=it->first;
			m_strSql+=L"\" ";
		}
	}
	else{
		m_strSql += L" * ";
	}

	m_strSql+= L"FROM ";
	m_strSql+=m_strTable;

	for (CFiledValueW::iterator it = m_filedValue.begin() ; it != m_filedValue.end() ; it++)
	{
		if (it == m_filedValue.begin()){
			m_strSql += L" WHERE ";
		}
		else{
			m_strSql += L" AND ";
		}

		m_strSql += L"\"";
		m_strSql += it->first;
		m_strSql += L"\" = ";

		m_strSql+= it->second.m_strValue;
	}

	m_strSql += L";";
	return m_strSql.c_str();
}


CSqlDeleteW::CSqlDeleteW(LPCWSTR szTable)
{
	m_strTable += L"\"";
	m_strTable += szTable;
	m_strTable += L"\"";
}

CSqlDeleteW& CSqlDeleteW::Where(const CFiledValueW& filed)
{
	m_where.clear();
	m_where = filed;
	return *this;
}

LPCWSTR CSqlDeleteW::GetSql()
{
	m_strSql.clear();

	m_strSql += L"DELETE FROM ";

	m_strSql += m_strTable;

	for (CFiledValueW::iterator it = m_where.begin() ; it != m_where.end() ; it++)
	{
		if (it == m_where.begin()){
			m_strSql += L" WHERE ";
		}
		else{
			m_strSql += L" AND ";
		}

		m_strSql += L"\"";
		m_strSql += it->first;
		m_strSql += L"\" = ";

		m_strSql+= it->second.m_strValue;
	}

	m_strSql += L";";
	return m_strSql.c_str();
}

CSqlUpdateW::CSqlUpdateW(LPCWSTR szTable)
{
	m_strTable += L"\"";
	m_strTable += szTable;
	m_strTable += L"\"";
}

CSqlUpdateW& CSqlUpdateW::Update(const CFiledValueW& filed)
{
	m_Update.clear();
	m_Update = filed;


	return *this;
}

CSqlUpdateW& CSqlUpdateW::Where(const CFiledValueW& filed)
{
	m_where.clear();
	m_where = filed;


	return *this;
}

LPCWSTR CSqlUpdateW::GetSql()
{
	m_strSql.clear();

	m_strSql += L"UPDATE ";
	m_strSql += m_strTable;
	m_strSql += L" SET ";

	for (CFiledValueW::iterator it = m_Update.begin() ; it != m_Update.end() ; it++)
	{
		if (it != m_Update.begin())
		{
			m_strSql += L" , ";
		}

		m_strSql += L"\"";
		m_strSql += it->first;
		m_strSql += L"\"=";
		m_strSql += it->second.m_strValue;
	}

	for (CFiledValueW::iterator it = m_where.begin() ; it != m_where.end() ; it++)
	{
		if (it == m_where.begin()){
			m_strSql += L" WHERE ";
		}
		else{
			m_strSql += L" AND ";
		}

		m_strSql += L"\"";
		m_strSql += it->first;
		m_strSql += L"\" = ";

		m_strSql+= it->second.m_strValue;
	}

	m_strSql += L";";
	return m_strSql.c_str();
}


CSqlTableW::CSqlTableW(LPCWSTR szTable)
{
	m_strTable += L"\"";
	m_strTable += szTable;
	m_strTable += L"\"";
}



CSqlTableW& CSqlTableW::Create(const CTableFieldW& field)
{
	m_create.clear();
	m_create = field;

	return *this;

}

LPCWSTR CSqlTableW::GetSql()
{
	m_strSql.clear();
	m_strSql = L"CREATE TABLE ";
	m_strSql += m_strTable;

	m_strSql += L" (";

	BOOL hasPrim = FALSE;

	for ( CTableFieldW::const_iterator it = m_create.begin() ; it != m_create.end() ; it++)
	{
		std::wstring strFieldName = it->first;
		int		nFieldType	 = it->second;

		if (it != m_create.begin())
		{
			m_strSql += L",";
		}

		m_strSql+=L"\""; 
		m_strSql+=strFieldName;
		m_strSql+=L"\" ";

		if ( nFieldType & SQL_FILED_TYPE_INTEGER)
		{
			m_strSql += L" INTEGER ";
		}
		else if (nFieldType & SQL_FILED_TYPE_REAL)
		{
			m_strSql += L" REAL ";
		}
		else if (nFieldType & SQL_FILED_TYPE_TEXT)
		{
			m_strSql += L" TEXT ";
		}

		if (nFieldType & SQL_FILED_TYPE_PRIMARY_KEY)
		{
			hasPrim = TRUE;
		}
		else if (nFieldType & SQL_FILED_TYPE_NOT_NULL)
		{
			m_strSql += L" NOT NULL ";
		}
	}

	

	if ( hasPrim )
	{
		m_strSql += L", PRIMARY KEY (";
		for ( CTableFieldW::const_iterator it = m_create.begin() ; it != m_create.end() ; it++)
		{
			int		nFieldType	 = it->second;
			if (nFieldType & SQL_FILED_TYPE_PRIMARY_KEY)
			{
				if (it != m_create.begin())
				{
					m_strSql += L",";
				}

				m_strSql+=L"\""; 
				m_strSql+=it->first;
				m_strSql+=L"\" ";
			}
		}

		m_strSql += L" )";
	}

	m_strSql += L" );";

	return m_strSql.c_str();
}

CSqlTableW& CSqlTableW::Drop()
{
	m_strSql += L"DELETE FROM ";
	m_strSql += m_strTable;
	return *this;
}



CSqlValueW::CSqlValueW() 
{
	m_strValue = L"";
}

CSqlValueW::CSqlValueW(int value)
{
	wchar_t buf[MAX_PATH] = {0};
	swprintf_s(buf, MAX_PATH, L"%d", value);
	m_strValue = buf;
}

CSqlValueW::CSqlValueW(double value)
{
	wchar_t buf[MAX_PATH] = {0};
	swprintf_s(buf, MAX_PATH, L"%f", value);
	m_strValue = buf;
}

CSqlValueW::CSqlValueW(std::wstring const& value)
{
	wchar_t buf[1024] = {0};
	swprintf_s(buf, MAX_PATH, L"\"%s\"", value.c_str());
	m_strValue = buf;
}

CSqlValueW::CSqlValueW(LPCWSTR value)
{
	wchar_t buf[1024] = {0};
	swprintf_s(buf, MAX_PATH, L"\"%s\"", value);
	m_strValue = buf;
}
