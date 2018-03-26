#include "StdAfx.h"
#include "DBConnRemote.h"
#include <string>
using namespace std;


class CTimeUsed
{
public:
	CTimeUsed()
	{
		m_dwCount = GetTickCount();
	};
	CTimeUsed(LPCTSTR lpFuncName)
	{
		m_strFuncName = lpFuncName;
		m_dwCount = GetTickCount();


	};
	~CTimeUsed()
	{
		DWORD dwTemp = 	GetTickCount();
		DWORD dwThreadID = 	GetCurrentThreadId();
		owstringstream oo;

		oo << _T("[线程],")<<dwThreadID;
		oo << _T(",")<<m_strFuncName;
		oo << _T(",")<<m_dwCount;
		oo << _T(",")<<dwTemp;
		oo << _T(",")<<dwTemp - m_dwCount;
		oo << _T("\n");
		wstring strTemp = oo.str();
		OutputDebugString(strTemp.c_str());
	};

private:
	DWORD m_dwCount;
	wstring m_strFuncName;

};


CFastDBConnRemote::CFastDBConnRemote(void)
{
	m_iSession = cli_bad_address;
}

CFastDBConnRemote::CFastDBConnRemote(IN LPCTSTR lpDBName,IN LPCTSTR lpIP,IN INT iPort)
{
	m_iSession = cli_bad_address;

	//HRESULT hr = Init(lpDBName,lpIP,iPort);
	//FAILD_LOGONLY(SUCCEEDED(hr));
}

CFastDBConnRemote::~CFastDBConnRemote(void)
{
	//HRESULT hr = Uninit();
	//FAILD_LOGONLY(SUCCEEDED(hr));
}

HRESULT CFastDBConnRemote::Init(IN LPCTSTR lpDBName,IN LPCTSTR lpIP,IN INT iPort)
{
	FAILEXIT_FAIL(lpDBName);
	FAILEXIT_FAIL(lpIP);
	FAILEXIT_FAIL(iPort);

	CTimeUsed theTimer(_T("CFastDBConnRemote::Init"));

	m_strDBName = lpDBName;

	Helper::CTransStr theTrans;
	string strIP = theTrans.MYW2A(lpIP);
	
	ostringstream oo;
	oo<<strIP<<":"<<iPort;
	string strUrl = oo.str();

	//连接指定数据库.
	{
		CTimeUsed theTimer(_T("CFastDBConnRemote::Init  cli_open"));
		m_iSession = cli_open(strUrl.c_str(), 3, 1);
		FAILEXIT_FAIL(m_iSession > 0);
	}
	
	return S_OK;
}
HRESULT CFastDBConnRemote::Uninit()
{
	if(m_iSession > 0)
	{
		cli_close(m_iSession);
		m_iSession = cli_bad_address;
	}

	return S_OK;
}

HRESULT CFastDBConnRemote::Connect ( LPCTSTR szConn, LONG lOptions )
{
	return E_NOTIMPL;
}
HRESULT CFastDBConnRemote::Close ( void ) 
{
	return E_NOTIMPL;
}

BOOL  CFastDBConnRemote::IsTableExists ( LPCTSTR szTable )
{	
	FAILEXIT_FALSE(szTable);
	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);

	cli_table_descriptor* pTables = NULL;
	INT nCount = cli_show_tables(m_iSession, &pTables);
	FAILEXIT_FALSE(nCount);
	FAILEXIT_FALSE(pTables);
		
	cli_table_descriptor* pTemp  = pTables;
	for(INT i = 0; i < nCount; ++i)
	{
		if(strTableName == pTemp->name)
		{
			free(pTables);
			return TRUE;
		}
		++ pTemp ;
	}
	free(pTables);
	return FALSE;
}

HRESULT CFastDBConnRemote::CompileStmt ( LPCTSTR szSQL, IGenericStmt** ppStmt )
{
	return E_NOTIMPL;
}

HRESULT CFastDBConnRemote::ExecuteDML ( LPCTSTR szSQL, LONG* pAffected )
{
	//因为,确认了,远程只读.所以时间紧的情况下,这里不细调试了.
	return E_NOTIMPL;

	FAILEXIT_FAIL(NULL != szSQL);
	//增,删,改.建表,
	//取得表名,对于
	Helper::CTransStr theTrans;
	BOOL bIsDel = theTrans.IsFirstWord(TEXT("delete"),szSQL);
	if(TRUE == bIsDel)
	{
		HRESULT hr = DeleteStatement(szSQL,pAffected);
		FAILEXIT_FAIL(SUCCEEDED(hr));
		return S_OK;
	}

	BOOL bIsIns = theTrans.IsFirstWord(TEXT("insert"),szSQL);
	if(TRUE == bIsIns)
	{
		HRESULT hr = InsertStatement(szSQL,pAffected);
		FAILEXIT_FAIL(SUCCEEDED(hr));
		return S_OK;
	}

	BOOL bIsUp = theTrans.IsFirstWord(TEXT("update"),szSQL);
	if(TRUE == bIsUp)
	{
		HRESULT hr = UpdateStatement(szSQL,pAffected);
		FAILEXIT_FAIL(SUCCEEDED(hr));
		return S_OK;
	}

	return S_OK;;
}

HRESULT CFastDBConnRemote::Execute( LPCTSTR szSQL, IGenericRS** ppRecordset, DWORD* pFields, INT nFields ) 
{
	FAILEXIT_FAIL(NULL != szSQL);
	
	//注:这里可以考虑自己提取Fields列表.

	Helper::CTransStr theTrans;

	//取得表名,对于
	wstring strTableName ;
	BOOL bFinded = theTrans.FindNextWord(TEXT("from"),szSQL,strTableName);
	FAILEXIT_FAIL(bFinded);

	HRESULT hr = SelectStatement(strTableName.c_str(),szSQL,ppRecordset,pFields,nFields);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}

HRESULT CFastDBConnRemote::GetTypeDecl ( VARTYPE vt, LPTSTR szBuf, INT nBufLen ) 
{
	
	return E_NOTIMPL;
}


HRESULT CFastDBConnRemote::BeginTrans ( VOID ) 
{
	
	return E_NOTIMPL;
}
HRESULT CFastDBConnRemote::CommitTrans ( VOID ) 
{
	int nRet = cli_precommit(m_iSession);
	FAILEXIT_FAIL(cli_ok == nRet);
	return S_OK;
}
HRESULT CFastDBConnRemote::RollbackTrans( VOID ) 
{
	int nRet = cli_abort(m_iSession);
	FAILEXIT_FAIL(cli_ok == nRet);
	return S_OK;
}


HRESULT CFastDBConnRemote::GetTypeDecl( RsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen ) 
{
	
	return E_NOTIMPL;
}

HRESULT CFastDBConnRemote::GetAutoIDDecl ( LPTSTR szBuf, INT nBufLen ) 
{

	return E_NOTIMPL;
}

HRESULT CFastDBConnRemote::AttachRealConn ( VOID ) 
{

	return E_NOTIMPL;
}



HRESULT CFastDBConnRemote::DetachRealConn ( VOID ) 
{

	return E_NOTIMPL;
}


HRESULT CFastDBConnRemote::GetTableFieldsCount ( LPCTSTR szTable,DWORD& dwCount ) 
{
	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);

	MAP_TABLE::iterator it = m_theTableSet.find(strTableName);
	if(it != m_theTableSet.end())
	{//找到表的信息
		dwCount = it->second.size();
		return S_OK;
	}
	//没找到,则远程取.	
	HRESULT hr = QueryFieldsInfo(szTable);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	it = m_theTableSet.find(strTableName);
	if(it != m_theTableSet.end())
	{//找到表的信息
		dwCount = it->second.size();
		return S_OK;
	}

	return E_FAIL;
}


HRESULT CFastDBConnRemote::GetTableFieldInfo ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize  ) 
{
	FAILEXIT_FAIL(szTable);
	FAILEXIT_FAIL(FieldName);
	
	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);

	MAP_TABLE::iterator it = m_theTableSet.find(strTableName);
	if(it != m_theTableSet.end())
	{//找到了.
		FAILEXIT_FAIL(index >= 0 && index < it->second.size());
		tag_Field* pField = &(it->second[index]);

		HRESULT hr = GetFindInfoOut(pField,FieldName,FieldType,dwSize);
		FAILEXIT_FAIL(SUCCEEDED(hr));
		
		return S_OK;
	}

	//没找到,则远程取.	
	HRESULT hr = QueryFieldsInfo(szTable);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	it = m_theTableSet.find(strTableName);
	if(it != m_theTableSet.end())
	{
		FAILEXIT_FAIL(index >= 0 && index < it->second.size());
		tag_Field* pField = &(it->second[index]);

		HRESULT hr = GetFindInfoOut(pField,FieldName,FieldType,dwSize);
		FAILEXIT_FAIL(SUCCEEDED(hr));

		return S_OK;
	}

	return E_FAIL;
}


BOOL CFastDBConnRemote::IsOpen( VOID ) 
{
	if(cli_bad_address == m_iSession)
		return FALSE;
	return TRUE;
}

HRESULT CFastDBConnRemote::RegistTableInfo(LPCWSTR lpTableName, tag_Field* pFieldsSet,INT iFieldsCount) 
{
	FAILEXIT_FAIL(lpTableName);
	FAILEXIT_FAIL(pFieldsSet);
	FAILEXIT_FAIL(iFieldsCount);

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(lpTableName);
	VEC_FIELDSET theFieldsSet;
	for(INT i = 0; i < iFieldsCount; ++i)
	{
		theFieldsSet.push_back(pFieldsSet[i]);
	}
	m_theTableSet[strTableName] = theFieldsSet;
	return S_OK;
}
HRESULT CFastDBConnRemote::RegistTableInfo(LPCWSTR lpTableName, vector<tag_Field>& theFieldSet) 
{
	FAILEXIT_FAIL(lpTableName);
	FAILEXIT_FAIL(theFieldSet.size())

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(lpTableName);
	m_theTableSet[strTableName] = theFieldSet;

	return S_OK;
}


HRESULT CFastDBConnRemote::CreateTableRemote(LPCWSTR lpTableName, tag_Field* pFieldsSet,INT iFieldsCount) 
{	

	HRESULT hr = RegistTableInfo(lpTableName,pFieldsSet,iFieldsCount);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	hr = CreateTableRemote(lpTableName);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}
HRESULT CFastDBConnRemote::CreateTableRemote(LPCWSTR lpTableName) 
{

	FAILEXIT_FAIL(lpTableName);

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(lpTableName);
	MAP_TABLE::iterator it = m_theTableSet.find(strTableName);	
	FAILEXIT_FAIL(it != m_theTableSet.end());

	VEC_FIELDSET& theVec = it->second;
	INT iFieldCount = theVec.size();
	FAILEXIT_FAIL(iFieldCount);
		
	Helper::CSimpleBuffer<cli_field_descriptor> theBuf(iFieldCount);
	cli_field_descriptor* pDescSet = theBuf.GetPointer();
	FAILEXIT_FAIL(pDescSet);
	
	for(INT i = 0;i < iFieldCount; ++i)
	{
		//遍历信息表,建立描述信息用来建表.
		cli_field_descriptor* pItem = &(pDescSet[i]);
		tag_Field* pField = &(theVec[i]);

		pItem->type = (cli_var_type)pField->GetCliTypeByVTType();
		pItem->name = (CONST CHAR*)pField->szFieldName;
		pItem->refTableName = NULL;
		pItem->inverseRefFieldName = NULL;
	}

	INT iRet = cli_create_table(
		m_iSession
		,strTableName.c_str()
		,iFieldCount
		,pDescSet
		);
	FAILD_LOGONLY(iRet != cli_table_already_exists && iRet != cli_not_implemented);
	FAILEXIT_FAIL(cli_ok == iRet);

	return S_OK;
}

HRESULT CFastDBConnRemote::InsertStatement(LPCTSTR szSQL, LONG* pAffected )
{
	FAILEXIT_FAIL(szSQL);	
	Helper::CTransStr theTrans;
	string strSql = theTrans.MYW2A(szSQL);

	INT iStatement = cli_statement(m_iSession, strSql.c_str());
	FAILEXIT_FAIL(iStatement >= 0);

	INT nRet = cli_insert(iStatement, NULL);
	FAILD_LOGONLY(cli_ok == nRet);

	nRet = cli_commit(m_iSession);
	FAILD_LOGONLY(cli_ok == nRet);

	nRet = cli_free(iStatement);
	FAILEXIT_FAIL(cli_ok == nRet);

	return S_OK;
}
HRESULT CFastDBConnRemote::UpdateStatement(LPCTSTR szSQL, LONG* pAffected )
{
	FAILEXIT_FAIL(szSQL);	
	Helper::CTransStr theTrans;
	string strSql = theTrans.MYW2A(szSQL);

	INT iStatement = cli_statement(m_iSession, strSql.c_str());
	FAILEXIT_FAIL(iStatement >= 0);

	INT nRet = cli_update(iStatement);
	FAILD_LOGONLY(cli_ok == nRet);

	nRet = cli_commit(m_iSession);
	FAILD_LOGONLY(cli_ok == nRet);

	nRet = cli_free(iStatement);
	FAILEXIT_FAIL(cli_ok == nRet);
	
	return S_OK;
}

HRESULT CFastDBConnRemote::DeleteStatement(LPCTSTR szSelectSql,LONG* pAffected )
{
	FAILEXIT_FAIL(szSelectSql);	
	
	Helper::CTransStr theTrans;
	string strSql = theTrans.MYW2A(szSelectSql);

	INT iStatement = cli_statement(m_iSession, strSql.c_str());
	FAILEXIT_FAIL(iStatement >= 0);

	INT nRet = cli_remove(iStatement);
	FAILD_LOGONLY(cli_ok == nRet);

	nRet = cli_commit(m_iSession);
	FAILD_LOGONLY(cli_ok == nRet);

	nRet = cli_free(iStatement);
	FAILEXIT_FAIL(cli_ok == nRet);

	return S_OK;
}

//注意,select 语句要取的字段集合,和pFields指定的要对上.
HRESULT CFastDBConnRemote:: SelectStatement (
	LPCTSTR			szTableName
	, LPCTSTR		szSQL
	, IGenericRS**	ppRecordset
	, DWORD*		pFields
	, INT			nFields
	) 
{
	FAILEXIT_FAIL(szTableName);
	FAILEXIT_FAIL(szSQL);
	FAILEXIT_FAIL(ppRecordset);
	
	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTableName);

	MAP_TABLE::iterator it = m_theTableSet.find(strTableName);
	if(it == m_theTableSet.end())
	{
		HRESULT hr = QueryFieldsInfo(szTableName);
		FAILEXIT_FAIL(SUCCEEDED(hr));

		it = m_theTableSet.find(strTableName);
		FAILEXIT_FAIL(it != m_theTableSet.end());

	}


	BOOL bAll = theTrans.FindCharInStr(szSQL,L'*');
	if(FALSE == bAll)
	{
		//解析sql语句,取得字段集列表.
		VEC_FIELDSET theSubSet;
		HRESULT hr = GetFieldSubSet(szSQL,it->second,theSubSet);
		FAILEXIT_FAIL(SUCCEEDED(hr));
		FAILEXIT_FAIL(false == theSubSet.empty());

		//构建一个记录集对象
		CDBRecordSetRemote* pRecordSet = new CDBRecordSetRemote(
			strTableName
			,theSubSet
			,szSQL
			,m_strDBName
			,m_iSession			
			);
		FAILEXIT_FAIL(pRecordSet);

		hr = pRecordSet->Init(
			strTableName
			,theSubSet
			,szSQL
			,m_strDBName
			,m_iSession
			);
		if(FAILED(hr))
		{
			delete pRecordSet;
			return E_FAIL;
		}

		hr = pRecordSet->QueryInterface(re_uuidof(IGenericRS),(VOID**)ppRecordset);
		if(FAILED(hr))
		{
			delete pRecordSet;
			return E_FAIL;
		}

		return S_OK;
	}

	//构建一个记录集对象
	CDBRecordSetRemote* pRecordSet = new CDBRecordSetRemote(
		strTableName
		,it->second
		,szSQL
		,m_strDBName
		,m_iSession			
		);
	FAILEXIT_FAIL(pRecordSet);

	HRESULT hr = pRecordSet->Init(
		strTableName
		,it->second
		,szSQL
		,m_strDBName
		,m_iSession
		);
	if(FAILED(hr))
	{
		delete pRecordSet;
		return E_FAIL;
	}

	hr = pRecordSet->QueryInterface(re_uuidof(IGenericRS),(VOID**)ppRecordset);
	if(FAILED(hr))
	{
		delete pRecordSet;
		return E_FAIL;
	}

	return S_OK;

}


HRESULT CFastDBConnRemote::GetFieldSubSet(IN LPCWSTR szSql,IN VEC_FIELDSET& theWholeSet,OUT VEC_FIELDSET& SubSet)
{
	FAILEXIT_FAIL(szSql);
	Helper::CTransStr theTrans;
	wstring strSql = szSql;
	//把分号替换成空格.
	BOOL bRet = theTrans.ReplaceWCHAR(strSql ,L','  ,L' ' );
	FAILEXIT_FAIL(bRet);

	vector<wstring> vTokenSet;
	bRet = theTrans.GetToken(strSql,vTokenSet,L"select",L"from");
	FAILEXIT_FAIL(bRet);

	for(UINT i = 0; i < vTokenSet.size(); ++i)
	{
		for(UINT j = 0; j < theWholeSet.size(); ++j)
		{
			BOOL bEqual = theTrans.CompareNoCase(vTokenSet[i].c_str(),theWholeSet[j].szFieldName);
			if(TRUE == bEqual)
			{
				SubSet.push_back(theWholeSet[j]);
				break;
			}
		}
	}

	return S_OK;
}

HRESULT CFastDBConnRemote::QueryFieldsInfo(LPCTSTR szTable)
{
	FAILEXIT_FAIL(szTable);
	FAILEXIT_FAIL(m_iSession > 0);

	Helper::CTransStr theTrans;
	string strTableName = theTrans.MYW2A(szTable);

	cli_field_descriptor* pDesc = NULL;
	INT nCount = cli_describe(m_iSession, strTableName.c_str(), &pDesc);
	FAILEXIT_FAIL(nCount);
	FAILEXIT_FAIL(pDesc);
	//取得字段集信息.
	std::vector<tag_Field> theFieldsSet;

	cli_field_descriptor* pTemp = pDesc;		
	for(INT i = 0; i < nCount; ++i)
	{
		tag_Field theField;
		theField.iFieldFlag = pTemp->flags;
		theField.iFieldType = theField.GetVTTypeByCliType(pTemp->type);
		wstring strFieldName = theTrans.MYA2W(pTemp->name);
		wcscpy_s(theField.szFieldName,MAX_PATH,strFieldName.c_str());
		theField.iFieldSize = 1;		//这里给添个默认值吧.
		theFieldsSet.push_back(theField);
		
		++pTemp;
	}
	
	HRESULT hr = RegistTableInfo(szTable,theFieldsSet);
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}

HRESULT CFastDBConnRemote::GetFindInfoOut(tag_Field* pField,LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize )
{
	FAILEXIT_FAIL(pField);
	FAILEXIT_FAIL(FieldName);
	FAILEXIT_FAIL(FieldType);
	FAILEXIT_FAIL(dwSize);

	wcscpy_s(FieldName,dwSize,pField->szFieldName);

	wstring strTypeName ;
	INT nType = pField->GetTypeNameByVTType(strTypeName);
	wcscpy_s(FieldType,dwSize,strTypeName.c_str());

	return S_OK;
}


