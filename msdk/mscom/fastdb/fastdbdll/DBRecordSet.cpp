#include "StdAfx.h"
#include "DBRecordSet.h"
#include <string>
using namespace std;
//加句注释
CDBRecordSet::CDBRecordSet(void)
{
	m_nCurIndex = 0;
	m_pDB = NULL;
	m_pCursor = NULL;
	m_bIsGetCount= FALSE;
	m_nCount = 0;
}

CDBRecordSet::CDBRecordSet(
			 string&		strTable
			 ,LPCTSTR		szSql
			 ,CMyDataBase*   pDB
			 )
{
	m_nCurIndex = 0;
	m_pCursor = NULL;
	m_pDB = NULL;
	m_bIsGetCount= FALSE; 
	m_nCount = 0;

}


CDBRecordSet::~CDBRecordSet(void)
{
	HRESULT hr = Uninit();
	FAILD_LOGONLY(SUCCEEDED(hr));
}

HRESULT CDBRecordSet::Init(
			 string&		strTable
			 ,LPCTSTR		szSql
			 ,CMyDataBase*  pDB
			 )
{
	//执行sql语句,构建记录集.
	FAILEXIT_FAIL(pDB);
	FAILEXIT_FAIL(szSql);

	m_pDB = pDB;

	Helper::CTransStr theTrans;
	string strSql = theTrans.MYW2A(szSql);
	CDBSqlStream stream(strSql);
	
	bool bRet = pDB->parse(stream,(dbAnyCursor**)&m_pCursor,&m_bIsGetCount);
	FAILEXIT_FAIL(bRet);
	FAILEXIT_FAIL(m_pCursor);

	m_nCount = m_pCursor->getNumberOfRecords();
	//如果是查询数量则保证成功.
	if(TRUE == m_bIsGetCount)
	{
		return S_OK;
	}

	int nCount = m_pCursor->getNumberOfRecords();
	if(0 == nCount)
		return E_FAIL;

	HRESULT hr = this->MoveFirst();
	FAILEXIT_FAIL(SUCCEEDED(hr));

	return S_OK;
}

HRESULT CDBRecordSet::Uninit()
{
	if(m_pCursor)
	{
		delete m_pCursor;
		m_pCursor = NULL;

		m_pDB->ExecDML("commit;");
	}	

	return S_OK;
}


HRESULT CDBRecordSet::GetRecord ( IProperty2** ppRecord ) 
{
	if(TRUE == m_bIsGetCount)
	{
		//如果是要取数量.与Cursor对象无关,相当于只有一条记录.
		//这里直接合成记录集
		//=================================
		UTIL::com_ptr<IProperty2> record;
		HRESULT hr = CPropertySet::CreateProp( &record.m_p ) ;
		FAILEXIT_FAIL(SUCCEEDED(hr));

		CPropVar prop; 
		prop = m_nCount;

		DWORD dwID = 0;
		hr = record->SetProperty( dwID, &prop );
		FAILEXIT_FALSE(SUCCEEDED(hr));
		*ppRecord = record.m_p;
		record->AddRef();

		return S_OK;
	}
	BOOL bRet = GetOneRecord(ppRecord);
	FAILEXIT_FAIL(bRet);

	return S_OK;
}


HRESULT CDBRecordSet:: GetScale ( PROPVARIANT* pScale ) 
{
	return E_NOTIMPL;
}

HRESULT CDBRecordSet:: MoveFirst ( VOID ) 
{
	if(TRUE == m_bIsGetCount)
	{
		//如果是要取数量.与Cursor对象无关,相当于只有一条记录.
		//=================================
		m_nCurIndex = 0;
		return S_OK;
	}
	m_nCurIndex = 0;
	bool bRet = m_pCursor->gotoFirstNew();
	FAILEXIT_FAIL(bRet);
	m_nCurIndex = 0;
	

	return S_OK;
}


HRESULT CDBRecordSet:: MoveNext ( VOID ) 
{
	if(TRUE == m_bIsGetCount)
	{
		//如果是要取数量.与Cursor对象无关,相当于只有一条记录.
		//=================================
		++m_nCurIndex;
		return S_OK;
	}

	bool bRet = m_pCursor->gotoNextNew();
	++m_nCurIndex;
	FAILEXIT_FAIL(bRet);
	

	return S_OK;
}

BOOL   CDBRecordSet::IsEOF ( VOID ) 
{
	if(TRUE == m_bIsGetCount)
	{
		//如果是要取数量.因为取count,相当于只有一条记录.
		//=================================
		if(m_nCurIndex >= 1)
			return TRUE;
		return FALSE;
	}

	INT nCount = m_pCursor->getNumberOfRecords();
    if(1 == nCount)
    {
        if(m_nCurIndex >= nCount)
            return TRUE;
        return FALSE;
    }

    if(m_nCurIndex +1 > nCount)
        return TRUE;
    return FALSE;
}

BOOL CDBRecordSet::GetOneRecord(IProperty2** ppRecord )
{
	CMyTableDescriptor* desc =  (CMyTableDescriptor*)m_pCursor->getTable();
	FAILEXIT_FALSE(desc);
	//这里对查到的结果进行显示.
	dbFieldDescriptor* columnList = desc->GetColumns();
	FAILEXIT_FALSE(columnList);

	BOOL bRet = MakeRecord((byte*)m_pDB->getRowNew(m_pCursor->GetCurrID()), columnList,ppRecord); 
	FAILEXIT_FALSE(bRet);

	return TRUE;
}

BOOL CDBRecordSet::MakeRecord(byte* base, dbFieldDescriptor* first,IProperty2** ppRecord)
{

	UTIL::com_ptr<IProperty2> record;
	HRESULT hr = CPropertySet::CreateProp( &record.m_p ) ;
	FAILEXIT_FALSE(SUCCEEDED(hr));

	dbFieldDescriptor* fd = first;
	int i = 0;
	do { 
		switch (fd->type) 
		{ 
				  case dbField::tpBool:
					  {
						  bool bVal = *(bool*)(base + fd->dbsOffs);
						  CPropVar prop; 
						  prop = bVal;

						  DWORD dwID = i;
						  hr = record->SetProperty( dwID, &prop );
						  FAILEXIT_FALSE(SUCCEEDED(hr));

						  ++i;
						  //printf("%s", *(bool*)(base + fd->dbsOffs) ? "true" : "false");
					  }
					  continue;
				  case dbField::tpInt1:
					  {
						  INT iVal = *(int1*)(base + fd->dbsOffs);
						  CPropVar prop; 
						  prop = iVal;

						  DWORD dwID = i;
						  hr = record->SetProperty( dwID, &prop );
						  FAILEXIT_FALSE(SUCCEEDED(hr));

						  ++i;
						  //printf("%d", *(int1*)(base + fd->dbsOffs)); 
					  }
					  continue;       
				  case dbField::tpInt2:
					  {
						INT iVal = *(int2*)(base + fd->dbsOffs);
						CPropVar prop; 
						prop = iVal;

						DWORD dwID = i;
						hr = record->SetProperty( dwID, &prop );
						FAILEXIT_FALSE(SUCCEEDED(hr));
						++i;
						 //printf("%d", *(int2*)(base + fd->dbsOffs)); 
					  }
					  continue;
				  case dbField::tpInt4:
					  {
						  INT iVal = *(int4*)(base + fd->dbsOffs);
						  CPropVar prop; 
						  prop = iVal;

						  DWORD dwID = i;
						  hr = record->SetProperty( dwID, &prop );
						  FAILEXIT_FALSE(SUCCEEDED(hr));
						  ++i;
						  //printf("%d", *(int4*)(base + fd->dbsOffs)); 
					  }
					  continue;
				  case dbField::tpInt8:
					  {
						  LONGLONG llVal = *(db_int8*)(base + fd->dbsOffs);
						  CPropVar prop; 
						  prop = llVal;

						  DWORD dwID = i;
						  hr = record->SetProperty( dwID, &prop );
						  FAILEXIT_FALSE(SUCCEEDED(hr));
						  ++i;
						  //printf(INT8_FORMAT, *(db_int8*)(base + fd->dbsOffs)); 
					  }
					  continue;
				  case dbField::tpReal4:
					  {
						  float fVal = *(real4*)(base + fd->dbsOffs);
						  CPropVar prop; 
						  prop = fVal;

						  DWORD dwID = i;
						  hr = record->SetProperty( dwID, &prop );
						  FAILEXIT_FALSE(SUCCEEDED(hr));
						  ++i;
						  //printf("%f", *(real4*)(base + fd->dbsOffs)); 	
					  }
					  
					  continue;
				  case dbField::tpReal8:
					  {
						  double dVal = *(real8*)(base + fd->dbsOffs);
						  CPropVar prop; 
						  prop = dVal;

						  DWORD dwID = i;
						  hr = record->SetProperty( dwID, &prop );
						  FAILEXIT_FALSE(SUCCEEDED(hr));
						  ++i;
						  //printf("%f", *(real8*)(base + fd->dbsOffs)); 
					  }
					  continue;
				  case dbField::tpString:
					  {
						   char* pTemp = (char*)base+((dbVarying*)(base+fd->dbsOffs))->offs;
						   string strVal;
						   if(pTemp)
						   {
							   strVal = pTemp;
							   CPropVar prop; 
							   prop = strVal.c_str();

							   DWORD dwID = i;
							   hr = record->SetProperty( dwID, &prop );
							   FAILEXIT_FALSE(SUCCEEDED(hr));
							   ++i;
						   }//printf("'%s'", (char*)base+((dbVarying*)(base+fd->dbsOffs))->offs);
					  }
					  continue;
				  case dbField::tpWString:
					  {
						  WCHAR* pTemp = (wchar_t*)((char*)base+((dbVarying*)(base+fd->dbsOffs))->offs);
						  wstring strVal;
						  if(pTemp)
						  {
							  strVal = pTemp;
							  CPropVar prop; 
							  prop = strVal.c_str() ;

							  DWORD dwID = i;
							  hr = record->SetProperty( dwID, &prop );
							  FAILEXIT_FALSE(SUCCEEDED(hr));
							  ++i;
						  }
						  //printf("'%ls'", (wchar_t*)((char*)base+((dbVarying*)(base+fd->dbsOffs))->offs));
					  }
					  continue;
				  case dbField::tpReference:
					  //printf("#%lx", (unsigned long)*(oid_t*)(base + fd->dbsOffs)); 
					  ++i;
					  continue;
				  case dbField::tpRectangle:
					  {
						/*  int i, sep = '(';
						  rectangle& r = *(rectangle*)(base + fd->dbsOffs);
						  for (i = 0; i < rectangle::dim*2; i++) { 
							  printf("%c%f", sep, (double)r.boundary[i]);
							  sep = ',';
						  }
						  printf(")");*/
						  ++i;
					  }
					  continue;
				  case dbField::tpRawBinary:
					  /*n = (int)fd->dbsSize;
					  elem = base + fd->dbsOffs;
					  printf("(");
					  for (i = 0; i < n; i++) { 
						  if (i != 0) { 
							  printf(", ");
						  }
						  printf("%02x", *elem++);
					  }
					  printf(")");*/
					  ++i;
					  continue;
				  case dbField::tpArray:
					 /* n = ((dbVarying*)(base + fd->dbsOffs))->size;
					  elem = base + ((dbVarying*)(base + fd->dbsOffs))->offs;
					  printf("(");
					  for (i = 0; i < n; i++) { 
						  if (i != 0) { 
							  printf(", ");
						  }
						  dumpRecord(elem, fd->components);
						  elem += fd->components->dbsSize;
					  }
					  printf(")");*/
					  ++i;
					  continue;
				  case dbField::tpStructure:
					 /* if (dateFormat != NULL 
						  && fd->components->next == fd->components 
						  && strcmp(fd->components->name, "stamp") == 0) 
					  { 
						  char buf[64];
						  printf(((dbDateTime*)(base + fd->components->dbsOffs))->asString(buf, sizeof buf, dateFormat));
						  continue;
					  }
					  printf("(");
					  dumpRecord(base, fd->components);
					  printf(")");*/
					  ++i;
					  break;
				}
				
	} while ((fd = fd->next) != first);

	*ppRecord = record.m_p;
	record->AddRef();
	return TRUE;
}





